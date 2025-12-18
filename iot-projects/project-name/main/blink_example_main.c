//     float current = (v_pp * 0.3535) / SENSITIVITY;

//     // Ap dung he so hieu chinh
//     current = current * CALIB_FACTOR; 

//     // Loc nhieu: cat dong dien nho (duoi 0.06A)
//     if (current < 0.06) {
//         current = 0.0;
//     }
//     return current;
// }

// // --- CHUONG TRINH CHINH ---
// void app_main(void) {
//     // Khoi tao NVS (van can cho mot so thu vien co ban cua ESP-IDF, mac du khong dung de luu tru)
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     // 1. Cau hinh ADC
//     adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT };
//     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));
//     adc_oneshot_chan_cfg_t config = { .bitwidth = ADC_BITWIDTH_DEFAULT, .atten = ADC_ATTEN };
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));
//     do_calibration = adc_calibration_init(ADC_UNIT, ADC_CHANNEL, ADC_ATTEN, &adc_cali_handle);

//     ESP_LOGI(TAG, "Measurement System Initialized. Starting loop...");

//     while (1) {
//         float I = get_current_rms(); // Dong dien RMS (Ampe)
//         // Uoc tinh Cong suat (Watt) voi dien ap luoi 220V
//         float P_watt = I * 220.0;
        
//         // Log ra Serial Monitor
//         if (I == 0.0) {
//             ESP_LOGI(TAG, "--- Load: OFF --- | Current: %.3f A | Power: 0.0 W", I);
//         } else {
//             ESP_LOGI(TAG, "--- Load: ON  --- | Current: %.3f A | Power: %.1f W", I, P_watt);
//         }

//         vTaskDelay(pdMS_TO_TICKS(2000)); // Do tre giua cac lan do (2 giay)
//     }

//     // Cleanup (khong bao gio chay toi)
//     if (do_calibration) adc_cali_delete_scheme_curve_fitting(adc_cali_handle);
//     adc_oneshot_del_unit(adc_handle);
// }



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// --- CAU HINH PHAN CUNG ADC (ACS712) ---
#define ADC_UNIT            ADC_UNIT_1
#define ADC_CHANNEL         ADC_CHANNEL_2       // GPIO 2
#define ADC_ATTEN           ADC_ATTEN_DB_12
#define SENSITIVITY         185.0               // Do nhay cho ACS712-5A (185 mV/A)
#define CALIB_FACTOR        1.0                 // He so hieu chinh (1.0 = khong hieu chinh)
#define ZERO_CURRENT_MV     2500                // Dien ap OUT khi I=0A (2.5V)

static const char *TAG = "ACS712";
// Bien toan cuc
adc_oneshot_unit_handle_t adc_handle = NULL;
adc_cali_handle_t adc_cali_handle = NULL;
bool do_calibration = false;

// --- HAM ADC VA HIEU CHUAN ---
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle) {
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit, 
        .chan = channel, 
        .atten = atten, 
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
        calibrated = true;
        *out_handle = handle;
    }
    return calibrated;
}

/**
 * @brief Do dong dien RMS voi ACS712 - TRU OFFSET 2.5V
 * @param raw_out: Tra ve gia tri ADC raw trung binh
 * @param voltage_out: Tra ve dien ap (mV) trung binh
 * @param vmax_out: Tra ve dien ap MAX (mV)
 * @param vmin_out: Tra ve dien ap MIN (mV)
 * @return Dong dien RMS (Ampere)
 */
