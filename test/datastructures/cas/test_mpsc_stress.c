#include <internal/datastructures/cas/mpsc_stream_buffer.h>
#include <internal/datastructures/utility/safe_linear_buffer.h>
#include <internal/sp_utils.h>
#include <stdbool.h>
#include <pthread.h>

#define THREADS           8
#define MESSAGE_LEN      50
#define BUFFER_SIZE    8192
#define MESSAGES        200
#define MESSAGES_LEN     20

void *consumer_function(void *arg)
{
    sp_mpsc_streambuffer *sb = (sp_mpsc_streambuffer *)arg;

    uint8_t buff[BUFFER_SIZE] = {0};

    for (int i = 0; i< MESSAGES * THREADS ; i++)
    {
        sp_mpsc_sb_pop(sb,buff,BUFFER_SIZE);
    }

    return NULL;
}

void *producers_function(void *arg)
{
    sp_mpsc_streambuffer *sb = (sp_mpsc_streambuffer *)arg;

    uint8_t buff[BUFFER_SIZE] = {0};

    for (int i = 0; i< MESSAGES ; i++)
    {
        sp_mpsc_sb_push(sb,buff,BUFFER_SIZE);
    }

    return NULL;
}

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

    pthread_t consumer;
    if ( pthread_create(&consumer,NULL,consumer_function,&sb) != 0 )
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("Could not create consumer.");
        return 1;
    }

    pthread_t producers[THREADS];
    for( int i=0 ; i< THREADS ; i++)
    {
        if ( pthread_create(&producers[i],NULL,producers_function,&sb) != 0)
        {
            TRACE_ERROR_POSITION();
            TEST_ERROR("Could not create producer.");
            return 1;
        }
    }

    pthread_join(consumer,NULL);

    for( int i=0 ; i< THREADS ; i++)
    {
        pthread_t producer = producers[i];
        pthread_join(producer,NULL);
    }

    return 0;
}

