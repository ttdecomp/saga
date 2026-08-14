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
    const char *name;
    NuSoundStreamDesc *stream_desc;

  public:
    NuSoundSource(const char *file, SourceType source_type, FeedType feed_type);
    virtual ~NuSoundSource();

    const char *GetName() const;

    void SetStreamDesc(NuSoundStreamDesc *desc);

    void VoiceReference();
    void VoiceRelease();
};
