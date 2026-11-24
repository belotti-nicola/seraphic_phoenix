#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define INSERT_TXT   "456"
#define INSERT_OFFSET 3

#define TESTBUFFER "1237890123456789012345678901234567890"

int main()
{
    sp_string_t string = spss_create(TESTBUFFER);

    bool b = spss_ninsert(&string,INSERT_OFFSET,INSERT_TXT,strlen(INSERT_TXT));
    if ( b == false )
    {
        TEST_ERROR("Error spss_ninsert.");
        return 1;
    }

    if ( spss_equals_cstr(&string,"1234567890123456789012345678901234567890") == false )
    {
        TEST_ERROR("Error spss_ninsert output.");
        return 1;
    }

    spss_destroy(&string);

    return 0;
}

