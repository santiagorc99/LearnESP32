#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    char c = 0;
    char string[100];
    memset(string, 0, sizeof(string));

    while (c != '\n') {
        c = getchar();

        if (c != 0xFF) {
            string[strlen(string)] = c;
            printf("%c", c);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("You typed %s\r\n", string);

}
