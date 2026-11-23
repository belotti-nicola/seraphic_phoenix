#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define BIG_STRING_SIZE 4096

int main()
{
    char big_buffer[BIG_STRING_SIZE];
    for ( int i = 0 ; i < BIG_STRING_SIZE ; i++ )
    {
        big_buffer[ i ] = 'a';
    }
    big_buffer[ BIG_STRING_SIZE - 1 ] = '\0';

    sp_string_t string = spss_create(big_buffer);

    sp_string_t cloned = spss_clone(&string);

    size_t original_size = spss_length(&string);
    size_t cloned_size   = spss_length(&string);

    if ( original_size != cloned_size )
    {
        TEST_ERROR("Cloned size(%ld) differs from original size(%ld)",cloned_size,original_size);
        return 1;
    }

    for ( int i = 0 ; i < BIG_STRING_SIZE ; i ++ )
    {
        char original_c = string.heap_string.heap[ i ];
        char cloned_c   = cloned.heap_string.heap[ i ];

        if ( original_c != cloned_c )
        {
            TEST_ERROR("Computed char is different from expected one at iteration %d.",i);
            TEST_ERROR("Computed: %02X", (unsigned char)original_c);
            TEST_ERROR("Expected: %02X", (unsigned char)cloned_c);
            return 1;
        }
    }

    if ( cloned.heap_string.heap[ BIG_STRING_SIZE - 1 ] != '\0' )
    {
        TEST_ERROR("Termination char is not present!");
        return 1;
    }

    spss_destroy(&string);
    spss_destroy(&cloned);

    return 0;
}

