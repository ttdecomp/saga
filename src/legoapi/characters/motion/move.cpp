#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"

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

static void PlayerCamPos(GameObject_s *object, NUVEC *camera_position, NUVEC *) {
    CHARACTERDATA *character = object->apiobj.character_data;
    f32 centre_height = (character->field15_0x34 + character->field16_0x38) * character->field17_0x3c * 0.5f;

    // This is the common character path in the original. Special vehicles,
    // grapples and targetable objects select a different focus point below
    // this branch; ordinary hub characters use their world position plus the
    // vertical centre of the configured character bounds.
    *camera_position = object->apiobj.position;
    camera_position->y += centre_height;
}

static void SetGameCameraView(GAMECAMERA_s *camera, const NUVEC &position, const NUVEC &target) {
    NUVEC delta;
    NuVecSub(&delta, const_cast<NUVEC *>(&target), const_cast<NUVEC *>(&position));

    NUANG pitch = static_cast<NUANG>(-NuAtan2D(delta.y, NuFsqrt(delta.x * delta.x + delta.z * delta.z)));
    NUANG yaw = static_cast<NUANG>(NuAtan2D(delta.x, delta.z));

    NuMtxSetRotationZ(&camera->mtx, 0);
    NuMtxRotateX(&camera->mtx, pitch);
    NuMtxRotateY(&camera->mtx, yaw);
    NuMtxTranslate(&camera->mtx, const_cast<NUVEC *>(&position));
    camera->render_mtx = camera->mtx;
    camera->pos = position;
    camera->target = target;

    if (pNuCam != NULL) {
        pNuCam->mtx = camera->render_mtx;
        NuCameraSet(pNuCam);
    }
}

void MoveGameCamera(GAMECAMERA_s *camera) {
    // Original title-camera mode (3): portal_places[2] contains one camera
    // position followed by its look target.  The complete function selects
    // many gameplay camera modes; only this currently reachable mode is
    // transcribed here.
    if (camera == NULL || WORLD == NULL || WORLD->current_level == NULL) {
        return;
    }

    if (WORLD->current_level == TITLES_LDATA) {
        if (WORLD->portal_places == NULL || WORLD->portal_places[2] == NULL ||
            WORLD->portal_places[2]->positions == NULL) {
            return;
        }
        f32 *points = WORLD->portal_places[2]->positions;
        NUVEC position = {points[0], points[1], points[2]};
        NUVEC target = {points[3], points[4], points[5]};
        camera->mode = 3;
        SetGameCameraView(camera, position, target);
        camera->previous_mode = camera->mode;
        return;
    }

    if (WORLD->sock_sys == NULL) {
        return;
    }

    camera->mode = 1;
    NUVEC player_camera_positions[2];
    NUVEC player_positions[2];
    i32 player_count = 0;
    for (i32 i = 0; i < 2; ++i) {
        if (Player[i] == NULL) {
            continue;
        }
        PlayerCamPos(Player[i], &player_camera_positions[player_count], &camera->pos);
        player_positions[player_count] = Player[i]->apiobj.position;
        ++player_count;
    }
    if (player_count == 0) {
        return;
    }

    NUVEC position;
    NUVEC target;
    f32 overlap_blend;
    f32 position_seek;
    f32 angle_seek;
    f32 terrain_clearance;
    f32 separation_scale;
    i32 found_socket =
        SockSysCamera(WORLD->sock_sys, &camera->pos, camera->mode != camera->previous_mode, player_camera_positions,
                      player_positions, player_count, &camera->sock_position, &position, &target, &overlap_blend,
                      &position_seek, &angle_seek, &terrain_clearance, &separation_scale);
    if (found_socket != 0) {
        camera->position_seek = position_seek;
        camera->angle_seek = angle_seek;
        SetGameCameraView(camera, position, target);
    }
    camera->previous_mode = camera->mode;
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

static __used__ void AtatPart_Stop(PART_s *) {
}

static __used__ void AtatPart_Update(PART_s *) {
}

static __used__ void BigJump_EndOfLand(GameObject_s *) {
}

static __used__ void BigJump_JumpAction_Default(GameObject_s *) {
}

static __used__ void BigJump_LandAction_Default(GameObject_s *) {
}

static __used__ bool AutoJump_UpdateHint(HINT_s *) {
    return {};
}

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

float SeekLinearF(float current, float target, float step) {
    if (current < target) {
        current += step;
        if (current > target) {
            current = target;
        }
    } else if (current > target) {
        current -= step;
        if (current < target) {
            current = target;
        }
    }
    return current;
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

void RotateGameMatrix(numtx_s *matrix, i32 order, u16 x, u16 y, u16 z) {
    switch (order) {
        case 0:
            if (x != 0)
                NuMtxRotateX(matrix, x);
            if (y != 0)
                NuMtxRotateY(matrix, y);
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            break;
        case 1:
            if (y != 0)
                NuMtxRotateY(matrix, y);
            if (x != 0)
                NuMtxRotateX(matrix, x);
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            break;
        case 2:
            if (y != 0)
                NuMtxRotateY(matrix, y);
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            if (x != 0)
                NuMtxRotateX(matrix, x);
            break;
        case 3:
            if (z != 0)
                NuMtxRotateZ(matrix, z);
            if (x != 0)
                NuMtxRotateX(matrix, x);
            if (y != 0)
                NuMtxRotateY(matrix, y);
            break;
    }
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

float SeekValF(float, float, float) {
    return 0.0f;
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
