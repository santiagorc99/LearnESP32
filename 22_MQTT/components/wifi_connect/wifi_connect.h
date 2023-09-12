#ifndef __WIFI_CONNECT_INC_
#define __WIFI_CONNECT_INC_

void wifi_connect_init(void);
void wifi_disconnect(void);
void wifi_connect_ap(const char *ssid, const char *password);
esp_err_t wifi_connect_sta(char *ssid, char *password, int timeout);

#endif /* __WIFI_CONNECT_INC_ */ 