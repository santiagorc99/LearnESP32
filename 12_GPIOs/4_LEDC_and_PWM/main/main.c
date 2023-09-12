#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

/** LEDC is used to control the intensity of the LEDs, however the PWM can be usedd for multiple purposes. This is how it works:
 * We use 2 structures, one for setting up our timer (Slow work on SW, fast work on HW) . The other structure is the channel, 
 * different timers can drive different channels on different pins.
 * 
*/

#define LED_PIN_NUMBER                  (2)

void app_main(void)
{
    ledc_timer_config_t led_timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&led_timer_config);

    ledc_channel_config_t led_channel_config = {
        .gpio_num = LED_PIN_NUMBER,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&led_channel_config);

    /** Call this to use the ledc_set_duty_and_update routine */
    ledc_fade_func_install(0);

    for (int increasing_duty = 0; increasing_duty < 1024/3; increasing_duty++) {
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, increasing_duty*3, 0);
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    for (int decreasing_duty = 1024/3; decreasing_duty > 0; decreasing_duty--) {
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, decreasing_duty*3, 0);
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    while (true) {
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024, 300, LEDC_FADE_WAIT_DONE);
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 300, LEDC_FADE_WAIT_DONE);
    }
}
