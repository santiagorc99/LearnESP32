#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#define LED_PIN         (2)

void app_main(void)
{
    uint8_t pin_level = 0;
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        pin_level = !pin_level;
        gpio_set_level(LED_PIN, pin_level);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
