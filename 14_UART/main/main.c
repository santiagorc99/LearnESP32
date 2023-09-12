#include "main.h"

#if ENABLE_QUEUE_EXAMPLE
#include "queue_example.h"
example_runner_t run_example = &run_queue_example;

#elif ENABLE_PC_EXAMPLE
#include "pc_example.h"
example_runner_t run_example = &run_pc_example;

#elif ENABLE_ECHO_EXAMPLE
#include "echo_example.h"
example_runner_t run_example = &run_echo_example;

#endif

void app_main(void)
{
    run_example();
}
