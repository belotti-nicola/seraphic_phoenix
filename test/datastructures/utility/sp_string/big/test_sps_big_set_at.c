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

    if ( spss_set_at(&string_big,100,'b') == false )
    {
        TEST_ERROR("First set fail.");
        return 1;
    }
    
    if ( spss_set_at(&string_big,200,'c') == false )
    {
        TEST_ERROR("Second set fail.");
        return 1;
    }
    
    if ( spss_set_at(&string_big,300,'d') == false )
    {
        TEST_ERROR("Third set fail.");
        return 1;
    }

    const char *cstr = spss_cstr(&string_big);

    if ( strlen(cstr) != BIG_BUFFER - 1 )
    {
        TEST_ERROR("Unexpected strlen.");
        return 1;
    }

    if ( cstr[100] != 'b' )
    {
        TEST_ERROR("First set fail.");
        return 1;
    }

    if ( cstr[200] != 'c' )
    {
        TEST_ERROR("Second set fail.");
        return 1;
    }

    if ( cstr[300] != 'd' )
    {
        TEST_ERROR("Third set fail.");
        return 1;
    }

    spss_destroy(&string_big);

    TEST_INFO("Test success.");
    return 0;
}

