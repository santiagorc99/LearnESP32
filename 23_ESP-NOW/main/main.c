#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define TAG                             "ESP-NOW"

#define ENABLE_KNOWN_LIST_EXAMPLE       (1)
#define ENABLE_AUTO_REGISTRY_EXAMPLE    (1)

#define PIN_SWITCH                      (0)


char *mac_to_str(char *buffer, uint8_t *mac)
{
  sprintf(buffer, MACSTR, MAC2STR(mac));
  return buffer;
}

static void on_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char mac_string[13];
    switch (status) {
        case ESP_NOW_SEND_SUCCESS:
            ESP_LOGI(TAG, "Message sent to %s OK", mac_to_str(mac_string, (uint8_t *) mac_addr));
            break;

        case ESP_NOW_SEND_FAIL:
            ESP_LOGE(TAG, "Message sent to %s NOK", mac_to_str(mac_string, (uint8_t *) mac_addr));
            break;
        
        default:
            break;
    }
}

#if ENABLE_AUTO_REGISTRY_EXAMPLE
typedef enum message_type_ {
    BROADCAST,
    SEND_MESSAGE,

} message_type_t;

typedef struct payload_ {
    char message[100];
    message_type_t type;

} payload_t;

typedef struct payload_extended_ {
    payload_t payload;
    uint8_t from_mac[ESP_NOW_ETH_ALEN];

} payload_extended_t;

static QueueHandle_t rx_queue;
static SemaphoreHandle_t button_semaphore;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  xSemaphoreGiveFromISR(button_semaphore, pdFALSE);
}

static void bsp_init(void)
{
    gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIN_SWITCH);
    gpio_pullup_dis(PIN_SWITCH);
    gpio_set_intr_type(PIN_SWITCH, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_SWITCH, gpio_isr_handler, (void *)PIN_SWITCH);
}

/** Make sure to free this routine ASAP since it's a high priority task handled by the system */
static void on_received(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
    printf("Data len is %d\n", data_len);
    if (data_len != sizeof(payload_t)) {
        ESP_LOGE(TAG, "Received an incorrect payload");
        return;
    }

    payload_extended_t payload_extented;
    payload_t *payload = (payload_t *)data;
    payload_extented.payload = *payload;
    memcpy(payload_extented.from_mac, mac_addr, ESP_NOW_ETH_ALEN);
    xQueueSend(rx_queue, &payload_extented, 0);

}

void broadcast_cb(TimerHandle_t xTimer)
{
    uint8_t broadcast_address[ESP_NOW_ETH_ALEN];
    memset(broadcast_address, 0xFF, ESP_NOW_ETH_ALEN);

    payload_t payload = {
        .message = "Ping",
        .type = BROADCAST,
    };
    ESP_ERROR_CHECK(esp_now_send(broadcast_address, (uint8_t *) &payload, sizeof(payload_t)));
}

void message_received_task(void *params)
{
    rx_queue = xQueueCreate(10, sizeof(payload_extended_t));
    payload_extended_t payload_extended;
    char mac_string[13];
    while (true)
    {
       xQueueReceive(rx_queue, &payload_extended, portMAX_DELAY);
       switch (payload_extended.payload.type)
       {
            case BROADCAST:
                if (!esp_now_is_peer_exist(payload_extended.from_mac)) 
                {
                    ESP_LOGI(TAG, "Got message from %s ... Registering new MAC", mac_to_str(mac_string, payload_extended.from_mac));
                    esp_now_peer_info_t esp_now_peer_info;
                    memset(&esp_now_peer_info, 0, sizeof(esp_now_peer_info_t));
                    memcpy(esp_now_peer_info.peer_addr, payload_extended.from_mac, ESP_NOW_ETH_ALEN);
#define USER_ENCRYPTION
#ifdef USER_ENCRYPTION
                    memcpy(esp_now_peer_info.lmk,"thepencilofester", ESP_NOW_KEY_LEN);
                    esp_now_peer_info.encrypt = true;
#endif /* USER_ENCRYPTION*/

                    ESP_ERROR_CHECK(esp_now_add_peer(&esp_now_peer_info));
                    ESP_LOGI(TAG, "Registered");
                } else {
                    ESP_LOGI(TAG, "Got message from %s (Already registered)\n", mac_to_str(mac_string, payload_extended.from_mac));
                }
                break;

            case SEND_MESSAGE:
                ESP_LOGI(TAG, "Got message: %s from %s", payload_extended.payload.message, mac_to_str(mac_string, payload_extended.from_mac));
                break;

            default:
                break;
       }
    }
}

