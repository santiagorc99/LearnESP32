#include "../main.h"
#include "RTOS_timer_example.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "driver/gpio.h"

#if USE_HIGH_RESOLUTION_TIMER

#define LED_PIN         (2)

void on_timer_cb(void *args)
{
    static bool on;
    on = !on;
    gpio_set_level(LED_PIN, on);
}

void run_rtos_timer_example(void)
{
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    const esp_timer_create_args_t esp_timer_create_args = 
    {
        .callback = &on_timer_cb,
        .name = "My timer",
    };

    esp_timer_handle_t esp_timer_handle;
    esp_timer_create(&esp_timer_create_args, &esp_timer_handle);
    esp_timer_start_periodic(esp_timer_handle, 50);

}

#elif USE_RTOS_TIMER
void on_timer_cb(TimerHandle_t xTimer)
{
    /** Do not delays this operations, use it to fire semaphores or so but just keep this unblocking*/
    printf("Time hits at %lld\n", esp_timer_get_time() / 1000);    
}

void run_rtos_timer_example(void)
{
    printf("APP has started %lld\n", esp_timer_get_time() / 1000);  
    TimerHandle_t xTimer = xTimerCreate("My Timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, on_timer_cb); 
    xTimerStart(xTimer, 0); 
}

#endif
