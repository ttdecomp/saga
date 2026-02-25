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

        /*
        NuSoundWeakPtrListNode *list = this->tail;

        NuSoundWeakPtrListNode::Payload *node_payload = node != NULL ? &node->payload : NULL;
        if (node == NULL) {
            node = END;
        }

        NuSoundWeakPtrListNode::Payload *list_payload = list != NULL ? &list->payload : NULL;
        if (list == NULL) {
            list = END;
        }

        NuSoundWeakPtrListNode::Payload *prev_payload =
            list_payload->prev != NULL ? &list_payload->prev->payload : NULL;

        list_payload->prev = node_payload != NULL ? node : NULL;
        node_payload->prev = prev_payload != NULL ? (NuSoundWeakPtrListNode *)((usize)prev_payload - 4) : NULL;

        prev_payload->next = node;
        node_payload->next = list;
        */

        this->weak_count++;

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }

    void Unlink(NuSoundWeakPtrListNode *node) {
        LOG_DEBUG("Unlinking node %p from weak pointer list %p", node, this);

        NuSoundWeakPtrListNode::sPtrListLock.Lock();

        NuSoundWeakPtrListNode *next = node->next;
        NuSoundWeakPtrListNode *prev;

        if (next == NULL) {
            prev = node->prev;
            if (prev != NULL) {
                this->weak_count--;
            LAB_0033010b:
                prev->next = NULL;
                goto LAB_003300ba;
            }
        } else {
            this->weak_count--;

            prev = node->prev;

            if (prev == NULL) {
                if ((usize)next == -4) {
                    goto LAB_003300ba;
                }
            } else {
                if ((usize)next == -4) {
                    goto LAB_0033010b;
                }
                prev->next = next;
            }

            next->prev = prev;

        LAB_003300ba:
            node->next = NULL;
            node->prev = NULL;
        }

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
                this->obj->Link(this);
            }

            this->obj = (NuSoundWeakPtrObj<T> *)ptr;
        }

        NuSoundWeakPtrListNode::sPtrListLock.Unlock();
    }
};
