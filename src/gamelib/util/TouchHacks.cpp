#include "gamelib_util_types.h"

#include "globals.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/nufloat.h"

NUCOLOUR3 flashCol = {2.0f, 2.0f, 2.0f};
bool TouchHacks::TouchControlsActive;
extern i32 BonusArea;

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

f32 TouchHacks::GetIncomingPartRange() {
    return 16.0f;
}

i32 TouchHacks::GetLoseStudsDieValue() {
    return BonusArea != 0 ? 10000 : 1000;
}

i32 TouchHacks::GetLoseStudsFallValue() {
    return 0;
}

bool TouchHacks::InParty(GameObject_s &object) {
    for (i32 index = 0; index < 8; ++index) {
        if (Player[index] == &object) {
            return true;
        }
    }
    return false;
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

bool TouchHacks::SolveRoot(float a, float b, float c, float &root1, float &root2) {
    if (a == 0.0f) {
        return false;
    }

    const f32 discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return false;
    }

    const f32 square_root = NuFsqrt(discriminant);
    const f32 denominator = a + a;
    root1 = (-b - square_root) / denominator;
    root2 = (square_root - b) / denominator;
    return true;
}

void TouchHacks::TriggerVehicleSmartBomb(GameObject_s &) {
}
