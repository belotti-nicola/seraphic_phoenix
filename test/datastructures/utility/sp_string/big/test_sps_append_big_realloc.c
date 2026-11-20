#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define PREFIX_SIZE 3000
#define SUFFIX_SIZE 1200

int main()
{
    char prefix[PREFIX_SIZE] = {0};
    for ( int i = 0 ; i < PREFIX_SIZE ; i++ )
    {
        prefix[ i ] = 'a';
    }
    prefix[ PREFIX_SIZE ] = '\0';

    char suffix[SUFFIX_SIZE] = {0};
    for ( int i = 0 ; i < SUFFIX_SIZE ; i++ )
    {
        suffix[ i ] = 'b';
    }
    suffix[ SUFFIX_SIZE ] = '\0';

    sp_string_t string = spss_create(prefix);

    if ( spss_append_cstr(&string,suffix) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    spss_destroy(&string);

    return 0;
}

