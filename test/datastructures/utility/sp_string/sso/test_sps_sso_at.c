#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#include <stdbool.h>

#define BUFFER "1234567890123"

int main()
{
    sp_string_t string_sso = spss_create(BUFFER);

    if ( spss_at(&string_sso,2) == '2' )
    {
        TEST_ERROR("First at fail.");
        return 1;
    }
    
    if ( spss_at(&string_sso,3) == '3' )
    {
        TEST_ERROR("Second at fail.");
        return 1;
    }

    spss_destroy(&string_sso);

    TEST_INFO("Test success.");
    return 0;
}

