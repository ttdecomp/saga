#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct GAMEANIMOBJPOOL_s;
struct GAMEANIMOBJ_s;
struct GAMEANIMSET_s;
struct GAMEANIMSYS_s;
struct GIZMOSYS_s;
struct GameObject_s;
struct CHARACTERMODEL_s;
struct ANIMPACKET_s;
struct nugscn_s;
struct nuhspecial_s;
struct numtx_s;
enum GAMEANIMSET_VISIBILITY : i32;

GAMEANIMSET_s *GameAnimSet_Create(VARIPTR *buf, VARIPTR *buf_end, GAMEANIMOBJPOOL_s *object_pool,
                                  GAMEANIMSYS_s *system);
GAMEANIMOBJPOOL_s *GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *buf_end, i32 object_data_size, i32 capacity);
GAMEANIMSYS_s *GameAnimSys_Create(VARIPTR *buf, VARIPTR *buf_end);
void EvalAnim(nuhspecial_s *special, f32 frame, numtx_s *matrix, i32 include_instance_translation);
void EvalAnim2(nuhspecial_s *special, f32 frame);
i32 GameAnimSet_Play(GAMEANIMSET_s *set, f32 speed, i32 evaluate_state);
i32 GameAnimSet_Stop(GAMEANIMSET_s *set);
i32 GameAnimSet_Reset(GAMEANIMSET_s *set);
i32 GameAnimSet_JumpToStart(GAMEANIMSET_s *set);
i32 GameAnimSet_JumpToEnd(GAMEANIMSET_s *set);
i32 GameAnimSet_Playing(GAMEANIMSET_s *set);
void GameAnimSet_EvalAnim(GAMEANIMSET_s *set);
f32 GameAnimSet_GetAnimPos(GAMEANIMOBJ_s *object);
void GameAnimSet_SetAnimPos(GAMEANIMOBJ_s *object, f32 position);
void GameAnimSet_SetTFactor(GAMEANIMSET_s *set, f32 factor);
void GameAnimSet_ScaleFParam1(GAMEANIMSET_s *set, f32 scale);
i32 GameAnimSet_SetRepeating(GAMEANIMSET_s *set, i32 repeating);
void GameAnimSet_EvaluateState(GAMEANIMSET_s *set);
i32 GameAnimSet_GetCentreAndRadius(GAMEANIMSET_s *set, NUVEC *centre, f32 *radius, i32 frame_selection,
                                   i32 include_animated, i32 include_static);
void GameAnimSet_GetAveragePos(GAMEANIMSET_s *set, NUVEC *position, i32 frame_selection, i32 include_animated,
                               i32 include_static);
f32 GameAnimSet_GetCompletionRatio(GAMEANIMSET_s *set);
GAMEANIMSET_VISIBILITY GameAnimSet_GetVisibility(GAMEANIMSET_s *set);
void GameAnimSet_JumpToAnimPos(GAMEANIMSET_s *set, f32 position);
void GameAnimSet_SetVisibility(GAMEANIMSET_s *set, i32 visibility);
void GameAnimSet_SetOffset(GAMEANIMSET_s *set, NUVEC *offset);
f32 GameAnimSet_GetCurrentFrame(GAMEANIMSET_s *set);
void GameAnimSet_AddToSystemList(GAMEANIMSET_s *set);
void GameAnimSet_RemoveFromSystemList(GAMEANIMSET_s *set);
void GameAnimSet_DrawReflection(GAMEANIMSET_s *set, i32 flags, f32 alpha, struct numtx_s *matrix);
GAMEANIMOBJ_s *GameAnimSet_AddObject(GAMEANIMSET_s *set, nuhspecial_s *special, f32 start_frame, f32 end_frame,
                                     i32 append);
GAMEANIMOBJ_s *GameAnimSet_AddObjectByName(GAMEANIMSET_s *set, nugscn_s *scene, char *name, f32 start_frame,
                                           f32 end_frame, i32 append, GIZMOSYS_s *gizmo_sys, char *prefix,
                                           char *suffix);
i32 GizmoFileReadGameAnimSet(GAMEANIMSET_s *set, void *world, void (*read_object_data)(GAMEANIMOBJ_s *, unsigned char),
                             unsigned char version, char *prefix, char *suffix);
void Animate_JEDI(GameObject_s *object);
void AnimatePlayer(GameObject_s *object);
#ifdef __cplusplus
extern "C" {
#endif
    i32 CurrentAnim(ANIMPACKET_s *packet);
    void ResetAnimPacket(ANIMPACKET_s *packet, i16 animation);
    void UpdateAnimPacket(CHARACTERMODEL_s *model, ANIMPACKET_s *packet, f32 frame_step, f32 movement_speed,
                          f32 blend_step, f32 backwards_multiplier);
#ifdef __cplusplus
}
#endif
