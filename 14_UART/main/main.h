#ifndef __MAIN_INC_
#define __MAIN_INC_

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/uart.h"

#define ENABLE_ECHO_EXAMPLE             (1)
#define ENABLE_PC_EXAMPLE               (1)
#define ENABLE_QUEUE_EXAMPLE            (1)

#define TXD_PIN                         (4)
#define RXD_PIN                         (5)

#define RX_BUFFER_LEN                   (1024)
#define TX_BUFFER_LEN                   (1024)

typedef void(*example_runner_t)(void);

#endif /* __MAIN_INC_ */
