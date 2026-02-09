#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct nulnkhdr_s;

    struct nulsthdr_s {
        nulnkhdr_s *free;
        nulnkhdr_s *free_tail;
        nulnkhdr_s *head;
        nulnkhdr_s *tail;
        u16 element_count;
        u16 element_size;
        u16 element_total_size;
        u16 used_count;
        u32 safe_thread;
    };

    struct nulnkhdr_s {
        nulsthdr_s *owner;
        nulnkhdr_s *succ;
        nulnkhdr_s *prev;
        u16 id;
        u16 used : 1;
    };

    nulsthdr_s *NuLstCreate(i32 element_count, i32 element_size);

#ifdef __cplusplus
}
#endif
