#include <stdio.h>

/** To flash the files inside the "files" folder into flash check the component.mk & CMakeList.txt files.
*/

void app_main(void)
{
    extern const unsigned char index_html[] asm("_binary_index_html_start");
    printf("HTML FILE IS: \n%s\n", index_html);

    extern const unsigned char sample_txt[] asm("_binary_sample_txt_start");
    printf("TXT FILE IS: \n%s\n", sample_txt);

    extern const unsigned char logo_start[] asm("_binary_logo_bmp_start");
    extern const unsigned char logo_end[]   asm("_binary_logo_bmp_end");
    printf("IMG size is: %d\n", logo_end-logo_start);
}
