#pragma once

#include "legoapi/gameobject.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nuvuvec.hpp"

struct BOLT_s;
struct GIZMOBLOWUP_s;
struct GIZFORCE_s;
struct AIPATHNODE_s;

namespace TouchHacks {

    extern bool TouchControlsActive;

    i32 GetLoseStudsDieValue();
    i32 GetLoseStudsFallValue();
    bool ShouldAutoGrabDragBomb(GameObject_s &object);
    f32 GetIncomingPartRange();
    bool ShouldBlock(GameObject_s &object);
    bool AiPlayerTakeDamageOnKillRescue(GameObject_s &object);
    bool CanUseVehicleSmartBomb(GameObject_s &object);
    void TriggerVehicleSmartBomb(GameObject_s &object);
    bool CanThrowBountyBomb(GameObject_s &object);
    bool CheckJumpForLandingSpot(GameObject_s &object, f32 distance);
    bool CheckForAboutToRunIntoKillTerrain(GameObject_s &object, f32 distance);
    bool CheckForAboutToRunOffAnEdge(GameObject_s &object, f32 distance);
    bool SolveRoot(f32 a, f32 b, f32 c, f32 &root1, f32 &root2);
    VuVec CalculateXZVelForArcToHitPoint(const VuVec &origin, const VuVec &target, f32 vertical_velocity, f32 gravity);
    VuVec CalculateJumpVelToHitPoint(GameObject_s &object, const VuVec &target);
    VuVec CalculateJumpVelToHitPointDblJump(GameObject_s &object, const VuVec &target);
    bool CanJumpToPoint(GameObject_s &object, const VuVec &target);
    bool CanJumpToPoint(GameObject_s &object, const AIPATHNODE_s &node);
    bool CanPoo(GameObject_s &object);
    void PlaySmartBombBuildupEffects(GameObject_s &object, f32 elapsed, f32 duration);
    bool CanLunge(GameObject_s &object);
    bool CanSlam(GameObject_s &object);
    bool CanShoot(GameObject_s &object);
    bool CanUseTeleport(GameObject_s &object);
    bool CanUseZipup(GameObject_s &object);
    bool ShouldKeepWeaponOut(GameObject_s &object);
    bool ShouldPutWeaponAway(GameObject_s &object);
    bool CanJump(GameObject_s &object);
    bool CanUseBuildIt(GameObject_s &object);
    bool CanUseGizForce(GameObject_s &object);
    bool CanUseGizForce(GameObject_s &object, GIZFORCE_s &force);
    bool CanTagTo(GameObject_s &object, GameObject_s &target);
    bool CanToggleTo(GameObject_s &object, i32 character_id);
    bool CanTagVehicle(GameObject_s &object, GameObject_s &target);
    MechObjectInterface *FindBombTarget(GameObject_s &object);
    bool CanForceTargetObj(GameObject_s &object, GameObject_s &target);
    bool InParty(GameObject_s &object);
    bool ShouldFlash(f32 time);
    NUCOLOUR3 *GetFlashColour();
    bool ShouldDeflectBolt(GameObject_s &object, BOLT_s &bolt);
    bool CanBlowupBeBlownUp(GIZMOBLOWUP_s &blowup, i32 hit_type);
    bool CanUseLever(GameObject_s &object);
    bool CanUseHatMachine(GameObject_s &object);

} // namespace TouchHacks
