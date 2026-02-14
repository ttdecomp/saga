#pragma once

#include "globals.h"
#include "nu2api.saga/nufile/nufile.h"

class NuSoundStreamingSample;

typedef struct nusound_filename_info_s {
    const char *name;
    void *field4_0x4;
    i32 index;
    i32 field3_0xc;
    u32 field1_0x4;
    NuSoundStreamingSample *sample;
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
    void NuSound3SetSampleTable(nusound_filename_info_s *info, variptr_u *buffer_start, variptr_u buffer_end);

#ifdef __cplusplus
}
#endif
