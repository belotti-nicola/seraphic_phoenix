#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define SSO_BUFFER      "1234"
#define BIG_BUFFER_SIZE  4096

int main()
{
    char big_buffer[BIG_BUFFER_SIZE] = {0};
    for ( int i = 0 ; i < BIG_BUFFER_SIZE - 1 ; i++ )
    {
        big_buffer[i] = 'a';
    }
    big_buffer[BIG_BUFFER_SIZE - 1] = '\0';

    sp_string_t string_sso = spss_ncreate(SSO_BUFFER,strlen(SSO_BUFFER));
    sp_string_t string_big = spss_ncreate(big_buffer,BIG_BUFFER_SIZE);

    

    spss_destroy(&string_sso);
    spss_destroy(&string_big);

    return 0;
}

