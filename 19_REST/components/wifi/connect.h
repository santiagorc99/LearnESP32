#ifndef __CONNECT_INC_
#define __CONNECT_INC_

#include "esp_err.h"


/** Refer to https://docs.espressif.com/projects/esp-idf/en/v4.4.5/esp32/api-guides/wifi.html */

void wifi_init(void);
void wifi_disconnect(void);
void wifi_connect_ap(const char *ssid, const char *pswd);
esp_err_t wifi_connect_sta(const char* ssid, const char *pswd, int timeout);

#endif /* __CONNECT_INC_ */
