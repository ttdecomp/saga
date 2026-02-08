#pragma once

#include "decomp.h"
#include "nu2api.saga/nucore/common.h"

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

    void push_back(const T &value) {
        if (length >= capacity) {
            resize();
        }
        data[length++] = value;
    }

  private:
    void resize() {
        UNIMPLEMENTED("NuVector::resize");
    }
};