void send_message_task(void *params)
{
    button_semaphore = xSemaphoreCreateBinary();
    while (true)
    {
        xSemaphoreTake(button_semaphore, portMAX_DELAY);
        esp_now_peer_num_t peer_num;
        esp_now_get_peer_num(&peer_num);
        if (peer_num.total_num <= 1)
        {
            ESP_LOGW(TAG, "no peers yet");
            continue;
        }
        payload_t payload = {
            .type = SEND_MESSAGE,
            .message = "BUTTON CLICKED"
        };
        ESP_ERROR_CHECK(esp_now_send(NULL, (uint8_t *)&payload, sizeof(payload_t)));
  }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    bsp_init();

    /** Init netwroking */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_received));

    /** 0xFF broadcasts to every one */
    esp_now_peer_info_t esp_now_peer_info;
    memset(&esp_now_peer_info, 0, sizeof(esp_now_peer_info_t));
    memset(esp_now_peer_info.peer_addr, 0xFF, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(&esp_now_peer_info));

    /** We want to broadcast periodically to any peer listening */
    TimerHandle_t timer_handle = xTimerCreate("Broadcast", pdMS_TO_TICKS(10000), pdTRUE, NULL, broadcast_cb);
    xTimerStart(timer_handle, 0);

    xTaskCreate(message_received_task, "message_received_task", 1024*2, NULL, 5, NULL);
    xTaskCreate(send_message_task, "send_message_task", 1024 * 2, NULL, 5, NULL);
}

#elif ENABLE_KNOWN_LIST_EXAMPLE

/** Our MACs are: 
 * 1. Main device:      7C:9E:BD:47:C3:44.
 * 2. Secondary device: 84:CC:A8:2C:13:5C.
 * 
 * We register all the peer address based on previous information we already know.
*/

uint8_t esp_1_mac[ESP_NOW_ETH_ALEN] = {0x7C,0x9E,0xBD,0x47,0xC3,0x44};
uint8_t esp_2_mac[ESP_NOW_ETH_ALEN] = {0x84,0xCC,0xA8,0x2C,0x13,0x5C};

static void on_received(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
    char mac_string[13];
    ESP_LOGI(TAG, "Got message from %s", mac_to_str(mac_string, (uint8_t *) mac_addr));

    printf("Message is: %.*s\n", data_len, data);
}

void app_main(void)
{
    uint8_t my_mac[ESP_NOW_ETH_ALEN];
    char my_mac_str[13];
    esp_efuse_mac_get_default(my_mac);
    ESP_LOGI(TAG, "My MAC address is: %s", mac_to_str(my_mac_str, my_mac));

    bool is_current_esp1 = (memcmp(my_mac, esp_1_mac, 6) == 0);
    uint8_t *peer_mac = (is_current_esp1) ? esp_2_mac : esp_1_mac;

    ESP_ERROR_CHECK(nvs_flash_init());

    /** Init netwroking */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_received));

    esp_now_peer_info_t esp_now_peer_info;
    memset(&esp_now_peer_info, 0, sizeof(esp_now_peer_info_t));
    memcpy(esp_now_peer_info.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(&esp_now_peer_info));

    char send_buffer[250];
    for (size_t id = 0; id < 200; id++) 
    {
        sprintf(send_buffer, "Hellow from %s. Message %d ID", my_mac_str, id);
        ESP_ERROR_CHECK(esp_now_send(NULL, (uint8_t *) send_buffer, strlen(send_buffer)));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_ERROR_CHECK(esp_now_deinit());
    ESP_ERROR_CHECK(esp_wifi_stop());
}

#endif 
