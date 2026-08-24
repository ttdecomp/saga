#ifndef LEGOAPI_AUDIO_SFX_H
#define LEGOAPI_AUDIO_SFX_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// Audio / SFX playback API (module legoapi/audio, sfx.cpp). PlaySfx is a
// C-linkage symbol in the original (unmangled); TickTockSfx is C++.

#ifdef __cplusplus
extern "C" {
#endif
    void PlaySfx(char *name, nuvec_s *pos);
#ifdef __cplusplus
}
#endif

void TickTockSfx(void);

#endif
