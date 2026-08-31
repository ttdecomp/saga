#pragma once

// HOST-ONLY: minimal OpenSL ES object model that backs the faithful
// NuSoundAndroid / NuVoiceAndroid code on the host. The decompiled code talks
// to OpenSL through raw interface vtable slots (see nusound_voice_android.cpp);
// this module implements exactly those slots over SDL3 audio streams, so the
// sample write (Queue Enqueue) reaches the real host audio device without any
// change to the decompiled call flow.

#include "decomp.h"

namespace hostsl { // Host OpenSL adapter diagnostics.

    // Aggregate counters for the host audio test: bytes pushed in through
    // Enqueue, bytes drained by the device, and players created.
    struct HostStats {
        u32 players_created;
        u32 players_playing;
        u64 bytes_enqueued;
        u64 bytes_consumed;
        f32 last_mix_rms;
    };

    HostStats HostGetStats();

} // namespace hostsl
