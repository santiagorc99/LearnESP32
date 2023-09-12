#include <stdio.h>
#include <string.h>
#include "connect.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "parse_weather.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "cJSON.h"

#define TEST_POST

/** I use https://apilist.fun/api or rapid api*/

#define TAG                             "REST"

#define STA_SSID                        "TE-2"
#define STA_PASSWORD                    "555titoma1666"

#ifdef TEST_POST

const char body_text[] = "<!DOCTYPE html>\
<html>\
<head>\
    <title>Oye</title>\
</head>\
<body>\
    <h1>Oye</h1>\
    <p>Esto es un correo desde ESP32</p>\
</body>\
</html>";

char *get_email_body(void)
{
    /** Create the whole json payload */
    cJSON *json_payload = cJSON_CreateObject();

    /** Add the item personalizations to the whole json payload */
    cJSON *personalizations = cJSON_CreateArray();
    cJSON_AddItemToObject(json_payload, "personalizations", personalizations);

    /** Since personalizations is an array, add the element 1 to it */
    cJSON *personalization_0 = cJSON_CreateObject();
    cJSON_AddItemToArray(personalizations, personalization_0);

    /** Add an element called "to" to personalization in the element 1 */
    cJSON *to = cJSON_CreateArray();
    cJSON_AddItemToObject(personalization_0, "to", to);

    /** Create an element, id 0, of to since it is an array */
    cJSON *to_0 = cJSON_CreateObject();

    /** Add the strings to the to element 0 */
    cJSON_AddStringToObject(to_0, "email", "santiagorinconc.05@gmail.com");
    cJSON_AddStringToObject(to_0, "name", "Santi personal");
    /** Add the element 0 to the to object */
    cJSON_AddItemToArray(to, to_0);

    /** Add a string the subject string to the personalization 0 element */
    cJSON_AddStringToObject(personalization_0, "subject", "Hello world!");

    /** Add the element content to the whole json */
    cJSON *content = cJSON_CreateArray();
    cJSON_AddItemToObject(json_payload, "content", content);

    /** Create an element of content */
    cJSON *content_0 = cJSON_CreateObject();

    /** Add type annd value to content element 0 */
    cJSON_AddStringToObject(content_0, "type", "text/html");
    cJSON_AddStringToObject(content_0, "value", body_text);

    

    /** Add the element 0 to content */
    cJSON_AddItemToArray(content, content_0);

    /** Add the element from to the whole json */
    cJSON *from = cJSON_CreateObject();
    cJSON_AddItemToObject(json_payload, "from", from);

    /** Add the string to from */
    cJSON_AddStringToObject(from, "email", "santiagorinconc.05@gmail.com");
    cJSON_AddStringToObject(from, "name", "Santiago Rincon");

    /** Add the element from to the whole json */
    cJSON *reply_to = cJSON_CreateObject();
    cJSON_AddItemToObject(json_payload, "reply_to", reply_to);

    /** Add the string to from */
    cJSON_AddStringToObject(reply_to, "email", "santiagorinconc.05@gmail.com");
    cJSON_AddStringToObject(reply_to, "name", "Santiago Rincon");

    char *payload_body = cJSON_Print(json_payload);
    printf("Body is: %s\n", payload_body);
    cJSON_Delete(json_payload);

    return payload_body;
}

void REST_send_email(void)
{
    esp_http_client_config_t esp_http_client_config = {
        .method = HTTP_METHOD_POST,
        .url = "https://api.sendgrid.com/v3/mail/send",
        /** Check on the main CA which use x.509*/
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", "Bearer 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");
    char *payload_body = get_email_body();
    esp_http_client_set_post_field(client, payload_body, strlen(payload_body));

    esp_err_t error_code = esp_http_client_perform(client);
    if (error_code != ESP_OK)
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(error_code));
    } else {
        ESP_LOGI(TAG, "HTTP POST status = %d\n", esp_http_client_get_status_code(client));
    }

    if (payload_body != NULL) {
        free(payload_body);
    }

    esp_http_client_cleanup(client);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta(STA_SSID, STA_PASSWORD, 10000));
    REST_send_email();
    wifi_disconnect();
}

#else

typedef struct chunk_payload_ {
    uint8_t *buffer;
    int index;

} chunk_payload_t; 

esp_err_t on_client_data(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA: {
            chunk_payload_t *chunk_payload = (chunk_payload_t *) evt->user_data;
            chunk_payload->buffer = realloc(chunk_payload->buffer, chunk_payload->index + evt->data_len + 1); // 1 is for NULL terminator
            memcpy(&chunk_payload->buffer[chunk_payload->index], (uint8_t *) evt->data, evt->data_len);
            chunk_payload->index += evt->data_len;
            chunk_payload->buffer[chunk_payload->index] = 0;
            break;
        }

        default:
            break;
    }

    return ESP_OK;
}





void REST_get_test(void)
{
    chunk_payload_t chunk_payload = {0};

    esp_http_client_config_t esp_http_client_config = {
        .method = HTTP_METHOD_GET,
        .url = "https://weatherapi-com.p.rapidapi.com/forecast.json?q=Bogota&days=2",
        .event_handler = on_client_data,
        .user_data = (void *) &chunk_payload,
        /** Check on the main CA which use x.509*/
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "X-RapidAPI-Host", "weatherapi-com.p.rapidapi.com");
    esp_http_client_set_header(client, "X-RapidAPI-Key", "be6c8f3e8emsh6f411ac38fc872ap170a65jsnd9a55886473d");
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t error_code = esp_http_client_perform(client);

    if (error_code == ESP_OK)
    {
        parse_weather((char *) chunk_payload.buffer);

    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(error_code));
    }

    if (chunk_payload.buffer != NULL) {
        free(chunk_payload.buffer);
    }
    esp_http_client_cleanup(client);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta(STA_SSID, STA_PASSWORD, 10000));
    REST_get_test();
    wifi_disconnect();
}

#endif /* TEST_POST */
