#ifndef FUTEX_H_
#define FUTEX_H_

#include <stdatomic.h>
#include <stdint.h>

#define FUTEX_LOCKED   1
#define FUTEX_UNLOCKED 0

typedef _Atomic uint32_t futex_t;

int futex_wait(futex_t *futexp, uint32_t expect_val);
int futex_wake(futex_t *futexp, uint32_t num_waiters);

#endif 