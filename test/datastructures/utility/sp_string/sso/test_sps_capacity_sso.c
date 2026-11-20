#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define SSO_BUFFER      "1234"

int main()
{
    sp_string_t string_sso = spss_create(SSO_BUFFER);

    size_t computed_size = spss_capacity(&string_sso);
    size_t expected_size = SSO_SIZE;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed sisze(%ld) differs from expected one(%ld)",computed_size,expected_size);
        return 1;
    }

    spss_destroy(&string_sso);

    return 0;
}

