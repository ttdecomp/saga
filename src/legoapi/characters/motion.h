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
GameObject_s *AddDynamicCreature(i32 model, nuvec_s *position, i32 angle, char *script_name, AIPATHINFO_s *path_info,
                                 AIGROUP_s *group, i32 set_on_surface, nugspline_s *spline, nuvec_s *spline_offset,
                                 i32 spline_mode, i32 creature_set);

#endif
