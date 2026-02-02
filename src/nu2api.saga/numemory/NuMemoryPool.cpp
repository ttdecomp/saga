#include "nu2api.saga/numemory/NuMemoryPool.h"

void NuMemoryPool::AddPage(void *ptr, unsigned int size) {
}

void NuMemoryPool::InterlockedAdd(volatile unsigned int *augend, unsigned int addend) {
    unsigned int expected;
    unsigned int actual;

    do {
        expected = *augend;
        actual = __sync_val_compare_and_swap(augend, expected, addend + expected);
    } while (actual != expected);
}

void NuMemoryPool::InterlockedSub(volatile unsigned int *minuend, unsigned int subtrahend) {
    unsigned int expected;
    unsigned int actual;

    do {
        expected = *minuend;
        actual = __sync_val_compare_and_swap(minuend, expected, expected - subtrahend);
    } while (actual != expected);
}
