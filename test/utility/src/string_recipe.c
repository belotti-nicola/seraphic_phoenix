#include "string_recipe.h"

size_t string_recipe_expected_size( recipe_t *recipe )
{
    if ( recipe == NULL ) return 0;

    if ( recipe->entries == 0 ) return 0;

    size_t curr_size = recipe->entries;
    size_t counter   = 0;

    for ( int i = 0 ; i < curr_size ; i++ )
    {
        counter += recipe->gen_size[ i ];
    }

    return counter;
}

bool string_recipe_add_entry(recipe_t *recipe, gen_kind_t gen_token, size_t gen_size)
{
    if ( recipe == NULL ) return false;

    if ( gen_size == 0 ) return false;

    if ( recipe->entries >= RECIPE_MAX_TOKENS ) return false;

    size_t curr_offset = recipe->entries;

    recipe->gen_token[curr_offset] = gen_token;
    recipe->gen_size[curr_offset]  = gen_size;

    return false;    
}