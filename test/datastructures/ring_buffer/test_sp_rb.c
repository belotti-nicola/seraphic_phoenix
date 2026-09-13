#include <internal/datastructures/sp_ring_buffer.h>
#include <internal/sp_utils.h>
#include <stdbool.h>

#define BUFFER_SIZE 100


int main()
{
    bool exit_code;
    uint8_t buffer[BUFFER_SIZE] = {0};

    ring_buffer rb;
    sp_rb_setup(&rb,buffer,BUFFER_SIZE);

    uint8_t test[]    = "test";
    size_t  test_len  = 4;
    if (sp_rb_push(&rb,test,test_len) == false)
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("sp rb push failed.");
        return 1;
    }

    if (rb.current_size != 4 )
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("sp current_size error: found %ld instead of 4.",rb.current_size);
        return 1;
    }

    if (rb.start != 0)
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("sp start error: found %ld instead of 0.",rb.start);
        return 1;
    }

    if (rb.end != 4)
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("sp end error: found %ld instead of 4.",rb.end);
        return 1;
    }


    return 0;
}