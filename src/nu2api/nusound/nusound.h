#pragma once

#include "globals.h"
#include "nu2api/nucore/common.h"

struct NuSoundStreamingSample;

typedef struct nusound_filename_info_s {
    const char *filename;
    void *field4_0x4;
    i32 index;
    i32 field3_0xc;
    u32 field1_0x4;
    u32 field5_0x14;
    struct NuSoundStreamingSample *sample;
    u32 field7_0x1c;
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

extern "C" {
#endif
    i32 NuSound3InitV(VARIPTR *bufferStart, VARIPTR bufferEnd, i32 zero1, i32 zero2);
    i32 NuSound3PlayStereoV(NUSOUNDPLAYTOK, ...);

    void NuSound3Init(i32 zero);
    void NuSound3SetSampleTable(NUSOUND_FILENAME_INFO *info, VARIPTR *buffer_start, VARIPTR buffer_end);
    void NuSound3SetRequestTable(u16 *request_bits, i32 short_count);
    void NuSound3SetLoopHoldTime(float t);
    i32 NuSound3LoadingSfx(void);

    // Stereo-stream control used by the NuMusic player. Streams live in slots
    // 0/1 (one per music voice). StreamKeyStatus reports 0 = nothing queued,
    // 1 = loaded, 2 = finished.
    void NuSound3Update(void);
    i32 NuSound3GetStereoStreamStatus(void);

    void NuSound3StopStereoStream(i32 stream_index);
    void NuSound3PauseStereoStream(i32 stream_index);
    void NuSound3ResumeStereoStream(i32 stream_index);
    i32 NuSound3StreamKeyStatus(i32 stream_index);
    void NuSound3SetStereoStreamVolume(i32 stream_index, i32 volume);
    f32 NuSound3dBToAmplitude(f32 db);
#ifdef __cplusplus
}
#endif
