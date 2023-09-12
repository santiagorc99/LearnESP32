#include "main.h"

typedef void (*example_runner_t)(void);

#if ENABLE_HIBERNATION_EXAMPLE
#include "hibernation_example.h"
example_runner_t run_example = &run_hibernation_example;

#elif ENABLE_DEEP_SLEEP_EXAMPLE
#include "deep_sleep_example.h"
example_runner_t run_example = &run_deep_sleep_example;

#elif ENABLE_LIGHT_SLEEP_EXAMPLE
#include "light_sleep_example.h"
example_runner_t run_example = &run_light_sleep_example;
#endif 

void app_main(void)
{
    run_example();
}
