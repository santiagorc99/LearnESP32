#include <stdio.h>

/** SPIFFS: SPI FLASH FILE SYSTEM
 * It's used to flash an entire directory to flash, modified with common C routine. We can create the FS by:
 * 1. Use a tool to convert a directory to a bin file at build time.
 * 2. Use a tool to load the file into flash at flash time.
 * 
 * For this example, please check partitions.csv  and CMakeLists.txtfiles.
*/

/** Loading data scripts
 * We can modify the script and upload it without need of recompiling the whole code again. To do so:
 * 1. Modify the file.
 * 2. Copy the esp-idf/components/spiifs/spiffsgen.py (Takes a folder and output a bin file out of code project) and 
 *    esp-idf/components/esptool_py/esptool/esptool.py (Take a new file and dump it into a flash partition) to the project.
 * 3. Open command line.
 * 4. Run python spiffs_dir.py image_size (Taken from partitions.csv) base_dir (Spiifs dir) output_file (Name):
 *    python spiffsgen.py 0x100000 spiffs spiffs.bin, the bin file will be generated in the project.
 * 5. Run esptool.py --chip esp32 --port <PORT> --baud 115200 write_flash -z <start address> <the bin file>
 *    esptool.py --chip esp32 --port COM31 --baud 115200 write_flash -z 0x110000 spiffs.bin
 *    Note: To calulate the start address go to partitions.csv and calculate it (NVS starts at 0x9000, just sum things up!)
*/

/** 
 * For more advanced info, check the 11_4_spiffs_sdk_example. 
*/

#include "esp_spiffs.h"
#include "esp_log.h"

#include <stdlib.h>
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#define TAG         "SPIFFS"

void spiffs_test_single_file(char *file_name);
void spiffs_test_iterate_through_files(void);

void app_main(void)
{
    spiffs_test_single_file("index.html");
    spiffs_test_iterate_through_files();
}

void spiffs_test_single_file(char *file_name)
{
    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    esp_vfs_spiffs_register(&spiffs_config);

    char file_path[50];
    sprintf(file_path, "/spiffs/%s", file_name);

    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        ESP_LOGE(TAG, "Could not open the file");

    } else {
        char *line = (char *)malloc(2048 * sizeof(char));
        while (fgets(line, 2048, file) != NULL) {
            printf("%s", line);
        }
        printf("\n");
        fclose(file);
        free(line);
    }
    esp_vfs_spiffs_unregister(NULL);
}

void spiffs_test_iterate_through_files(void)
{
    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    esp_vfs_spiffs_register(&spiffs_config);

    char full_path[300];
    struct dirent *entry;
    DIR *directory = opendir("/spiffs");
    struct stat entry_stat;

    while ((entry = readdir(directory)) != NULL) {
        sprintf(full_path, "/spiffs/%s", entry->d_name);
        if (stat(full_path, &entry_stat) == -1) {
            ESP_LOGE(TAG, "Error getting stats for %s", full_path);
        } else {
            ESP_LOGI(TAG, "Full path = %s, file size = %ld", full_path, entry_stat.st_size);
        }
    } 

    size_t total_space = 0;
    size_t used_space = 0;
    esp_spiffs_info(NULL, &total_space, &used_space);
    ESP_LOGI(TAG, "Total space %d, used space %d", total_space, used_space);

    esp_vfs_spiffs_unregister(NULL);
}
