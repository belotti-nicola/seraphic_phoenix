#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include "test_utils.h"

#include <string.h>
#include <stdbool.h>

#define MAX_TESTS 100

size_t test_compute_size_check(size_t size, size_t pos, size_t len)
{
    if ( pos == 0 && size < len ) return 0;

    if ( pos + len > size ) return pos;

    return size - len;
}

typedef struct nerase_data_t
{
    char   *buf;
    size_t  pos;
    size_t  len;
    bool    exit_code;

    recipe_t buf_recipe;

} nerase_data_t;

int main()
{
    test_cases_t tests;
    size_t test_size = sizeof(nerase_data_t);

    if ( test_cases_setup(&tests,MAX_TESTS, test_size) == false )
    {
        TEST_ERROR("test_cases_setup error.");
        return 1;
    }

    nerase_data_t nerase_data_test[] ={ 
        { 
            .pos = 2, .len = 5, .exit_code = true,
            .buf_recipe = { .gen_token = { GEN_ALPHA, GEN_DIGITS }, .gen_size = { 33, 33 }, .entries = 2 }
        },
        { 
            .pos = 11, .len = 8, .exit_code = true,
            .buf_recipe = { .gen_token = { GEN_COUNTER, GEN_SAME, GEN_DIGITS ,GEN_SAME }, .gen_size =  { 11,2,3,2}, .entries = 4 }
        },
        { 
            .pos = 0, .len = 2, .exit_code = true,
            .buf_recipe = { .gen_token = { GEN_CASE_MIX, GEN_SAME, GEN_DIGITS, GEN_SAME }, .gen_size =  { 5,5,1,10}, .entries = 4 }
        }
    };

    string_builder_t builder = {0};
    for ( int i = 0 ; i < sizeof(nerase_data_test) / sizeof(nerase_data_t) ; i++ )
    {
        recipe_t rec = nerase_data_test[ i ].buf_recipe;
        
        size_t malloc_size = string_recipe_expected_size(&rec) + 1;
        char *ptr = (char *)malloc( malloc_size );
        if ( ptr == NULL )
        {
            TEST_ERROR("malloc fail at iteration %d.",i);
            return 1;
        }
        
        if ( string_builder_init(&builder,ptr,malloc_size) == false )
        {
            TEST_ERROR("string_builder_init failed at iteration %d.",i);
            return 1;
        }

        if ( string_builder_recipe(&builder,&rec) == false )
        {
            TEST_ERROR("string_builder_init failed at iteration %d.",i);
            return 1;
        }

        nerase_data_test[i].buf = ptr;

        test_cases_add(&tests,&nerase_data_test[i]);
    }

    nerase_data_t *current = (nerase_data_t *)test_cases_next(&tests);
    if ( current == NULL ) 
    {
        TEST_ERROR("Test set is empty.");
        return 1;
    }

    size_t do_while_i = 0;
    do
    {
        char *buffer = current->buf;
        size_t pos   = current->pos;
        size_t len   = current->len;

        sp_string_t string = spss_create(buffer);
        sp_string_t cloned = spss_clone(&string);

        size_t old_size = spss_length(&string);

        if ( spss_erase(&string,pos,len) == false )
        {
            TEST_ERROR("p fail.");
            return 1;
        }

        size_t computed_size = spss_length(&string);
        size_t expected_size = test_compute_size_check(old_size,pos,len);
        if ( computed_size != expected_size )
        {
            TEST_ERROR("Error at iteration %ld :: spss_erase :: length check fail.",do_while_i);
            TEST_ERROR("Computed %zu",computed_size);
            TEST_ERROR("Expected %zu",expected_size);
            return 1;
        }

        size_t before_bytes = 0;
        if ( pos > 0 ) before_bytes = pos;
        else           before_bytes = 0;

        for ( int before_pos_i = 0 ; before_pos_i < before_bytes ; before_pos_i++ )
        {
            char computed = spss_at( &string, before_pos_i);
            char expected = spss_at( &cloned, before_pos_i);

            if ( computed != expected )
            {
                TEST_ERROR("Error for test sample %ld",do_while_i);
                TEST_ERROR("Checking %d-th char (before pos)",before_pos_i);
                TEST_ERROR("Computed: %c (%02X)",computed,(unsigned char)computed);
                TEST_ERROR("Expected: %c (%02X)",expected,(unsigned char)expected);
                TEST_ERROR("len: %ld",len);
                TEST_ERROR("pos: %ld",pos);
                TEST_ERROR("string computed: %s",spss_cstr(&string));
                TEST_ERROR("string expected: %s",spss_cstr(&cloned));
                return 1;
            }
        }

        size_t after_bytes = 0; // care: underflow 
        if ( pos <=computed_size && len <=computed_size - pos) after_bytes =computed_size - pos - len;
        else                                           after_bytes = 0;

        for ( int after_pos_i = 0 ; after_pos_i < after_bytes ; after_pos_i++ )
        {
            char computed = spss_at( &string, pos + after_pos_i      );
            char expected = spss_at( &cloned, pos + after_pos_i + len);

            if ( computed != expected )
            {
                TEST_ERROR("Error for test sample %ld",do_while_i);
                TEST_ERROR("Checking char %d after pos",after_pos_i);
                TEST_ERROR("Computed: %c",computed);
                TEST_ERROR("Expected: %c",expected);
                TEST_ERROR("len: %ld",len);
                TEST_ERROR("pos: %ld",pos);
                TEST_ERROR("string computed: %s",spss_cstr(&string));
                TEST_ERROR("string expected: %s",spss_cstr(&cloned));
                return 1;
            }
        }

        spss_destroy(&string);
        spss_destroy(&cloned);

        do_while_i++;

        current = test_cases_next(&tests);
    }
    while ( current != NULL);

    test_cases_destroy(&tests);

    TEST_INFO("Test success");
    return 0;

}