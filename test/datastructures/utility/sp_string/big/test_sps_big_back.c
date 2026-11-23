#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#include <stdbool.h>

#define BIG_BUFFER 4096

int main()
{
    char big_buffer[BIG_BUFFER] = {0};
    for ( int i = 0 ; i < BIG_BUFFER - 1 ; i++ )
    {
        big_buffer[i] = 'a';
    }
    big_buffer[BIG_BUFFER - 1] = '\0';

    sp_string_t string_big = spss_create(big_buffer);

    if ( spss_back(&string_big) != 'a' )
    {
        TEST_ERROR("Back fail.");
        return 1;
    }

    spss_destroy(&string_big);

    TEST_INFO("Test success.");
    return 0;
}

