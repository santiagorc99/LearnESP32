#include "../main.h"
#include "deep_sleep_example.h"
#include "driver/rtc_io.h"

#define TAG                             "DEEP_SLEEP"

#define WAKEUP_TIMER_IN_US              (5000000)

#define BUTTON_WAKEUP_0                 (GPIO_NUM_25)
#define BUTTON_WAKEUP_1                 (GPIO_NUM_26)

RTC_DATA_ATTR int times_woken_up = 0;

void run_deep_sleep_example(void)
{
    /** If timer wakeup is enabled */
    // esp_sleep_enable_timer_wakeup(WAKEUP_TIMER_IN_US);

    /** If ext0 wakuep is enabled */
    // rtc_gpio_deinit(BUTTON_WAKEUP_0);
    // // Use the GPIO as a normal GPIO.
    // rtc_gpio_pullup_en(BUTTON_WAKEUP_0);
    // rtc_gpio_pulldown_dis(BUTTON_WAKEUP_0);
    // esp_sleep_enable_ext0_wakeup(BUTTON_WAKEUP_0, 0);

    /** If ext1 wakuep is enabled, gives us more capabilities */
    rtc_gpio_deinit(BUTTON_WAKEUP_0);
    rtc_gpio_deinit(BUTTON_WAKEUP_1);
    // Use the GPIO as a normal GPIO.
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    rtc_gpio_pullup_en(BUTTON_WAKEUP_0);
    rtc_gpio_pulldown_dis(BUTTON_WAKEUP_0);
    rtc_gpio_pullup_en(BUTTON_WAKEUP_1);
    rtc_gpio_pulldown_dis(BUTTON_WAKEUP_1);

    uint64_t wakeup_mask = (1U << BUTTON_WAKEUP_0) | (1U << BUTTON_WAKEUP_1);
    esp_sleep_enable_ext1_wakeup(wakeup_mask, ESP_EXT1_WAKEUP_ALL_LOW);

    ESP_LOGI(TAG, "Going to sleep %d", times_woken_up++);
    esp_deep_sleep_start();

}
