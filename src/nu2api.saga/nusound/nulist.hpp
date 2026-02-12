#pragma once

#include "nu2api.saga/nucore/common.h"

template <typename T> class NuListNode {
  public:
    NuListNode<T> *prev;
    NuListNode<T> *next;
    T *value;

    NuListNode(NuListNode<T> *prev, NuListNode<T> *next) : prev(prev), next(next), value(nullptr) {
    }
};

template <typename T> class NuList {
  public:
    NuListNode<T> start;
    NuListNode<T> end;

    NuListNode<T> *head;
    NuListNode<T> *tail;

    i32 length;

    NuList() : start(NULL, &end), end(&start, NULL), head(&start), tail(&end), length(0) {
    }

    ~NuList() {
        while (this->head != this->tail) {
            Remove(this->head);
        }
    }

    void Remove(NuListNode<T> *node) {
        length--;

        NuListNode<T> *prev = node->prev;
        NuListNode<T> *next = node->next;

        if (prev != NULL) {
            prev->next = next;
        }
        if (next != NULL) {
            next->prev = prev;
        }

        delete node;
    }
};
