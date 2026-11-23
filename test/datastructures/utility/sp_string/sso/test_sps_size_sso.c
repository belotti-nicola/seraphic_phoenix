#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define SSO_BUFFER "12345"

int main()
{
    sp_string_t string_sso = spss_create(SSO_BUFFER);

    size_t computed_len = spss_length(&string_sso);
    size_t expected_len = strlen(SSO_BUFFER);

    if ( computed_len != expected_len )
    {
        TEST_ERROR("Computed size(%ld) differs from expected one(%ld)",computed_len,expected_len);
        return 1;        
    }
   
    spss_destroy(&string_sso);

    return 0;
}

