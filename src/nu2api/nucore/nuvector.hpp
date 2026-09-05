#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/numemory.h"

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
        T *new_data = (T *)NuMemoryGet()->GetThreadMem()->_BlockReAlloc(data, new_length * sizeof(T), 4, 0x41, "",
                                                                        NUMEMORY_CATEGORY_NONE);

        if (new_data != data) {
            if (length != 0) {
                memmove(new_data, data, length * sizeof(T));
            }
            NU_FREE(data);
        }

        data = new_data;
        capacity = new_length;
    }
};
