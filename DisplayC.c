#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"


// DEFINIÇÕES PARA O DISPLAY SSD1306

#define I2C_PORT    i2c1
#define I2C_SDA     14
#define I2C_SCL     15
#define SSD1306_ADDR 0x3C
#define WIDTH       128
#define HEIGHT      64

// DEFINIÇÕES PARA A MATRIZ WS2812 (5x5)

#define IS_RGBW     false
#define NUM_PIXELS  25
#define WS2812_PIN  7

// Intensidades de cor para os WS2812
uint8_t led_r = 0;   // vermelho
uint8_t led_g = 0;   // verde
uint8_t led_b = 1;   // azul

bool led[NUM_PIXELS] = {0};

// DEFINIÇÕES PARA OS BOTÕES E LED RGB
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Conforme o enunciado, o LED RGB possui os pinos 11, 12 e 13.
// Como precisamos controlar somente o LED verde e o LED azul e 
// considerando que no seu hardware o verde deve estar em GPIO 11
// e o azul em GPIO 12, faremos:
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12

#define DEBOUNCE_TIME_MS 350

volatile bool led_green_state = false;
volatile bool led_blue_state = false;

volatile uint32_t last_interrupt_time_A = 0;
volatile uint32_t last_interrupt_time_B = 0;

// Instância global do display para acesso nas interrupções
ssd1306_t ssd;

// Função para enviar os dados de um pixel via PIO para os WS2812
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Função para converter valores R, G, B para o formato WS2812
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Atualiza a matriz de LEDs de acordo com o número (0 a 9)

void update_led_matrix(int number) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        led[i] = 0;
    }
  
    // Define os LEDs que devem ser ligados para cada número
    switch (number)
    {
        case 0:
            // Número 0
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 1; led[16] = 0; led[17] = 0; led[18] = 0; led[19] = 1;
            led[14] = 1; led[13] = 0; led[12] = 0; led[11] = 0; led[10] = 1;
            led[5] = 1; led[6] = 0; led[7] = 0; led[8] = 0; led[9] = 1;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 1:
            // Número 1
            led[24] = 0; led[23] = 0; led[22] = 1; led[21] = 0; led[20] = 0;
            led[15] = 0; led[16] = 1; led[17] = 1; led[18] = 0; led[19] = 0;
            led[14] = 0; led[13] = 0; led[12] = 1; led[11] = 0; led[10] = 0;
            led[5] = 0; led[6] = 0; led[7] = 1; led[8] = 0; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 2:
            // Número 2
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 0; led[17] = 0; led[18] = 1; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 1; led[7] = 0; led[8] = 0; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 3:
            // Número 3
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 0; led[17] = 0; led[18] = 1; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 0; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 4:
            // Número 4
            led[24] = 0; led[23] = 1; led[22] = 0; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 1; led[17] = 0; led[18] = 1; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 0; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 0; led[2] = 0; led[1] = 1; led[0] = 0;
            break;
        case 5:
            // Número 5
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 1; led[17] = 0; led[18] = 0; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 0; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 6:
            // Número 6
            led[24] = 0; led[23] = 0; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 1; led[17] = 0; led[18] = 0; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 1; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 7:
            // Número 7
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 0; led[17] = 0; led[18] = 1; led[19] = 0;
            led[14] = 0; led[13] = 0; led[12] = 0; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 0; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 0; led[2] = 0; led[1] = 1; led[0] = 0;
            break;
        case 8:
            // Número 8
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 1; led[17] = 0; led[18] = 1; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 1; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        case 9:
            // Número 9
            led[24] = 0; led[23] = 1; led[22] = 1; led[21] = 1; led[20] = 0;
            led[15] = 0; led[16] = 1; led[17] = 0; led[18] = 1; led[19] = 0;
            led[14] = 0; led[13] = 1; led[12] = 1; led[11] = 1; led[10] = 0;
            led[5] = 0; led[6] = 0; led[7] = 0; led[8] = 1; led[9] = 0;
            led[4] = 0; led[3] = 1; led[2] = 1; led[1] = 1; led[0] = 0;
            break;
        default:
            break;
    }
  
    // Atualiza a matriz WS2812: envia os dados para cada pixel
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (led[i]) {
            put_pixel(urgb_u32(led_r, led_g, led_b));
        } else {
            put_pixel(0);
        }
    }
}

