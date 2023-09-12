#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_err.h"

#define TAG                 "MQTT"

#define STA_TEST_SSID       "santi"
#define STA_TEST_PSWD       "555titoma1666"
#define STA_CONN_TIMEOUT    (10000)

static esp_mqtt_client_handle_t esp_mqtt_client_handle = NULL;

static void test_send_messages_task(void *params);
static int mqtt_send(const char *topic, char *payload, bool retain);
static void on_mqtt_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta(STA_TEST_SSID, STA_TEST_PSWD, STA_CONN_TIMEOUT));

#define TEST_LAST_WILL
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        /** You can find this the using mqtt explorer */
        /** 1883 is insecure, not using TLS, 8883 is secure */
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io:1883",

#ifdef TEST_LAST_WILL
        /** To configure the last will */
        /** 
         * esp_mqtt_client_stop does not trigger the last will for it actually closes mqtt correctly.
        */
        .session.last_will = {
            .topic = "animal/on-chip-death",
            .msg = "I died ☠️",
            .msg_len = strlen("I died ☠️"),
            .qos = 1,
            .retain = 1,
        }};
#endif /* TEST_LAST_WILL */
    };
    esp_mqtt_client_handle = esp_mqtt_client_init(&esp_mqtt_client_config);
    esp_mqtt_client_register_event(esp_mqtt_client_handle, MQTT_EVENT_ANY,  on_mqtt_event, NULL);
    esp_mqtt_client_start(esp_mqtt_client_handle);

// #define TEST_PERIODIC_PUBLISH
#ifdef TEST_PERIODIC_PUBLISH
    xTaskCreate(test_send_messages_task, "test_send_messages_task", 1024*2, NULL, 5, NULL);
#else 
    /** Test retain */
    vTaskDelay(pdMS_TO_TICKS(3000));
    mqtt_send("animal/a-message", "A message", true);

#endif /* TEST_PERIODIC_PUBLISH */
}

void on_mqtt_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    esp_mqtt_event_handle_t mqtt_event_data = event_data;

    switch ((esp_mqtt_event_id_t) event_id) {
        /** Connected to the broker */
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            /** This topics are modified using mqtt explorer */
            esp_mqtt_client_subscribe(esp_mqtt_client_handle, "animal/mammal/cat/felix", 1);
            esp_mqtt_client_subscribe(esp_mqtt_client_handle, "animal/reptiles/+/slither", 1);
            esp_mqtt_client_subscribe(esp_mqtt_client_handle, "animal/fish/#", 1);
            esp_mqtt_client_subscribe(esp_mqtt_client_handle, "animal/configuration", 1);
            break;

        /** Disconnected from the broker */
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("topic: %.*s. Message %.*s\n", 
                                                mqtt_event_data->topic_len, 
                                                mqtt_event_data->topic,
                                                mqtt_event_data->data_len,
                                                mqtt_event_data->data);

            if (strncmp(mqtt_event_data->topic, "animal/configuration", strlen("animal/configuration")) == 0) {
                printf("GOT CONFIG DATA: %.*s\n", mqtt_event_data->data_len, mqtt_event_data->data);
            }

            break;
        
        /** Any error in MQTT*/
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "ERROR %s", strerror(mqtt_event_data->error_handle->esp_transport_sock_errno));
            break;

        default:
            break;
    }
}

int mqtt_send(const char *topic, char *payload, bool retain)
{
    /** Use retain to configure things */
    return esp_mqtt_client_publish(esp_mqtt_client_handle, topic, payload, strlen(payload), 1, retain);
}

void test_send_messages_task(void *params)
{
    char message[50];
    int count = 0;
    while (true) {
        sprintf(message, "Hello from ESP32 count %d", count++);
        mqtt_send("animal/message/from/esp32", message, false);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
