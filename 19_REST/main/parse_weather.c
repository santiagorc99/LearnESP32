#include <stdio.h>
#include <string.h>
#include "parse_weather.h"
#include "cJSON.h"

#include "esp_log.h"
#include "esp_heap_caps.h"

#define TAG                     "PARSER"

void *json_malloc(size_t size) 
{
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
}

void json_free(void *ptr)
{
    heap_caps_free(ptr);
}

esp_err_t parse_weather(char *weather_string)
{
    /** SPIRAM is enabled in the menuconfig, this is done to not run out of memory while parsing the jSON */
    // cJSON_Hooks hooks = {
    //     .malloc_fn = json_malloc,
    //     .free_fn   = json_free,
    // };
    // cJSON_InitHooks(&hooks);

    cJSON *weather_json = cJSON_Parse(weather_string);

    /** Check if json is empty */
    if (weather_json == NULL) {
        const char *err = cJSON_GetErrorPtr();
        if (err) {
            ESP_LOGE(TAG, "Error parsing jSON before %s\n", err);
        }
        return ESP_FAIL;
    }

    cJSON *location = cJSON_GetObjectItemCaseSensitive(weather_json, "location");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(location, "name");
    printf("City %s\n", name->valuestring);

    return ESP_OK;



}