#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

int main()
{
    sp_string_t string_sso = spss_create("1234");

    spss_destroy(&string_sso);
    spss_destroy(&string_sso);

    return 0;
}

