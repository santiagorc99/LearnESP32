#include <stdio.h>
#include <time.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_sntp.h"
#include "esp_log.h"

#define TAG                     "SNTP"

#define STA_TEST_SSID           "TE-2"
#define STA_TEST_PSWD           "555titoma1666"
#define STA_CONN_TIMEOUT        (10000)

static SemaphoreHandle_t got_time_semaphore;

void print_time(void)
{
    time_t now = 0;
    time(&now);

    struct tm *time_info = localtime(&now);
    char time_buffer[50];
    strftime(time_buffer, sizeof(time_buffer), "%c", time_info);
    ESP_LOGI(TAG,"************************ Time is: %s", time_buffer);
}

void on_got_time(struct timeval *tv)
{
    ESP_LOGI(TAG, "ON GOT CALLBACK %lld secs", tv->tv_sec);
    print_time();
    xSemaphoreGive(got_time_semaphore);
}

void app_main(void)
{
    got_time_semaphore = xSemaphoreCreateBinary();

    ESP_ERROR_CHECK(nvs_flash_init());
    
    /** Set the local zone, look for opnode time zone */
    setenv("TZ", "UTC+5", 1);
    tzset();

    ESP_LOGI(TAG,"Startup...");
    /** Print time before trying to connect */
    print_time();

    /** Connect */
    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta(STA_TEST_SSID, STA_TEST_PSWD, STA_CONN_TIMEOUT));

    /** Get SNTP */
    esp_sntp_init();
    esp_sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_set_time_sync_notification_cb(on_got_time);

    xSemaphoreTake(got_time_semaphore, portMAX_DELAY);
    ESP_LOGI(TAG, "Time was updated, code unlocked...");

    for (int iteration = 0; iteration < 5; iteration++)
    {
        print_time();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    /** SW restart does not effect the time of the MCU, hard reset does */
    esp_restart();
    
}
