#pragma once

#include "nu2api/nucore/common.h"

#include <pthread.h>

class NuSoundWeakPtrListNode {
  public:
    NuSoundWeakPtrListNode *value;
    NuSoundWeakPtrListNode *next;

    static pthread_mutex_t sPtrListLock;
    static pthread_mutex_t sPtrAccessLock;

    virtual ~NuSoundWeakPtrListNode() = default;
};

NuSoundWeakPtrListNode *const INVALID_NODE = static_cast<NuSoundWeakPtrListNode *>((void *)-4);

template <typename T> class NuSoundWeakPtrObj {
  private:
    u8 padding[0x18];

    NuSoundWeakPtrListNode *list_node;
    i32 ref_count;

  public:
    void Link(NuSoundWeakPtrListNode *node) {
        pthread_mutex_lock(&NuSoundWeakPtrListNode::sPtrListLock);

        NuSoundWeakPtrListNode *esi = list_node;

        NuSoundWeakPtrListNode **node_value = node ? &node->value : NULL;
        if (!node) {
            node = INVALID_NODE;
        }

        NuSoundWeakPtrListNode **list_value = list_node ? &list_node->value : NULL;
        if (!list_node) {
            esi = INVALID_NODE;
        }

        NuSoundWeakPtrListNode **list_value_ptr = *list_value ? &(*list_value)->value : NULL;

        *list_value = node_value ? node : NULL;

        *node_value = list_value_ptr ? (NuSoundWeakPtrListNode *)((char *)list_value_ptr - 4) : NULL;
        *(void **)((char *)list_value_ptr + 4) = node;
        *(void **)((char *)node_value + 4) = esi;

        ref_count++;

        pthread_mutex_unlock(&NuSoundWeakPtrListNode::sPtrListLock);
    }
};

template <typename T> class NuSoundWeakPtr {
    T *shared;
};

class NuSoundBufferCallback;
template class NuSoundWeakPtrObj<NuSoundBufferCallback>;
