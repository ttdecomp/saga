#include "decomp.h"
#include "legoapi_types.h"

void MoveBlocks(WORLDINFO_s *, pushblock_s *, i32, nuvec_s *) {
}

void MovePlayer(GameObject_s *) {
}

void Move_BEAST(GameObject_s *) {
}

void Move_BARMAN(GameObject_s *) {
}

void Move_CANNON(GameObject_s *) {
}

void Move_WALKER(GameObject_s *) {
}

void Move_WEIRDO(GameObject_s *) {
}

void Move_CRITTER(GameObject_s *) {
}

void Move_DEFAULT(GameObject_s *) {
}

void Move_DRAGBOMB(GameObject_s *) {
}

void Move_DROIDEKA(GameObject_s *) {
}

void MoveGameCamera(GAMECAMERA_s *) {
}

void MovePlayer_POD(GameObject_s *) {
}

void Move_CHARACTER(GameObject_s *) {
}

void Move_GEONOSIAN(GameObject_s *) {
}

void Move_HOVERDROID(GameObject_s *) {
}

void MovePlayerSpline(GameObject_s *) {
}

void MovePlayer_TWIST(GameObject_s *) {
}

void MoveSubItemsLeft(i32 *, nuvec_s *, i32) {
}

void Move_SPEEDERBIKE(GameObject_s *) {
}

void MovePlayer_CIRCLE(GameObject_s *) {
}

void MoveSubItemsRight(i32 *, nuvec_s *, i32) {
}

void Move_DROIDGENERIC(GameObject_s *) {
}

void MovePlayer_NETWORK(GameObject_s *) {
}

void MovePlayer_ROLLING(GameObject_s *) {
}

void MoveSplinePosition(SPLINEPOS_s *, float) {
}

void MoveBlocksOverBlock(WORLDINFO_s *, pushblock_s *, i32, nuvec_s *) {
}

void MoveInactiveVehicle(GameObject_s *, i32, GameObject_s **) {
}

void MovePlayer_GUNSHIPIN(GameObject_s *) {
}

void Move_REPUBLICGUNSHIP(GameObject_s *) {
}

void Move_SUPERBATTLEDROID(GameObject_s *) {
}

void MovePlayer_DIRECTIONAL(GameObject_s *) {
}

void MovePlayer_VEHICLEDIRECTIONAL(GameObject_s *) {
}

void Move_POD(GameObject_s *) {
}

void Move_ATAT(GameObject_s *) {
}

void Move_JAWA(GameObject_s *) {
}

void Move_JEDI(GameObject_s *) {
}

void MoveToMarker::BlowUp() {
}

void MoveToMarker::FadeOut() {
}

MoveToMarker::MoveToMarker(MechObjectInterface &) {
}

void MoveToMarker::Process(float) {
}

void MoveToMarker::Render() {
}

static __used__ void JumpAnimCode(GameObject_s *) {
}
static __used__ i32 Jump_UpdateHint(HINT_s *) {
    return 0;
}
static __used__ bool Jump_PreventJump(GameObject_s *) {
    return false;
}
static __used__ void Jump_EndOfLandContext(GameObject_s *) {
}
static __used__ void MoveAnim_Check(GameObject_s *) {
}
static __used__ void MoveAnim_Manage(GameObject_s *, f32, i32, i32) {
}
static __used__ i32 Move_UpdateHint(HINT_s *) {
    return 0;
}
static __used__ i32 LastSafePosExtra(GameObject_s *) {
    return 0;
}

struct LEVER_s;
static __used__ void Lever_Reset(LEVER_s *) {
}
static __used__ bool Lever_UpdateHint(HINT_s *) {
    return false;
}
struct _vuv_s;
static __used__ void MakeWingFormation(_vuv_s *, _vuv_s *, f32, i32) {
}

static __used__ void AtatPart_Stop(PART_s*) {}

static __used__ void AtatPart_Update(PART_s*) {}

static __used__ void BigJump_EndOfLand(GameObject_s*) {}

static __used__ void BigJump_JumpAction_Default(GameObject_s*) {}

static __used__ void BigJump_LandAction_Default(GameObject_s*) {}

static __used__ bool AutoJump_UpdateHint(HINT_s*) { return {}; }

static __used__ void VehicleStuff_UpdateHint(HINT_s *) {
}

void Slam_Start(GameObject_s *, float) {
}

void StartLunge(GameObject_s *, float, float) {
}

void StartSlide(GameObject_s *, i32) {
}

void CanObjSlide(GameObject_s *, i32) {
}

void CanStepBack(GameObject_s *) {
}

void FlattenCode(GameObject_s *) {
}

void Glide_Start(GameObject_s *) {
}

void JetPackCode(GameObject_s *, i32, i32, i32) {
}

void SeekLinearF(float, float, float) {
}

void StartLaunch(GameObject_s *) {
}

void ApplyGravity(GameObject_s *, float *, float, float, float *) {
}

void SetObjTarget(GameObject_s *, GameObject_s *) {
}

void SnapPosTaken(WORLDINFO_s *, pushblock_s *, nuvec_s *, i32) {
}

void StartFlatten(GameObject_s *, GameObject_s *) {
}

void UpdateMidPos(GIZMOBLOWUP_s *) {
}

void Hang_MoveCode(GameObject_s *) {
}

void HoldCode_Copy(GameObject_s *) {
}

void SetHeadTarget(GameObject_s *, nuvec_s *, signed char, float, float, float) {
}

void StartBackFlip(GameObject_s *) {
}

void ForcedBackCode(GameObject_s *) {
}

void Glide_MoveCode(GameObject_s *) {
}

void SetObjOnSurface(GameObject_s *, i32) {
}

void TurnCodeCamSafe(GameObject_s *, numtx_s *) {
}

void RotateGameMatrix(numtx_s *, i32, u16, u16, u16) {
}

void Hang_SetTargetMom(GameObject_s *) {
}

void Glide_SetTargetMom(GameObject_s *) {
}

void SetObjAsHeadTarget(GameObject_s *, GameObject_s *, signed char, float, float, float) {
}

void Slide_SetTargetMom(GameObject_s *, u16, float) {
}

void StepBackFromTarget(GameObject_s *) {
}

void ApplyGravity_Network(GameObject_s *) {
}

void VehicleCollisionCode(GameObject_s *) {
}

void VehicleTurnOrLoopOffset(GameObject_s *) {
}

void WallShuffle_SetTargetMom(GameObject_s *, u16) {
}

void SetMoveAndAnimateFunctions(u32, u32, u32, u32, i32, void *, void *, void *) {
}

void SeekRot(u16, u16, float) {
}

void SeekVec(nuvec_s *, nuvec_s *, nuvec_s *, float) {
}

void TurnRot(u16, u16, i32, i32 *) {
}

void HoldCode(GameObject_s *) {
}

void SeekValF(float, float, float) {
}

void TurnCode(GameObject_s *, i32, GAMEPAD_s *) {
}

void FloatCode(GameObject_s *) {
}

void SlideCode(GameObject_s *) {
}

void StartHold(GameObject_s *) {
}

void StartTurn(GameObject_s *) {
}
