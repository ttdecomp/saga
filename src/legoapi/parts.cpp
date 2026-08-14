#include "legoapi_types.h"

// Forward declarations for local (static) part/gizmo helper stubs.
struct CUSTOMPIECEANIM;
struct spacelevel_s;
struct quickboltinfo;

void PartObjectInterface::GetPos(VuVec &, i32) const {
}

void PartObjectInterface::GetRadius() const {
}

void PartObjectInterface::GetTargetName() const {
}

PartObjectInterface::PartObjectInterface(PART_s &) {
}

PartObjectInterface::~PartObjectInterface() {
}

void PART_s::ClearMechObjectInterface() {
}

void PART_s::GetMechObjectInterface() {
}

// Local (static) part (PART_s), pickup (GIZMOPICKUP_s), gizmo flow
// (GIZFLOW_s/FLOWBOX_s), power-up and space/starfighter helpers. Stubbed as
// local `t` symbols matching res/libTTapp.so.

static __used__ void PartCollide(PART_s *, i32) {
}

static __used__ void TiePart_Kill(PART_s *, i32) {
}

static __used__ void TiePart_Move(PART_s *, f32) {
}

static __used__ void TiePart_Impact(PART_s *) {
}

static __used__ void TiePart_KillExplode(PART_s *, i32) {
}

static __used__ void TieSpinZPart_Move(PART_s *, f32) {
}

static __used__ void PartImpact_Coin(PART_s *) {
}

static __used__ void PartStolen_Coin(PART_s *) {
}

static __used__ void PartExtra_BlueCoin(PART_s *) {
}

static __used__ void PartExtra_PurpleCoin(PART_s *) {
}

static __used__ void PowerUp_DrawPart(PART_s *) {
}

static __used__ void PowerUp_ImpactPart(PART_s *) {
}

static __used__ void PowerUp_UpdatePart(PART_s *) {
}

static __used__ void PowerUp_EndMsg(GAMEMESSAGE_s *) {
}

static __used__ void PowerUp_UpdateMsg(GAMEMESSAGE_s *) {
}

static __used__ void SpeederPart_Draw(PART_s *) {
}

static __used__ void SpeederPart_Kill(PART_s *, i32) {
}

static __used__ void SpeederPart_Update(PART_s *) {
}

static __used__ void SuperCarry_PartKill(PART_s *, i32) {
}

static __used__ void SuperCarry_PartImpact(PART_s *) {
}

static __used__ void SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_s *, nuvec_s *, u16, i32) {
}

static __used__ void PartDraw_VehicleHeart(PART_s *) {
}

static __used__ void PartKill_DrawCreature(PART_s *) {
}

static __used__ void PartMove_VehicleHeart(PART_s *, f32) {
}

static __used__ void PartMove_VehiclePickup(PART_s *, f32) {
}

static __used__ void PartKill_EjectedCreature(PART_s *, i32) {
}

static __used__ void UpdateAnimTimer(CHARACTERMODEL_s *, ANIMPACKET_s *, i16, f32, f32, f32, i32, char *, i32, f32) {
}

static __used__ void UpdateCustomPieceAnim(CUSTOMPIECEANIM *, u16, u16) {
}

static __used__ void Pup_CollectHeart(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectCharKit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectMinikit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectPowerUp(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectRedBrick(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_UpdatePowerUp(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdateBlueCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdatePurpleCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void ProcessFlowBox(GIZFLOW_s *, FLOWBOX_s *, u8) {
}

static __used__ void ResetForLoopEx(GIZFLOW_s *, FLOWBOX_s *, FLOWBOX_s *, i32) {
}

static __used__ void ResetGizmoFlowBox(GIZFLOW_s *, FLOWBOX_s *) {
}

static __used__ void ProcessGizmoFlowBox(GIZFLOW_s *, FLOWBOX_s *, u8) {
}

static __used__ void ProcessActionFlowBox(GIZFLOW_s *, FLOWBOX_s *, u8) {
}

static __used__ void ProcessConditionFlowBox(GIZFLOW_s *, FLOWBOX_s *, u8) {
}

static __used__ void ProcessSpaceLevel(spacelevel_s *) {
}

static __used__ void ResetSpaceLevel(WORLDINFO_s *, spacelevel_s *) {
}

static __used__ void ProcessStarFighter(starfighter_s *, quickboltinfo *) {
}

static __used__ void StarFighterAlign(starfighter_s *, _vuv_s *, f32, i32) {
}

static __used__ void TrooperTeamSetStateCode(minitrooperteam_s *) {
}
