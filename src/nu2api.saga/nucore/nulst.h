#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct nulsthdr_s;

    typedef struct nulnkhdr_s {
        struct nulsthdr_s *owner;
        struct nulnkhdr_s *next;
        struct nulnkhdr_s *prev;
        u16 id;
        u16 is_used : 1;
    } NULNKHDR;

    typedef struct nulsthdr_s {
        NULNKHDR *free;
        NULNKHDR *free_tail;
        NULNKHDR *head;
        NULNKHDR *tail;
        u16 element_count;
        u16 element_size;
        u16 element_size_total;
        u16 used_count;
        u32 safe_thread;

        u16 padding[2];
    } NULSTHDR;

    NULSTHDR *NuLstCreate(i32 element_count, i32 element_size);
    void NuLstDestroy(NULSTHDR *list);

    NULNKHDR *NuLstGetNext(NULSTHDR *list, NULNKHDR *node);
#ifdef __cplusplus
}
#endif
