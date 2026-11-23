#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define PREFIX_SIZE 50
#define SUFFIX_SIZE 500

int main()
{
    char prefix[PREFIX_SIZE] = {0};
    for ( int i = 0 ; i < PREFIX_SIZE ; i++ )
    {
        prefix[ i ] = 'a';
    }
    prefix[ PREFIX_SIZE - 1 ] = '\0';

    char suffix[SUFFIX_SIZE] = {0};
    for ( int i = 0 ; i < SUFFIX_SIZE ; i++ )
    {
        suffix[ i ] = 'b';
    }
    suffix[ SUFFIX_SIZE - 1 ] = '\0';

    sp_string_t string = spss_create(prefix);

    size_t computed_size = spss_capacity(&string);
    size_t expected_size = SSO_SIZE - 1;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Expected size(%ld) differs from computed one(%ld).",expected_size,computed_size);
        return 1;
    }

    if ( spss_append_cstr(&string,suffix) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    computed_size = spss_length(&string);
    expected_size = PREFIX_SIZE + SUFFIX_SIZE - 2;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Expected size(%ld) differs from computed one(%ld).",expected_size,computed_size);
        return 1;
    }

    spss_destroy(&string);

    return 0;
}

