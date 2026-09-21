#include <internal/datastructures/cas/mpsc_stream_buffer.h>

#include <sched.h>

#define B4_YIELD 10000

bool sp_mpsc_sb_init(sp_mpsc_streambuffer *mpsc_sb, safe_buffer_t *safe_buffer)
{
    if (mpsc_sb == NULL || safe_buffer == NULL) return false;

    if(mpsc_sb->buffer == NULL) return false;

    if(safe_buffer->buffer == NULL || safe_buffer->buffer_size == 0 ) return false;

    mpsc_sb->buffer = safe_buffer;

    atomic_init(&mpsc_sb->producers_data,0);
    atomic_init(&mpsc_sb->consumer_data,0);
    
    return true;
}

bool sp_mpsc_sb_push(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    if (sb == NULL || buff == NULL || buff_size == 0) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0) return false;

    uint128_t expected = atomic_load(&sb->producers_data);
    uint128_t desired = 0;

    uint64_t reserved_push_index = 0;

    // --- RESERVATION ---
    int reservation_i = 0;
    do
    {
        if (reservation_i == B4_YIELD) 
        {
            reservation_i = 0;
            sched_yield();
            continue;
        }

        uint64_t push_index   = (uint64_t)(expected >> 64);
        uint64_t consumer_pos = (uint64_t)(atomic_load(&sb->consumer_data) >> 64);

        uint64_t current_size = 0;
        if (push_index >= consumer_pos) 
            current_size = push_index - consumer_pos;
        else 
            current_size = max_size - (consumer_pos - push_index);

        if (current_size + buff_size > max_size) 
        {
            sleep(100);//FUTEX IMPLEMENTATION GOES HERE
            reservation_i++;
            expected = atomic_load(&sb->producers_data);
            continue;
        }

        reserved_push_index     = push_index;
        uint64_t new_push_index = push_index + buff_size;
        
        uint64_t low_part = (uint64_t)(expected & 0xFFFFFFFFFFFFFFFFULL) + 1;
        desired = ((uint128_t)new_push_index << 64) | low_part;

        reservation_i++;
    } 
    while (!atomic_compare_exchange_strong(&sb->producers_data, &expected, desired));

    size_t offset = (size_t)(reserved_push_index % max_size);
    if (sb_copy_in(sb->buffer, offset, buff, buff_size) == false)
    {
       return false;
    }

    uint128_t commit_expected = atomic_load(&sb->producers_data);
    uint128_t commit_desired = 0;
    int commit_i = 0;
    
    do
    {
        if (commit_i == B4_YIELD) 
        {
            commit_i = 0;
            sched_yield();
            continue;
        }

        uint64_t push_hi = (uint64_t)(commit_expected >> 64);
        uint64_t push_lo = (uint64_t)(commit_expected & 0xFFFFFFFFFFFFFFFFULL);
        
        commit_desired = ((uint128_t)push_hi << 64) | (push_lo); 

        commit_i++;
    }
    while (!atomic_compare_exchange_strong(&sb->producers_data, &commit_expected, commit_desired));

    return true;
}

bool sp_mpsc_sb_pop(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    if (sb == NULL || buff == NULL || buff_size == 0) return false;

    size_t max_size = sb->buffer->buffer_size;
    if (max_size == 0) return false;

    uint64_t read_index = 0;
    uint64_t write_index = 0;
    uint64_t available_data = 0;
    uint128_t consumer_val = 0;

    int i = 0;

    while (true)
    {
        if (i == B4_YIELD) 
        {
            sched_yield();
            i = 0;
        }

        consumer_val = atomic_load(&sb->consumer_data);
        uint128_t producer_val = atomic_load(&sb->producers_data);

        read_index  = (uint64_t)(consumer_val >> 64);
        write_index = (uint64_t)(producer_val >> 64); // Indice dei produttori

        if (write_index >= read_index) 
        {
            available_data = write_index - read_index;
        } 
        else 
        {
            available_data = max_size - (read_index - write_index);
        }

        if (available_data >= buff_size) 
        {
            break;
        }

        i++;
    }

    size_t offset = (size_t)(read_index % max_size);
    if (sb_copy_out(sb->buffer, offset, buff, buff_size) == false)
    {
        return false;
    }

    uint64_t new_read_index = read_index + buff_size;
    
    uint64_t consumer_version = (uint64_t)(consumer_val & 0xFFFFFFFFFFFFFFFFULL) + 1;
    uint128_t new_consumer_val = ((uint128_t)new_read_index << 64) | consumer_version;

    atomic_store(&sb->consumer_data, new_consumer_val);

    return true;
}

bool sp_mpsc_sb_peek(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size)
{
    return true;
}

bool sp_mpsc_sb_advance(sp_mpsc_streambuffer *sb, size_t buff_size)
{
    return true;
}