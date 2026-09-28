#ifndef SP_MPSC_STREAMBUFFER
#define SP_MPSC_STREAMBUFFER

#include <internal/datastructures/utility/safe_linear_buffer.h>
#include <internal/datastructures/utility/futex.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>


#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
    #include <immintrin.h>
    #define CPU_PAUSE() _mm_pause()
#elif defined(__aarch64__) || defined(_ARM_)
    #define CPU_PAUSE() __asm__ __volatile__("yield" ::: "memory")
#else
    #define CPU_PAUSE() ((void)0) // Fallback vuoto se architettura sconosciuta
#endif

//16 BYTE ALIGNMENT TO ENSURE HARDWARE ATOMICITY
typedef struct __attribute__((aligned(16))) sp_mpsc_streambuffer
{
    safe_buffer_t *buffer;

    uint64_t  producers_index;
    uint64_t  producers_commit;
    
    uint64_t  consumer_index;

    futex_t   futex_empty;
    futex_t   futex_full;

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