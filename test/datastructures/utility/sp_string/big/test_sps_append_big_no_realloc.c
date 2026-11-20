#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define SUFFIX          "1234"
#define BIG_STRING_SIZE  1500

int main()
{
    char big_buffer[BIG_STRING_SIZE];
    for ( int i = 0 ; i < BIG_STRING_SIZE ; i++ )
    {
        big_buffer[ i ] = 'a';
    }
    big_buffer[ BIG_STRING_SIZE ] = '\0';

    sp_string_t string = spss_create(big_buffer);

    if ( spss_append_cstr(&string,SUFFIX) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    spss_destroy(&string);

    return 0;
}

