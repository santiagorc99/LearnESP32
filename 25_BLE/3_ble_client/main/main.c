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

#define TAG                         "BLE-CLIENT"

#define MY_BLE_DEVICE               "SANTI-ESP"

static uint8_t ble_addr_type;
uint16_t conn_handle;

static void ble_app_scan(void);

int on_attribute_read(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg)
{
    if (error->status == ESP_OK) {
        printf("Got char value %.*s\n", attr->om->om_len, attr->om->om_data);
    } else {
        ESP_LOGE("VALUE", "Error reading");
    }
    return error->status;
}

int on_chars_disc(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *chr, void *arg)
{
    static uint16_t attribute_handle;
    char buffer[50];
    switch (error->status)
    {
        case ESP_OK:
            memset(buffer, 0, sizeof(buffer));
            ble_uuid_to_str(&chr->uuid.u, buffer);
            ESP_LOGI("CHARACTERISTIC", "Service %s", buffer);
            if (strcmp(buffer, "0x2a00") == 0) {
                attribute_handle = chr->val_handle;
            }
            break;

        case BLE_HS_EDONE:
            ble_gattc_read(conn_handle, attribute_handle, on_attribute_read, NULL);
            ESP_LOGI("SERVICE", "Reading all values");
            break;
        
        default:
            break;
    }

    return error->status; 
}

int on_service_disc(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *service, void *arg)
{
    static uint16_t start = 0;
    static uint16_t end = 0;
    char buffer[50];
    switch (error->status)
    {
        case ESP_OK:
            memset(buffer, 0, sizeof(buffer));
            ble_uuid_to_str(&service->uuid.u, buffer);
            ESP_LOGI("SERVICE", "Service %s", buffer);

            if (strcmp(buffer, "0x1800") == 0) {
                start = service->start_handle;
                end = service->end_handle;
            }

            break;

        case BLE_HS_EDONE:
            ble_gattc_disc_all_chrs(conn_handle, start, end, on_chars_disc, NULL);
            ESP_LOGI("SERVICE", "All services were discovered");
            break;
        
        default:
            break;
    }

    return error->status;
}

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_hs_adv_fields fields;

    switch (event->type)
    {
        case BLE_GAP_EVENT_DISC:
            ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
            printf("Discovered device %.*s\n", fields.name_len, fields.name);
            if ((fields.name_len == strlen(MY_BLE_DEVICE)) &&
                    (memcmp(fields.name, MY_BLE_DEVICE, strlen(MY_BLE_DEVICE)) == 0)) {
                printf("Device was found\n");
                ble_gap_disc_cancel();
                ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &event->disc.addr, 1000, NULL, ble_gap_event, NULL);
                ESP_LOGI(TAG, "Connecting to device");
            }
            break;

        case BLE_GAP_EVENT_CONNECT :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_CONNECT %s", event->connect.status == 0 ? "OK" : "FAILED");
            conn_handle = event->connect.conn_handle;
            if (event->connect.status == 0) {
                ble_gattc_disc_all_svcs(conn_handle, on_service_disc, NULL);
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_DISCONNECT");
            ble_app_scan();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE :
            ESP_LOGI(TAG, "BLE_GAP_EVENT_SUBSCRIBE");
            break;

        default:
            break;
    }
    return ESP_OK;
}

void ble_app_scan(void)
{
    struct ble_gap_disc_params ble_gap_disc_params;
    memset(&ble_gap_disc_params, 0, sizeof(ble_gap_disc_params));
    ble_gap_disc_params.filter_duplicates = 1;
    ble_gap_disc_params.passive = 1;                                /** Do not send scan reponses */

    ble_gap_disc(ble_addr_type, BLE_HS_FOREVER, &ble_gap_disc_params, ble_gap_event, NULL);
}

static void be_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); /** Get the best address type to use */
    ESP_LOGI(TAG, "Address type is %d", ble_addr_type);

    ble_app_scan();
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

    ble_svc_gap_device_name_set(MY_BLE_DEVICE);
    ble_svc_gap_init();

    /** This happens whent the host and the controller are sync */
    ble_hs_cfg.sync_cb = be_app_on_sync;

    nimble_port_freertos_init(host_task);
}
