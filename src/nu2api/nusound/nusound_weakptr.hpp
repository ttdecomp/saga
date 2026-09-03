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

        // Append the node at the tail of this object's weak-pointer list
        // (libTTapp.so 0x315320: the new node chains onto the current tail
        // and the list bookkeeping moves to it).
        node->prev = this->tail;
        node->next = NULL;

        if (this->tail != NULL) {
            this->tail->next = node;
        } else {
            this->head = node;
        }

        this->tail = node;
        this->weak_count++;

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }

    void Unlink(NuSoundWeakPtrListNode *node) {
        LOG_DEBUG("Unlinking node %p from weak pointer list %p", node, this);

        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        if (node->prev != NULL) {
            node->prev->next = node->next;
        } else {
            this->head = node->next;
        }
        if (node->next != NULL) {
            node->next->prev = node->prev;
        } else {
            this->tail = node->prev;
        }

        this->weak_count--;

        node->prev = NULL;
        node->next = NULL;

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }

    ~NuSoundWeakPtrObj() {
        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        // Drop every weak pointer still registered against this object.
        NuSoundWeakPtrListNode *node = this->head;
        while (node != NULL) {
            NuSoundWeakPtrListNode *next = node->next;
            node->Clear();
            node = next;
        }

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }
};

template <typename T> class NuSoundWeakPtr : public NuSoundWeakPtrListNode {
  public:
    NuSoundWeakPtrObj<T> *obj;

  public:
    NuSoundWeakPtr() : obj(NULL) {
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
