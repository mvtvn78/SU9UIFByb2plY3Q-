#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
static const char *TAG = "BLE_ESP32C3";

// Service và Characteristic UUIDs
#define GATTS_SERVICE_UUID      0x00FF
#define GATTS_CHAR_UUID_RX      0xFF01  // Nhận từ Flutter
#define GATTS_CHAR_UUID_TX      0xFF02  // Gửi cho Flutter
#define GATTS_NUM_HANDLE        8       // Tăng từ 4 lên 8

#define DEVICE_NAME             "ESP32C3_BLE"
#define PREPARE_BUF_MAX_SIZE    1024

static uint8_t adv_config_done = 0;
#define ADV_CONFIG_FLAG      (1 << 0)

static uint16_t gatts_if_store;
static uint16_t conn_id_store = 0xFFFF;
static uint16_t char_handle_tx = 0;
char ssid[128];
char password[128];
static char received_data[PREPARE_BUF_MAX_SIZE];

// Advertising data
static uint8_t adv_service_uuid[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid),
    .p_service_uuid = adv_service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle_rx;
    uint16_t char_handle_tx;
    uint16_t descr_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_uuid;
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                       esp_gatt_if_t gatts_if,
                                       esp_ble_gatts_cb_param_t *param);

#define PROFILE_NUM      1
#define PROFILE_APP_IDX  0

static struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    },
};
void send_response(const char *message) {
    if (conn_id_store != 0xFFFF && char_handle_tx != 0) {
        // Gửi response
        esp_err_t ret = esp_ble_gatts_send_indicate(
            gatts_if_store, 
            conn_id_store, 
            char_handle_tx, 
            strlen(message), 
            (uint8_t *)message, 
            false
        );
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "✓ Đã gửi: %s", message);
        } else {
            ESP_LOGE(TAG, "✗ Lỗi gửi: %d", ret);
        }
    }
}
bool wifi_is_connected(void)
{
    wifi_ap_record_t ap_info;
    return esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK;
}
void wifi_disconnect_if_needed(void)
{
    if (wifi_is_connected())
    {
        ESP_LOGI("WIFI", "WiFi is connected → disconnecting...");
        send_response("WiFi is connected → disconnecting...");
        esp_wifi_disconnect();
        vTaskDelay(pdMS_TO_TICKS(500)); // chờ ngắt hoàn toàn
    }
}

// call when bluetooth receive ssid and pass
void wifi_connect_from_bt(void)
{
    wifi_disconnect_if_needed();
    wifi_config_t wifi_config = {0};

    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char *)wifi_config.sta.password, password);

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_connect();
}
// Hiển thị thông tin QR Code
void print_qr_code() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    
    ESP_LOGI(TAG, "\n");
    ESP_LOGI(TAG, "╔════════════════════════════════╗");
    ESP_LOGI(TAG, "║   QUÉT QR ĐỂ KẾT NỐI BLE      ║");
    ESP_LOGI(TAG, "╠════════════════════════════════╣");
    ESP_LOGI(TAG, "║                                ║");
    ESP_LOGI(TAG, "║  Device: %-21s ║", DEVICE_NAME);
    ESP_LOGI(TAG, "║  MAC: %02X:%02X:%02X:%02X:%02X:%02X        ║",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ESP_LOGI(TAG, "║                                ║");
    ESP_LOGI(TAG, "║  [█████████ QR CODE █████████] ║");
    ESP_LOGI(TAG, "║                                ║");
    ESP_LOGI(TAG, "║  Nội dung: %-19s ║", DEVICE_NAME);
    ESP_LOGI(TAG, "║                                ║");
    ESP_LOGI(TAG, "╚════════════════════════════════╝");
    ESP_LOGI(TAG, "\n");
}

// Xử lý tin nhắn và tạo response
void process_message(const char *msg, char *response, size_t response_size) {
    
    // Chuyển sang lowercase để so sánh (chỉ với ASCII)
    char lower_msg[256];
    strncpy(lower_msg, msg, sizeof(lower_msg) - 1);
    lower_msg[sizeof(lower_msg) - 1] = '\0';
    
    // Chuyển thành lowercase (chỉ với ký tự ASCII)
    for (int i = 0; lower_msg[i]; i++) {
        if (lower_msg[i] >= 'A' && lower_msg[i] <= 'Z') {
            lower_msg[i] = lower_msg[i] + 32;
        }
    }
    // Xử lý các lệnh - kiểm tra cả UTF-8 và ASCII
    if (strstr(lower_msg, "ping")) {
        snprintf(response, response_size, "ESP32-C3: Pong!");
    }
    else if(sscanf(msg,"ssid=%128[^,],pass=%128[^\n]", ssid, password) == 2) {
        snprintf(response, response_size, "ESP32-C3: Nhận SSID: %s và Pass: %s", ssid, password);
        wifi_connect_from_bt();
    }
    else {
        snprintf(response, response_size, "ESP32-C3: Lệnh không xác định.");
    }
}


