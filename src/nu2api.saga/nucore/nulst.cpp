#include "nu2api.saga/nucore/nulst.h"

#include <stddef.h>

#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nucore/nuthread.h"

NULSTHDR *NuLstCreate(i32 element_count, i32 element_size) {
    NULSTHDR *list;
    i32 element_size_total;
    i32 i;
    NULNKHDR *first_free;
    union {
        char *char_ptr;
        NULNKHDR *node_ptr;
    } next;
    union {
        char *char_ptr;
        NULNKHDR *node_ptr;
    } curr;

    element_size_total = element_size + sizeof(NULNKHDR);
    list =
        (NULSTHDR *)NU_ALLOC(element_count * element_size_total + sizeof(nulsthdr_s), 4, 1, "", NUMEMORY_CATEGORY_NONE);

    if (list != NULL) {
        list->free = (NULNKHDR *)(list + 1);
        list->head = NULL;
        list->tail = NULL;

        list->element_count = element_count;
        list->element_size = element_size;
        list->element_size_total = element_size_total;
        list->used_count = 0;

        first_free = list->free;
        curr.node_ptr = first_free;
        next.char_ptr = curr.char_ptr + element_size_total;
        for (i = 1; i < element_count; i++) {
            curr.node_ptr->next = next.node_ptr;
            curr.node_ptr->id = i - 1;
            curr.node_ptr->owner = list;
            curr.node_ptr = next.node_ptr;

            next.char_ptr = next.char_ptr + element_size_total;
        }

        curr.node_ptr->next = NULL;
        list->free_tail = curr.node_ptr;
        curr.node_ptr->id = i - 1;

        curr.node_ptr->owner = list;
        list->safe_thread = nu_current_thread_id;
    }

    return list;
}

void NuLstDestroy(NULSTHDR *list) {
    NuMemoryGet()->GetThreadMem()->BlockFree(list, 0);
}

NULNKHDR *NuLstGetNext(NULSTHDR *list, NULNKHDR *node) {
    if (node != NULL) {
        node--;

        if (node[0].next != NULL) {
            return node[0].next + 1;
        }
    } else if (list->head != NULL) {
        return list->head + 1;
    }

    return NULL;
}
