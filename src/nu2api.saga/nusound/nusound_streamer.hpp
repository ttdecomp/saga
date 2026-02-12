#pragma once

#include "nu2api.saga/nusound/nulist.hpp"
#include "nu2api.saga/nusound/nusound_sample.hpp"

class NuSoundStreamingSample;

class NuSoundStreamer : NuSoundSample {
  public:
    static NuList<NuSoundStreamer *> s_NuSoundStreams;

    void RequestCue(NuSoundStreamingSample *streaming_sample, bool loop, float start_offset, bool always_false);
};
