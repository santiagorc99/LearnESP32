#include <stdio.h>

#include "esp_log.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * Check partitions.csv to check the completion of the example, it was also modified using the menuconfig to access
 * the new partition table.
*/

#define TAG             "NVS"

typedef struct cat_ {
    char name[20];
    int age;
    int id;

} cat_t;

void app_main(void)
{
    vTaskDelay(2000 / portTICK_RATE_MS);

    ESP_ERROR_CHECK(nvs_flash_init_partition("my_nvs"));

    nvs_stats_t nvs_stats;
    nvs_get_stats("my_nvs", &nvs_stats);
    ESP_LOGI(TAG, "Used %d, Free %d, Total %d, Namespace count %d",
                                                                    nvs_stats.used_entries,
                                                                    nvs_stats.free_entries,
                                                                    nvs_stats.total_entries,
                                                                    nvs_stats.namespace_count);
    
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("my_nvs","cat_storage", NVS_READWRITE, &nvs_handle));

    char cat_key[40];
    cat_t single_cat;
    size_t cat_size = sizeof(cat_t);
    esp_err_t err_code = ESP_ERR_NOT_FOUND;

    for (int cat_idx = 0; cat_idx < 5; cat_idx++) {
        sprintf(cat_key, "cat_%d", cat_idx);
        err_code = nvs_get_blob(nvs_handle, cat_key, &single_cat, &cat_size);

         switch (err_code) {
            case ESP_ERR_NVS_NOT_FOUND:
            case ESP_ERR_NOT_FOUND:
                ESP_LOGE(TAG, "Cat not found");
                break;

            case ESP_OK:
                ESP_LOGI(TAG, "CAT. Name: %s, age: %d, id %d", 
                                                                single_cat.name,
                                                                single_cat.age,
                                                                single_cat.id);
                break;

            default:
                ESP_LOGE(TAG, "ERROR (%s) opening NVS handle\n", esp_err_to_name(err_code));
                break;
        }
    }

    for (int cat_idx = 0; cat_idx < 5; cat_idx++) {
        sprintf(cat_key, "cat_%d", cat_idx);
        sprintf(single_cat.name, "Mr cat %d", cat_idx);
        single_cat.age = cat_idx + 2;
        single_cat.id = cat_idx;
        ESP_ERROR_CHECK(nvs_set_blob(nvs_handle, cat_key, &single_cat, sizeof(cat_t)));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    }

    nvs_close(nvs_handle);
}
