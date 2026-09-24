#ifndef SP_MPSC_STREAMBUFFER
#define SP_MPSC_STREAMBUFFER

#include <internal/datastructures/utility/safe_linear_buffer.h>
#include <internal/datastructures/utility/futex.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

typedef unsigned __int128 uint128_t;

//16 BYTE ALIGNMENT TO ENSURE HARDWARE ATOMICITY
typedef struct __attribute__((aligned(16))) sp_mpsc_streambuffer
{
    safe_buffer_t *buffer;

    uint128_t producers_data;
    uint128_t consumer_data;

    futex_t   futexp;

} sp_mpsc_streambuffer;

bool sp_mpsc_sb_init(sp_mpsc_streambuffer *mpsc_sb, safe_buffer_t *safe_buffer);

bool sp_mpsc_sb_push(
    sp_mpsc_streambuffer *sb,
    uint8_t *buff,
    size_t buff_size);

bool sp_mpsc_sb_pop(
    sp_mpsc_streambuffer *sb,
    uint8_t *buff,
    size_t buff_size);

bool sp_mpsc_sb_peek(sp_mpsc_streambuffer *sb, uint8_t *buff, size_t buff_size);
bool sp_mpsc_sb_advance(sp_mpsc_streambuffer *sb, size_t buff_size);




#endif