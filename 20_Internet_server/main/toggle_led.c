#include "toggle_led.h"
#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"

#define LED_PIN                 (2)

void led_init(void)
{
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void led_toggle(bool is_on)
{
    gpio_set_level(LED_PIN, is_on);    
}
