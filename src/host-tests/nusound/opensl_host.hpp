#pragma once

// HOST-ONLY: minimal OpenSL ES object model that backs the faithful
// NuSoundAndroid / NuVoiceAndroid code on the host. The decompiled code talks
// to OpenSL through raw interface vtable slots (see nusound_voice_android.cpp);
// this module implements exactly those slots over SDL3 audio streams, so the
// sample write (Queue Enqueue) reaches the real host audio device without any
// change to the decompiled call flow.

#include "decomp.h"

namespace hostsl {

    // slCreateEngine equivalent. The original calls the Android loader entry with
    // (SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE) engine options; the fake
    // engine object it returns exposes the ObjectItf and EngineItf vtable slots
    // used by NuSoundAndroid::InitAudioDevice and NuVoiceAndroid.
    u32 slCreateEngine(void **engine_object, u32 num_options, void *options, u32 num_interfaces,
                       const void **interface_ids, const u32 *required);

    // Aggregate counters for the host audio test: bytes pushed in through
    // Enqueue, bytes drained by the device, and players created.
    struct Stats {
        u32 players_created;
        u32 players_playing;
        u64 bytes_enqueued;
        u64 bytes_consumed;
        f32 last_mix_rms;
    };

    Stats GetStats();

} // namespace hostsl
