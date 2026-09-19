#include <internal/datastructures/utility/safe_linear_buffer.h>

#include <internal/sp_utils.h>
#include <stdbool.h>

#define BUFFER_SIZE 100


int main()
{
    uint8_t       buffer[BUFFER_SIZE];
    safe_buffer_t safe_buffer;

    // instatiate the struct
    bool ret_code = sb_init(&safe_buffer,buffer,BUFFER_SIZE);
    if ( ret_code == false)
    {
        TRACE_ERROR_POSITION();
        return 1;
    }

    // check they equals 0
    for( size_t i=0; i<BUFFER_SIZE ; i++ )
    {
        uint8_t expected = buffer[i];
        uint8_t found    = safe_buffer.buffer[i];
        if ( buffer[i] != safe_buffer.buffer[i] )
        {
            TRACE_ERROR_POSITION();
            TEST_ERROR("Error in initialization: safe_buffer differs from buffer at index %ld(found %u instead of %u)",i,found,expected);
            return 1;
        }

        if ( buffer[i] != 0 )
        {
            TRACE_ERROR_POSITION();
            TEST_ERROR("Error in initialization: safe_buffer is not equal to 0 at index %ld(found %u instead of 0)",i,found);
            return 1;
        }
    }

    return 0;
}

