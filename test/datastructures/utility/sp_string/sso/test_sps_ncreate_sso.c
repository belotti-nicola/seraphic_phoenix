#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define SSO_BUFFER "1234"

int main()
{
    sp_string_t string_sso = spss_ncreate(SSO_BUFFER,strlen(SSO_BUFFER));

    size_t expected_size = strlen(SSO_BUFFER);
    size_t computed_size = spss_length(&string_sso);

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed size(%ld) is different from expected one(%ld).",computed_size,expected_size);
        return 1;
    }

    for ( int i = 0 ; i < expected_size ; i++)
    {
        char computed_c = string_sso.stack_string.stack[ i ];
        char expected_c = SSO_BUFFER[i];

        if ( computed_c != expected_c )
        {
            TEST_ERROR("Computed char(%c) is different from expected one(%c).",computed_c,expected_c);
            return 1;
        }
    }

    if ( string_sso.stack_string.stack[ expected_size ] != '\0' )
    {
        TEST_ERROR("Termination char is not present!");
        return 1;
    } 

    spss_destroy(&string_sso);

    return 0;
}

