#include <stdio.h>
#include "nvs_flash.h"
#include "esp_nimble_hci.h"                 /*!< Host controller interface */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"                    /*!< This is the stack          */
#include "services/gap/ble_svc_gap.h"       /*!< How we connect             */
#include "services/gatt/ble_svc_gatt.h"     /*!< How data is transmitted    */
#include "freertos/timers.h"

/** 
 * Enable bluetooth in the menuconfig, select bluedroid or nimble 
*/

#define TAG                         "BLE"

#define MY_BLE_DEVICE               "SANTI-ESP"

/**
 * UUIDs can be found in bluetooth.com -> Specifications
*/

/** Service 1 */
#define DEVICE_INFO_SERVICE_UUID    (0x180A)
#define MAN_NAME_STRING_UUID        (0x2A29)

/** Service 2*/
#define BATTERY_SERVICE_UUID        (0x180F)
#define BATTERY_LEVEL_UUID          (0x2A19)
#define BATT_CHAR_CONFIG_DESC_UUID  (0x2902)

static xTimerHandle timer_handle;

static uint8_t ble_addr_type;

static uint16_t battery_characteristic_attribute_handle;
static uint16_t conn_handle;

static void ble_app_advertise(void);

int on_manufacturer_name(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Santi is the manufacturer", strlen("Santi is the manufacturer"));
    return ESP_OK;
}

int on_device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("*****Incoming message is: '%.*s' *****\n", ctxt->om->om_len, ctxt->om->om_data);
    return ESP_OK;
}

static int on_battery_level_read(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t battery_level = 85;
    os_mbuf_append(ctxt->om, &battery_level, sizeof(battery_level));
    return ESP_OK;
}

/** Enable notifications, disable indications */
uint8_t config[2] = {0x00, 0x00};
static int on_battery_descriptor(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("********************DESCRIPTOR\n");
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
        os_mbuf_append(ctxt->om, config, sizeof(config));
    }
    else {
        memcpy(config, ctxt->om->om_data, ctxt->om->om_len);
    }

    /** If notifications are enabled */
    if (config[0] == 0x01) {
        xTimerStart(timer_handle, 0);
    } else {
        xTimerStop(timer_handle, 0);
    }
    return ESP_OK;
}

static const struct ble_gatt_svc_def gat_svcs[] = 
{
    /** Device info service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(DEVICE_INFO_SERVICE_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = BLE_UUID16_DECLARE(MAN_NAME_STRING_UUID),
                .flags = BLE_GATT_CHR_F_READ,
                .access_cb = on_manufacturer_name,
                .arg = NULL,
            },
            {
                .uuid = BLE_UUID128_DECLARE(0x9a,0x28,0x75,0x5c,0x51,0x84,0x11,0xee,0xbe,0x56,0x02,0x42,0xac,0x12,0x00,0x02),
                .flags = BLE_GATT_CHR_F_WRITE,
                .access_cb = on_device_write,
                .arg = NULL,
            },
            
            /** Last char must be NULL teminated */
            {
                0
            }
        }
    },

    /** Battery service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BATTERY_SERVICE_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = BLE_UUID16_DECLARE(BATTERY_LEVEL_UUID),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .access_cb = on_battery_level_read,
                .arg = NULL,
                .val_handle = &battery_characteristic_attribute_handle,
                .descriptors = (struct ble_gatt_dsc_def[]){
                    {
                        .uuid = BLE_UUID16_DECLARE(BATT_CHAR_CONFIG_DESC_UUID), 
                        .att_flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
                        .access_cb = on_battery_descriptor
                    },
                    {0}
                }
            },
            /** Last char must be NULL teminated */
            {
                0
            }
        }
    },
    /** Last field must be NULL terminated */
    {
        0
    },
};


static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {

        case BLE_GAP_EVENT_CONNECT :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_CONNECT %s", event->connect.status == 0 ? "OK" : "FAILED");
            if (event->connect.status != 0) 
            {
                ble_app_advertise();
            }
            conn_handle = event->connect.conn_handle;
            break;

        case BLE_GAP_EVENT_DISCONNECT :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_DISCONNECT");
            ble_app_advertise();
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_ADV_COMPLETE");
            ble_app_advertise();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_SUBSCRIBE");
            if (event->subscribe.attr_handle == battery_characteristic_attribute_handle) {
                /** If notifications are enabled */
                if (config[0] == 0x01) {
                    config[0] = 0;
                    xTimerStop(timer_handle, 0);
                } else {
                    config[0] = 1;
                    xTimerStart(timer_handle, 0);
                }
            }
            break;

        default:
            break;
    }
    return ESP_OK;
}

void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (uint8_t *) ble_svc_gap_device_name();
    fields.name_len = strlen(ble_svc_gap_device_name());
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);


    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

static void be_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); /** Get the best address type to use */
    ESP_LOGI(TAG, "Address type is %d", ble_addr_type);

    ble_app_advertise();
}

static void host_task(void *params)
{
    nimble_port_run();
}

void update_battery_status()
{
    static uint8_t battery_level = 100;
    if (battery_level-- == 0) {
        battery_level = 100;
    }
    printf("Notify battery level is %d\n", battery_level);

    struct os_mbuf *om = ble_hs_mbuf_from_flat(&battery_level, sizeof(uint8_t));
    ble_gattc_notify_custom(conn_handle, battery_characteristic_attribute_handle, om);

}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_nimble_hci_and_controller_init();   /*!< Init the link layers and the HCI between the NIMBLE host and the ESP BLE controller */
    nimble_port_init();                     /*!< Init NIMBLE port (As a FreeRTOS one), memory allocations and so */

    ble_svc_gap_device_name_set(MY_BLE_DEVICE);
    ble_svc_gap_init();

    ble_svc_gatt_init();
    ble_gatts_count_cfg((const struct ble_gatt_svc_def *) &gat_svcs);
    ble_gatts_add_svcs((const struct ble_gatt_svc_def *) &gat_svcs);

    timer_handle = xTimerCreate("Update battery status", pdMS_TO_TICKS(1000), pdTRUE, NULL, update_battery_status);

    /** This happens whent the host and the controller are sync */
    ble_hs_cfg.sync_cb = be_app_on_sync;

    nimble_port_freertos_init(host_task);
}
