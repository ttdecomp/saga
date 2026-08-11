#include "legoapi/gameobject.h"

struct GAMECHARACTERDATA_s;

extern "C" {
i16 LEGOACT_BUILD;
i16 LEGOACT_FALL;
i16 LEGOACT_LUNGE;
i16 LEGOACT_SLAM;
f32 animduration_blendouttime;

f32 AnimDuration(i32, i32, f32 start, f32 end, i32) {
    return end > start ? end - start : 1.0f;
}

f32 AnimSpeed(GAMECHARACTERDATA_s *, i32) {
    return 1.0f;
}

i32 AnimPlaying(void *, i32, i32, i32) {
    return 0;
}

void ResetAnimPacket(void *, i32) {}

void ResetMiniAnimPacket(void *, i32) {}

void UpdateAnimPacket(GAMECHARACTERDATA_s *, void *, f32, f32, f32, f32) {}

f32 NuAnimEndFrame(void *) {
    return 0.0f;
}
}

void SetAnimTimeRandom(GAMECHARACTERDATA_s *, f32 *frame) {
    if (frame != NULL) {
        *frame = 0.0f;
    }
}
