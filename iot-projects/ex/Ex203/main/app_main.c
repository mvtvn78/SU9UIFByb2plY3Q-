#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "led_strip.h"
#include "esp_event.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "touch_button.h"
#include "driver/gpio.h"
static const char *TAG = "MQTT_EXAMPLE";
#define ESP_WIFI_SSID "Gau Nghe01"
#define ESP_WIFI_PASS "GauNghe01"
#define ESP_BROKER_IP "mqtt://192.168.1.8:1883" //mqtt://192.168.1.4:1883
uint32_t MQTT_CONNECTED = 0;
const char *topicSub = "/iot/2251068263/led/cmd";
const char* topicPub = "/iot/2251068263/led/state";
const char* topicTouch = "/iot/2251068263/touch";
// =============
static uint32_t toggle_led = 0; 
static bool src = true; // true=touch, false=mqtt
#define LED_PIN 10
#define NUM_LEDS 12
/*
    idf.py add-dependency "espressif/led_strip"
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
static led_strip_handle_t led_strip;
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
// ==========
static void mqtt_app_start(void);
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                ESP_LOGI(TAG, "Trying to connect with Wi-Fi");
            break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "Disconnected: Retrying Wi-Fi");
                esp_wifi_connect();
            break;
            default:
            break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG, "Got IP: Starting MQTT Client");
        mqtt_app_start();
    }
}
void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
    wifi_config_t wifi_config = {
    .sta = {
    .ssid = ESP_WIFI_SSID,
    .password = ESP_WIFI_PASS,
    .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
void handle_mqtt_message(const char *topic, int topic_len,
                         const char *data, int data_len)
{
    char topic_buf[128];
    char data_buf[512];

    // Copy topic và data sang buffer có null-terminate để dễ xử lý
    memcpy(topic_buf, topic, topic_len);
    topic_buf[topic_len] = '\0';

    memcpy(data_buf, data, data_len);
    data_buf[data_len] = '\0';

    printf("🚀 Received MQTT Message\n");
    printf("Topic: %s\n", topic_buf);
    printf("Data : %s\n", data_buf);
    // === Anh xử lý tùy theo topic ở đây ===
    if (strcmp(topic_buf, topicSub) == 0)
    {
        src = false; // mqtt event
        cJSON *root = cJSON_Parse(data_buf);
        if (!root) {
            ESP_LOGE("MQTT", "JSON parse error!");
               if (strcmp(data_buf, "on") == 0) {
                   for (int i = 0; i < NUM_LEDS; i++) {
                            set_pixel_brightness(led_strip, i);
                        }
                        led_strip_refresh(led_strip);
                        printf("LED ON\n");
                        toggle_led = 1;
                }
                else if(strcmp(data_buf, "off") == 0){
                    led_strip_clear(led_strip);
                    printf("LED OFF\n");
                    led_strip_refresh(led_strip);
                    toggle_led = 0;
                }
                else if(strcmp(data_buf, "red") == 0){
                current_color = color_wheel[0]; // red
                for (int i = 0; i < NUM_LEDS; i++) {
                        set_pixel_brightness(led_strip, i); 
                    }
                    led_strip_refresh(led_strip);
                    toggle_led = 1;
                }
                 else if(strcmp(data_buf, "green") == 0){
                current_color = color_wheel[1]; // red
                for (int i = 0; i < NUM_LEDS; i++) {
                        set_pixel_brightness(led_strip, i); 
                    }
                    led_strip_refresh(led_strip);
                    toggle_led = 1;
                }
                else if(strcmp(data_buf, "blue") == 0){
                current_color = color_wheel[2]; // red
                for (int i = 0; i < NUM_LEDS; i++) {
                        set_pixel_brightness(led_strip, i); 
                    }
                    led_strip_refresh(led_strip);
                    toggle_led = 1;
                }
                else if(strcmp(data_buf, "white") == 0){
                current_color = color_wheel[3]; // red
                for (int i = 0; i < NUM_LEDS; i++) {
                        set_pixel_brightness(led_strip, i); 
                    }
                    led_strip_refresh(led_strip);
                    toggle_led = 1;
                }
                else if(strcmp(data_buf, "toggle") == 0){
                if (toggle_led == 0){
                            for (int i = 0; i < NUM_LEDS; i++) {
                                set_pixel_brightness(led_strip, i);
                            }
                            led_strip_refresh(led_strip);
                            printf("LED ON\n");
                            toggle_led = 1;
                        }
                        else{
                            led_strip_clear(led_strip);
                            printf("LED OFF\n");
                            led_strip_refresh(led_strip);
                            toggle_led = 0;
                        }
                }
                else{
                    ESP_LOGE("MQTT", "Unknown command: %s", data_buf);
                }
        }
        else{
            cJSON *on = cJSON_GetObjectItem(root, "on");
            cJSON *brightness = cJSON_GetObjectItem(root, "brightness");
            cJSON *color = cJSON_GetObjectItem(root, "color");
            cJSON *effect = cJSON_GetObjectItem(root, "effect");
            cJSON *duration_ms = cJSON_GetObjectItem(root, "duration_ms");
            int countValid = 0;
            if (cJSON_IsBool(on)) {
                ESP_LOGI("MQTT", "on = %d", cJSON_IsTrue(on));
                countValid++;
            }
            if (cJSON_IsNumber(brightness)) {
                ESP_LOGI("MQTT", "brightness = %d", brightness->valueint);
                countValid++;
            }
            if (cJSON_IsString(color)) {
                ESP_LOGI("MQTT", "color = %s", color->valuestring);
                 countValid++;
            }
            if (cJSON_IsString(effect)) {
                ESP_LOGI("MQTT", "effect = %s", effect->valuestring);
                 countValid++;
            }
            if(cJSON_IsNumber(duration_ms)){
                ESP_LOGI("MQTT", "duration_ms = %d", duration_ms->valueint);
                 countValid++;
            }
            if(countValid ==5){
                ESP_LOGI("MQTT", "Valid command received. Updating LED state.");
                // Cập nhật trạng thái LED dựa trên các giá trị nhận được
                if (cJSON_IsBool(on)) {
                    current_brightness = brightness->valueint;
                    // Cập nhật màu sắc từ chuỗi 255,255,0
                    if (cJSON_IsString(color)) {
                        int r, g, b;
                        if (sscanf(color->valuestring, "R=%d,G=%d,B=%d", &r, &g, &b) == 3) {
                            current_color.r = (uint8_t)r;
                            current_color.g = (uint8_t)g;
                            current_color.b = (uint8_t)b;
                        }
                    }
                    if(cJSON_IsTrue(on)){
                        for (int i = 0; i < NUM_LEDS; i++) {
                            set_pixel_brightness(led_strip, i);
                        }
                        led_strip_refresh(led_strip);
                        printf("LED ON\n");
                        toggle_led = 1;
                    }
                    else{
                        led_strip_clear(led_strip);
                        printf("LED OFF\n");
                        led_strip_refresh(led_strip);
                        toggle_led = 0;
                    }
                }
            }
            cJSON_Delete(root);
        }
        
    }
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, (long)event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            MQTT_CONNECTED = 1;
            msg_id = esp_mqtt_client_subscribe(client, topicSub, 1);
            ESP_LOGI(TAG, "Subscribed, msg_id=%d", msg_id);
        break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            MQTT_CONNECTED = 0;
        break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            handle_mqtt_message(event->topic, event->topic_len,
                        event->data, event->data_len);
        break;
        default:
            ESP_LOGI(TAG, "Other event id:%ld", (long)event->event_id);
        break;
    }
}
esp_mqtt_client_handle_t client = NULL;
static void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "STARTING MQTT");
    esp_mqtt_client_config_t mqttConfig = {
    .broker.address.uri = ESP_BROKER_IP,
    };
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void touch_event_handler(touch_evt_t evt, int64_t ts)
{
    const char *type = (evt == TOUCH_EVT_SINGLE) ? "single" :
                       (evt == TOUCH_EVT_DOUBLE) ? "double" : "long";

    ESP_LOGI(TAG, "Touch event: %s  ts=%lld", type, ts);
    char payload[80];
    snprintf(payload, sizeof(payload),
            "{\"event\":\"%s\",\"ts\":%lld}", type, ts);
    esp_mqtt_client_publish(client, topicTouch, payload, 0, 0, 0);
    src = true; // touch event
     if (strcmp(type, "single") == 0){
        if (toggle_led == 0){
            for (int i = 0; i < NUM_LEDS; i++) {
                set_pixel_brightness(led_strip, i);
            }
            led_strip_refresh(led_strip);
            printf("LED ON\n");
            toggle_led = 1;
        }
        else{
            led_strip_clear(led_strip);
             printf("LED OFF\n");
            led_strip_refresh(led_strip);
            toggle_led = 0;
        }
    }
    else if(strcmp(type, "double") == 0){
       // đổi màu kế tiếp theo vòng:
        increase_color();
        for (int i = 0; i < NUM_LEDS; i++) {
            set_pixel_brightness(led_strip, i); 
        }
        led_strip_refresh(led_strip);
        toggle_led = 1;
    }
    else if(strcmp(type, "long") == 0){
        // tăng brightness theo bậc
        increase_brightness();
        for (int i = 0; i < NUM_LEDS; i++) {
            set_pixel_brightness(led_strip, i); 
        }
        led_strip_refresh(led_strip);
        toggle_led = 1;
    }

}
void Led_State_Task(void *params){
    while(true)
    {
        if(MQTT_CONNECTED){
            char rgb_char[32];
            snprintf(rgb_char, sizeof(rgb_char),
             "R=%u,G=%u,B=%u",
             current_color.r, current_color.g, current_color.b);
            char payload[150];
            int64_t ts = esp_timer_get_time() / 1000ULL; // epoch ms
            
            snprintf(payload, sizeof(payload),
                "{"
                    "\"on\": %s,"
                    "\"color\": \"%s\","
                    "\"brightness\": %u,"
                    "\"effect\": \"%s\","
                    "\"src\": \"%s\","
                    "\"ts\": %lld"
                "}",
                toggle_led == 1 ? "true" : "false",
                rgb_char,
                current_brightness,
                "breathe",
                src ? "touch" : "mqtt",
                ts
            );
            esp_mqtt_client_publish(client, topicPub, payload, 0, 0, 0);
        }
        vTaskDelay(10000/ portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init();
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
    led_strip_clear(led_strip); 
    touch_btn_init(touch_event_handler);
    xTaskCreate(touch_btn_task, "touch_btn_task", 4096, NULL, 5, NULL);
    xTaskCreate(Led_State_Task, "Led_State_Task", 4096, NULL, 5, NULL);
}