#include <stdio.h>
#include "esp_log.h"

#define TAG     "DEFAULT_LOG"
#define TAG2    "MODIFIED_LOG"

void app_main(void)
{
    /** The default log level is to be define by menuconfig */
    ESP_LOGE(TAG, "This is an error");
    ESP_LOGW(TAG, "This is a warning");
    ESP_LOGI(TAG, "This is an info");
    ESP_LOGD(TAG, "This is debug");
    ESP_LOGV(TAG, "This is Verbose");

    /** To change the default log level */
    esp_log_level_set(TAG2, ESP_LOG_INFO);
    ESP_LOGE(TAG2, "This is an error");
    ESP_LOGW(TAG2, "This is a warning");
    ESP_LOGI(TAG2, "This is an info");
    ESP_LOGD(TAG2, "This is debug");
    ESP_LOGV(TAG2, "This is verbose");

}
