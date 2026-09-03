#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/legoapi_types.h"

void SuperCarry_Start(GameObject_s *, GIZMOBLOWUP_s *, i32) {
}

void SuperCarry_Throw(GameObject_s *, i32) {
}

void SuperCarry_Release(GameObject_s *) {
}

void SuperCarry_Carrying(GameObject_s *) {
}

void SuperCarry_MoveCode(WORLDINFO_s *, GameObject_s *) {
}

void SuperCarry_Possible(GameObject_s *, i32) {
}

void SuperCarry_YRotation(GameObject_s *, u16) {
}

void SuperCarry_DrawObject(GameObject_s *) {
}

void SuperCarry_GetObjectPos(GameObject_s *, nuvec_s *, nuvec_s *) {
}

void SuperCarry_SetTargetMom(GameObject_s *, float) {
}

static __used__ unsigned int CanSuperCarry_Game(GameObject_s *) {
    return {};
}

void ObjLandReady(GameObject_s *) {
}

void LetGoOfBalloon(GameObject_s *) {
}

bool MovingBackwards(GameObject_s *object) {
    GAMEPAD_s *pad = object->pad_gamepad;
    if (pad == NULL || pad->operator_data == NULL || pad->input_magnitude == 0.0f) {
        return false;
    }

    const u16 camera_yaw = GameCam != NULL ? static_cast<u16>(GameCam->input_yaw) : 0;
    const i32 difference = RotDiff(static_cast<u16>(pad->input_angle + camera_yaw), object->apiobj.field_0x276);
    return static_cast<u32>(difference + 0x4000) > 0x8000;
}
