#pragma once

#include "nu2api.saga/nucore/nuelist.hpp"

#include "nu2api.saga/nusound/nusound_buffer.hpp"
#include "nu2api.saga/nusound/nusound_loader.hpp"
#include "nu2api.saga/nusound/nusound_sample.hpp"

extern "C" {
    void NuSound3Init(i32 zero);
}

class NuSoundStreamingSample : public NuSoundSample {
  public:
    NuSoundBuffer *buffer1;
    NuSoundBuffer *buffer2;

    NuSoundLoader *loader;
};
