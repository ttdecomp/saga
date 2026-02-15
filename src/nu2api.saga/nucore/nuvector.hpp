#pragma once

#include "decomp.h"
#include "nu2api.saga/nucore/common.h"

#include "nu2api.saga/nucore/numemory.h"

#include <cstring>

template <typename T> class NuVector {
  public:
    T *data;
    usize length;
    usize capacity;

  public:
    NuVector() : data(nullptr), length(0), capacity(0) {
    }

    ~NuVector() {
        if (data != nullptr) {
            // not delete[] for some reason
            delete data;
        }
    }

    void PushBack(const T &value) {
        if (length >= capacity) {
            resize(length + 1);
        }
        data[length++] = value;
    }

  private:
    void resize(usize new_length) {
        // T *new_data = (T *)NuMemoryGet()->GetThreadMem()->_BlockReAlloc(data, new_length * sizeof(T), 4, 0x31, "",
        // NUMEMORY_CATEGORY_NONE);
        T *new_data = (T *)NU_ALLOC(new_length * sizeof(T), 4, 1, "", NUMEMORY_CATEGORY_NONE);

        if (new_data != data) {
            memmove(new_data, data, length * sizeof(T));
            NU_FREE(data);
        }

        data = new_data;
        capacity = new_length;
    }
};
