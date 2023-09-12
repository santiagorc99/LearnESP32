#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_types.h"

#define SDA_GPIO                (26)
#define SCL_GPIO                (25)
#define BMP280_ADDRESS          (0x76 << 1)
#define BMP280_ID_REGISTER      (0xD0)

void app_main(void)
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    uint8_t id_value;
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);

    i2c_master_write_byte(cmd_handle, BMP280_ADDRESS | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, BMP280_ID_REGISTER, true);
    i2c_master_start(cmd_handle);

    i2c_master_write_byte(cmd_handle, BMP280_ADDRESS | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, &id_value, 1, I2C_MASTER_ACK);
    i2c_master_stop(cmd_handle);

    i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd_handle);

    printf("ID Value is 0x%02x\n", id_value);

}
