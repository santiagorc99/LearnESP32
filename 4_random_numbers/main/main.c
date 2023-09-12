#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "math.h"

#define TAG     "DICE"

int dice_role(void)
{
    int random = esp_random();
    int positive_number = abs(random);
    return (positive_number%6 + 1);
}

void app_main(void)
{
    while (1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Random number %d", dice_role());
    }
}
