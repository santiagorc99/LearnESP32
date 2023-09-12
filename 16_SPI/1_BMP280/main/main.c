#include <stdio.h>

#include "esp_log.h"
#include "driver/spi_common.h"

#define SPI_MISO_PIN        (19)
#define SPI_MOSI_PIN        (21)
#define SPI_CLK_PIN         (18)
#define SPI_CS_PIN          (19)

#define TAG             "SMP280-SPI"

void app_main(void)
{
    spi_bus_config_t bus_config = {
        .miso_io_num = SPI_MISO_PIN,
        .mosi_io_num = SPI_MOSI_PIN,
        .sclk_io_num = SPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI1_HOST, &bus_config, SPI_DMA_CH_AUTO));

}
