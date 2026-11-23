#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#include <stdbool.h>

#define BUFFER "1234567890123"

int main()
{
    sp_string_t string_sso = spss_create(BUFFER);

    bool b  = spss_pop_back(&string_sso);
    if ( b == false )
    {
        TEST_ERROR("Pop back fail.");
        return 1;
    }

    if ( spss_length(&string_sso) != strlen(BUFFER) - 1 )
    {
        TEST_ERROR("Expected size differs from computed one.");
        TEST_ERROR("%ld",spss_length(&string_sso));
        return 1;
    }

    spss_destroy(&string_sso);

    TEST_INFO("Test success.");
    return 0;
}

