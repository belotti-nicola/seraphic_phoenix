#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <stdbool.h>

#define TESTBUFFER "1234567890"

int main()
{
    sp_string_t string = spss_create(TESTBUFFER);

    bool b = spss_append_cstr(&string,TESTBUFFER);
    if ( b == false )
    {
        TEST_ERROR("Could not append string.");
        return 1;
    }

    size_t expected = 20;
    if ( spss_length(&string) != expected )
    {
        TEST_ERROR("Computed length(%ld) is different from expected(%ld)",spss_length(&string),expected);
        return 1;
    }

    spss_destroy(&string);

    return 0;
}

