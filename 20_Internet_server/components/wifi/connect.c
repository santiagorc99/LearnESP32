#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#define TAG                     "WIFI"

#define WIFI_EVT_CONNECTED_GOT_IP       (BIT0)
#define WIFI_EVT_DISCONNECTED           (BIT1)

static esp_netif_t *esp_netif;
static EventGroupHandle_t wifi_events;

const char *get_wifi_error(uint8_t code)
{
    switch (code)
    {
        case WIFI_REASON_UNSPECIFIED                       : return "WIFI_REASON_UNSPECIFIED                       ";
        case WIFI_REASON_AUTH_EXPIRE                       : return "WIFI_REASON_AUTH_EXPIRE                       ";
        case WIFI_REASON_AUTH_LEAVE                        : return "WIFI_REASON_AUTH_LEAVE                        ";
        case WIFI_REASON_ASSOC_EXPIRE                      : return "WIFI_REASON_ASSOC_EXPIRE                      ";
        case WIFI_REASON_ASSOC_TOOMANY                     : return "WIFI_REASON_ASSOC_TOOMANY                     ";
        case WIFI_REASON_NOT_AUTHED                        : return "WIFI_REASON_NOT_AUTHED                        ";
        case WIFI_REASON_NOT_ASSOCED                       : return "WIFI_REASON_NOT_ASSOCED                       ";
        case WIFI_REASON_ASSOC_LEAVE                       : return "WIFI_REASON_ASSOC_LEAVE                       ";
        case WIFI_REASON_ASSOC_NOT_AUTHED                  : return "WIFI_REASON_ASSOC_NOT_AUTHED                  ";
        case WIFI_REASON_DISASSOC_PWRCAP_BAD               : return "WIFI_REASON_DISASSOC_PWRCAP_BAD               ";
        case WIFI_REASON_DISASSOC_SUPCHAN_BAD              : return "WIFI_REASON_DISASSOC_SUPCHAN_BAD              ";
        case WIFI_REASON_BSS_TRANSITION_DISASSOC           : return "WIFI_REASON_BSS_TRANSITION_DISASSOC           ";
        case WIFI_REASON_IE_INVALID                        : return "WIFI_REASON_IE_INVALID                        ";
        case WIFI_REASON_MIC_FAILURE                       : return "WIFI_REASON_MIC_FAILURE                       ";
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT            : return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT            ";
        case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT          : return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT          ";
        case WIFI_REASON_IE_IN_4WAY_DIFFERS                : return "WIFI_REASON_IE_IN_4WAY_DIFFERS                ";
        case WIFI_REASON_GROUP_CIPHER_INVALID              : return "WIFI_REASON_GROUP_CIPHER_INVALID              ";
        case WIFI_REASON_PAIRWISE_CIPHER_INVALID           : return "WIFI_REASON_PAIRWISE_CIPHER_INVALID           ";
        case WIFI_REASON_AKMP_INVALID                      : return "WIFI_REASON_AKMP_INVALID                      ";
        case WIFI_REASON_UNSUPP_RSN_IE_VERSION             : return "WIFI_REASON_UNSUPP_RSN_IE_VERSION             ";
        case WIFI_REASON_INVALID_RSN_IE_CAP                : return "WIFI_REASON_INVALID_RSN_IE_CAP                ";
        case WIFI_REASON_802_1X_AUTH_FAILED                : return "WIFI_REASON_802_1X_AUTH_FAILED                ";
        case WIFI_REASON_CIPHER_SUITE_REJECTED             : return "WIFI_REASON_CIPHER_SUITE_REJECTED             ";
        case WIFI_REASON_TDLS_PEER_UNREACHABLE             : return "WIFI_REASON_TDLS_PEER_UNREACHABLE             ";
        case WIFI_REASON_TDLS_UNSPECIFIED                  : return "WIFI_REASON_TDLS_UNSPECIFIED                  ";
        case WIFI_REASON_SSP_REQUESTED_DISASSOC            : return "WIFI_REASON_SSP_REQUESTED_DISASSOC            ";
        case WIFI_REASON_NO_SSP_ROAMING_AGREEMENT          : return "WIFI_REASON_NO_SSP_ROAMING_AGREEMENT          ";
        case WIFI_REASON_BAD_CIPHER_OR_AKM                 : return "WIFI_REASON_BAD_CIPHER_OR_AKM                 ";
        case WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION      : return "WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION      ";
        case WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS       : return "WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS       ";
        case WIFI_REASON_UNSPECIFIED_QOS                   : return "WIFI_REASON_UNSPECIFIED_QOS                   ";
        case WIFI_REASON_NOT_ENOUGH_BANDWIDTH              : return "WIFI_REASON_NOT_ENOUGH_BANDWIDTH              ";
        case WIFI_REASON_MISSING_ACKS                      : return "WIFI_REASON_MISSING_ACKS                      ";
        case WIFI_REASON_EXCEEDED_TXOP                     : return "WIFI_REASON_EXCEEDED_TXOP                     ";
        case WIFI_REASON_STA_LEAVING                       : return "WIFI_REASON_STA_LEAVING                       ";
        case WIFI_REASON_END_BA                            : return "WIFI_REASON_END_BA                            ";
        case WIFI_REASON_UNKNOWN_BA                        : return "WIFI_REASON_UNKNOWN_BA                        ";
        case WIFI_REASON_TIMEOUT                           : return "WIFI_REASON_TIMEOUT                           ";
        case WIFI_REASON_PEER_INITIATED                    : return "WIFI_REASON_PEER_INITIATED                    ";
        case WIFI_REASON_AP_INITIATED                      : return "WIFI_REASON_AP_INITIATED                      ";
        case WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT     : return "WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT     ";
        case WIFI_REASON_INVALID_PMKID                     : return "WIFI_REASON_INVALID_PMKID                     ";
        case WIFI_REASON_INVALID_MDE                       : return "WIFI_REASON_INVALID_MDE                       ";
        case WIFI_REASON_INVALID_FTE                       : return "WIFI_REASON_INVALID_FTE                       ";
        case WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED: return "WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED";
        case WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED       : return "WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED       ";
        case WIFI_REASON_BEACON_TIMEOUT                    : return "WIFI_REASON_BEACON_TIMEOUT                    ";
        case WIFI_REASON_NO_AP_FOUND                       : return "WIFI_REASON_NO_AP_FOUND                       ";
        case WIFI_REASON_AUTH_FAIL                         : return "WIFI_REASON_AUTH_FAIL                         ";
        case WIFI_REASON_ASSOC_FAIL                        : return "WIFI_REASON_ASSOC_FAIL                        ";
        case WIFI_REASON_HANDSHAKE_TIMEOUT                 : return "WIFI_REASON_HANDSHAKE_TIMEOUT                 ";
        case WIFI_REASON_CONNECTION_FAIL                   : return "WIFI_REASON_CONNECTION_FAIL                   ";
        case WIFI_REASON_AP_TSF_RESET                      : return "WIFI_REASON_AP_TSF_RESET                      ";
        case WIFI_REASON_ROAMING                           : return "WIFI_REASON_ROAMING                           ";
        case WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG      : return "WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG      ";
        case WIFI_REASON_SA_QUERY_TIMEOUT                  : return "WIFI_REASON_SA_QUERY_TIMEOUT                  ";
        default: return "WIFI_REASON_UNKNOWN";
    }
}

