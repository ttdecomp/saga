#include "nu2api/nusound/nusound_source.hpp"

u32 NuSoundSource::sNumInitialBuffers = 1;

u32 NuSoundSource::GetNumInitialBuffers() const {
    return NuSoundSource::sNumInitialBuffers;
}
#include "nu2api/nusound/nusound_weakptr.hpp"

#include "nu2api/nucore/numemory.h"
#include "nu2api/nusound/nusound_loader.hpp"

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
    // libTTapp.so 0x32486a: both type fields are written as the literal 1
    // (STREAMING); the constructor's type arguments are ignored.
    this->feed_type = FeedType::STREAMING;
    this->source_type = SourceType::STREAMING;
    // this->field8_0x18 = NULL;
    // this->field9_0x1c = -1;
}

const char *NuSoundSource::GetName() const {
    return this->name;
}

void NuSoundSource::SetStreamDesc(NuSoundStreamDesc *desc) {
    if (desc != NULL && this->source_type != SourceType::ZERO) {
        // (*(code *)desc->vtable->get_decoded_data_format)(desc);
        desc->GetDecodedDataFormat();
    }

    this->stream_desc = desc;
}

NuSoundSource::~NuSoundSource() {
}

void NuSoundSource::VoiceReference() {
}

void NuSoundSource::VoiceRelease() {
}