// Função de interrupção para os botões (IRQ)
// Utilizado para tratar a ação dos botões com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Se ambos os botões estiverem pressionados (nível baixo), limpa o display
    if (gpio_get(BUTTON_A_PIN) == 0 && gpio_get(BUTTON_B_PIN) == 0) {
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "", 10, 30); //"Display Limpo"
        ssd1306_send_data(&ssd);
        return;
    }
    
    if (gpio == BUTTON_A_PIN) {
        if (current_time - last_interrupt_time_A < DEBOUNCE_TIME_MS) {
            return;
        }
        last_interrupt_time_A = current_time;
        
        // Alterna o estado do LED RGB Verde
        led_green_state = !led_green_state;
        gpio_put(LED_GREEN_PIN, led_green_state);
        
        // Atualiza o display SSD1306 com a mensagem informativa
        ssd1306_fill(&ssd, false);
        if (led_green_state) {
            ssd1306_draw_string(&ssd, "LED Verde", 10, 20);
            ssd1306_draw_string(&ssd, "Ligado", 10, 30);
            printf("\nBotao A: LED Verde Ligado\n");
        } else {
            ssd1306_draw_string(&ssd, "LED Verde", 10, 20);
            ssd1306_draw_string(&ssd, "Desligado", 10, 30);
            printf("\nBotao A: LED Verde Desligado\n");
        }
        ssd1306_send_data(&ssd);
    }
    else if (gpio == BUTTON_B_PIN) {
        if (current_time - last_interrupt_time_B < DEBOUNCE_TIME_MS) {
            return;
        }
        last_interrupt_time_B = current_time;
        
        // Alterna o estado do LED RGB Azul
        led_blue_state = !led_blue_state;
        gpio_put(LED_BLUE_PIN, led_blue_state);
        
        // Atualiza o display SSD1306 com a mensagem informativa
        ssd1306_fill(&ssd, false);
        if (led_blue_state) {
            ssd1306_draw_string(&ssd, "LED Azul", 10, 20);
            ssd1306_draw_string(&ssd, "Ligado", 10, 30);
            printf("\nBotao B: LED Azul Ligado\n");
        } else {
            ssd1306_draw_string(&ssd, "LED Azul", 10, 20);
            ssd1306_draw_string(&ssd, "Desligado", 10, 30);
            printf("\nBotao B: LED Azul Desligado\n");
        }
        ssd1306_send_data(&ssd);
    }
}

int main() {
    stdio_init_all();

    // INICIALIZAÇÃO DO DISPLAY SSD1306 via I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display (inicia com todos os pixels apagados)
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // INICIALIZAÇÃO DA MATRIZ WS2812 (5x5) via PIO
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // INICIALIZAÇÃO DOS BOTÕES
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    
    // INICIALIZAÇÃO DOS LEDS RGB (Verde e Azul)
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, led_green_state);
    
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, led_blue_state);
    
    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    
    // Variável para armazenar o último caractere válido recebido
    char lastChar = '\0';

    // LOOP PRINCIPAL – PROCESSA A ENTRADA SERIAL
    while (true) {
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT) {
            char c = (char)ch;
            // Ignora caracteres de nova linha e retorno de carro
            if (c == '\n' || c == '\r') {
                continue;
            }
            lastChar = c;
            printf("Recebeu: %c\n", c);

            // Atualiza o display somente quando um novo caractere válido é recebido
            ssd1306_fill(&ssd, false);  // limpa o display
            char str[2] = {c, '\0'};
            ssd1306_draw_string(&ssd, str, 10, 10);
            ssd1306_send_data(&ssd);

            // Se o caractere for um dígito (0 a 9), atualiza a matriz WS2812
            if (c >= '0' && c <= '9') {
                int digit = c - '0';
                update_led_matrix(digit);
            }
        }
        sleep_ms(100);
    }
  
    return 0;
}
