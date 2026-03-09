#pragma once

#include "nusound_decoder.hpp"

class NuSoundDecoderOGG : public NuSoundDecoder {
    class OGGReadCallbacksDecoder {};

    const char *GetName();

  public:
    NuSoundDecoderOGG(char *name, NuSoundSource *source);
};