float get_current_rms(int *raw_out, int *voltage_out, int *vmax_out, int *vmin_out) {
    int voltage_raw = 0;
    int voltage_mv = 0;
    int max_mv = 0;
    int min_mv = 5000; // Khoi tao lon de dam bao cap nhat
    
    int sum_raw = 0;
    int sum_mv = 0;
    int count = 0;
    
    uint32_t start_tick = xTaskGetTickCount();
    
    // Do trong 100ms (khoang 5 chu ky AC 50Hz)
    while ((xTaskGetTickCount() - start_tick) < pdMS_TO_TICKS(100)) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &voltage_raw));
        
        if (do_calibration) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, voltage_raw, &voltage_mv));
        } else {
            voltage_mv = voltage_raw * 3300 / 4095;
        }

        // Cap nhat min/max
        if (voltage_mv > max_mv) max_mv = voltage_mv;
        if (voltage_mv < min_mv) min_mv = voltage_mv;
        
        // Tinh trung binh
        sum_raw += voltage_raw;
        sum_mv += voltage_mv;
        count++;
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    // Gia tri trung binh
    *raw_out = (count > 0) ? (sum_raw / count) : 0;
    *voltage_out = (count > 0) ? (sum_mv / count) : 0;
    *vmax_out = max_mv;
    *vmin_out = min_mv;

    // Kiem tra loi doc ADC
    if (count == 0 || max_mv == 0) {
        return 0.0;
    }

    // *** PHAN QUAN TRONG: TRU OFFSET 2.5V ***
    // Tinh V_peak-to-peak (mV) - DA TRU OFFSET
    float v_pp = (float)(max_mv - min_mv);
    
    // Tinh V_peak (mV)
    float v_peak = v_pp / 2.0;
    
    // Tinh V_rms (mV)
    float v_rms = v_peak * 0.707; // 1/sqrt(2)
    
    // Tinh I_rms (A)
    float current = v_rms / SENSITIVITY;
    
    // Ap dung he so hieu chinh
    current = current * CALIB_FACTOR;
    
    // Loc nhieu - bo qua gia tri qua nho
    // Nguong 0.25A de tranh nhieu ESP32 ADC
    if (current < 0.25) {
        current = 0.0;
    }

    return current;
}

/**
 * @brief Lay dong dien RMS co loc trung binh de giam nhieu
 * @param raw_out: Tra ve gia tri ADC raw
 * @param voltage_out: Tra ve dien ap (mV)
 * @param vmax_out: Tra ve dien ap MAX (mV)
* @param vmin_out: Tra ve dien ap MIN (mV)
 * @return Dong dien RMS trung binh (Ampere)
 */
float get_filtered_current(int *raw_out, int *voltage_out, int *vmax_out, int *vmin_out) {
    float sum = 0.0;
    int samples = 3; // Lay 3 mau va tinh trung binh
    
    for (int i = 0; i < samples; i++) {
        sum += get_current_rms(raw_out, voltage_out, vmax_out, vmin_out);
        if (i < samples - 1) {
            vTaskDelay(pdMS_TO_TICKS(50)); // Delay 50ms giua cac mau
        }
    }
    
    return sum / samples;
}

// --- CHUONG TRINH CHINH ---
void app_main(void) {
    // Khoi tao NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 1. Cau hinh ADC
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));
    
    adc_oneshot_chan_cfg_t config = { 
        .bitwidth = ADC_BITWIDTH_DEFAULT, 
        .atten = ADC_ATTEN 
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));
    
    do_calibration = adc_calibration_init(ADC_UNIT, ADC_CHANNEL, ADC_ATTEN, &adc_cali_handle);

    ESP_LOGI(TAG, "System Init OK. Calibration: %s", do_calibration ? "YES" : "NO");

    // Vong lap do chinh
    while (1) {
        int raw = 0;
        int voltage = 0;
        int vmax = 0;
        int vmin = 0;
        
        // Su dung ham loc trung binh de giam nhieu
        float I = get_filtered_current(&raw, &voltage, &vmax, &vmin);
        
        // Tinh Vpp va cong suat
        int vpp = vmax - vmin;
        float P = I * 220.0; // Cong suat (W)
        
        // In log chi tiet
        if (I == 0.0) {
            ESP_LOGI(TAG, "raw=%d, V=%d mV (Vpp=%d mV) | I=%.3f A | P=0 W", 
                     raw, voltage, vpp, I);
        } else {
            ESP_LOGI(TAG, "raw=%d, V=%d mV (max=%d, min=%d, Vpp=%d mV) | I=%.3f A | P=%.1f W", 
                     raw, voltage, vmax, vmin, vpp, I, P);
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    // Cleanup
    if (do_calibration) {
        adc_cali_delete_scheme_curve_fitting(adc_cali_handle);
    }
    adc_oneshot_del_unit(adc_handle);
}