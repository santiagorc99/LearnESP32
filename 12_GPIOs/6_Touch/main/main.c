#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"

void app_main(void)
{
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_NUM4, 0);

    uint16_t touch_value;
    while (true) {
        touch_pad_read(TOUCH_PAD_NUM4, &touch_value);
        printf("Value is %d\n", touch_value);
        vTaskDelay(300/ portTICK_RATE_MS);
    }


}
