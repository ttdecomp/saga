#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/characters/motion.h"
#include "legoapi/menus/core/text.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <string.h>

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

f32 GetAspectRatio() {
    return static_cast<f32>(g_backingWidth) / static_cast<f32>(g_backingHeight);
}

static u8 ScreenGrabNeeded;
i32 pause_rt;
NUMTL *pause_rndr_mtl;
extern i32 pause_rndr_on;
extern i32 pause_fade;

extern "C" i32 NuRndrBeginScene(i32);
extern "C" void NuRndrEndScene(void);
extern "C" void NuBackbufferCopy(i32);

void NeedScreenGrab(i32 needed) {
    ScreenGrabNeeded = needed != 0;
}

extern f32 CameraZoom;
extern "C" f32 NuIOS_GetAspectRatio(void);

void WidescreenCode(i32) {
    pNuCam->aspect = 1.0f / NuIOS_GetAspectRatio();
    pNuCam->fov = (1.0f / NuIOS_GetAspectRatio() + 0.75f) * 0.5f * (1.0f / CameraZoom);
    SmartTextSetWidescreen(1.3333334f / NuIOS_GetAspectRatio(), 1.0f);
}

void GrabStillScreen() {
    if (ScreenGrabNeeded != 0) {
        ScreenGrabNeeded = 0;
        NuRndrBeginScene(-1);
        NuBackbufferCopy(pause_rt);
        NuRndrEndScene();
        pause_fade = 0;
    }
}

__attribute__((weak)) void InitStillRender(variptr_u *, variptr_u) {
    static NUNATIVETEX nativePauseTex;

    pause_rt = NuTexGenTexture(&nativePauseTex);
    nativePauseTex.ref_count = 1;
    memset(nativePauseTex.checksum, 0, sizeof(nativePauseTex.checksum));
    nativePauseTex.width = g_backingWidth;
    nativePauseTex.height = g_backingHeight;
    nativePauseTex.image_data = NULL;
    nativePauseTex.size = 0;

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/legoapi.saga/screen.cpp", 0x561);
    glGenTextures(1, &nativePauseTex.platform.gl_tex);
    glActiveTexture(GL_TEXTURE0);
    g_currentTexUnit = 0;
    glBindTexture(GL_TEXTURE_2D, nativePauseTex.platform.gl_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_backingWidth, g_backingHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/legoapi.saga/screen.cpp", 0x56d);

    pause_rndr_mtl = NuMtlCreate3D(1);
    pause_rndr_mtl->sort_pri = 0x7fff;
    pause_rndr_mtl->shader_desc.flags = 0x1000;
    pause_rndr_mtl->opacity = 1.0f;
    u8 *attrib = reinterpret_cast<u8 *>(&pause_rndr_mtl->attribs);
    attrib[1] = static_cast<u8>((attrib[1] & 0x30) | 0xc5);
    attrib[2] = static_cast<u8>((attrib[2] & 0xfc) | 6);
    attrib[0] = static_cast<u8>((attrib[0] & 0xc0) | 0x11);
    attrib[1] = 0xe5;
    pause_rndr_mtl->tex_id = static_cast<i16>(pause_rt);
    pause_rndr_mtl->shader_desc.diffuse_color[0] = -1;
    pause_rndr_mtl->shader_desc.unknown_a8 = 1;
    pause_rndr_mtl->shader_desc.vtx_desc.flags |= 0x40800;
    NuMtlUpdate(pause_rndr_mtl);
    pause_rndr_on = 0;
}

i8 IsGrabbingScreen() {
    return ScreenGrabNeeded;
}

void LoadShaderSource(char **, i32 *, u32, bool) {
}

void ScreenDumpAviEnd() {
}

void UpdateCutBorders() {
    f32 target_scale = 1.0f;

    if (CUTSTOPGAME == 0 && (LEGOCAMMODE_DOORCUT == -1 || GameCam->mode != LEGOCAMMODE_DOORCUT)) {
        if (LEGOCAMMODE_OBSTACLE == -1 || GameCam->mode != LEGOCAMMODE_OBSTACLE || ObstacleCamBorders == 0) {
            target_scale = 0.0f;
        }
    }

    CutBorderScale = SeekLinearF(CutBorderScale, target_scale, FRAMETIME * 2.0f);
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
