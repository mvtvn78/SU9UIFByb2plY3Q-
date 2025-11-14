#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#define BLINK_GPIO GPIO_NUM_10
TaskHandle_t HelloWorldTaskHandle = NULL;
TaskHandle_t BlinkyTaskHandle = NULL;
void HelloWorld_Task(void *arg)
{
    while (1)
    {
        printf("Task running: Hello World ..\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void Blinky_Task(void *arg)
{
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    int count_second = 0;
    while (1)
    {
        count_second += 1;
        switch (count_second)
        {
        case 10:
            vTaskSuspend(HelloWorldTaskHandle);
            printf("HelloWorld task suspended .. \n");
            break;
        case 14:
            vTaskResume(HelloWorldTaskHandle);
            printf("HelloWorld task resumed .. \n");
            break;
        case 20:
            vTaskDelete(HelloWorldTaskHandle);
            printf("HelloWorld task deleted .. \n");
            break;
        default:
            break;
        }
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    /*
    BaseType_t xTaskCreate(
        TaskFunction_t pvTaskCode,   // Hàm thực thi task
        const char * const pcName,   // Tên task (chuỗi, chỉ để debug)
        configSTACK_DEPTH_TYPE usStackDepth, // Kích thước stack (tính bằng từ 4-byte)
        void *pvParameters,          // Tham số truyền vào task
        UBaseType_t uxPriority,      // Priority của task
        TaskHandle_t *pxCreatedTask  // Handle của task, để quản lý (suspend/resume/delete)
    );*/
    xTaskCreate(Blinky_Task, "Blinky", 2048, NULL, 10, &BlinkyTaskHandle); // Core 0
    xTaskCreate(HelloWorld_Task, "HelloWorld", 2048, NULL, 10, &HelloWorldTaskHandle); // Core 0
}
