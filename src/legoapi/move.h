#pragma once

#include "legoapi/gameobject.h"

struct GIZMOBLOWUP_s;
struct PART_s;
struct LEVELDATA_s;
struct AREADATA_s;
struct HINT_s;
struct nuhspecial_s;

bool TwistLevel(LEVELDATA_s *level);
bool GunshipInLevel(LEVELDATA_s *level);
bool CircleLevel(LEVELDATA_s *level);
bool PodLevel(AREADATA_s *area);
i32 ObjZappedBlue(GameObject_s *object);
bool FaceOpponent(GameObject_s *object, NUVEC *position);
f32 GetVehicleAreaRememberSpeed();
void AveragePlayerCurrentSpeedMul();
f32 ForceAlongSock(GameObject_s *object);
f32 ForceTowardsMid(GameObject_s *object);
void CurrentStart(GameObject_s *object, i32 always, i32 use_socket_rotation);
f32 VehicleTurnOrLoopOffset(GameObject_s *object);
f32 GetHoverPosY(GameObject_s *object);
f32 GetVehicleHoverHeight(GameObject_s *object, f32 *vertical_offset);
f32 GetVehicleSpeedMul(GameObject_s *object, f32 speed);
void PostResetCode(GameObject_s *object);
void DrawOffsetCode(GameObject_s *object, i32 immediate);
void NewSeekHalfLife(i32 &current, i32 target, f32 amount);
void StartJetPackFall(GameObject_s *object, i32 immediate);
void JetPackCode(GameObject_s *object, i32 land, i32 fall, i32 immediate);
void ComboRotateCode(GameObject_s *object, i32 hold);
i32 Dodge_UpdateHint(HINT_s *hint);
void MakeThrowVector(NUVEC *output, NUVEC *source, NUVEC *target, NUVEC *target_velocity, f32 speed, f32 gravity);
void LightSabreDebris(GameObject_s *object);
void StartLaunch(GameObject_s *object);
void StartFlatten(GameObject_s *attacker, GameObject_s *victim);
void FlattenCode(GameObject_s *object);
void ReleaseEat(GameObject_s *object);
void GrabVictim(GameObject_s *object, GameObject_s *victim);
void EatVictim(GameObject_s *object);
void StartTurn(GameObject_s *object);
void ApplyExtraRotation(GameObject_s *object, NUMTX *matrix);
void MoveInactiveVehicle(GameObject_s *object, i32 mode, GameObject_s **target_player);
bool NeedsPretendAnim(GameObject_s *object);
void SetBallooningHeight(GameObject_s *object, f32 height);
void StartBallooning(GameObject_s *object, i32 type);
void LetGoOfBalloon(GameObject_s *object);
void PopBalloon(GameObject_s *object);
i32 Glide_Start(GameObject_s *object);
void Glide_MoveCode(GameObject_s *object);
i32 Glide_SetTargetMom(GameObject_s *object);
i32 ForcePushed_YRotation(GameObject_s *object);
i32 ForcePushed_SetTargetMom(GameObject_s *object, f32 *speed);
i32 ForcePushed_SuperPush_Occurring(GameObject_s *first, GameObject_s *second);
void ForcedBackCode(GameObject_s *object);
void ForceLightning_Origin(GameObject_s *object, NUVEC *origin, NUVEC *target);
i32 LightSabre_ColourFromObj(i32 colour, i32 *object_id);
void EndForce(GameObject_s *object, i32 immediate);
void ReleaseForce(GameObject_s *object, i32 immediate);
void ResetForceBack();
void SetForceBack(GameObject_s *object, NUVEC *position, f32 radius, i32 type);
void DrawForceBackEffect(nuhspecial_s *special);
void ResetForceGlow(PLAYERPACKET *player_packet);
i32 SetObjTarget(GameObject_s *object, GameObject_s *target);
i32 SetGizmoBlowUpTarget(GameObject_s *object, GIZMOBLOWUP_s *target);
i32 SetPartTarget(GameObject_s *object, PART_s *target);

extern f32 avg_currentspeed_mul;
