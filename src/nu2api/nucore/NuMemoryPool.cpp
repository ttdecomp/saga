#include "nu2api/nucore/NuMemoryPool.h"

void NuMemoryPool::AddPage(void *ptr, u32 size) {
}

void NuMemoryPool::InterlockedAdd(volatile u32 *augend, u32 addend) {
    u32 expected;
    u32 actual;

    do {
        expected = *augend;
        actual = __sync_val_compare_and_swap(augend, expected, addend + expected);
    } while (actual != expected);
}

void NuMemoryPool::InterlockedSub(volatile u32 *minuend, u32 subtrahend) {
    u32 expected;
    u32 actual;

    do {
        expected = *minuend;
        actual = __sync_val_compare_and_swap(minuend, expected, expected - subtrahend);
    } while (actual != expected);
}