static void event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id)
    {
    /** Events for STA mode */
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "Connecting");

        /** 4. Connect phase */
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "Connected");
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = event_data;
        if (wifi_event_sta_disconnected->reason != WIFI_REASON_ASSOC_LEAVE) {
            /** Try to reconnect if user has not triggered disconnection */
            ESP_LOGE(TAG, "Disconnected: %s", get_wifi_error(wifi_event_sta_disconnected->reason));
            esp_wifi_connect();
            break;
        } 
        ESP_LOGI(TAG, "Disconnected");
        xEventGroupSetBits(wifi_events, WIFI_EVT_DISCONNECTED);
        break;
    }

    case IP_EVENT_STA_GOT_IP:
        /** 5. GOT IP */
        ESP_LOGI(TAG, "Got IP");
        xEventGroupSetBits(wifi_events, WIFI_EVT_CONNECTED_GOT_IP);
        break;

    /** Event for AP mode */
    case WIFI_EVENT_AP_START:
        ESP_LOGI(TAG, "AP started");
        break;

    case WIFI_EVENT_AP_STOP:
        ESP_LOGI(TAG, "AP stopped");
        break;

    case WIFI_EVENT_AP_STACONNECTED:
    {
        wifi_event_ap_staconnected_t *wifi_event_ap_stadisconnected = event_data;
        ESP_LOGI(TAG, "Device %s connected", wifi_event_ap_stadisconnected->mac);
        break;
    }
    default:
        break;
    }
    
}

void wifi_init(void)
{
    /** 1. Init Phase */
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
}

esp_err_t wifi_connect_sta(const char* ssid, const char *pswd, int timeout)
{
    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();
    /** 2. Configuration phase */
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *) wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *) wifi_config.sta.password, pswd, sizeof(wifi_config.sta.password) - 1);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    /** 3. Start phase */
    esp_wifi_start();

    EventBits_t result = xEventGroupWaitBits(wifi_events, WIFI_EVT_CONNECTED_GOT_IP | WIFI_EVT_DISCONNECTED, pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));

    if (result == WIFI_EVT_CONNECTED_GOT_IP) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

void wifi_connect_ap(const char *ssid, const char *pswd)
{
    esp_netif = esp_netif_create_default_wifi_ap();

    /** 2. Configuration phase */
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *) wifi_config.ap.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *) wifi_config.ap.password, pswd, sizeof(wifi_config.sta.password) - 1);
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.max_connection = 4;


    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);

    /** 3. Start phase */
    esp_wifi_start();
    
}

void wifi_disconnect(void)
{
    esp_wifi_disconnect();
    esp_wifi_stop();
}