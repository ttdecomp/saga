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
i32 RotDiff(u16 current, u16 target);
u16 TurnRot(u16 current, u16 target, i32 speed, i32 *difference);
u16 SeekRot(u16 current, u16 target, float rate);
void SeekVec(NUVEC *result, NUVEC *current, NUVEC *target, float rate);
void GameCam_NewShake(GAMECAMERA_s *cam, float a, float b, float c);
void GameCam_HitJudder(void);
void ChrisAllocLevelStuff(WORLDINFO_s *world);
void PodKeyReset(void);
void PodLoseSpeed(GameObject_s *obj, i32 type, i32 unknown);
void DrawMeleeTargets(i16 *targets, char *icon, float *a, i32 count);
void StartBigJump(GameObject_s *object, nuvec_s *destination, i32 mode, f32 height_scale, f32 speed_scale,
                  i32 animation, i8 flags);
GameObject_s *AddDynamicCreature(i32 model, nuvec_s *position, i32 angle, char *script_name, AIPATHINFO_s *path_info,
                                 AIGROUP_s *group, i32 set_on_surface, nugspline_s *spline, nuvec_s *spline_offset,
                                 i32 spline_mode, i32 creature_set);

#endif
