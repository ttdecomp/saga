#pragma once

#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nu3d/nucamera.h"

void do_Pad_flymode_camera(edcam_s *camera, f32 delta_time, nupad_s *pad);
void edcamSetContoller(i32 invert_pitch);

extern "C" {
    extern i32 PadFlyMode;
    extern NUMTX *ed_remap_mtx;

    f32 edcamGetDist(void);
    edcam_s *edcamGetEdCam(void);
    void edcamGetOffset(NUVEC *offset);
    void edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw);
    void edcamGetPosAngSnap(NUVEC *position, i32 *pitch, i32 *yaw);
    NUVEC *edcamGetPosPointer(void);
    void edcamMove(nupad_s *pad);
    void edcamMoveEx(nupad_s *pad, f32 delta_time);
    void edcamMtx(NUMTX *matrix);
    void edcamSet(void);
    void edcamSetAdjustFreedom(bool position_x, bool position_y, bool position_z, bool pitch, bool yaw, bool distance);
    void edcamSetAng(i32 pitch, i32 yaw);
    void edcamSetAutoSpeed(f32 move_base, f32 move_distance_scale, f32 zoom_base, f32 zoom_distance_scale);
    void edcamSetDist(f32 distance);
    void edcamSetMouseSensitivity(f32 pitch, f32 yaw, f32 movement);
    void edcamSetOffset(NUVEC *offset);
    void edcamSetPos(NUVEC *position);
    void edcamSetPosAng(NUVEC *position, i32 pitch, i32 yaw);
    void edcamSetSpeed(f32 position_x, f32 position_y, f32 position_z, f32 distance);
    void edcamSetSpeedPos(f32 position_x, f32 position_y, f32 position_z);

    void edmainExtCamera(NUCAMERA *camera);
    NUCAMERA *edmainGetCamera(void);
    i32 edmainGetCursorEnabled(void);
    void edmainSetCamera(NUMTX *matrix);
}
