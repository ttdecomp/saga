#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/numemory.h"

class NuListNodeBase {
    NuListNodeBase *prev;
    NuListNodeBase *next;

  public:
    NuListNodeBase(NuListNodeBase *prev, NuListNodeBase *next) : prev(prev), next(next) {
    }

    void SetPrev(NuListNodeBase *prev) {
        this->prev = prev;
    }

    void SetNext(NuListNodeBase *next) {
        this->next = next;
    }

    NuListNodeBase *GetNext() const {
        return this->next;
    }

    void Remove() {
        if (prev != NULL) {
            prev->next = next;
        }
        if (next != NULL) {
            next->prev = prev;
        }
    }
};

template <typename T> class NuListNode : public NuListNodeBase {
  public:
    T value;

  public:
    NuListNode(NuListNode<T> *prev, NuListNode<T> *next, T value) : NuListNodeBase(prev, next), value(value) {
    }
};

template <typename T> class NuList {
    NuListNodeBase start;
    NuListNodeBase end;

    NuListNodeBase *head;
    NuListNodeBase *tail;

    i32 length;

  public:
    NuList() : start(NULL, &end), end(&start, NULL), head(&start), tail(&end), length(0) {
    }

    ~NuList() {
        while (head != tail) {
            Remove(head);
        }
    }

    void Append(NuListNode<T> *node) {
        NuListNodeBase *tail = this->tail;
        tail->SetPrev(node);

        node->SetNext(tail);
        node->SetPrev(head);

        head->SetNext(node);

        length++;
    }

    NuListNodeBase *Head() const {
        return this->head;
    }

    NuListNodeBase *Tail() const {
        return this->tail;
    }

    void Remove(NuListNodeBase *node) {
        length--;

        node->Remove();

        NU_FREE(node);
    }
};
