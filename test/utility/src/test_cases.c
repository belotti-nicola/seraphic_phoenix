#include "test_cases.h"

#include <stdlib.h>
#include <string.h>

bool test_cases_setup(test_cases_t *test_cases, size_t max_size, size_t entry_size)
{
    if ( test_cases == NULL || max_size == 0 ) return false;

    void *ptr =  malloc( max_size * entry_size );
    if (  ptr == NULL) return false;

    test_cases->curr       = 0;
    test_cases->size       = 0;
    test_cases->max_size   = max_size;
    test_cases->data       = ptr;
    test_cases->entry_size = entry_size;

    return true;
}

bool test_cases_add(test_cases_t *test_cases, void *test)
{
    if ( test_cases == NULL || test == NULL ) return false;

    size_t max_size   = test_cases->max_size;
    size_t curr_size  = test_cases->size;
    size_t entry_size = test_cases->entry_size;

    void *ptr         = test_cases->data + curr_size * entry_size ;

    if ( curr_size >= max_size ) return false;

    memcpy( ptr, test, entry_size );

    test_cases->size += 1;

    return true;
}

void *test_cases_next(test_cases_t *test_cases)
{
    if ( test_cases == NULL ) return false;

    if ( test_cases->size == 0 ) return NULL;

    if ( test_cases->curr == test_cases->size ) return NULL;

    size_t entry_size = test_cases->entry_size;
    size_t curr       = test_cases->curr;

    size_t offset = curr * entry_size;
    void *target  = test_cases->data + offset; 

    test_cases->curr += 1;

    return target;
}

void test_cases_destroy(test_cases_t *test_cases)
{
    if ( test_cases == NULL || test_cases->data == NULL ) return;

    free(test_cases->data);
}