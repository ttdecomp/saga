#pragma once

#include "nu2api/nucore/common.h"

#include <pthread.h>

struct NuSoundWeakPtrListNode;

struct NuSoundWeakPtrListNodePayload {
    NuSoundWeakPtrListNode *prev;
    NuSoundWeakPtrListNode *next;
};

class NuSoundWeakPtrListNode {
  public:
    static pthread_mutex_t sPtrListLock;
    static pthread_mutex_t sPtrAccessLock;

    struct NuSoundWeakPtrListNodePayload payload;

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

        NuSoundWeakPtrListNode *list = this->list_node;

        NuSoundWeakPtrListNodePayload *node_payload = node != NULL ? &node->payload : NULL;
        if (node == NULL) {
            node = INVALID_NODE;
        }

        NuSoundWeakPtrListNodePayload *list_payload = list != NULL ? &list->payload : NULL;
        if (list == NULL) {
            list = INVALID_NODE;
        }

        NuSoundWeakPtrListNodePayload *prev_payload = list_payload->prev != NULL ? &list_payload->prev->payload : NULL;

        list_payload->prev = node_payload != NULL ? node : NULL;
        node_payload->prev = prev_payload != NULL ? (NuSoundWeakPtrListNode *)((usize)prev_payload - 4) : NULL;

        prev_payload->next = node;
        node_payload->next = list;

        this->ref_count++;

        pthread_mutex_unlock(&NuSoundWeakPtrListNode::sPtrListLock);
    }
};

template <typename T> class NuSoundWeakPtr {
    T *shared;
};

class NuSoundBufferCallback;
template class NuSoundWeakPtrObj<NuSoundBufferCallback>;
