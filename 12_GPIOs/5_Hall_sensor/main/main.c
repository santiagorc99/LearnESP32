#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

uint32_t hall_sens_read(void);

void app_main(void)
{
    while (true) {
        printf("Hall sensor: %d\n", hall_sens_read());
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
