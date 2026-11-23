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

    sp_string_t string_big = spss_create(big_buffer);

    size_t computed_size = spss_capacity(&string_big);
    size_t expected_size = ( BIG_BUFFER_SIZE - 1 ) * 2;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed sisze(%ld) differs from expected one(%ld)",computed_size,expected_size);
        return 1;
    }

    for ( int i = 0 ; i < BIG_BUFFER_SIZE - 1 ; i++)
    {
        char computed_c = string_big.heap_string.heap[ i ];
        char expected_c = big_buffer[i];

        if ( computed_c != expected_c )
        {
            TEST_ERROR("Computed char is different from expected one at iteration %d.",i);
            TEST_ERROR("Computed: %02X", (unsigned char)computed_c);
            TEST_ERROR("Expected: %02X", (unsigned char)expected_c);
            return 1;
        }
    }

    if ( string_big.heap_string.heap[ BIG_BUFFER_SIZE - 1 ] != '\0' )
    {
        TEST_ERROR("Termination char is not present!");
        return 1;
    } 

    spss_destroy(&string_big);

    return 0;
}

