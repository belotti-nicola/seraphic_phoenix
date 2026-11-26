#pragma once

#include "string_common_defs.h"
#include "string_recipe.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef struct string_builder_t
{
    char    *buffer;
    size_t   buffer_size;
    size_t   current_size;

} string_builder_t;

bool string_builder_init(string_builder_t *encoder,char *data,size_t data_size);
bool string_builder_add(string_builder_t *encoder, gen_kind_t kind, uint64_t seed, size_t n);
bool string_builder_recipe(string_builder_t *encoder, recipe_t *recipe);
