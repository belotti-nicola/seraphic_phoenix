#include <internal/datastructures/utility/safe_linear_buffer.h>

#include <internal/sp_utils.h>
#include <stdbool.h>

#define BUFFER_SIZE 100
#define OFFSET       90
#define BYTES_COPY   50


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

    // setup the message
    uint8_t data[BYTES_COPY] = {0};
    for (size_t i=0;i<BYTES_COPY;i++)
    {
        data[i] = (uint8_t)i;
    }

    // copy in data
    ret_code = sb_copy_in(&safe_buffer,OFFSET,data,BYTES_COPY);
    if ( ret_code == false)
    {
        TRACE_ERROR_POSITION();
        return 1;
    }

    for ( size_t i=0 ; i<BYTES_COPY ; i++)
    {
        size_t  index    = (i+OFFSET)%BUFFER_SIZE;
        uint8_t found    = safe_buffer.buffer[index];
        uint8_t expected = data[i];
        
        if ( found != expected )
        {
            TRACE_ERROR_POSITION();
            TEST_ERROR("found %u instead of %u (index %ld)",found,expected,i);
            return 1;
        }
    }
    

    
    return 0;
}

