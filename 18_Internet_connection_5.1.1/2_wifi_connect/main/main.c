#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STA_TEST_SSID           "TE-2"
#define STA_TEST_PSWD           "555titoma1666"
#define STA_CONN_TIMEOUT        (10000)

/** Must be larger than 7 characters */
#define AP_TEST_SSID            "user-admin"
#define AP_TEST_PSWD            "user-admin"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_connect_init();

    while (true) {
        ESP_ERROR_CHECK(wifi_connect_sta(STA_TEST_SSID, STA_TEST_PSWD, STA_CONN_TIMEOUT));
        vTaskDelay(pdMS_TO_TICKS(10000));
        wifi_disconnect();

        wifi_connect_ap(AP_TEST_SSID, AP_TEST_PSWD);
        vTaskDelay(pdMS_TO_TICKS(10000));
        wifi_disconnect(); 
    }  
}
