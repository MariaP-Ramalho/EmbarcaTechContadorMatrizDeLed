# EmbarcaTechContadorMatrizDeLed

**Video do funcionamento:** https://youtu.be/Fvy0dcUThrI

Projeto desenvolvido durante a primeira fase da residência Embarcatech.

Este projeto utiliza um microcontrolador Raspberry Pi Pico para exibir números de 0 a 9 em uma matriz de LEDs WS2812. A alteração dos números é feita através de dois botões, que permitem incrementar ou decrementar o valor exibido.


## Recursos e Funcionalidades

- Recursos e FuncionalidadesControle de matriz de LEDs WS2812 (NeoPixel) via PIO (Programmable  - Input/Output).
- Exibição de números de 0 a 9 em um display 5x5 de LEDs.
- Controle por botões para avançar ou retroceder entre os números.
- Implementação de debounce por software para evitar leituras erradas dos botões.
- Indicação visual com um LED auxiliar piscando.

## Componentes Utilizados
- Microcontrolador Raspberry Pi Pico
- Matriz de LEDs WS2812 (25 LEDs)
- Dois botões de entrada
- LED indicador

## Dependências
- Biblioteca pico-sdk para programação do Raspberry Pi Pico.
- Configuração de PIO para comunicação com os LEDs.
- Uso de hardware/pio.h e hardware/clocks.h para manipulação dos periféricos do Raspberry Pi Pico.

## Estrutura do Código
- main.c: Contém a lógica principal do programa, incluindo inicialização de hardware, controle dos LEDs e manipulação dos botões.
- main.pio.h: Definição do programa PIO para comunicação com os LEDs WS2812.

## Instalação

Clone este repositório e configure o ambiente de desenvolvimento do Raspberry Pi Pico utilizando o pico-sdk.

```bash
git clone https://github.com/MariaP-Ramalho/EmbarcaTechContadorMatrizDeLed.git
cd projeto-leds
mkdir build
cd build
cmake ..
make
```

## Como Executar

- Envie o binário gerado (.uf2) para o Raspberry Pi Pico via USB.
- O programa iniciará automaticamente exibindo o LED RGB vermelho piscando.
