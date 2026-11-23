#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

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

    size_t computed_len = spss_length(&string_big);
    size_t expected_len = BIG_BUFFER - 1;

    if ( computed_len != expected_len )
    {
        TEST_ERROR("Computed length(%ld) is different from expected(%ld)",computed_len,expected_len);
        return 1;
    }

    for ( int i = 0 ; i < BIG_BUFFER ; i ++ )
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

    if ( string_big.heap_string.heap[ BIG_BUFFER - 1 ] != '\0' )
    {
        TEST_ERROR("Termination char is not present!");
        return 1;
    }

    spss_destroy(&string_big);

    TEST_INFO("Test success.");
    return 0;
}

