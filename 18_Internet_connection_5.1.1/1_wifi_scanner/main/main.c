#include <stdio.h>
#include "esp_wifi.h"
#include "nvs_flash.h"

#define MAX_APs                     (20)

char *get_authentication_mode_name(wifi_auth_mode_t auth_mode)
{
    static char *name[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", 
                                    "WPA3 PSK", "WPA2 WPA3 PSK", "WAPI PSK", "OWE", "MAX"};
    return name[auth_mode];
}

void wifi_scan(void)
{
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_scan_config_t wifi_scan_config = {
        .show_hidden = true,   
    };
    int error_code = esp_wifi_scan_start(&wifi_scan_config, true);
    printf("Error code is %s\n", esp_err_to_name(error_code));

    wifi_ap_record_t wifi_ap_records[MAX_APs];
    uint16_t wifi_max_records = MAX_APs;

    esp_wifi_scan_get_ap_records(&wifi_max_records, wifi_ap_records);

    printf("\n\nFound %d access points:\n", wifi_max_records);
    printf("\n");
    printf("              SSID               | Channel | RSSI |   Auth Mode \n");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < wifi_max_records; i++) {
        printf(" %31s | %7d | %4d | %12s\n", 
                                            (char *)wifi_ap_records[i].ssid, 
                                            wifi_ap_records[i].primary, 
                                            wifi_ap_records[i].rssi,
                                            get_authentication_mode_name(wifi_ap_records[i].authmode));
    }
    printf("---------------------------------------------------------------\n");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    
    wifi_scan();
}
