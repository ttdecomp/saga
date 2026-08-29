#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/text.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void ClearScreen() {
}

void RenderQuads(i16 *) {
}

void InitAlphaList() {
}

void GetAspectRatio() {
}

void NeedScreenGrab(i32) {
}

extern f32 CameraZoom;
extern "C" f32 NuIOS_GetAspectRatio(void);

void WidescreenCode(i32) {
    f32 aspect = NuIOS_GetAspectRatio();
    pNuCam->aspect = 1.0f / aspect;
    aspect = NuIOS_GetAspectRatio();
    pNuCam->fov = (1.0f / aspect + 0.75f) * 0.5f * (1.0f / CameraZoom);
    aspect = NuIOS_GetAspectRatio();
    SmartTextSetWidescreen(1.3333334f / aspect, 1.0f);
}

void GrabStillScreen() {
}

void InitStillRender(variptr_u *, variptr_u) {
}

void IsGrabbingScreen() {
}

void LoadShaderSource(char **, i32 *, u32, bool) {
}

void ScreenDumpAviEnd() {
}

void UpdateCutBorders() {
}

void HandleStillRender() {
}

void LinkShaderProgram(u32) {
}

void PreRenderFlashHack() {
}

void ScreenDumpAviBegin(i32, char *, i32, i32, i32, i32, float, float, i32) {
}

void ScreenDumpAviFrame() {
}

void UCStretchToCorners(i16 *, i16 *) {
}

void PostRenderFlashHack() {
}

void LookupPreloadedShaderObject(u32, u32 **, LoadedUniqueShaderRecord *, u32) {
}
