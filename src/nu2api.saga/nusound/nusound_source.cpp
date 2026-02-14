#include "nu2api.saga/nusound/nusound_source.hpp"

#include "nu2api.saga/nucore/numemory.h"

#include "globals.h"
#include <cstring>

NuSoundSource::NuSoundSource(const char *name, SourceType source_type, FeedType feed_type) {
    // this->field3_0xc = true;
    // this->field4_0xd = false;
    // this->field5_0xe = 1;

    this->name = theEmptyString;

    if (name != NULL) {
        u16 size = strlen(name) + 4 & 0xfffc;
        char *mem = (char *)NU_ALLOC(
            size, 4, 5, "i:/SagaTouch-Android_9176564/nu2api.saga/../nu2api.2013/numemory/NuMemory.h :328", 0);

        // this->field3_0xc = size;
        memcpy(mem, name, size);

        this->name = mem;

        // this->field5_0xe = (short)(sVar1 + 1);
    }

    // this->sound_stream_desc = NULL;
    this->feed_type = feed_type;
    this->source_type = source_type;
    // this->field8_0x18 = NULL;
    // this->field9_0x1c = -1;
}

const char *NuSoundSource::GetName() const {
    return this->name;
}
