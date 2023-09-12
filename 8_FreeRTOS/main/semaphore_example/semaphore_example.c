#include "../main.h"
#include "semaphore_example.h"
#include "freertos/semphr.h"


/** This is used to modify the sequence in which a process is executed */
xSemaphoreHandle binary_semaphore;

void listen_for_HTTP(void *params)
{
    while (true) {
        printf("Received HTTP message\n");
        xSemaphoreGive(binary_semaphore);
        printf("Processed HTTP message\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void task1(void *params)
{
    while (true) {
        xSemaphoreTake(binary_semaphore, portMAX_DELAY);
        printf("Do something with the HTTP message\n");
    }
}

void run_semaphore_example(void)
{
    binary_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(&listen_for_HTTP, "Get HTTP", 2048, NULL, 3, NULL);
    xTaskCreate(&task1, "Do something with HTTP", 2048, NULL, 3, NULL);
}