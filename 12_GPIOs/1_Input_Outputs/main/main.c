#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"   

#define LED_PIN         (2)
#define BUTTON_PIN      (0)

#include "freertos/queue.h"

QueueHandle_t interrupt_queue;

static void IRAM_ATTR gpio_isr_handle(void *args)
{
    int pin_number = (int) args;    
    /* this is done to return to the main code */
    xQueueSendFromISR(interrupt_queue, &pin_number, NULL);
}

void button_pushed_task(void *args)
{
    int pin_number;
    int count = 0;
    int button_pressed = 0;
    while (true) {
        if (xQueueReceive(interrupt_queue, &pin_number, portMAX_DELAY)) {
            /** This code also handles debouncing */

            /** Disable the interrupt */
            gpio_isr_handler_remove(pin_number);

            /** Debounce */
            do {
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(20));
            } while (gpio_get_level(pin_number) == 0);
            
            /** Process the information */
            button_pressed = gpio_get_level(pin_number) ? 0 : 1;
            printf("GPIO %d was pressed %d times. The button state is %s\n", 
                                                                        pin_number,
                                                                        ++count,
                                                                        button_pressed ? "PRESSED" : "RELEASED");
            gpio_set_level(LED_PIN, button_pressed);

            /** Enable the interrupt*/
            gpio_isr_handler_add(pin_number, gpio_isr_handle, (void *) pin_number);
        }

    }
}

void app_main(void)
{
// #define MANUAL_CONFIG
#ifdef MANUAL_CONFIG
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);
#else
    gpio_config_t config = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT0, // OR all the needed input pins
    };
    gpio_config(&config);

    config.intr_type = GPIO_INTR_DISABLE;
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = BIT2; // OR all the needed output pins
    gpio_config(&config);


#endif /* MANUAL_CONFIG */

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handle, (void *) BUTTON_PIN);

    interrupt_queue = xQueueCreate(10, sizeof(int));
    xTaskCreate(button_pushed_task, "Button pushed task", 2048, NULL, 1, NULL);


}

