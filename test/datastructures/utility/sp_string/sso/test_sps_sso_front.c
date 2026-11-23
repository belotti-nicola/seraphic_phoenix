#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#include <stdbool.h>

#define BUFFER "1234567890123"

int main()
{
    sp_string_t string_sso = spss_create(BUFFER);

    if ( spss_set_at(&string_sso,1,'b') == false )
    {
        TEST_ERROR("First set fail.");
        return 1;
    }
    
    if ( spss_set_at(&string_sso,2,'c') == false )
    {
        TEST_ERROR("Second set fail.");
        return 1;
    }
    
    if ( spss_set_at(&string_sso,3,'d') == false )
    {
        TEST_ERROR("Third set fail.");
        return 1;
    }

    const char *cstr = spss_cstr(&string_sso);

    if ( strlen(cstr) != strlen(BUFFER) )
    {
        TEST_ERROR("Unexpected strlen.");
        return 1;
    }

    if ( cstr[1] != 'b' )
    {
        TEST_ERROR("First set fail.");
        return 1;
    }

    if ( cstr[2] != 'c' )
    {
        TEST_ERROR("Second set fail.");
        return 1;
    }

    if ( cstr[3] != 'd' )
    {
        TEST_ERROR("Third set fail.");
        return 1;
    }

    spss_destroy(&string_sso);

    TEST_INFO("Test success.");
    return 0;
}

