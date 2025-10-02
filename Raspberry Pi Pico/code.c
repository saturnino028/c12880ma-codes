#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Definições de pinos
#define SPEC_TRG     18
#define SPEC_ST      14
#define SPEC_CLK     16

// ADC2 (GPIO28)
#define SPEC_VIDEO   28
#define WHITE_LED    17
#define LASER_404    15

// Configurações do espectrômetro
#define SPEC_CHANNELS 288
uint16_t data[SPEC_CHANNELS];

void readSpectrometer() {
    // delay em microsegundos
    int delayTime = 1;

    // Pulso inicial
    gpio_put(SPEC_CLK, 0);
    sleep_us(delayTime);
    gpio_put(SPEC_CLK, 1);
    sleep_us(delayTime);
    gpio_put(SPEC_CLK, 0);
    gpio_put(SPEC_ST, 1);
    sleep_us(delayTime);

    // 15 ciclos
    for (int i = 0; i < 15; i++) {
        gpio_put(SPEC_CLK, 1);
        sleep_us(delayTime);
        gpio_put(SPEC_CLK, 0);
        sleep_us(delayTime);
    }

    // SPEC_ST = LOW
    gpio_put(SPEC_ST, 0);

    // 85 ciclos
    for (int i = 0; i < 85; i++) {
        gpio_put(SPEC_CLK, 1);
        sleep_us(delayTime);
        gpio_put(SPEC_CLK, 0);
        sleep_us(delayTime);
    }

    // 1 pulso extra
    gpio_put(SPEC_CLK, 1);
    sleep_us(delayTime);
    gpio_put(SPEC_CLK, 0);
    sleep_us(delayTime);

    // Leitura dos canais
    for (int i = 0; i < SPEC_CHANNELS; i++) {
        // leitura do ADC
        data[i] = adc_read();
        gpio_put(SPEC_CLK, 1);
        sleep_us(delayTime);
        gpio_put(SPEC_CLK, 0);
        sleep_us(delayTime);
    }

    // SPEC_ST = HIGH
    gpio_put(SPEC_ST, 1);

    // 7 ciclos extras
    for (int i = 0; i < 7; i++) {
        gpio_put(SPEC_CLK, 1);
        sleep_us(delayTime);
        gpio_put(SPEC_CLK, 0);
        sleep_us(delayTime);
    }

    gpio_put(SPEC_CLK, 1);
    sleep_us(delayTime);
}

void printData() {
    for (int i = 0; i < SPEC_CHANNELS; i++) {
        printf("%u,", data[i]);
    }
    printf("\n");
}

int main() {
    stdio_init_all();

    // Inicializa GPIOs
    gpio_init(SPEC_CLK);
    gpio_set_dir(SPEC_CLK, GPIO_OUT);
    gpio_init(SPEC_ST);
    gpio_set_dir(SPEC_ST, GPIO_OUT);
    gpio_init(LASER_404);
    gpio_set_dir(LASER_404, GPIO_OUT);
    gpio_init(WHITE_LED);
    gpio_set_dir(WHITE_LED, GPIO_OUT);

    // Configura ADC
    adc_init();
    // GPIO28 → ADC2
    adc_gpio_init(SPEC_VIDEO);
    adc_select_input(3);

    // Inicializa estado
    gpio_put(SPEC_CLK, 1);
    gpio_put(SPEC_ST, 0);

    while (true) {
        readSpectrometer();
        printData();
        sleep_ms(10);
    }
}
