#include <internal/datastructures/cas/mpsc_stream_buffer.h>

#include <sched.h>
#include <limits.h>

size_t internal_compute_current_size(sp_mpsc_streambuffer *sb, size_t start, size_t end)
{
    return end - start;
}

bool sp_mpsc_sb_init(sp_mpsc_streambuffer *mpsc_sb, safe_buffer_t *safe_buffer)
{
    if (mpsc_sb == NULL || safe_buffer == NULL ) return false;

    mpsc_sb->buffer = safe_buffer;

    atomic_init( &mpsc_sb->producers_index  , 0);
    atomic_init( &mpsc_sb->producers_commit , 0);
    atomic_init( &mpsc_sb->consumer_index   , 0);

    mpsc_sb->futex_empty = 0;
    mpsc_sb->futex_full  = 1;
    
    return true;
}

bool sp_mpsc_sb_push(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    if (sb == NULL || buff == NULL || buff_size == 0) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0 || buff_size > max_size) return false;

    uint64_t producers_i,consumer_i,current_size;

    size_t desired_i,desired_c;

    // ====================================
    // ======== RESERVATION LOOP ==========
    // ====================================
    do
    {
        producers_i = atomic_load(&sb->producers_index);
        consumer_i  = atomic_load(&sb->consumer_index);

        if ( internal_compute_current_size(sb,consumer_i, producers_i) + buff_size > max_size )
        {           
            futex_t expected_futex = atomic_load(&sb->futex_full);
            futex_wait(&sb->futex_full,expected_futex);
            continue;
        }

        desired_i = producers_i + buff_size;

    } while ( atomic_compare_exchange_strong( &sb->producers_index, &producers_i, desired_i ) == false ) ;    
    
    // ====================================
    // ======== COPY INSTRUCTION ==========
    // ====================================
    if ( sb_copy_in(sb->buffer, producers_i % max_size, buff, buff_size) == false ) 
    {
        return false;
    }

    // ====================================
    // ========== COMMIT PHASE ============
    // ====================================
    while ( atomic_load_explicit(&sb->producers_commit, memory_order_acquire) != producers_i )
    {
        sched_yield();
    }
    atomic_store_explicit(&sb->producers_commit, producers_i + buff_size, memory_order_release);

    // ====================================
    // ========== UPDATE FUTEX ============
    // ====================================
    atomic_fetch_add(&sb->futex_empty, 1);

    futex_wake(&sb->futex_empty, INT_MAX);

    return true;
}

bool sp_mpsc_sb_pop(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    if (sb == NULL || buff == NULL || buff_size == 0) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0 || buff_size > max_size) return false;

    size_t producers_i, consumer_i;

    // ====================================
    // =========== CHECK DATA =============
    // ====================================
    while (true)
    {
        consumer_i  = atomic_load_explicit(&sb->consumer_index, memory_order_relaxed);
        producers_i = atomic_load_explicit(&sb->producers_commit, memory_order_acquire);

        if (internal_compute_current_size(sb, consumer_i, producers_i) >= buff_size)
        {
            break;
        }

        futex_t futex_status = atomic_load(&sb->futex_empty);
        
        // avoid lost wake ups.
        consumer_i  = atomic_load_explicit(&sb->consumer_index, memory_order_relaxed);
        producers_i = atomic_load_explicit(&sb->producers_commit, memory_order_acquire);
        if (internal_compute_current_size(sb, consumer_i, producers_i) >= buff_size)
        {
            break;
        }

        futex_wait(&sb->futex_empty, futex_status);
    }

    // ====================================
    // ======== COPY INSTRUCTION ==========
    // ====================================
    if (sb_copy_out(sb->buffer, consumer_i % max_size, buff, buff_size) == false) 
    {
        return false;
    }

    // ====================================
    // ======== UPDATE CONSUMER ===========
    // ====================================
    
    atomic_fetch_add_explicit(&sb->consumer_index, buff_size, memory_order_release);

    atomic_store_explicit(&sb->futex_full, atomic_load(&sb->futex_full) + 1, memory_order_release);

    futex_wake(&sb->futex_full, INT_MAX);

    return true;
}
bool sp_mpsc_sb_peek(sp_mpsc_streambuffer *sb, uint8_t *out, size_t size)
{
    if (  sb == NULL ) return false;
 
    if ( out == NULL || size == 0 ) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0 || size > max_size) return false;

    uint64_t offset = atomic_load_explicit( &sb->consumer_index, memory_order_relaxed );

    if ( sb_copy_out(sb->buffer, offset % max_size, out, size) == false )
    {
        return false;
    }

    return true;
}

bool sp_mpsc_sb_advance(sp_mpsc_streambuffer *sb, size_t size)
{
    if (  sb == NULL ) return false;
 
    if ( size == 0 ) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0 || size > max_size) return false;

    uint64_t offset = atomic_load_explicit( &sb->consumer_index, memory_order_acquire );

    atomic_store_explicit( &sb->consumer_index,offset + size, memory_order_release );

    return true;

}