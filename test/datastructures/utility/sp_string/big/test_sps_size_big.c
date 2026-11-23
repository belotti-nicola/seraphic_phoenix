#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define BIG_BUFFER_SIZE 2048

int main()
{
    char big_buffer[BIG_BUFFER_SIZE] = {0};
    for ( int i = 0 ; i < BIG_BUFFER_SIZE - 1 ; i++ )
    {
        big_buffer[i] = 'a';
    }
    big_buffer[BIG_BUFFER_SIZE - 1] = '\0';

    sp_string_t string_big = spss_ncreate(big_buffer,BIG_BUFFER_SIZE - 1);

    size_t computed_len = spss_length(&string_big);
    size_t expected_len = BIG_BUFFER_SIZE - 1;

    if ( computed_len != expected_len )
    {
        TEST_ERROR("Computed size(%ld) differs from expected one(%ld)",computed_len,expected_len);
        return 1;        
    }
   
    for ( int i = 0 ; i < BIG_BUFFER_SIZE ; i ++ )
    {
        char computed = string_big.heap_string.heap[ i ];
        char expected = big_buffer[ i ];

        if ( computed != expected )
        {
            TEST_ERROR("Computed char is different from expected one at iteration %d.",i);
            TEST_ERROR("Computed: %02X", (unsigned char)computed);
            TEST_ERROR("Expected: %02X", (unsigned char)expected);
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

