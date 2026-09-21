#include <internal/datastructures/cas/mpsc_stream_buffer.h>
#include <internal/sp_utils.h>

#define BUFFER_SIZE 100

int main()
{
    bool exit_code;
    uint8_t buffer[BUFFER_SIZE];    
    
    safe_buffer_t safe_buffer;
    exit_code = sb_init(&safe_buffer,buffer,BUFFER_SIZE);
    if (exit_code == false)
    {
        TEST_ERROR("sb_init failed");
        return 1;
    }
    
    sp_mpsc_streambuffer sb;
    exit_code = sp_mpsc_sb_init(&sb,&safe_buffer);
    if (exit_code == false)
    {
        TEST_ERROR("sp_mpsc_sb_init failed");
        return 1;
    }

    return 0;
}