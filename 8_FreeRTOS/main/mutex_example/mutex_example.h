#ifndef __MUTEX_EXAMPLE_INC_ 
#define __MUTEX_EXAMPLE_INC_

/** The goal is to write to avoid writing the bus at the same time by multiple tasks */
/** We only want to write to the bus while it's ready to be used */

void run_mutex_example(void);

#endif /* __MUTEX_EXAMPLE_INC_ */
