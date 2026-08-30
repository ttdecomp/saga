#include "nu2api/nusound/nusound_source.hpp"

const u32 NuSoundSource::sNumInitialBuffers[2] = {1, 2};

u32 NuSoundSource::GetNumInitialBuffers() const {
    return NuSoundSource::sNumInitialBuffers[(u32)this->feed_type];
}
#include "nu2api/nusound/nusound_weakptr.hpp"

#include "nu2api/nucore/numemory.h"
#include "nu2api/nusound/nusound_loader.hpp"

#include "globals.h"
#include <cstring>

NuSoundSource::NuSoundSource(const char *name, SourceType source_type, FeedType feed_type) {
    this->name_length = 1;
    this->name_capacity = 1;
    this->name = theEmptyString;

    if (name != NULL) {
        u16 length = strlen(name) + 1;
        u16 size = (length + 3) & 0xfffc;
        char *mem = (char *)NU_ALLOC(
            size, 4, 5, "i:/SagaTouch-Android_9176564/nu2api.saga/../nu2api.2013/numemory/NuMemory.h :328", 0);

        this->name = mem;
        this->name_length = size;
        memcpy(mem, name, length);
        this->name_capacity = length;
    }

    // libTTapp.so 0x3248f2..0x324904.
    this->stream_desc = NULL;
    this->feed_type = feed_type;
    this->source_type = source_type;
    this->field_0x18 = 0;
    this->field_0x1c = -1;
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
    this->field_0x18++;
}

void NuSoundSource::VoiceRelease() {
    this->field_0x18--;
}
