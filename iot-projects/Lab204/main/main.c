#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define BLINK_GPIO GPIO_NUM_10
TaskHandle_t BlinkyTaskHandle = NULL;
void Blinky_Task(void *arg)
{
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while (1)
    {
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    // ESP32-C3 : 1core ==> xTaskCreatePinnedToCore không cần gán core như ESP32-S3
    xTaskCreate(Blinky_Task, "Blinky", 2048, NULL, 5, &BlinkyTaskHandle);
}