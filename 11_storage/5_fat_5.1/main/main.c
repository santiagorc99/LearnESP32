#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_vfs_fat.h"

#define TAG         "FAT"
#define BASE_PATH   "/store"

#define USE_FAT_READ_ONLY       (1)

void read_file(char *full_path);
void write_file(char *full_path, char *content);

#if USE_FAT_READ_ONLY
void app_main(void)
{
    esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_fat_spiflash_mount_ro(BASE_PATH, "storage", &esp_vfs_fat_mount_config);

    read_file("/store/flash-loaded.txt");

    esp_vfs_fat_spiflash_unmount_ro(BASE_PATH, "storage");
}

#else
void app_main(void)
{
    wl_handle_t wl_handle;
    esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, "storage", &esp_vfs_fat_mount_config, &wl_handle);

    read_file("/store/flash-loaded.txt");
    write_file("/store/text.txt", "Hello world!\r\n");
    read_file("/store/text.txt");

    esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, wl_handle);
}

#endif 

void read_file(char *full_path)
{
    ESP_LOGI(TAG, "Reading file %s", full_path);
    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "File not found");
        return;
    }
    char buffer[100];
    fgets(buffer, 99, file);
    fclose(file);
    ESP_LOGI(TAG, "File contains: %s", buffer);
}

void write_file(char *full_path, char *content)
{
    ESP_LOGI(TAG, "Writing to %s: %s", full_path, content);
    FILE *file = fopen(full_path, "w");
    fputs(content, file);
    fclose(file);
}
