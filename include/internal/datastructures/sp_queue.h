#ifndef SP_QUEUE_H
#define SP_QUEUE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "internal/datastructures/utility/safe_linear_buffer.h"

typedef struct sp_queue_t 
{
    size_t first_index;
    size_t last_index;
    
    size_t        size;
    size_t        max_size;
    safe_buffer_t safe_buffer;

} sp_queue_t;

bool    sp_queue_setup(sp_queue_t *q,uint8_t *buffer, size_t max_dim);
bool    sp_queue_pop(sp_queue_t *q, uint8_t *out, size_t out_size);
bool    sp_queue_push(sp_queue_t *q, const uint8_t *buffer,size_t buffer_size);


#endif