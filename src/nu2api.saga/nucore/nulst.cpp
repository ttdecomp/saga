#include "nu2api.saga/nucore/nulst.h"

#include "globals.h"
#include "nu2api.saga/nucore/numemory.h"

#include <cstddef>

nulsthdr_s *NuLstCreate(i32 element_count, i32 element_size) {
    nulsthdr_s *list = (nulsthdr_s *)NU_ALLOC(element_count * (element_size + sizeof(nulnkhdr_s)) + sizeof(nulsthdr_s),
                                              4, 1, "", NUMEMORY_CATEGORY_NONE);

    if (list != NULL) {
        list->free = (nulnkhdr_s *)&list[1];
        list->head = NULL;
        list->tail = NULL;

        list->element_count = element_count;
        list->element_size = element_size;
        list->element_total_size = (element_size + sizeof(nulnkhdr_s));
        list->used_count = 0;

        nulnkhdr_s *free = list->free;
        nulnkhdr_s *next;
        for (i32 i = 1; next = (nulnkhdr_s *)((usize)&free[1].owner + element_size), i < element_count; i++) {
            free->succ = next;
            free->id = i - 1;
            free->owner = list;
            free = next;
        }

        free->succ = NULL;
        free->id = element_count - 1;
        list->free_tail = free;

        free->owner = list;
        list->safe_thread = nu_current_thread_id;
    }

    return list;
}
