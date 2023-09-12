#include <stdio.h>

/** We can also add modules by using the Show component registry tool of VSCode*/

/** Sum was added as a subfolder of main, check CMakeLists.txt main's file */
#include "sum.h"

/** Sum was added as a comopnent, check its CMakeLists.txt file */
#include "minus.h"

void app_main(void)
{
    int result_sum = add(3, 5);
    int result_minus = minus(5, 3);
    printf("Results [%d][%d]\n", result_sum, result_minus);
}
