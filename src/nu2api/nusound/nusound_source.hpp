#pragma once

#include "nu2api/nucore/common.h"

class NuSoundStreamDesc;

class NuSoundSource {

  public:
    enum class FeedType : u32 {
        ZERO = 0,
        STREAMING = 1,
    };

    enum class SourceType : u32 {
        ZERO = 0,

    };

  protected:
    FeedType feed_type;
    SourceType source_type;
    u16 name_capacity;
    u16 name_length;
    const char *name;
    NuSoundStreamDesc *stream_desc;
    void *encoded_source;
    i32 field_0x1c;

  public:
    NuSoundSource(const char *file, SourceType source_type, FeedType feed_type);
    virtual ~NuSoundSource();

    const char *GetName() const;

    void SetStreamDesc(NuSoundStreamDesc *desc);
};
