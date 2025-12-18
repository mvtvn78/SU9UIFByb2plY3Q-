#include "touch_button.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"           
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"      

static const char *TAG = "TOUCH_BTN";

#define DEBOUNCE_MS     80
#define SINGLE_MS       400
#define DOUBLE_MS       500
#define LONG_MS         1200

static touch_btn_callback_t s_callback = NULL;

static bool pressing = false;
static bool long_fired = false;

static int64_t t_press = 0;
static int64_t last_tap = 0;

static esp_timer_handle_t debounce_timer;

// ISR phải khai báo trước khi sử dụng
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    static int64_t last_interrupt = 0;
    int64_t now = esp_timer_get_time() / 1000;

    if (now - last_interrupt < DEBOUNCE_MS)
        return;

    last_interrupt = now;

    esp_timer_start_once(debounce_timer, DEBOUNCE_MS * 1000);
}

static void debounce_timer_cb(void *arg)
{
    int level = gpio_get_level(TOUCH_GPIO);

    if (level == 1 && !pressing)
    {
        pressing = true;
        long_fired = false;
        t_press = esp_timer_get_time() / 1000;
    }
}

void touch_btn_init(touch_btn_callback_t cb)
{
    s_callback = cb;

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << TOUCH_GPIO,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&io_conf);

    const esp_timer_create_args_t tmr_args = {
        .callback = debounce_timer_cb,
        .name = "touch_debounce"
    };
    esp_timer_create(&tmr_args, &debounce_timer);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(TOUCH_GPIO, gpio_isr_handler, NULL);
}

void touch_btn_task(void *arg)
{
    while (1)
    {
        int level = gpio_get_level(TOUCH_GPIO);
        int64_t now = esp_timer_get_time() / 1000;

        if (pressing)
        {
            if (!long_fired && (now - t_press >= LONG_MS))
            {
                long_fired = true;
                pressing = false;
                if (s_callback)
                    s_callback(TOUCH_EVT_LONG, now);
            }

            if (level == 0 && !long_fired)
            {
                pressing = false;
                int64_t duration = now - t_press;

                if (duration < SINGLE_MS)
                {
                    if (now - last_tap <= DOUBLE_MS)
                    {
                        last_tap = 0;
                        if (s_callback)
                            s_callback(TOUCH_EVT_DOUBLE, now);
                    }
                    else
                    {
                        last_tap = now;
                        if (s_callback)
                            s_callback(TOUCH_EVT_SINGLE, now);
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
