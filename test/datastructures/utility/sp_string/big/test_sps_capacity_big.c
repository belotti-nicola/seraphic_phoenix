#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define BIG_BUFFER_SIZE 2048

int main()
{
    char big_buffer[BIG_BUFFER_SIZE] = {0};
    for ( int i = 0 ; i < BIG_BUFFER_SIZE ; i ++ )
    {
        big_buffer[ i ] = 'a';
    }
    big_buffer[ BIG_BUFFER_SIZE - 1 ] = '\0';

    sp_string_t string_sso = spss_create(big_buffer);

    size_t computed_size = spss_capacity(&string_sso);
    size_t expected_size = ( BIG_BUFFER_SIZE - 1 ) * 1.5;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed sisze(%ld) differs from expected one(%ld)",computed_size,expected_size);
        return 1;
    }

    spss_destroy(&string_sso);

    return 0;
}

