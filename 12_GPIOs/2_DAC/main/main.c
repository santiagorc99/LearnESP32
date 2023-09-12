#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"   
#include "driver/dac.h"

#define USER_DAC_CHANNEL        (DAC_CHANNEL_1)
#define DAC_MAX_VALUE           (255)

void app_main(void)
{
    dac_output_enable(USER_DAC_CHANNEL);
    dac_output_voltage(USER_DAC_CHANNEL, 255);
    while (true) {
        for (int step = 0; step <= DAC_MAX_VALUE; step ++) {
            dac_output_voltage(USER_DAC_CHANNEL, step);
            vTaskDelay(10 / portTICK_RATE_MS);
        }

        for (int step = DAC_MAX_VALUE; step >= 0; step --) {
            dac_output_voltage(USER_DAC_CHANNEL, step);
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    
}
