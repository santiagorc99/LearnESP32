#include "../main.h"
#include "hibernation_example.h"
#include "driver/rtc_io.h"

/** Only enables the RTC */

#define TAG                             "HIBERNATION"

#define WAKEUP_TIMER_IN_US              (5000000)

RTC_DATA_ATTR int times_woken_up = 0;

void run_hibernation_example(void)
{
    /** Enable slow memory to work with RTC variables */
    /** If all above are turned off it only consumes ~5uA*/
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_enable_timer_wakeup(WAKEUP_TIMER_IN_US);  

    ESP_LOGI(TAG, "Going to sleep %d", times_woken_up++);
    esp_deep_sleep_start();
}