#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "main.pio.h"

// Definições para o controle dos leds e botões
#define IS_RGBW false // Define se os LEDs possuem um canal branco adicional
#define NUM_PIXELS 25 // Número total de LEDs na matriz
#define WS2812_PIN 7  // Pino onde os LEDs estão conectados
#define tempo 100     // Tempo de espera para o piscar do LED
#define led_r 13      // Pino do LED indicador
#define button_a 5    // Pino do botão A
#define button_b 6    // Pino do botão B

static volatile uint a = 0;
static volatile uint b = 0;

// Variáveis globais para controle do sistema
static volatile uint32_t last_time = 0;  // Último tempo registrado para debounce
static volatile bool interrupt_flag = 0; // Flag para indicar interrupção
static volatile uint gpio_value = 0;     // Indica qual botão foi pressionado
static volatile uint index = 0;         // Índice do número exibido

// Declaração das funções
static void gpio_irq_handler(uint gpio, uint32_t events);
int getIndex(int x, int y);
uint matrix_rgb(float r, float g, float b);
void desenho_pio(bool *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void imprime_numeros(uint index, PIO pio, uint sm);
void init_hardware();

//--------------------- NUMEROS --------------------------------
// Definição dos números na matriz 5x5
bool numero_0[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 1, 0, 1, 1,
    1, 1, 0, 1, 1,
    1, 1, 0, 1, 1,
    1, 1, 1, 1, 1};

bool numero_1[NUM_PIXELS] = {
    0, 0, 1, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 1, 1, 0,
    0, 0, 1, 1, 0,
    1, 1, 1, 1, 1};

bool numero_2[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 0, 0, 0,
    1, 1, 1, 1, 1};

bool numero_3[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    1, 1, 1, 1, 1};

bool numero_4[NUM_PIXELS] = {
    1, 1, 0, 1, 1,
    1, 1, 0, 1, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    0, 0, 0, 1, 1};

bool numero_5[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 1, 0, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    1, 1, 1, 1, 1};

bool numero_6[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 1, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 1, 0, 0, 1,
    1, 1, 1, 1, 1};

bool numero_7[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    0, 0, 0, 1, 1,
    0, 0, 0, 1, 1,
    0, 0, 0, 1, 1};

bool numero_8[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 1, 0, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 0, 1, 1,
    1, 1, 1, 1, 1};

bool numero_9[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 1, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 1,
    1, 1, 1, 1, 1};

int main()
{
    init_hardware(); // Inicializa os periféricos

    // Inicializa a PIO
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    desenho_pio(numero_0, 0, pio, sm, 0.0, 0.0, 0.1);

    while (1)
    {
        // Faz o LED piscar
        gpio_put(led_r, true);
        sleep_ms(tempo);
        gpio_put(led_r, false);
        sleep_ms(tempo);

        // Verifica se houve uma interrupção
        if (interrupt_flag)
        {
            interrupt_flag = 0;
            switch (gpio_value)
            {
            case button_a:
                index++;
                if (index >= 0 && index <= 9)
                {
                    imprime_numeros(index, pio, sm);
                }
                else
                {
                    index--;
                }

                break;

            case button_b:
                index--;
                if (index >= 0 && index <= 9)
                {
                    imprime_numeros(index, pio, sm);
                }
                else
                {
                    index++;
                }
                break;
            default:
                break;
            }
        }
    }

    return 0;
}

// Manipulador de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events)
{
    static absolute_time_t last_time = {0}; // Garante que a variável mantenha seu valor entre chamadas
    absolute_time_t current_time = get_absolute_time();

    a++;
    printf("Sem debounce: %d\n", a);

    // Calcula a diferença de tempo em microsegundos
    int64_t diff = absolute_time_diff_us(last_time, current_time);
    printf("Tempo decorrido: %lld us\n", diff);

    if (diff > 250000) // 250ms
    {
        if (!gpio_get(button_a) || !gpio_get(button_b)) // Verifica se ainda está pressionado
        {
            b++;
            printf("Com debounce: %d\n", b);
            interrupt_flag = 1;
            gpio_value = gpio;
        }
        last_time = current_time; // Atualiza o tempo da última interrupção válida
    }
}

// Calcula o índice correto na matriz de LEDs
int getIndex(int x, int y)
{
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0)
    {
        return 24 - (y * 5 + x); // Linha par (esquerda para direita).
    }
    else
    {
        return 24 - (y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

// Converte valores RGB para um único valor a ser enviado ao LED
uint matrix_rgb(float r, float g, float b)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Exibe um desenho na matriz de LEDs
void desenho_pio(bool *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int y = 0; y < 5; y++) // Percorre as linhas da matriz
    {
        for (int x = 0; x < 5; x++) // Percorre as colunas da matriz
        {
            int real_index = getIndex(x, y); // Obtém o índice correto com base na posição
            valor_led = matrix_rgb(desenho[real_index] * r, desenho[real_index] * g, desenho[real_index] * b);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
}

// Exibe um número na matriz de LEDs
void imprime_numeros(uint index, PIO pio, uint sm)
{
    uint valor_led;

    switch (index)
    {
    case 0:
        desenho_pio(numero_0, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 1:
        desenho_pio(numero_1, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 2:
        desenho_pio(numero_2, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 3:
        desenho_pio(numero_3, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 4:
        desenho_pio(numero_4, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 5:
        desenho_pio(numero_5, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 6:
        desenho_pio(numero_6, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 7:
        desenho_pio(numero_7, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 8:
        desenho_pio(numero_8, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    case 9:
        desenho_pio(numero_9, valor_led, pio, sm, 0.0, 0.0, 0.1);
        break;
    }
}

// Inicializa os periféricos
void init_hardware()
{
    stdio_init_all();
    gpio_init(led_r);              // Inicializa o pino do LED
    gpio_set_dir(led_r, GPIO_OUT); // Configura o pino como saida

    gpio_init(button_a);             // Inicializa o pino do botao
    gpio_set_dir(button_a, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_a);          // Habilita Pull up

    gpio_init(button_b);             // Inicializa o pino do botao
    gpio_set_dir(button_b, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_b);          // Habilita Pull up

    gpio_set_irq_enabled_with_callback(button_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}