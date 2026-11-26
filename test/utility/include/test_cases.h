#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct test_cases_t
{
    void   *data;

    size_t  size;
    size_t  curr;
    size_t  max_size;
    size_t  entry_size;

} test_cases_t;

bool test_cases_setup(test_cases_t *test_cases, size_t max_size, size_t entry_size);
bool test_cases_add(test_cases_t *test_cases, void *test);
void test_cases_destroy(test_cases_t *test_cases);

void *test_cases_next(test_cases_t *test_cases);
