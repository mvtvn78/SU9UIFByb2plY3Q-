#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "led_strip.h"
#include "driver/rmt.h"
#include "DHT11.h"
static const char *TAG = "MQTT_EXAMPLE";
#define ESP_WIFI_SSID "IphoneMvt"
#define ESP_WIFI_PASS "11conso1"
#define ESP_BROKER_IP "mqtt://172.20.10.2:1883" //mqtt://192.168.1.4:1883
uint32_t MQTT_CONNECTED = 0;
volatile int led_mode = 0;
int DHT_GPIO =9;
#define LED_PIN 10
#define NUM_LEDS 12
const char *topicLed = "/mvt/led";
const char *topicDHT11 = "/mvt/dth11";
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
    char data_buf[256];

    // Copy topic và data sang buffer có null-terminate để dễ xử lý
    memcpy(topic_buf, topic, topic_len);
    topic_buf[topic_len] = '\0';

    memcpy(data_buf, data, data_len);
    data_buf[data_len] = '\0';

    printf("🚀 Received MQTT Message\n");
    printf("Topic: %s\n", topic_buf);
    printf("Data : %s\n", data_buf);

    // === Anh xử lý tùy theo topic ở đây ===
    if (strcmp(topic_buf, topicLed) == 0)
    {
        if (strcmp(data_buf, "1") == 0) {
        led_mode = 1;
        printf("LED MODE = 1 (RUN)\n");
    } else if (strcmp(data_buf, "0") == 0) {
        led_mode = 0;
        printf("LED MODE = 0 (STOP)\n");
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
            msg_id = esp_mqtt_client_subscribe(client, topicLed, 0);
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

void DHT_task(void *params){
    printf("Starting DHT Task\n\n");
    setDHTgpio(DHT_GPIO);
    while(true)
    {
        if(MQTT_CONNECTED){
            int ret= readDHT();
            errorHandler(ret);
            float h= getHumidity();
            float t= getTemperature();
            char data[50];
            sprintf(data, "Nhiet do: %.1fC, Do am: %.1f%%", t, h);
            esp_mqtt_client_publish(client, topicDHT11, data, 0, 0, 0);
        }
        vTaskDelay(3000/ portTICK_PERIOD_MS);
    }
}
led_strip_handle_t led_strip;
void init_led(){
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
}
void Led_task(void *params){
    while (1) {
        if (led_mode == 1) {
            // Đỏ
            for (int i = 0; i < NUM_LEDS; i++) {
                led_strip_set_pixel(led_strip, i, 255, 0, 0);
            }
            led_strip_refresh(led_strip);
            vTaskDelay(pdMS_TO_TICKS(500));
            // Tắt
            led_strip_clear(led_strip);
            vTaskDelay(pdMS_TO_TICKS(300));

            // Xanh lá
            for (int i = 0; i < NUM_LEDS; i++) {
                led_strip_set_pixel(led_strip, i, 0, 255, 0);
            }
            led_strip_refresh(led_strip);
            vTaskDelay(pdMS_TO_TICKS(500));
            // Xanh dương
            for (int i = 0; i < NUM_LEDS; i++) {
                led_strip_set_pixel(led_strip, i, 0, 0, 255);
            }
            led_strip_refresh(led_strip);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        else {
            // Nếu led_mode = 0 → luôn tắt và nghỉ
            led_strip_clear(led_strip);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
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
    init_led();
    xTaskCreate(&DHT_task, "DHT_task", 1024*5, NULL, 5, NULL);
    xTaskCreate(&Led_task, "Led_task", 1024*5, NULL, 5, NULL);
}