#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#define CHAR        'Z'
#define PREFIX_SIZE 2000

int main()
{
    char prefix[PREFIX_SIZE] = {0};
    for ( int i = 0 ; i < PREFIX_SIZE - 1 ; i++ )
    {
        prefix[ i ] = 'a';
    }
    prefix[ PREFIX_SIZE - 1 ] = '\0';
    sp_string_t string = spss_create(prefix);

    if ( spss_append_char(&string,CHAR) == false )
    {
        TEST_ERROR("Append failed.");
        return 1;
    }

    size_t computed_size = spss_length(&string);
    size_t expected_size = PREFIX_SIZE;

    if ( computed_size != expected_size )
    {
        TEST_ERROR("Computed size(%ld) differs from expected one(%ld)",computed_size,expected_size);
        return 1;
    }

    for ( int i = 0 ; i < PREFIX_SIZE - 1 ; i++ )
    {
        char computed_char = string.heap_string.heap[ i ];
        char expected_char = prefix[ i ];

        if ( computed_char != expected_char )
        {
            TEST_ERROR("Unexpected char at iteration %d.",i);
            TEST_ERROR("Expected: %02X ( ASCII %c)", (unsigned char)expected_char,expected_char);
            TEST_ERROR("Computed: %02X ( ASCII %c)", (unsigned char)computed_char,computed_char);
            return 1;
        }
    }

    char appended = string.heap_string.heap[ PREFIX_SIZE - 1 ];

    if ( appended != CHAR )
    {
        TEST_ERROR("Termination char %c is not present!",CHAR);
        TEST_ERROR("Found instead: %02X ( ASCII %c)", (unsigned char)appended,appended);
        return 1;
    } 

    char termination = string.heap_string.heap[ PREFIX_SIZE ];

    if ( termination != '\0' )
    {
        TEST_ERROR("Termination char is not present!");
        TEST_ERROR("Found instead: %02X ( ASCII %c)", (unsigned char)termination,termination);
        return 1;
    } 

    return 0;
}