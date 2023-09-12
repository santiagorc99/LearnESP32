#include "task_notification_example.h"
#include "../main.h"

#define RUN_BASIC_EXAMPLE       1
#define RUN_ADVANCED_EXAMPLE    1

static TaskHandle_t receiver_handler = NULL;

void sender(void *params)
{
    while (true) {
#if RUN_ADVANCED_EXAMPLE
        xTaskNotify(receiver_handler, (1<<0), eSetBits);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xTaskNotify(receiver_handler, (1<<1), eSetBits);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xTaskNotify(receiver_handler, (1<<2), eSetBits);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xTaskNotify(receiver_handler, (1<<3), eSetBits);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

#elif RUN_BASIC_EXAMPLE
        xTaskNotifyGive(receiver_handler);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
#endif
    }
}

void receiver(void *params)
{
    int count = 0;
    uint state = 0;
    while (true) {
#if RUN_ADVANCED_EXAMPLE
        xTaskNotifyWait(0xFFFFFFFF, 0, &state, portMAX_DELAY);
        printf("Received state [%d]\n", state);

#elif RUN_BASIC_EXAMPLE
        /** Use true to check only once, false to count all the times it has been triggered */
        count = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        printf("Received notification [%d]\n", count);
#endif
    }
}

void run_example_notification(void)
{
    xTaskCreate(&receiver, "Receiver", 2048, NULL, 2, &receiver_handler);
    xTaskCreate(&sender, "Sender", 2048, NULL, 2, NULL);
}
