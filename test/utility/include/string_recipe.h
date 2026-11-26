#pragma once

#include "string_common_defs.h"

#include <stdbool.h>
#include <stddef.h>

#define RECIPE_MAX_TOKENS 10

typedef struct recipe_t 
{
    gen_kind_t gen_token[RECIPE_MAX_TOKENS];
    size_t     gen_size[RECIPE_MAX_TOKENS];
    size_t     entries;

} recipe_t;

bool string_recipe_add_entry(recipe_t *recipe, gen_kind_t gen_token, size_t gen_size);
size_t string_recipe_expected_size( recipe_t *recipe );