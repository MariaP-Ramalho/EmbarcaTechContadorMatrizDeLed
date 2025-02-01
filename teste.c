#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "main.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define LED_PIN 7
#define tempo 100

const uint led_b = 12;   // LED AZUL
const uint button_a = 5; // Botão A = 5, Botão B = 6 , BotãoJoy = 22
const uint button_b = 6;

// Variáveis globais
static volatile uint a = 1;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile bool interrupt = 0;
static volatile uint pressed = 0;
static volatile uint index = 0;

static void gpio_irq_handler(uint gpio, uint32_t events);

uint32_t matrix_rgb(float r, float g, float b)
{
    uint8_t R = (uint8_t)(r * 255);
    uint8_t G = (uint8_t)(g * 255);
    uint8_t B = (uint8_t)(b * 255);
    return (R << 16) | (G << 8) | B;  // Certifique-se de que a ordem dos bytes está correta
}


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

// Funcao para desenhar a matriz
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{

    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(desenho[i] * r, desenho[i] * g, desenho[i] * b);
        pio_sm_put_blocking(pio, sm, valor_led);
    };
}

//--------------------- NUMEROS --------------------------------
double numero_0[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

double numero_1[NUM_PIXELS] = {0.0, 1.0, 1.0, 0.0, 0.0,
                               0.0, 1.0, 1.0, 1.0, 0.0,
                               0.0, 1.0, 1.0, 0.0, 0.0,
                               0.0, 1.0, 1.0, 0.0, 0.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

double numero_2[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 0.0, 0.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

double numero_3[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 0.0,
                               0.0, 0.0, 1.0, 1.0, 0.0,
                               1.0, 1.0, 1.0, 1.0, 0.0,
                               0.0, 0.0, 1.0, 1.0, 0.0,
                               1.0, 1.0, 1.0, 1.0, 0.0};

double numero_4[NUM_PIXELS] = {1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0};

double numero_5[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 0.0, 0.0,
                               1.0, 1.0, 1.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

double numero_6[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 0.0, 0.0,
                               1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 0.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

double numero_7[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0};

double numero_8[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 1.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

double numero_9[NUM_PIXELS] = {1.0, 1.0, 1.0, 1.0, 1.0,
                               1.0, 0.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0,
                               0.0, 0.0, 0.0, 1.0, 1.0,
                               1.0, 1.0, 1.0, 1.0, 1.0};

void imprime_numeros(uint index, PIO pio, uint sm)
{
    uint valor_led;

    switch (index)
    {
    case 0:
        desenho_pio(numero_0, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 1:
        desenho_pio(numero_1, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 2:
        desenho_pio(numero_2, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 3:
        desenho_pio(numero_3, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 4:
        desenho_pio(numero_4, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 5:
        desenho_pio(numero_5, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 6:
        desenho_pio(numero_6, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 7:
        desenho_pio(numero_7, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 8:
        desenho_pio(numero_8, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    case 9:
        desenho_pio(numero_9, valor_led, pio, sm, 0.0, 1.0, 0.0);
        break;
    }
}

int main()
{
    stdio_init_all();

    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);

    uint offset = pio_add_program(pio, &ws2812_program);
    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, LED_PIN);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    // Inicializa o LED
    gpio_init(led_b);              // Inicializa o pino do LED
    gpio_set_dir(led_b, GPIO_OUT); // Configura o pino como saída

    gpio_init(button_a);
    gpio_set_dir(button_a, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_a);

    gpio_init(button_b);
    gpio_set_dir(button_b, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_b);

    gpio_set_irq_enabled_with_callback(button_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    uint valor_led;

    while (true)
    {
        gpio_put(led_b, true);
        sleep_ms(tempo);
        gpio_put(led_b, false);
        sleep_ms(tempo);

        if (interrupt)
        {
            interrupt = 0;
            switch (pressed)
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
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time;
        interrupt = 1;
        pressed = gpio;
    }
}
