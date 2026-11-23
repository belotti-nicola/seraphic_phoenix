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
    big_buffer[ 1 ]            = 'b' ;
    big_buffer[BIG_BUFFER - 1] = '\0';

    sp_string_t string_big = spss_create(big_buffer);

    if ( spss_pop_front(&string_big) == false )
    {
        TEST_ERROR("spss_pop_back fail.");
        return 1;
    }

    size_t expected_size = BIG_BUFFER - 2;
    size_t computed_size = spss_length(&string_big);

    if ( expected_size != computed_size )
    {
        TEST_ERROR("Expected size differs from computed one.");
        return 1;
    }

    if ( spss_front(&string_big) != 'b' )
    {
        TEST_ERROR("Expected first character differs from computed one.");
        return 1;
    }

    spss_destroy(&string_big);

    TEST_INFO("Test success.");
    return 0;
}