static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising start failed");
        } else {
            ESP_LOGI(TAG, "✓ Advertising started");
        }
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                       esp_gatt_if_t gatts_if,
                                       esp_ble_gatts_cb_param_t *param) {
    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(TAG, "REGISTER_APP_EVT, status %d, app_id %d",
                 param->reg.status, param->reg.app_id);
        
        heart_rate_profile_tab[PROFILE_APP_IDX].service_id.is_primary = true;
        heart_rate_profile_tab[PROFILE_APP_IDX].service_id.id.inst_id = 0x00;
        heart_rate_profile_tab[PROFILE_APP_IDX].service_id.id.uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_APP_IDX].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID;

        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(DEVICE_NAME);
        if (set_dev_name_ret) {
            ESP_LOGE(TAG, "set device name failed, error code = %x", set_dev_name_ret);
        }

        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret) {
            ESP_LOGE(TAG, "config adv data failed, error code = %x", ret);
        }
        adv_config_done |= ADV_CONFIG_FLAG;

        esp_ble_gatts_create_service(gatts_if, &heart_rate_profile_tab[PROFILE_APP_IDX].service_id, GATTS_NUM_HANDLE);
        break;
    }
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
        break;
    case ESP_GATTS_WRITE_EVT: {
        if (!param->write.is_prep) {
            ESP_LOGI(TAG, "✓ Nhận được data, len = %d", param->write.len);
            
            memset(received_data, 0, sizeof(received_data));
            
            // Giới hạn độ dài để tránh overflow
            int copy_len = (param->write.len < sizeof(received_data) - 1) 
                          ? param->write.len 
                          : sizeof(received_data) - 1;
            
            memcpy(received_data, param->write.value, copy_len);
            received_data[copy_len] = '\0';
            
            ESP_LOGI(TAG, "📨 Tin nhắn: %s", received_data);
            
            // Gửi response confirmation
            if (param->write.need_rsp) {
                esp_ble_gatts_send_response(
                    gatts_if,
                    param->write.conn_id,
                    param->write.trans_id,
                    ESP_GATT_OK,
                    NULL
                );
            }
            
            // Đợi 100ms trước khi gửi notification
            vTaskDelay(100 / portTICK_PERIOD_MS);
            
            // Xử lý tin nhắn và tạo response
            char response[512];
            process_message(received_data, response, sizeof(response));
            
            ESP_LOGI(TAG, "📤 Phản hồi: %s", response);
            
            // Gửi lại Flutter
            send_response(response);
        }
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_CONF_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGI(TAG, "✓ Thiết bị đã kết nối!");
        ESP_LOGI(TAG, "   conn_id = %d", param->connect.conn_id);
        
        conn_id_store = param->connect.conn_id;
        gatts_if_store = gatts_if;
        
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;
        
        esp_ble_gap_update_conn_params(&conn_params);
        
        // Test gửi tin nhắn chào mừng
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        send_response("ESP32-C3: Ket noi thanh cong!");
        
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "✗ Thiết bị ngắt kết nối, reason = 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        conn_id_store = 0xFFFF;
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != GATTS_NUM_HANDLE) {
            ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) doesn't equal to GATTS_NUM_HANDLE(%d)",
                     param->add_attr_tab.num_handle, GATTS_NUM_HANDLE);
        } else {
            ESP_LOGI(TAG, "✓ Attribute table created, num_handle = %d", param->add_attr_tab.num_handle);
        }
        break;
    }
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(TAG, "CREATE_SERVICE_EVT, status %d, service_handle %d",
                 param->create.status, param->create.service_handle);
        
        heart_rate_profile_tab[PROFILE_APP_IDX].service_handle = param->create.service_handle;
        heart_rate_profile_tab[PROFILE_APP_IDX].char_uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_APP_IDX].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_RX;

        esp_ble_gatts_start_service(heart_rate_profile_tab[PROFILE_APP_IDX].service_handle);

        esp_err_t add_char_ret = esp_ble_gatts_add_char(
            heart_rate_profile_tab[PROFILE_APP_IDX].service_handle,
            &heart_rate_profile_tab[PROFILE_APP_IDX].char_uuid,
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
            NULL, NULL);
        
        if (add_char_ret) {
            ESP_LOGE(TAG, "add char failed, error code =%x", add_char_ret);
        }
        break;
    case ESP_GATTS_ADD_CHAR_EVT: {
        ESP_LOGI(TAG, "ADD_CHAR_EVT, status %d, attr_handle %d, service_handle %d",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        
        if (param->add_char.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Add characteristic failed, status = %d", param->add_char.status);
            break;
        }
        
        if (heart_rate_profile_tab[PROFILE_APP_IDX].char_handle_rx == 0) {
            // Đây là RX characteristic
            heart_rate_profile_tab[PROFILE_APP_IDX].char_handle_rx = param->add_char.attr_handle;
            
            ESP_LOGI(TAG, "✓ RX Characteristic added, handle = %d", param->add_char.attr_handle);
            
            // Thêm TX characteristic
            esp_bt_uuid_t char_uuid_tx;
            char_uuid_tx.len = ESP_UUID_LEN_16;
            char_uuid_tx.uuid.uuid16 = GATTS_CHAR_UUID_TX;
            
            esp_err_t add_char_ret = esp_ble_gatts_add_char(
                heart_rate_profile_tab[PROFILE_APP_IDX].service_handle,
                &char_uuid_tx,
                ESP_GATT_PERM_READ,
                ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
                NULL, NULL);
                
            if (add_char_ret) {
                ESP_LOGE(TAG, "Add TX char failed, error = %x", add_char_ret);
            }
        } else {
            // Đây là TX characteristic
            heart_rate_profile_tab[PROFILE_APP_IDX].char_handle_tx = param->add_char.attr_handle;
            char_handle_tx = param->add_char.attr_handle;
            
            ESP_LOGI(TAG, "✓ TX Characteristic added, handle = %d", param->add_char.attr_handle);
            
            // Thêm Client Characteristic Configuration Descriptor (CCCD)
            esp_bt_uuid_t descr_uuid;
            descr_uuid.len = ESP_UUID_LEN_16;
            descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
            
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(
                heart_rate_profile_tab[PROFILE_APP_IDX].service_handle,
                &descr_uuid,
                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                NULL, NULL);
                
            if (add_descr_ret) {
                ESP_LOGE(TAG, "Add descriptor failed, error = %x", add_descr_ret);
            } else {
                ESP_LOGI(TAG, "✓ CCCD Descriptor added");
            }
        }
        break;
    }
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGI(TAG, "reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE ||
                gatts_if == heart_rate_profile_tab[idx].gatts_if) {
                if (heart_rate_profile_tab[idx].gatts_cb) {
                    heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

static void wifi_event_handler(void* arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void* event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            printf("WiFi started\n");
            send_response("WiFi started");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            printf("WiFi connected to AP\n");
            send_response("WiFi connected to AP");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            printf("WiFi disconnected\n");
            wifi_event_sta_disconnected_t* dis =
                (wifi_event_sta_disconnected_t*)event_data;
            if (dis->reason == WIFI_REASON_AUTH_FAIL)
            {
                send_response("WiFi disconnected WIFI_WRONG_PASS");
            }
            else if (dis->reason == WIFI_REASON_NO_AP_FOUND)
            {
                send_response("WiFi disconnected WIFI_NO_AP");
            }
            else{
                send_response("WiFi disconnected ");
            }
            break;
        }
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        // ==> KẾT NỐI WIFI THÀNH CÔNG
        send_response("WiFi connected successfully");
    }
}

void wifi_init_sta(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        &instance_any_id);

    esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        &instance_got_ip);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}

void app_main(void) {
    esp_err_t ret;

    // Khởi tạo NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "╔═══════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   ESP32-C3 BLE SERVER STARTING...    ║");
    ESP_LOGI(TAG, "╚═══════════════════════════════════════╝");

    // Khởi tạo Bluetooth (chỉ BLE cho ESP32-C3)
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(PROFILE_APP_IDX);
    if (ret) {
        ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret) {
        ESP_LOGE(TAG, "set local MTU failed, error code = %x", local_mtu_ret);
    }
    wifi_init_sta();
    // Hiển thị QR Code
    print_qr_code();

    ESP_LOGI(TAG, "✓ ESP32-C3 BLE Server sẵn sàng!");
    ESP_LOGI(TAG, "✓ Đang chờ kết nối...");
}