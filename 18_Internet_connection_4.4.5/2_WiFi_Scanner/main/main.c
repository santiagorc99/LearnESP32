#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define MAX_APs             (20)

static esp_err_t event_handler(void * p_context, system_event_t *event)
{
    return ESP_OK;
}

void wifi_init(void)
{
    /** Init flash for all the WiFi configuration is saved to flash */
    ESP_ERROR_CHECK(nvs_flash_init());

    /** Init the TCPIP adapter */
    tcpip_adapter_init();

    /** Create this by default */
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    /** Init the WiFi configuration as a station to scan different networks */
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

char *get_authentication_mode_name(wifi_auth_mode_t auth_mode)
{
    static char *name[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", 
                                    "WPA3 PSK", "WPA2 WPA3 PSK", "WAPI PSK", "MAX"};
    return name[auth_mode];
}

void app_main(void)
{
    wifi_init();

    wifi_scan_config_t scan_config = {
        /** All set to 0 look for anything, not just an specific network and blocks the program */
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true,
    };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    uint16_t wifi_max_records = MAX_APs;
    wifi_ap_record_t wifi_records[wifi_max_records];
    /** Returns the networks and the max records found (<= max)*/
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&wifi_max_records, wifi_records));

    printf("\n\nFound %d access points:\n", wifi_max_records);
    printf("\n");
    printf("              SSID               | Channel | RSSI |   Auth Mode \n");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < wifi_max_records; i++) {
        printf(" %31s | %7d | %4d | %12s\n", 
                                            (char *)wifi_records[i].ssid, 
                                            wifi_records[i].primary, 
                                            wifi_records[i].rssi,
                                            get_authentication_mode_name(wifi_records[i].authmode));
    }
    printf("---------------------------------------------------------------\n");
}
