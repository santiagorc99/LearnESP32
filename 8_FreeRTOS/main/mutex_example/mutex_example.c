#include "../main.h"
#include "mutex_example.h"
#include "freertos/semphr.h"

xSemaphoreHandle mutex_bus;

void write_to_bus(char *message)
{
    printf("BUS: %s\n", message);
}

void task1(void *params)
{
    while (true) {
        printf("Reading temperature \n");
        if (xSemaphoreTake(mutex_bus, 1000 / portTICK_PERIOD_MS)) {
            write_to_bus("Temperature is 25c");
            xSemaphoreGive(mutex_bus);
        } else {
            printf("Reading temperature timed out\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2(void *params)
{
    while (true) {
        printf("Reading humidity \n");
        if (xSemaphoreTake(mutex_bus, 1000 / portTICK_PERIOD_MS)) {
            write_to_bus("Humidity is 50\%");
            xSemaphoreGive(mutex_bus);
        } else {
            printf("Reading humidity timed out\n");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void run_mutex_example(void)
{
    mutex_bus = xSemaphoreCreateMutex();
    xTaskCreate(&task1, "Task 1", 2048, NULL, 2, NULL);
    xTaskCreate(&task2, "Task 2", 2048, NULL, 2, NULL);
}