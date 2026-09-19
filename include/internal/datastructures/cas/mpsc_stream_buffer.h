#ifndef SP_MPSC_STREAMBUFFER
#define SP_MPSC_STREAMBUFFER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

typedef struct sp_mpsc_streambuffer
{
    uint8_t *data;
    size_t   data_size;


} sp_mpsc_streambuffer;

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