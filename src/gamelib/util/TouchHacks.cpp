#include "gamelib_util_types.h"

#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/nufloat.h"

NUCOLOUR3 flashCol = {2.0f, 2.0f, 2.0f};

void TouchHacks::AiPlayerTakeDamageOnKillRescue(GameObject_s &) {
}

void TouchHacks::CalculateJumpVelToHitPoint(GameObject_s &, VuVec const &) {
}

void TouchHacks::CalculateJumpVelToHitPointDblJump(GameObject_s &, VuVec const &) {
}

void TouchHacks::CalculateXZVelForArcToHitPoint(VuVec const &, VuVec const &, float, float) {
}

void TouchHacks::CanBlowupBeBlownUp(GIZMOBLOWUP_s &, i32) {
}

void TouchHacks::CanForceTargetObj(GameObject_s &, GameObject_s &) {
}

void TouchHacks::CanJump(GameObject_s &) {
}

void TouchHacks::CanJumpToPoint(GameObject_s &, AIPATHNODE_s const &) {
}

void TouchHacks::CanJumpToPoint(GameObject_s &, VuVec const &) {
}

void TouchHacks::CanLunge(GameObject_s &) {
}

void TouchHacks::CanPoo(GameObject_s &) {
}

void TouchHacks::CanShoot(GameObject_s &) {
}

void TouchHacks::CanSlam(GameObject_s &) {
}

void TouchHacks::CanTagTo(GameObject_s &, GameObject_s &) {
}

void TouchHacks::CanTagVehicle(GameObject_s &, GameObject_s &) {
}

void TouchHacks::CanThrowBountyBomb(GameObject_s &) {
}

void TouchHacks::CanToggleTo(GameObject_s &, i32) {
}

void TouchHacks::CanUseBuildIt(GameObject_s &) {
}

void TouchHacks::CanUseGizForce(GameObject_s &) {
}

void TouchHacks::CanUseGizForce(GameObject_s &, GIZFORCE_s &) {
}

void TouchHacks::CanUseHatMachine(GameObject_s &) {
}

void TouchHacks::CanUseLever(GameObject_s &) {
}

void TouchHacks::CanUseTeleport(GameObject_s &) {
}

void TouchHacks::CanUseVehicleSmartBomb(GameObject_s &) {
}

void TouchHacks::CanUseZipup(GameObject_s &) {
}

void TouchHacks::CheckForAboutToRunIntoKillTerrain(GameObject_s &, float) {
}

void TouchHacks::CheckForAboutToRunOffAnEdge(GameObject_s &, float) {
}

void TouchHacks::CheckJumpForLandingSpot(GameObject_s &, float) {
}

void TouchHacks::CleanupAllMechObjectInterfaces(WORLDINFO_s *) {
}

void TouchHacks::FindBombTarget(GameObject_s &) {
}

nucolour3_s *TouchHacks::GetFlashColour() {
    return &flashCol;
}

void TouchHacks::GetIncomingPartRange() {
}

void TouchHacks::GetLoseStudsDieValue() {
}

void TouchHacks::GetLoseStudsFallValue() {
}

void TouchHacks::InParty(GameObject_s &) {
}

void TouchHacks::PlaySmartBombBuildupEffects(GameObject_s &, float, float) {
}

void TouchHacks::ShouldAutoGrabDragBomb(GameObject_s &) {
}

void TouchHacks::ShouldBlock(GameObject_s &) {
}

void TouchHacks::ShouldDeflectBolt(GameObject_s &, BOLT_s &) {
}

bool TouchHacks::ShouldFlash(float timer) {
    return timer > 0.0f && NuFmod(timer, 0.3f) < 0.15f;
}

void TouchHacks::ShouldKeepWeaponOut(GameObject_s &) {
}

void TouchHacks::ShouldPutWeaponAway(GameObject_s &) {
}

void TouchHacks::SolveRoot(float, float, float, float &, float &) {
}

void TouchHacks::TriggerVehicleSmartBomb(GameObject_s &) {
}
