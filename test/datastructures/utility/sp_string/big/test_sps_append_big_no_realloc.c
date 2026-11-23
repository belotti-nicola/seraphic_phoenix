#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define SUFFIX          "1234"
#define BIG_STRING_SIZE  1500

int main()
{
    char big_buffer[BIG_STRING_SIZE] = {0};
    for ( int i = 0 ; i < BIG_STRING_SIZE ; i ++ )
    {
        big_buffer[ i ] = 'a';
    }
    big_buffer[ BIG_STRING_SIZE - 1 ] = '\0';

    sp_string_t string = spss_create(big_buffer);

    if ( spss_nappend_cstr(&string,SUFFIX,strlen(SUFFIX)) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    size_t computed_size = spss_capacity(&string);
    size_t expected_size = ( BIG_STRING_SIZE - 1 ) * 2;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed sisze(%ld) differs from expected one(%ld)",computed_size,expected_size);
        return 1;
    }

    size_t available_data = spss_length(&string);
    size_t expected_data  = strlen(SUFFIX) + BIG_STRING_SIZE - 1;

    if ( available_data != expected_data )
    {
        TEST_ERROR("Computed sisze(%ld) differs from expected one(%ld)",available_data,expected_data);
        return 1;
    }

    size_t prefix_size = BIG_STRING_SIZE ;
    for ( int i = 0 ; i < prefix_size - 1 ; i++)
    {   
        char computed_c = string.heap_string.heap[ i ];
        char expected_c = big_buffer[i];

        if ( computed_c != expected_c )
        {
            TEST_ERROR("Computed char is different from expected one at iteration %d.",i);
            TEST_ERROR("Computed: %02X", (unsigned char)computed_c);
            TEST_ERROR("Expected: %02X", (unsigned char)expected_c);
            return 1;
        }
    }
    
    size_t suffix_size = strlen(SUFFIX) ;
    for ( int i = 0 ; i < suffix_size - 1 ; i++)
    {   
        char computed_c = string.heap_string.heap[ i ];
        char expected_c = big_buffer[i];

        if ( computed_c != expected_c )
        {
            TEST_ERROR("Computed char is different from expected one at iteration %d.",i);
            TEST_ERROR("Computed: %02X", (unsigned char)computed_c);
            TEST_ERROR("Expected: %02X", (unsigned char)expected_c);
            return 1;
        }
    }

    char termination_c = string.heap_string.heap[ available_data ];
    if ( termination_c != '\0' )
    {
        TEST_ERROR("Termination char is not present!");
        TEST_ERROR("Found: %02X", (unsigned char)termination_c);
        return 1;
    } 

    spss_destroy(&string);

    return 0;
}

