#include "internal/datastructures/sp_queue.h"

#include <stdlib.h>
#include <string.h>

bool sp_queue_setup(sp_queue_t *q,uint8_t *buffer, size_t max_dim)
{
  if ( q == NULL || buffer == NULL || max_dim == 0 ) return false;

  q->last_index  = 0;
  q->first_index = 0;
  q->size        = 0;
  q->max_size    = max_dim;

  return sb_init(&q->safe_buffer,buffer,max_dim);
}

bool sp_queue_pop(sp_queue_t *q,uint8_t *out, size_t out_size)
{
  if(q == NULL || out == NULL || out_size == 0) return false;

  size_t  offset  = q->first_index;
  uint8_t *target = q->safe_buffer.buffer + offset;
  size_t      len = q->size;
  if( len > q->max_size ) return false;

  if ( sb_copy_out(&q->safe_buffer,offset,out,out_size) == false ) return false;

  offset = (offset + 1) % q->max_size;

  q->first_index = offset;
  q->size--;

  return true;
}

bool sp_queue_push(sp_queue_t *q, const uint8_t *out, size_t out_size)
{
  if(q->size >= q->max_size) return false;

  size_t   offset = q->last_index;
  uint8_t *target = q->safe_buffer.buffer + offset;
  
  if ( sb_copy_in(&q->safe_buffer,offset,out,out_size) == false ) return false;
  
  q->last_index = (offset + 1) % q->max_size;
  q->size++;

  return true;
}