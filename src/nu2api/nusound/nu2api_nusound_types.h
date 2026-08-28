#ifndef NU2API_NUSOUND_TYPES_H
#define NU2API_NUSOUND_TYPES_H
#pragma once

// Aggregate header for the NuSound subsystem. The legacy per-struct stub
// catalog has been replaced by the real headers; anything still missing a
// definition is forward-declared below.

#include "nu2api/nucore/fixed_width.h"

#include "nu2api/nucore/nuelist.hpp"
#include "nu2api/nucore/nuvuvec.hpp"
#include "nu2api/nusound/nusound_system.hpp"
#include "nu2api/nusound/nusound_weakptr.hpp"

#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_loader.hpp"
#include "nu2api/nusound/nusound_sample.hpp"
#include "nu2api/nusound/nusound_loader_ogg.hpp"
#include "nu2api/nusound/nusound_decoder.hpp"
#include "nu2api/nusound/nusound_streamer.hpp"
#include "nu2api/nusound/nusound_voice.hpp"

struct OPTIONSSAVE_s;
struct WORLDINFO_s;
struct nuvec_s;

// The OGG decoder (nu2api.2013/nusound/nusound_decoder_ogg.cpp): wraps a
// NuSoundSource, decodes OGG data into ring buffers and feeds the voice's
// buffer callback. Off the title-music path; the bodies are faithful stubs.
class NuSoundDecoderOGG : public NuSoundBufferCallback {
  public:
    class OGGReadCallbacksDecoder {
      public:
        NuSoundDecoderOGG *decoder;

      public:
        OGGReadCallbacksDecoder();
        void Read(void *dest, u32 size);
        void Seek(i32 origin, u32 offset);
        void Close();
        i32 GetPosition() const;
        void SetDecoder(NuSoundDecoderOGG *decoder);
    };

    NuSoundDecoderOGG(char const *name, NuSoundSource *source);
    virtual ~NuSoundDecoderOGG();

    void Decode(NuSoundSource &source, NuSoundBuffer &buffer, bool flag);
    void DecodeOggChunk(char *data, u32 size);
    void Reset();
    void SubmitBuffer(NuSoundBuffer *buffer) override;
};

struct NuSoundAndroid;
struct NuSoundClock;
struct NuSoundDecodeThread;
struct NuSoundDecoder;
struct NuSoundDecoderOGG;
struct NuSoundEffectDoppler;
struct NuSoundEffectFader;
struct NuSoundEffectPitchRamp;
struct NuSoundHandle;
struct NuSoundListener;
struct NuSoundMemoryBuffer;
struct NuSoundMemoryManager;
struct NuSoundMixer;
struct NuSoundOutOfMemCallback;
struct NuSoundRoutingTable;
struct NuThread;
struct VuMtx;
struct VuVec;

#endif // NU2API_NUSOUND_TYPES_H