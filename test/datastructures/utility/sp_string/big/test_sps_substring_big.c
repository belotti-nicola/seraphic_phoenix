#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include "test_utils.h"

#include <string.h>
#include <stdlib.h>

#define TEST_CASES 10

typedef struct substring_case_t
{
    char  *buf;
    size_t buf_size;
    size_t pos;
    size_t len;

    bool   expected_res;

} substring_case_t;

static char *make_buf(size_t n)
{
    char *b = malloc(n + 1);
    if ( b == NULL ) return NULL;
    for ( size_t i = 0; i < n; i++ ) b[i] = (char)('a' + (i % 26));
    b[n] = '\0';
    return b;
};

bool setup_test_cases(substring_case_t *substrings, size_t size)
{
    for( int i = 0; i < size ; i++ )
    {
        char *ptr = malloc(1024);
        if ( ptr == NULL ) return false;

        substrings[i].buf = ptr;
    }

    return true;
}

int main()
{
    substring_case_t substrings_cases[TEST_CASES] = {0};
    bool b = setup_test_cases(substrings_cases,TEST_CASES);
    if ( b == false )
    {
        TEST_ERROR("Error creating test cases.");
        return 1;
    }


    for ( int i = 0 ; i < TEST_CASES ; i ++ )
    {
        size_t dim = substrings_cases[ i ].buf_size;
        size_t pos = substrings_cases[ i ].pos;
        size_t len = substrings_cases[ i ].len;
        bool success = substrings_cases[ i ].expected_res;

        char *buf = make_buf(dim);

        sp_string_t string = spss_create(buf);

        sp_string_t substring = spss_substring(&string,pos,len);

        if ( success == false ) continue; 

        size_t computed_size = spss_length(&substring);
        size_t expected_size = len;

        if ( computed_size != expected_size ) 
        {
            TEST_ERROR("Error at iteration %d",i);
            TEST_ERROR("Computed size (%ld) differs from expected one(%ld)",computed_size, expected_size);
            return 1;
        }

        char *computed_cstr = (char *)spss_cstr(&substring);
        char *expected_cstr = buf + pos;
        size_t cmp_size     = len; 

        if ( strncmp(computed_cstr,expected_cstr,cmp_size) == false )
        {
            TEST_ERROR("Error at iteration %d",i);
            TEST_ERROR("spss_equals_cstr is false!");
            TEST_ERROR("Computed: %s",computed_cstr);
            TEST_ERROR("Expected: %.*s",(int)len,expected_cstr);
            return 1;
        }
        
        spss_destroy(&string);

        free(buf);

        TEST_INFO("Test case %d: success.",i);
    }

    for ( int i = 0 ; i < TEST_CASES ; i ++ )
    {
        char *tmp = substrings_cases[i].buf;
        free(tmp);
    }

    return 0;
}