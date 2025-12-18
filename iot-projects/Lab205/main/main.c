#include "led_strip.h"
#include "driver/rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define LED_PIN 10
#define NUM_LEDS 12
/*
    LAB 205. "led_strip.h not found"
    idf.py add-dependency "espressif/led_strip"
    idf.py build
*/
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;
static const RGB_t color_wheel[] = {
    {255,   0,   0},   // red
    {0,   255,   0},   // green
    {0,     0, 255},   // blue
    {255, 255, 255}    // white
};
static int current_color_index = 0;
static RGB_t current_color = color_wheel[0];
static const int COLOR_COUNT = sizeof(color_wheel) / sizeof(color_wheel[0]);
static const int brightness_steps[] = {20, 40, 60, 80, 100};
static const int BRIGHTNESS_COUNT = sizeof(brightness_steps) / sizeof(brightness_steps[0]);
static int current_step = 0;   
static int current_brightness = brightness_steps[0]; 
void increase_brightness()
{
    current_step = (current_step + 1) % BRIGHTNESS_COUNT; // vòng 20→100
    current_brightness = brightness_steps[current_step];
    printf("Brightness = %d%%\n", current_brightness);
}
void increase_color()
{
    current_color_index = (current_color_index + 1) % COLOR_COUNT;
    current_color = color_wheel[current_color_index];
    printf("Color changed to R=%d, G=%d, B=%d\n", current_color.r, current_color.g, current_color.b);
}
static void set_pixel_brightness(led_strip_handle_t strip, int index                            )
{
   uint8_t r2 = (current_color.r * current_brightness) / 100;
    uint8_t g2 = (current_color.g * current_brightness) / 100;
    uint8_t b2 = (current_color.b * current_brightness) / 100;

    led_strip_set_pixel(strip, index, r2, g2, b2);
}
void app_main(void)
{
    led_strip_handle_t led_strip;
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,
        .max_leds = NUM_LEDS,
    };
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .mem_block_symbols = 64,
    };
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    led_strip_clear(led_strip); // tắt
    while (1) {
        // Đỏ
        // led_strip_set_pixel(led_strip, 0, 255, 0, 0);
        for (int i = 0; i < NUM_LEDS; i++) {
            set_pixel_brightness(led_strip, i); 
        }
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(500));
        // Tắt
        led_strip_clear(led_strip);
        vTaskDelay(pdMS_TO_TICKS(300));
        // Xanh lá
        increase_brightness();
        // led_strip_set_pixel(led_strip, 0, 0, 255, 0);
        for (int i = 0; i < NUM_LEDS; i++) {
            set_pixel_brightness(led_strip, i);
        }
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(500));
        increase_brightness();
        // Xanh dương
        // led_strip_set_pixel(led_strip, 0, 0, 0, 255);
        for (int i = 0; i < NUM_LEDS; i++) {
            set_pixel_brightness(led_strip, i);
        }
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(500));
        increase_color();
    }
}