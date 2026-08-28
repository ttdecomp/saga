#pragma once

#include "nu2api/nucore/common.h"

class NuSoundStreamDesc;
class NuSoundBufferCallback;
template <typename T> class NuSoundWeakPtr;

class NuSoundSource {

  public:
    enum class FeedType : u32 {
        ZERO = 0,
        STREAMING = 1,
    };

    enum class SourceType : u32 {
        ZERO = 0,
        STREAMING = 1,
    };

  public:
    FeedType feed_type;
    SourceType source_type;
    const char *name;
    NuSoundStreamDesc *stream_desc;

  public:
    NuSoundSource(const char *file, SourceType source_type, FeedType feed_type);
    virtual ~NuSoundSource();

    const char *GetName() const;

    void SetStreamDesc(NuSoundStreamDesc *desc);
    NuSoundStreamDesc *GetStreamDesc() const {
        return this->stream_desc;
    }

    // Source virtuals the voice layer dispatches through (the original went
    // through the source vtable; NuSoundSample / NuSoundStreamingSample and
    // NuSoundDecoder override them).
    virtual bool OpenStream(bool loop) {
        (void)loop;
        return true;
    }
    virtual void CloseStream() {
    }
    virtual bool IsStreamOpen() const = 0;
    virtual u32 GetMaxBufferSize() = 0;
    virtual u32 GetNumInitialBuffers() const;
    virtual void RequestBuffer(bool loop, NuSoundWeakPtr<NuSoundBufferCallback> callback) = 0;
    virtual void Lock() {
    }
    virtual void Unlock() {
    }
    virtual void VoiceReference();
    virtual void VoiceRelease();

    // Number of buffers Play() requests before starting the hardware voice
    // (the original read this global through an inline accessor).
    static u32 sNumInitialBuffers;
};
