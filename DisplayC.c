//Marcio de Arruda Fonseca

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"

#define LED_AZUL 12
#define LED_VERMELHO 13
#define LED_VERDE 11
#define BOTAO_JOYSTICK 22
#define BOTAO_A 5
#define CANAL_ADC_Y 0
#define CANAL_ADC_X 1

#define CENTRO_JOYSTICK 2048
#define ZONA_MORTA 200

#define PORTA_I2C i2c1
#define PIN_I2C_SDA 14
#define PIN_I2C_SCL 15
#define ENDERECO_SSD1306 0x3C
#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

volatile bool pwm_ativado = true;
volatile bool led_verde_ligado = false;
volatile uint8_t estilo_borda = 0;
volatile absolute_time_t ultimo_tempo_joystick;
volatile bool botao_joystick_pressionado = false;
volatile absolute_time_t ultimo_tempo_botao_a;
volatile bool botao_a_pressionado = false;

void configurar_pwm(uint pino) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 4095);
    pwm_init(slice, &config, true);
}

uint16_t mapear_joystick_para_pwm(uint16_t valor) {
    if (abs((int16_t)valor - CENTRO_JOYSTICK) < ZONA_MORTA)
        return 0;
    else if (valor < CENTRO_JOYSTICK)
        return CENTRO_JOYSTICK - valor;
    else
        return valor - CENTRO_JOYSTICK;
}

void desenhar_borda_simples(ssd1306_t *display) {
    ssd1306_rect(display, 3, 3, 122, 58, true, false);
}

void desenhar_borda_dupla(ssd1306_t *display) {
    ssd1306_rect(display, 3, 3, 122, 58, true, false);
    ssd1306_rect(display, 6, 6, 116, 52, true, false);
}

void desenhar_borda_tripla(ssd1306_t *display) {
    ssd1306_rect(display, 3, 3, 122, 58, true, false);
    ssd1306_rect(display, 6, 6, 116, 52, true, false);
    ssd1306_rect(display, 9, 9, 110, 46, true, false);
}

void callback_gpio(uint gpio, uint32_t eventos) {
    absolute_time_t agora = get_absolute_time();
    if (gpio == BOTAO_JOYSTICK) {
        if (absolute_time_diff_us(ultimo_tempo_joystick, agora) > 50000) {
            ultimo_tempo_joystick = agora;
            if (!gpio_get(BOTAO_JOYSTICK)) {
                if (!botao_joystick_pressionado) {
                    botao_joystick_pressionado = true;
                    led_verde_ligado = !led_verde_ligado;
                    gpio_put(LED_VERDE, led_verde_ligado);
                    estilo_borda = (estilo_borda + 1) % 3;
                }
            } else {
                botao_joystick_pressionado = false;
            }
        }
    } else if (gpio == PIN_BOTAO_A) {
        if (absolute_time_diff_us(ultimo_tempo_botao_a, agora) > 50000) {
            ultimo_tempo_botao_a = agora;
            if (!gpio_get(PIN_BOTAO_A)) {
                if (!botao_a_pressionado) {
                    botao_a_pressionado = true;
                    pwm_ativado = !pwm_ativado;
                    if (!pwm_ativado) {
                        pwm_set_gpio_level(LED_AZUL, 0);
                        pwm_set_gpio_level(LED_VERMELHO, 0);
                    }
                }
            } else {
                botao_a_pressionado = false;
            }
        }
    }
}

int main() {
    stdio_init_all();

    i2c_init(PORTA_I2C, 400 * 1000);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);

    ssd1306_t display;
    ssd1306_init(&display, LARGURA_DISPLAY, ALTURA_DISPLAY, false, ENDERECO_SSD1306, PORTA_I2C);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);

    gpio_init(BOTAO_JOYSTICK);
    gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(BOTAO_JOYSTICK);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);

    gpio_init(PIN_BOTAO_A);
    gpio_set_dir(PIN_BOTAO_A, GPIO_IN);
    gpio_pull_up(PIN_BOTAO_A);

    gpio_set_irq_enabled_with_callback(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, callback_gpio);
    gpio_set_irq_enabled(PIN_BOTAO_A, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    configurar_pwm(LED_AZUL);
    configurar_pwm(LED_VERMELHO);

    char str_x[6], str_y[6];
    int16_t pos_x = ALTURA_DISPLAY / 2.5;
    int16_t pos_y = LARGURA_DISPLAY / 2;
    uint16_t ultimo_x = CENTRO_JOYSTICK;
    uint16_t ultimo_y = CENTRO_JOYSTICK;

    while (1) {
        adc_select_input(CANAL_ADC_Y);
        uint16_t y = adc_read();
        adc_select_input(CANAL_ADC_X);
        uint16_t x = adc_read();

        if (abs((int16_t)x - (int16_t)ultimo_x) > 10 || abs((int16_t)y - (int16_t)ultimo_y) > 10) {
            uint16_t pwm_azul = mapear_joystick_para_pwm(y);
            uint16_t pwm_vermelho = mapear_joystick_para_pwm(x);

            int16_t delta_x = (int16_t)x - CENTRO_JOYSTICK;
            int16_t delta_y = (int16_t)y - CENTRO_JOYSTICK;

            int16_t offset_x = (-delta_y * 40) / 2048;
            int16_t offset_y = (delta_x * 20) / 2048;

            pos_x += offset_x;
            pos_y += offset_y;

            pos_x = (pos_x < 0) ? 0 : (pos_x > 56) ? 56 : pos_x;
            pos_y = (pos_y < 0) ? 0 : (pos_y > 120) ? 120 : pos_y;

            if (pwm_ativado) {
                pwm_set_gpio_level(LED_AZUL, pwm_azul);
                pwm_set_gpio_level(LED_VERMELHO, pwm_vermelho);
            } else {
                pwm_set_gpio_level(LED_AZUL, 0);
                pwm_set_gpio_level(LED_VERMELHO, 0);
            }

            ultimo_x = x;
            ultimo_y = y;
        }

        sprintf(str_x, "%d", x);
        sprintf(str_y, "%d", y);

        ssd1306_fill(&display, false);
        ssd1306_rect(&display, pos_x, pos_y, 8, 8, true, true);

        if (led_verde_ligado) {
            switch (estilo_borda) {
                case 0:
                    desenhar_borda_dupla(&display);
                    break;
                case 1:
                    desenhar_borda_simples(&display);
                    break;
                case 2:
                    desenhar_borda_tripla(&display);
                    break;
            }
        }

        ssd1306_send_data(&display);
        sleep_ms(10);
    }
    
    return 0;
}
