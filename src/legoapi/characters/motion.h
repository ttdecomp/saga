#ifndef LEGOAPI_CHARACTERS_MOTION_H
#define LEGOAPI_CHARACTERS_MOTION_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"

// Character motion / animation / camera helpers (module legoapi/characters).

#ifdef __cplusplus
extern "C" {
#endif
    float AnimEndFrame(void *animset, i32 unknown);
#ifdef __cplusplus
}
#endif
float SeekLinearF(float current, float target, float step);
float SeekValF(float current, float target, float step);
void GameCam_NewShake(GAMECAMERA_s *cam, float a, float b, float c);
void GameCam_HitJudder(void);
void ChrisAllocLevelStuff(WORLDINFO_s *world);
void PodKeyReset(void);
void PodLoseSpeed(GameObject_s *obj, i32 type, i32 unknown);
void DrawMeleeTargets(i16 *targets, char *icon, float *a, i32 count);
GameObject_s *AddDynamicCreature(i32 model, nuvec_s *pos, i32 unknown, char *group, AIPATHINFO_s *path,
                                 AIGROUP_s *ai_group, i32 a, nugspline_s *spline, nuvec_s *dir, i32 b, i32 c);

#endif
