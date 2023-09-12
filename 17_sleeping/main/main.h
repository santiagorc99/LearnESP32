#ifndef __MAIN_INC_
#define __MAIN_INC_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp32/rom/uart.h"

/** Please check the documentation to know what things are available in what modes */

#define ENABLE_LIGHT_SLEEP_EXAMPLE      (1)
#define ENABLE_DEEP_SLEEP_EXAMPLE       (1)
#define ENABLE_HIBERNATION_EXAMPLE      (1)

#endif /* __MAIN_INC_ */ 
