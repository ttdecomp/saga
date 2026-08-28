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

// The OGG decoder lives in nusound_decoder_ogg.hpp / nusound_decoder_ogg.cpp.
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