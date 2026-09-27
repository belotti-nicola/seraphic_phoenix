#include <internal/datastructures/utility/futex.h>

#include <internal/sp_utils.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#define MAGIC_NUMBER  74


void *thread_waiter( void *arg )
{
    futex_t *futex = (futex_t *)arg;

    while ( atomic_load(futex) != MAGIC_NUMBER ) 
    {
        uint32_t current = atomic_load(futex);
        
        if (current == MAGIC_NUMBER) break;

        futex_wait(futex, current);
    }
    
    return NULL;
}

void *thread_waker( void *arg )
{
    futex_t *futex = (futex_t *)arg;

    sleep_ms(20);

    atomic_store(futex,MAGIC_NUMBER);

    futex_wake(futex,1);

    return NULL;
}


int main()
{
    int err;

    futex_t futex;

    atomic_store(&futex,0);

    pthread_t waiter;
    err = pthread_create(&waiter,NULL,thread_waiter,&futex);
    if ( err != 0 )
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("Waiter thread error (%d).",err);
        return 1;
    }

    pthread_t waker;
    err = pthread_create(&waker,NULL,thread_waker,&futex);
    if ( err != 0 )
    {
        TRACE_ERROR_POSITION();
        TEST_ERROR("Waker thread error(%d).",err);
        return 1;
    }

    return 0;
}

