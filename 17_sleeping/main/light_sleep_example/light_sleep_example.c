#include "../main.h"
#include "light_sleep_example.h"
#include "driver/rtc_io.h"

#define TAG                     "LIGTH_SLEEP"

/* Wake up after 5 seconds or with an interrupt */
#define WAKEUP_TIME_IN_US           (5000000)
#define WAKEUP_PIN                  (0)

void run_light_sleep_example(void)
{
    gpio_pad_select_gpio(WAKEUP_PIN);
    gpio_set_direction(WAKEUP_PIN, GPIO_MODE_INPUT);
    gpio_wakeup_enable(WAKEUP_PIN, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_sleep_enable_timer_wakeup(WAKEUP_TIME_IN_US);

    while (true) {
        if (rtc_gpio_get_level(WAKEUP_PIN) == 0) {
            ESP_LOGI(TAG, "Release button before continuing");
            do {
                vTaskDelay(pdMS_TO_TICKS(10));
            } while (rtc_gpio_get_level(WAKEUP_PIN) == 0);
        }
        ESP_LOGI(TAG, "Taking a nap");

        /** Wait until the Tx has finished */
        uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
        int64_t time_before_sleeping = esp_timer_get_time();
        esp_light_sleep_start();

        int64_t time_after_sleeping = esp_timer_get_time();
        esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
        ESP_LOGI(TAG, "Just woke up [Reason %s][%lld]", 
                                                    wakeup_cause == ESP_SLEEP_WAKEUP_TIMER ? "timer" : "button",
                                                    (time_after_sleeping - time_before_sleeping)/1000);
    }

}