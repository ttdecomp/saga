#pragma once

#include "globals.h"
#include "nu2api.saga/nufile/nufile.h"

struct NuSoundStreamingSample;
typedef struct nusound_filename_info_s {
    char *name;
    void *field4_0x4;
    NuSoundStreamingSample *streaming_sample;
} NUSOUND_FILENAME_INFO;

typedef enum {
    NUSOUNDPLAYTOK_END = 1,
    NUSOUNDPLAYTOK_STEREOSTREAM = 2,
    NUSOUNDPLAYTOK_SAMPLE = 3,
    NUSOUNDPLAYTOK_VOL = 6,
    NUSOUNDPLAYTOK_PITCH = 9,
    NUSOUNDPLAYTOK_STARTOFFSET = 10,
    NUSOUNDPLAYTOK_LOOPTYPE = 11,
} NUSOUNDPLAYTOK;

#ifdef __cplusplus
NUSOUND_FILENAME_INFO *ConfigureMusic(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd);
#endif

#ifdef __cplusplus
extern "C" {
#endif

    i32 NuSound3InitV(VARIPTR *bufferStart, VARIPTR bufferEnd, i32 zero1, i32 zero2);
    i32 NuSound3PlayStereoV(NUSOUNDPLAYTOK, ...);

    void NuSound3Init(i32 zero);

#ifdef __cplusplus
}
#endif
