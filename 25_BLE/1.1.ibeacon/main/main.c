#include <stdio.h>
#include "nvs_flash.h"
#include "esp_nimble_hci.h"                 /*!< Host controller interface */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"                    /*!< This is the stack          */
#include "services/gap/ble_svc_gap.h"       /*!< How we connect             */

/** 
 * Enable bluetooth in the menuconfig, select bluedroid or nimble 
*/

/** 
 * A iBeacon is always sending data, it advertises data but it cannot be connected.
*/

/** This happens whent the host and the controller are sync */
static void be_app_on_sync(void)
{
    ble_addr_t addr;
    ble_hs_id_gen_rnd(1, &addr);
    ble_hs_id_set_rnd(addr.val);

    uint8_t uuid128[16];
    memset(uuid128, 0x11, sizeof(uuid128));
    ble_ibeacon_set_adv_data(uuid128, 2, 10, -50);

    struct ble_gap_adv_params adv_params = (struct ble_gap_adv_params){0};
    ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
}

static void host_task(void *params)
{
    nimble_port_run();
}


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_nimble_hci_and_controller_init();   /*!< Init the link layers and the HCI between the NIMBLE host and the ESP BLE controller */
    nimble_port_init();                     /*!< Init NIMBLE port (As a FreeRTOS one), memory allocations and so */

    ble_hs_cfg.sync_cb = be_app_on_sync;

    nimble_port_freertos_init(host_task);
}
