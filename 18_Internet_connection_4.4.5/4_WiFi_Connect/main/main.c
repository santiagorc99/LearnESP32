#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#define TAG                                 "MAIN"

#define STA_SSID                        "TE-2"
#define STA_PASSWORD                    "555titoma1666"

#define AP_SSID                         "SANTI-SSID"
#define AP_PASSWORD                     "SANTI-PSWD"

static void wifi_connect_task(void *params)
{
    wifi_connect_ap(AP_SSID, AP_PASSWORD);

    for (size_t countdown = 20; countdown > 0; countdown--) {
        printf("Disconnecting AP in %d\n", countdown);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    wifi_disconnect();

    esp_err_t err = wifi_connect_sta(STA_SSID, STA_PASSWORD, 10000);
    if (err) {
        ESP_LOGE(TAG, "Failed to connect");
        vTaskDelete(NULL);
    }

    for (size_t countdown = 5; countdown > 0; countdown--) {
        printf("Disconnecting STA in %d\n", countdown);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    wifi_disconnect();
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    xTaskCreate(wifi_connect_task, "WiFi_connect", 1024*5, NULL, 5, NULL);
}
