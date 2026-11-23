#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define PREFIX_SIZE 2000
#define SUFFIX_SIZE 3200

int main()
{
    char prefix[PREFIX_SIZE] = {0};
    for ( int i = 0 ; i < PREFIX_SIZE - 1 ; i++ )
    {
        prefix[ i ] = 'a';
    }
    prefix[ PREFIX_SIZE - 1 ] = '\0';
    sp_string_t string = spss_create(prefix);

    char suffix[SUFFIX_SIZE] = {0};
    for ( int i = 0 ; i < SUFFIX_SIZE - 2 ; i++ )
    {
        suffix[ i ] = 'b';
    }
    suffix[ SUFFIX_SIZE ] = '\0';

    if ( spss_append_cstr(&string,suffix) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    for ( int i = 0; i < PREFIX_SIZE - 1 ; i++ )
    {
        char computed = string.heap_string.heap[ i ];
        char expected = 'a';

        if ( computed != expected )
        {
            TEST_ERROR("Unexpected char at iteration %d.",i);
            TEST_ERROR("Expected: %02X ( ASCII %c)", (unsigned char)expected,expected);
            TEST_ERROR("Computed: %02X ( ASCII %c)", (unsigned char)computed,computed);
            return 1;
        }
    }

    for ( int i = 0 ; i < SUFFIX_SIZE - 1 ; i++ )
    {
        size_t offset   = PREFIX_SIZE - 1 ;
        size_t i_offset = offset + i;

        char computed = string.heap_string.heap[ i_offset ];
        char expected = suffix[ i ];
        
        if ( computed != expected )
        {
            TEST_ERROR("Unexpected char at iteration %d.",i);
            TEST_ERROR("Expected: %02X ( ASCII %c)", (unsigned char)expected,expected);
            TEST_ERROR("Computed: %02X ( ASCII %c)", (unsigned char)computed,computed);
            return 1;
        }
    }

    spss_destroy(&string);

    return 0;
}

