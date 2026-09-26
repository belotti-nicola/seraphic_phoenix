#include <internal/datastructures/cas/mpsc_stream_buffer.h>
#include <internal/datastructures/utility/safe_linear_buffer.h>
#include <internal/sp_utils.h>
#include <stdbool.h>
#include <pthread.h>

#define BUFFER_SIZE        8192
#define THREADS               8
#define MESSAGES         200000
#define MESSAGES_LEN         20

void *consumer_function(void *arg)
{
    bool pop_exit_code;

    sp_mpsc_streambuffer *sb = (sp_mpsc_streambuffer *)arg;

    uint8_t buff[MESSAGES_LEN] = {0};

    for (int i = 0; i< MESSAGES * THREADS ; i++)
    {
        sp_mpsc_sb_pop(sb,buff,MESSAGES_LEN);
        if ( pop_exit_code == false )
        {
            perror("PUSH FAIL");
        }
    }

    return NULL;
}

void *producers_function(void *arg)
{
    bool push_exit_code;

    sp_mpsc_streambuffer *sb = (sp_mpsc_streambuffer *)arg;

    uint8_t buff[MESSAGES_LEN] = {0};
    for (int i = 0; i< MESSAGES ; i++)
    {
        push_exit_code = sp_mpsc_sb_push(sb,buff,MESSAGES_LEN);
        if ( push_exit_code == false )
        {
            perror("PUSH FAIL");
        }
    }

    return NULL;
}

int main()
{
    bool exit_code;
    uint8_t buffer[BUFFER_SIZE] = {0};    
    
    safe_buffer_t safe_buffer = {0};
    exit_code = sb_init(&safe_buffer,buffer,BUFFER_SIZE);
    if (exit_code == false)
    {
        TEST_ERROR("sb_init failed");
        return 1;
    }
    
    sp_mpsc_streambuffer sb = {0};
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

