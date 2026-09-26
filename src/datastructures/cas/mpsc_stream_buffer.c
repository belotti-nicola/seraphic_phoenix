#include <internal/datastructures/cas/mpsc_stream_buffer.h>

#include <sched.h>

#define B4_YIELD 10000

bool sp_mpsc_sb_init(sp_mpsc_streambuffer *mpsc_sb, safe_buffer_t *safe_buffer)
{
    if (mpsc_sb == NULL || safe_buffer == NULL ) return false;

    mpsc_sb->buffer = safe_buffer;

    atomic_init(&mpsc_sb->producers_index,0);
    atomic_init(&mpsc_sb->producers_commit,0);
    atomic_init(&mpsc_sb->consumer_index,0);

    mpsc_sb->futex_empty = 0;
    mpsc_sb->futex_full  = 0;
    
    return true;
}

bool sp_mpsc_sb_push(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    if (sb == NULL || buff == NULL || buff_size == 0) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0) return false;

    // RESERVATION
    // no particular reservation loop is needed since the hardware can take care of it
    size_t offset = atomic_fetch_add(&sb->producers_index,buff_size);

    if (sb_copy_in(sb->buffer, offset % max_size, buff, buff_size) == false)
    {
       return false;
    }

    // COMMIT LOOP :
    // acquire/release usage
    // we "sleep" untill offset is the one threads figured (other threads could be still committing or have not committed)
    while ( atomic_load_explicit(&sb->producers_commit, memory_order_acquire) != offset )
    {
        sched_yield();
    }
    atomic_store_explicit(&sb->producers_commit, offset + buff_size, memory_order_release);


    return true;
}

bool sp_mpsc_sb_pop(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    if (sb == NULL || buff == NULL || buff_size == 0) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0) return false;

    uint64_t offset = atomic_load_explicit( &sb->consumer_index, memory_order_relaxed );

    if ( sb_copy_out(sb->buffer,offset,buff,buff_size) == false )
    {
        return false;
    }

    atomic_store_explicit( &sb->consumer_index,offset + buff_size, memory_order_relaxed );

    return true;
}

bool sp_mpsc_sb_peek(sp_mpsc_streambuffer *sb, uint8_t *out, size_t size)
{
    if (  sb == NULL ) return false;
 
    if ( out == NULL || size == 0 ) return false;

    uint64_t offset = atomic_load_explicit( &sb->consumer_index, memory_order_relaxed );

    if ( sb_copy_out(sb->buffer,offset,out,size) == false )
    {
        return false;
    }

    return true;
}

bool sp_mpsc_sb_advance(sp_mpsc_streambuffer *sb, size_t size)
{
    if (  sb == NULL ) return false;
 
    if ( size == 0 ) return false;

    uint64_t offset = atomic_load_explicit( &sb->consumer_index, memory_order_acquire );

    atomic_store_explicit( &sb->consumer_index,offset + size, memory_order_acquire );

    return true;

}