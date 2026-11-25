#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct string_builder_t
{
    char    *buffer;
    size_t   buffer_size;
    size_t   offset;
    size_t   current_size;

} string_builder_t;

void string_encoder_init(string_builder_t *encoder,char *data,size_t data_size);