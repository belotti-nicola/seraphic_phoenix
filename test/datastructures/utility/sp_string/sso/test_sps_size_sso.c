#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>

#define SSO_BUFFER_SIZE 512

int main()
{
    char big_buffer[SSO_BUFFER_SIZE] = {0};
    for ( int i = 0 ; i < SSO_BUFFER_SIZE - 1 ; i++ )
    {
        big_buffer[i] = 'a';
    }
    big_buffer[SSO_BUFFER_SIZE - 1] = '\0';

    sp_string_t string_sso = spss_ncreate(big_buffer,SSO_BUFFER_SIZE - 1);

    size_t computed_len = spss_length(&string_sso);
    size_t expected_len = SSO_BUFFER_SIZE - 1;

    if ( computed_len != expected_len )
    {
        TEST_ERROR("Computed size(%ld) differs from expected one(%ld)",computed_len,expected_len);
        return 1;        
    }
   
    spss_destroy(&string_sso);

    return 0;
}

