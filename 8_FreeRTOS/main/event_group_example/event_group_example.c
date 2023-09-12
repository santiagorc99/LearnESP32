#include "../main.h"
#include "semaphore_example.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#define EVENT_GOT_HTTP          (BIT0)
#define EVENT_GOT_BL            (BIT1)

EventGroupHandle_t event_group;

void listen_for_HTTP(void *params)
{
    while (true) {
        printf("Received HTTP message\n");
        xEventGroupSetBits(event_group, EVENT_GOT_HTTP);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void listen_for_Bluetooth(void *params)
{
    while (true) {
        printf("Received BLE message\n");
        xEventGroupSetBits(event_group, EVENT_GOT_BL);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void task1(void *params)
{
    int count = 0;
    while (true) {
        xEventGroupWaitBits(event_group, EVENT_GOT_HTTP | EVENT_GOT_BL, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("Receive Bluetooth and HTTP\n");
    }
}

void run_event_group_example(void)
{
    event_group = xEventGroupCreate();
    xTaskCreate(&listen_for_HTTP, "Get HTTP", 2048, NULL, 1, NULL);
    xTaskCreate(&listen_for_Bluetooth, "Get BL", 2048, NULL, 1, NULL);
    xTaskCreate(&task1, "Do something with HTTP and BL", 2048, NULL, 1, NULL);
}