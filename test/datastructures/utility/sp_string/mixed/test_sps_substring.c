#include "internal/datastructures/utility/sp_string.h"
#include "internal/sp_utils.h"

#include "test_utils.h"

#include <string.h>
#include <stdlib.h>

#define TEST_CASES 8

typedef struct substring_case_t
{
    char  *buf;
    size_t pos;
    size_t len;

    bool   expected_res;

    recipe_t recipe;

} substring_case_t;

bool setup_test_cases(substring_case_t *substrings, size_t substring_cases)
{   
    string_builder_t builder = {0};

    for( int i = 0; i < substring_cases ; i++ )
    {
        recipe_t recipe = substrings[i].recipe;
        size_t expected_size = string_recipe_expected_size(&recipe) + 1;
        
        char *ptr = malloc(expected_size);
        if ( ptr == NULL ) return false;

        if ( string_builder_init(&builder,ptr,expected_size) == false )
        {
            TEST_ERROR("Error at iteration %d: string_builder_init failed.",i);
            return false;
        }

        if ( string_builder_recipe(&builder,&recipe) == false )
        {
            TEST_ERROR("Error at iteration %d: string_builder_recipe failed.",i);
            return false;
        }

        substrings[i].buf = ptr;
    }

    return true;
}

int main()
{
    substring_case_t substrings_cases[TEST_CASES] = {0};
    
    substrings_cases[0].expected_res         = true;
    substrings_cases[0].pos                  = 10;
    substrings_cases[0].len                  = 20;
    substrings_cases[0].recipe.gen_token[0]  = GEN_ALPHA;
    substrings_cases[0].recipe.gen_size[0]   = 33;
    substrings_cases[0].recipe.gen_token[1]  = GEN_DIGITS;
    substrings_cases[0].recipe.gen_size[1]   = 33;
    substrings_cases[0].recipe.entries       = 2;
    
    substrings_cases[1].expected_res         = true;
    substrings_cases[1].pos                  = 10;
    substrings_cases[1].len                  = 20;
    substrings_cases[1].recipe.gen_token[0]  = GEN_COUNTER;
    substrings_cases[1].recipe.gen_size[0]   = 100;
    substrings_cases[1].recipe.gen_token[1]  = GEN_SAME;
    substrings_cases[1].recipe.gen_size[1]   = 8;
    substrings_cases[1].recipe.entries       = 2;

    substrings_cases[2].expected_res         = true;
    substrings_cases[2].pos                  = 10;
    substrings_cases[2].len                  = 20;
    substrings_cases[2].recipe.gen_token[0]  = GEN_COUNTER;
    substrings_cases[2].recipe.gen_size[0]   = 100;
    substrings_cases[2].recipe.gen_token[1]  = GEN_SAME;
    substrings_cases[2].recipe.gen_size[1]   = 8;
    substrings_cases[2].recipe.entries       = 2;

    substrings_cases[3].expected_res         = true;
    substrings_cases[3].pos                  = 10;
    substrings_cases[3].len                  = 20;
    substrings_cases[3].recipe.gen_token[0]  = GEN_COUNTER;
    substrings_cases[3].recipe.gen_size[0]   = 100;
    substrings_cases[3].recipe.gen_token[1]  = GEN_SAME;
    substrings_cases[3].recipe.gen_size[1]   = 8;
    substrings_cases[3].recipe.entries       = 2;

    substrings_cases[4].expected_res         = true;
    substrings_cases[4].pos                  = 10;
    substrings_cases[4].len                  = 20;
    substrings_cases[4].recipe.gen_token[0]  = GEN_CASE_MIX;
    substrings_cases[4].recipe.gen_size[0]   = 11;
    substrings_cases[4].recipe.gen_token[1]  = GEN_SAME;
    substrings_cases[4].recipe.gen_size[1]   = 3;
    substrings_cases[4].recipe.gen_token[2]  = GEN_DIGITS;
    substrings_cases[4].recipe.gen_size[2]   = 44;
    substrings_cases[4].recipe.gen_token[3]  = GEN_SAME;
    substrings_cases[4].recipe.gen_size[3]   = 5;
    substrings_cases[4].recipe.entries       = 4;

    substrings_cases[5].expected_res         = true;
    substrings_cases[5].pos                  = 10;
    substrings_cases[5].len                  = 20;
    substrings_cases[5].recipe.gen_token[0]  = GEN_SAME;
    substrings_cases[5].recipe.gen_size[0]   = 1000;
    substrings_cases[5].recipe.entries       = 1;

    substrings_cases[6].expected_res         = true;
    substrings_cases[6].pos                  = 500;
    substrings_cases[6].len                  = 55;
    substrings_cases[6].recipe.gen_token[0]  = GEN_SAME;
    substrings_cases[6].recipe.gen_size[0]   = 20;
    substrings_cases[6].recipe.gen_token[1]  = GEN_DIGITS;
    substrings_cases[6].recipe.gen_size[1]   = 400;
    substrings_cases[6].recipe.gen_token[2]  = GEN_CASE_MIX;
    substrings_cases[6].recipe.gen_size[2]   = 500;
    substrings_cases[6].recipe.gen_token[3]  = GEN_COUNTER;
    substrings_cases[6].recipe.gen_size[3]   = 500;
    substrings_cases[6].recipe.gen_token[4]  = GEN_SAME;
    substrings_cases[6].recipe.gen_size[4]   = 100;
    substrings_cases[6].recipe.entries       = 5;

    substrings_cases[7].expected_res         = true;
    substrings_cases[7].pos                  = 15;
    substrings_cases[7].len                  = 20;
    substrings_cases[7].recipe.gen_token[0]  = GEN_CASE_MIX;
    substrings_cases[7].recipe.gen_size[0]   = 17;
    substrings_cases[7].recipe.gen_token[1]  = GEN_DIGITS;
    substrings_cases[7].recipe.gen_size[1]   = 17;
    substrings_cases[7].recipe.entries       = 2;

    bool b = setup_test_cases(substrings_cases,TEST_CASES);
    if ( b == false )
    {
        TEST_ERROR("Error creating test cases.");
        return 1;
    }

    for ( int i = 0 ; i < TEST_CASES ; i ++ )
    {
        char *buf  = substrings_cases[ i ].buf;
        size_t pos = substrings_cases[ i ].pos;
        size_t len = substrings_cases[ i ].len;
        bool success = substrings_cases[ i ].expected_res;

        sp_string_t string = spss_create(buf);

        sp_string_t substring = spss_substring(&string,pos,len);

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

        if ( strncmp(computed_cstr,expected_cstr,cmp_size) != 0 )
        {
            TEST_ERROR("Error at iteration %d",i);
            TEST_ERROR("spss_equals_cstr is false!");
            TEST_ERROR("Computed: %s",computed_cstr);
            TEST_ERROR("Expected: %.*s",(int)len,expected_cstr);
            return 1;
        }
        
        spss_destroy(&string);

        TEST_INFO("Test case %d: success.",i);
    }

    for ( int i = 0 ; i < TEST_CASES ; i ++ )
    {
        char *tmp = substrings_cases[i].buf;
        free(tmp);
    }

    return 0;
}