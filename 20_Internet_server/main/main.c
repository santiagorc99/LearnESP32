#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "toggle_led.h"
#include "push_button.h"
#include "cJSON.h"

#define TAG                                 "SERVER"
#define STA_SSID                            "TE-2"
#define STA_PASSWORD                        "555titoma1666"

/** Web socket support was enabled using menuconfig, http header lenght was increase to 1024 */

static httpd_handle_t server = NULL;

static esp_err_t on_default_url(httpd_req_t *request)
{
    ESP_LOGI(TAG, "URL: %s", request->uri);
    httpd_resp_sendstr(request, "Madre mia el bicho, siiiuuuuu");
    return ESP_OK;
}

static esp_err_t on_toggle_led(httpd_req_t *request)
{
    /** Receive a really simple jSON structure */
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    /** Make sure the len is less than the buffer size */
    httpd_req_recv(request, buffer, request->content_len);
    cJSON *payload = cJSON_Parse(buffer);
    /** Make sure the payload is not NULL */
    cJSON *is_on_json = cJSON_GetObjectItem(payload, "is_on");
    bool is_on = cJSON_IsTrue(is_on_json);
    cJSON_Delete(payload);
    led_toggle(is_on);

    /** This is how we go back to the client and respond to it */
    httpd_resp_set_status(request, "204 NO CONTENT");
    httpd_resp_send(request, NULL, 0);
    return ESP_OK;

}

/************************************* WEB SOCKET ********************************************/
#define WS_MAX_SIZE                 (1024)

/** 
 * Open the web socket using ws://santi-server.local/ws
*/

static int client_session_id;

esp_err_t send_ws_message(char *message)
{
    if (!client_session_id) {
        ESP_LOGE(TAG, "No client ssion ID");
        return ESP_FAIL;
    }

    httpd_ws_frame_t ws_message = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *) message,
        .len = strlen(message),
    };
    return httpd_ws_send_frame_async(server, client_session_id, &ws_message);
}

static esp_err_t on_web_socket(httpd_req_t *request)
{
    client_session_id = httpd_req_to_sockfd(request);
    if (request->method == HTTP_GET) {
        return ESP_OK;
    }

    httpd_ws_frame_t ws_packet;
    memset(&ws_packet, 0, sizeof(httpd_ws_frame_t));
    ws_packet.type = HTTPD_WS_TYPE_TEXT;
    ws_packet.payload = malloc(WS_MAX_SIZE);
    httpd_ws_recv_frame(request, &ws_packet, WS_MAX_SIZE);
    printf("WS Payload: %.*s\n", ws_packet.len, ws_packet.payload);
    free(ws_packet.payload);

    char *response = "connected OK 😁";
    httpd_ws_frame_t ws_response = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *) response,
        .len = strlen(response),
    };
    return httpd_ws_send_frame(request, &ws_response);
}
/*********************************************************************************************/

/** This configures the server, we should ping the ESP IP to access the server */
void init_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(httpd_start(&server, &config));

    /** Get to the mdns or IP */
    httpd_uri_t default_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = on_default_url,
    };
    httpd_register_uri_handler(server, &default_url);

    /** Post to the toogle led API: http://santi-server.local/api/toggle-led */
    httpd_uri_t toggle_url = {
        .uri = "/api/toggle-led",
        .method = HTTP_POST,
        .handler = on_toggle_led,
    };
    httpd_register_uri_handler(server, &toggle_url);

    httpd_uri_t web_socket_url = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = on_web_socket,
        .is_websocket = true,
    };
    httpd_register_uri_handler(server, &web_socket_url);
    
}

/** This configures the mdns (Set IP to a name), access it as http://santi-esp32.local or ping it as santi-esp32.local */
/** If IDF V5 is used, make sure to install the component and rebuild   */
void start_mdns_service(void)
{
    mdns_init();
    mdns_hostname_set("santi-server");
    mdns_instance_name_set("Learn esp32 thing");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    led_init();
    button_init();
    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta(STA_SSID, STA_PASSWORD, 10000));
    start_mdns_service();
    init_server();
}
