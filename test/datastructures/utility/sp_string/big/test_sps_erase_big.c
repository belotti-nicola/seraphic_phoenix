#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include <stdlib.h>

typedef struct erase_case_t
{
    const int line;
    size_t buf_size;
    size_t pos;
    size_t len;

    bool   expected_res;

} erase_case_t;

static const erase_case_t ERASE_CASES[] =
{
    {
        .line = __LINE__,
        .buf_size = 0,
        .pos = 1,
        .len = 0,
        .expected_res = false
    }
    /*,
    {
        .line = __LINE__,
        .buf_size = 63,
        .pos = 1,
        .len = 0,
        .expected_res = false
    },
    {
        .line = __LINE__,
        .buf_size = 64,
        .pos = 1,
        .len = 1,
        .expected_res = true
    },
    {
        .line = __LINE__,
        .buf_size = 65,
        .pos = 1,
        .len = 3,
        .expected_res = false
    },
    {
        .line = __LINE__,
        .buf_size = 100,
        .pos = 1,
        .len = 4,
        .expected_res = false
    },
    {
        .line = __LINE__,
        .buf_size = 3000,
        .pos = 1,
        .len = 5,
        .expected_res = false
    }
    */

};

static char *make_buf(size_t n)
{
    char *b = malloc(n + 1);
    if ( b == NULL ) return NULL;
    for ( size_t i = 0; i < n; i++ ) b[i] = (char)('a' + (i % 26));
    b[n] = '\0';
    return b;
};

int main()
{
    size_t ERASE_CASES_LEN = sizeof( ERASE_CASES ) / sizeof( erase_case_t );

    for ( int i = 0 ; i < ERASE_CASES_LEN ; i ++ )
    {
        size_t dim = ERASE_CASES[ i ].buf_size;
        size_t pos = ERASE_CASES[ i ].pos;
        size_t len = ERASE_CASES[ i ].len;

        const int line =  ERASE_CASES[ i ].line;

        char *buf = make_buf(dim);

        sp_string_t string = spss_create(buf);

        bool computed_res = spss_erase(&string,pos,len);
        bool expected_res =  ERASE_CASES[ i ].expected_res;

        if ( computed_res != expected_res )
        {
            TEST_ERROR("Error at iteration %d : spss_erase exit code (%s) differs from expected one (%s)",
                i,
                computed_res ? "true" : "false",
                expected_res ? "true" : "false" );
            TEST_ERROR("Test case line: %d",line);
            return 1;
        }

        spss_destroy(&string);

        free(buf);

        TEST_INFO("Test case %d: success.",i);
    }

    return 0;
}