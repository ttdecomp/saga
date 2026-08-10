#include "nu2api/nusound/nusound_source.hpp"

#include "nu2api/nucore/numemory.h"
#include "nu2api/nusound/nusound_loader.hpp"

#include "globals.h"
#include <cstring>

NuSoundSource::NuSoundSource(const char *name, SourceType source_type, FeedType feed_type) {
    this->name_capacity = 1;
    this->name_length = 1;

    this->name = theEmptyString;

    if (name != NULL) {
        u16 length = strlen(name) + 1;
        u16 size = length + 3 & 0xfffc;
        char *mem = (char *)NU_ALLOC(
            size, 4, 5, "i:/SagaTouch-Android_9176564/nu2api.saga/../nu2api.2013/numemory/NuMemory.h :328", 0);

        this->name_capacity = size;
        memcpy(mem, name, length);

        this->name = mem;
        this->name_length = length;
    }

    this->feed_type = feed_type;
    this->source_type = source_type;
    this->stream_desc = NULL;
    this->encoded_source = NULL;
    this->field_0x1c = -1;
}

NuSoundSource::~NuSoundSource() {
    if (this->name != theEmptyString) {
        NuMemoryGet()->GetThreadMem()->BlockFree((void *)this->name, 4);
    }

    this->name = NULL;
    this->name_capacity = 0;
    this->name_length = 0;
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
