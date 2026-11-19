#include "internal/datastructures/sp_queue.h"
#include "internal/sp_utils.h"

#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 100

#define TEST "test"

int main()
{
    bool exit_code;
    uint8_t buffer[BUFFER_SIZE] = {0};

    sp_queue_t q;
    if ( sp_queue_setup(&q,buffer,BUFFER_SIZE) == false )
    {
        TEST_ERROR("Error setting Queue.");
        return 1;
    }

    if ( sp_queue_push(&q,TEST,strlen(TEST)) == false )
    {
        TEST_ERROR("Error setting Queue.");
        return 1;
    }

    uint8_t check_this[BUFFER_SIZE];
    if ( sp_queue_pop(&q,check_this,strlen(TEST)) == false )
    {
        TEST_ERROR("Error setting Queue.");
        return 1;
    }

    for (int i = 0 ; i < strlen(TEST) ; i++ )
    {
        if ( check_this[i] != TEST[i] )
        {
            TEST_ERROR("Error setting Queue.");
            return 1;
        }
    }

    return 0;
}