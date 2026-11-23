#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define OFFSET            10
#define BIG_BUFFER_SIZE 3200

int main()
{
    char big_buffer[BIG_BUFFER_SIZE] = {0};
    for ( int i = 0 ; i < BIG_BUFFER_SIZE - 1 ; i++ )
    {
        big_buffer[ i ] = 'a';
    }
    big_buffer[ BIG_BUFFER_SIZE - 1 ] = '\0';
    sp_string_t string = spss_create(big_buffer);

    const char *string_cstr = spss_cstr(&string) + OFFSET ;
    if ( spss_append_cstr( &string, string_cstr ) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    size_t computed_size = spss_length(&string);
    size_t expected_size = BIG_BUFFER_SIZE * 2 - 2 - OFFSET;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed size(%ld) differs from expected one(%ld)",computed_size,expected_size);
        return 1;
    }

    size_t offset = 0;
    for ( int i = 0 ; i < expected_size ; i++ )
    {
        if ( i <  BIG_BUFFER_SIZE - 1 ) offset = i;
        else                            offset = i - ( BIG_BUFFER_SIZE - 1 ) + OFFSET ;

        char expected_char = big_buffer[ offset ];
        char computed_char = string.heap_string.heap[i];

        if ( computed_char != expected_char)
        {
            TEST_ERROR("Unexpected char at iteration %d (offset %ld).",i,offset);
            TEST_ERROR("Expected: %02X ( ASCII '%c')", (unsigned char)expected_char,expected_char);
            TEST_ERROR("Computed: %02X ( ASCII '%c')", (unsigned char)computed_char,computed_char);

            return 1;
        }
    }

    spss_destroy(&string);

    return 0;
}

