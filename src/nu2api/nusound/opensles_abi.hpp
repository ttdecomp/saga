#pragma once

#include "nu2api/nucore/common.h"

// The shipping binary imports this small OpenSL ES surface from
// libOpenSLES.so.  Keeping the API declarations local avoids making shared
// NuSound code depend on an Android SDK header; the host supplies the same C
// symbols from its SDL-backed OpenSL adapter.
extern "C" {
    extern const void *SL_IID_PLAY;
    extern const void *SL_IID_ANDROIDSIMPLEBUFFERQUEUE;
    extern const void *SL_IID_VOLUME;
    extern const void *SL_IID_ENGINE;
    extern const void *SL_IID_ENGINECAPABILITIES;
    extern const void *SL_IID_ENVIRONMENTALREVERB;

    u32 slCreateEngine(void **engine_object, u32 num_options, const void *options, u32 num_interfaces,
                       const void **interface_ids, const u32 *required);
}
