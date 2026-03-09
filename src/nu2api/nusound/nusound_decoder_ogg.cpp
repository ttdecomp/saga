#include "nusound_decoder_ogg.hpp"

NuSoundDecoderOGG::NuSoundDecoderOGG(char *name, NuSoundSource *source) : NuSoundDecoder(name, source) {
}

const char *NuSoundDecoderOGG::GetName() {
    return this->name;
}
