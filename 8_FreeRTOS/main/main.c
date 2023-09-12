#include <stdio.h>
#include "main.h"

typedef void (*routine_pointer_t)(void); 

#if RUN_RTOS_TIMER_EXAMPLE
#include "RTOS_timer_example.h"
routine_pointer_t run_example = &run_rtos_timer_example;

#elif RUN_EVENT_GROUP_EXAMPLE
#include "event_group_example.h"
routine_pointer_t run_example = &run_event_group_example;

#elif RUN_QUEUE_EXAMPLE
#include "queue_example.h"
routine_pointer_t run_example = &run_queue_example;

#elif RUN_MUTEX_EXAMPLE
#include "mutex_example.h"
routine_pointer_t run_example = &run_mutex_example;

#elif RUN_SEMAPHORE_EXAMPLE
#include "semaphore_example.h"
routine_pointer_t run_example = &run_semaphore_example;

#elif RUN_TASK_NOTIFICATION_EXAMPLE
#include "task_notification_example.h"
routine_pointer_t run_example = &run_example_notification;
#endif

void app_main(void)
{
    run_example();
}
