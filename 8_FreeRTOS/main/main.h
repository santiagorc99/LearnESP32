#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * Below how to use different resource controllers
 * Controller         | Description
 * Task Notification  | I want to communicate directly to another task. I care about counting or sending simple data.
 * Semaphore          | Someone needs to block or unblock a task but I don't care who
 * Mutex              | Only the task that blocks another task can unblock it
 * Queue              | I need to pass data from one task to another
 * Event group        | Multiple things need to happen before I unclock a task
*/

#define RUN_TASK_NOTIFICATION_EXAMPLE   (0)
#define RUN_SEMAPHORE_EXAMPLE           (0)
#define RUN_MUTEX_EXAMPLE               (0)
#define RUN_QUEUE_EXAMPLE               (0)
#define RUN_EVENT_GROUP_EXAMPLE         (0)
#define RUN_RTOS_TIMER_EXAMPLE          (1)

#endif /* __MAIN_H_ */