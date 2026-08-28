#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuthread.h"

#include <pthread.h>

#include "decomp.h"

struct NuSoundWeakPtrListNode {
  public:
    static NuCriticalSection sPtrListLock;
    static NuCriticalSection sPtrAccessLock;

  public:
    // struct Payload {
    NuSoundWeakPtrListNode *prev;
    NuSoundWeakPtrListNode *next;
    //} payload;

  public:
    virtual ~NuSoundWeakPtrListNode() {
    }

    virtual void Clear() = 0;
};

template <typename T> class NuSoundWeakPtrObj {
  public:
    u8 padding[0x14];

    NuSoundWeakPtrListNode *head;
    NuSoundWeakPtrListNode *tail;
    i32 weak_count;

  public:
    void Link(NuSoundWeakPtrListNode *node) {
        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        NuSoundWeakPtrListNode *list = this->tail;

        NuSoundWeakPtrListNode *node_prev = node;
        if (node == NULL) {
            node = (NuSoundWeakPtrListNode *)-4;
        }

        NuSoundWeakPtrListNode *prev = list;
        if (list == NULL) {
            list = (NuSoundWeakPtrListNode *)-4;
        }

        NuSoundWeakPtrListNode *ppNVar1 = prev->prev == NULL ? NULL : prev->next;

        NuSoundWeakPtrListNode *pNVar1 = node_prev == NULL ? NULL : node;

        prev->prev = pNVar1;

        NuSoundWeakPtrListNode **pNVar2 = NULL;
        if (ppNVar1 != NULL) {
            pNVar2 = &ppNVar1->prev;
        }

        node_prev->prev = (NuSoundWeakPtrListNode *)pNVar2;
        ppNVar1->prev = node;
        node_prev->next = list;

        this->weak_count++;

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }

    void Unlink(NuSoundWeakPtrListNode *node) {
        LOG_DEBUG("Unlinking node %p from weak pointer list %p", node, this);

        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        if (node->prev != NULL || node->next != NULL) {
            this->weak_count--;
        }

        if (node->prev != NULL) {
            node->prev->next = node->next;
        }
        if (node->next != NULL) {
            node->next->prev = node->prev;
        }

        node->prev = NULL;
        node->next = NULL;

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }

    ~NuSoundWeakPtrObj() {
        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        if (this->weak_count != 0) {
            for (NuSoundWeakPtrListNode *node = this->head->next; node != NULL; node = node->next) {
                node->Clear();
            }
        }

        while (this->weak_count != 0) {
            Unlink(this->head->next);
        }

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();

        while (this->weak_count != 0) {
            NuSoundWeakPtrListNode *node = this->head->next;
            Unlink(node);
            node->~NuSoundWeakPtrListNode(); //(*(code *)vtable->destructor2)(node3);
        }
    }
};

template <typename T> class NuSoundWeakPtr : public NuSoundWeakPtrListNode {
  public:
    NuSoundWeakPtrObj<T> *obj;
    bool bool_value;

  public:
    NuSoundWeakPtr() : obj(NULL), bool_value(false) {
    }

    virtual ~NuSoundWeakPtr() {
        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        if (this->obj != NULL) {
            this->obj->Unlink(this);
            this->obj = NULL;
        }

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }

    void Clear() {
        this->obj = NULL;
    }

    void Set(T *ptr) {
        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        if (this->obj != (void *)ptr) {
            if (this->obj != NULL) {
                this->obj->Unlink(this);
            }

            if (ptr != NULL) {
                // Register this weak pointer in the target's list.
                ((NuSoundWeakPtrObj<T> *)ptr)->Link(this);
            }

            this->obj = (NuSoundWeakPtrObj<T> *)ptr;
        }

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }
};
