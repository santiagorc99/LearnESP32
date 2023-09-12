#include "../main.h"
#include "semaphore_example.h"
#include "freertos/semphr.h"

#define QUEUE_LEN           (3)
#define QUEUE_ELEMENT_SIZE  (sizeof(int))

xQueueHandle queue;

void listen_for_HTTP(void *params)
{
    int count = 0;
    while (true) {
        count++;
        printf("Received HTTP message\n");
        long ok = xQueueSend(queue, &count, 10/portTICK_PERIOD_MS);
        printf("Message %s to queue\n", (ok) ? "added" : "not added");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task1(void *params)
{
    int count = 0;
    while (true) {
        if (xQueueReceive(queue, &count, 5000/portTICK_PERIOD_MS)) {
            printf("Do something with the HTTP message [%d]\n", count);
        }
    }
}

void run_queue_example(void)
{
    queue = xQueueCreate(QUEUE_LEN, QUEUE_ELEMENT_SIZE);
    xTaskCreate(&listen_for_HTTP, "Get HTTP", 2048, NULL, 1, NULL);
    xTaskCreate(&task1, "Do something with HTTP", 2048, NULL, 2, NULL);
}