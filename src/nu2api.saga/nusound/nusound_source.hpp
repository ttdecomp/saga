#pragma once

#include "nu2api.saga/nucore/common.h"

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

  private:
    FeedType feed_type;
    SourceType source_type;
    const char *name;
    NuSoundStreamDesc *stream_desc;

  public:
    NuSoundSource(const char *file, SourceType source_type, FeedType feed_type);
    const char *GetName() const;
};
