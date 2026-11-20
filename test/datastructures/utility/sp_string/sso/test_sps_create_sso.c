#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#define BIG_BUFFER 4096

int main()
{
    char big_buffer[BIG_BUFFER] = {0};
    for ( int i = 0 ; i < BIG_BUFFER - 1 ; i++ )
    {
        big_buffer[i] = 'a';
    }
    big_buffer[BIG_BUFFER - 1] = '\0';

    sp_string_t string_sso = spss_create("1234");
    sp_string_t string_big = spss_create(big_buffer);

    spss_destroy(&string_sso);
    spss_destroy(&string_big);

    return 0;
}

