#include "push_button.h"
#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "app.h"

#include "cJSON.h"

#define BUTTON_PIN                  (0)

static xSemaphoreHandle button_semaphore;

static void IRAM_ATTR on_button_pushed(void *args)
{
    xSemaphoreGiveFromISR(button_semaphore, NULL);
}

static void button_push_task(void *params)
{
    while (true) {
        xSemaphoreTake(button_semaphore, portMAX_DELAY);

        cJSON *payload = cJSON_CreateObject();
        cJSON_AddBoolToObject(payload, "button_state", !gpio_get_level(BUTTON_PIN));
        char *message = cJSON_Print(payload);
        send_ws_message(message);

        cJSON_Delete(payload);
        free(message);
    }
}

void button_init(void)
{
    button_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(button_push_task, "button_push_task", 1024*2, NULL, 5, NULL);
    gpio_pad_select_gpio(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, on_button_pushed, NULL);
}
