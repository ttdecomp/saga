#include "legoapi/world_shared.h"

#include "SwipeDecalRenderer.h"
#include "nu2api/nufile/nufile.h"

struct GAMEMESSAGE_s;
struct HINT_s;
struct MENU_s;
struct NuBloomParameters;
struct STATUSPACKET_s;
struct TouchHolder;
struct _vum_s;
struct _vuv_s;
struct nuhspecial_s;
struct ripple_node_s;
struct ripple_set_s;
struct VuVec;

// Camera zoom state
f32 CameraZoom;

// Graphics loading flags
i32 RemoveDirectionalMaps = 0;
i32 RemoveNormalMaps = 0;

extern "C" {
    void NuDisplaySceneRndr(void *scene) {
        (void)scene;
    }

    void NuPortalVisibility(NUGSCN *scene) {
        (void)scene;
    }
    void NuGScnRndr3(NUGSCN *scene) {
        NuDisplaySceneRndr(scene->display_list);
    }
}

void SetCameraZoom(f32 zoom) {
    CameraZoom = zoom;
}

extern "C" void NuGScnUpdate(NUGSCN *gscn, i32 param) {
    (void)gscn;
    (void)param;
}

// --- NuGScn gfx-upload helpers: C++ / file-local (static) in original ---
// NuGScnUploadGfxDataFromFilePS has C++ linkage (mangled `_Z29NuGScnUploadGfxDataFromFilePSP9variptr_uS_i`);
// NuReadGraphicsData is a C++ static function in the original (GCC clones it,
// hence the `.isra.NNN` suffix in the ROM symbol table).

i32 NuGScnUploadGfxDataFromFilePS(VARIPTR *buf, VARIPTR buf_end, i32 file) {
    (void)buf;
    (void)buf_end;
    (void)file;
    return 0;
}

static NUGSCN *NuReadGraphicsData(VARIPTR *buf, VARIPTR *buf_end, char *path, NUGSCN *scene) {
    if (scene != NULL) {
        return scene;
    }

    char converted_path[1033];
    NuFileExtConvert(converted_path, path, 0x400);
    NUFILE file = NuFileOpen(converted_path, NUFILE_READ);
    if (file == 0) {
        return NULL;
    }

    i32 file_size = (i32)NuFileOpenSize(file);
    buf->addr = ALIGN(buf->addr, 0x20);
    i32 uploaded = NuGScnUploadGfxDataFromFilePS(buf, *buf_end, (i32)file);
    NUGSCN *loaded = (NUGSCN *)ALIGN(buf->addr, 0x20);
    buf->addr = (usize)((char *)loaded + file_size - uploaded);
    NuFileRead(file, loaded, file_size - uploaded);
    NuFileClose(file);
    return loaded;
}

// --- Extern "C": NuGScn functions have C linkage in original ---
extern "C" {
    void *NuGScnRead(VARIPTR *buf, VARIPTR buf_end, char *path) {
        RemoveDirectionalMaps = 1;
        RemoveNormalMaps = 1;
        NUGSCN *scene = NuReadGraphicsData(buf, &buf_end, path, NULL);
        RemoveNormalMaps = 0;
        RemoveDirectionalMaps = 0;
        return scene;
    }
    void NuGScnReadFromMemory(NUGSCN *scene) {
        NuReadGraphicsData(NULL, NULL, NULL, scene);
    }
    void NuGHGFixup(NUGSCN *scene) {
        NuGScnReadFromMemory(scene);
    }
} // extern "C"

void NuSpecialFind(NUGSCN *scene, void **dest, char *name) {
    (void)scene;
    (void)dest;
    (void)name;
}

void DrawCables() {
}

void DrawRipple(ripple_node_s *) {
}

void DrawShop3D(WORLDINFO_s *) {
}

void DrawAreaBox(nuvec_s *, nuvec_s *, i32, i32) {
}

void DrawBox_Now(_vuv_s *, _vuv_s *, i32, i32) {
}

void DrawLocator(nuvec_s *, float, i32, i32) {
}

void DrawStreaks() {
}

void Draw_LOADED() {
}

void Draw3DObject(WORLDINFO_s *, i32, nuvec_s *, u16, u16, u16, float, float, float,
                  i32) {
}

void DrawCharIcon(i32, float, float, float, float, i32, float, float, i32, nuhspecial_s *) {
}

void DrawCodeMenu() {
}

void DrawHint_LSW(HINT_s *, i32) {
}

void DrawLine_Now(_vuv_s *, _vuv_s *, i32, i32) {
}

void DrawParallax(nuhspecial_s *) {
}

void DrawQuestion(nuvec_s *, float, float) {
}

void DrawRectRGBA(float, float, float, float, u32, numtl_s *, i32, float) {
}

void DrawSubItems() {
}

void DrawTopShelf(i32) {
}

void DrawTorpedos(GameObject_s *) {
}

void Draw_LOADING() {
}

void DrawAlphaGrid(i32, i32, NuBloomParameters *) {
}

void DrawArrow_Now(_vum_s *, float, i32, i32) {
}

void DrawBonusTime(STATUSPACKET_s *, float, i32) {
}

void DrawCross_Now(_vuv_s *, float, i32, i32) {
}

void DrawGameState(float, float, i32, i32) {
}

void DrawPauseFade() {
}

void DrawRippleSet(ripple_set_s *) {
}

void DrawSaveSlots(MENU_s *, float) {
}

void DrawShopPanel() {
}

void DrawSnakeBody(GameObject_s *) {
}

void DrawAlphaImage(i32, i32, numtl_s *, i32, NuBloomParameters *) {
}

void DrawBezierLine(VuVec &, VuVec &, VuVec &, VuVec &, numtl_s *, i32) {
}

void DrawBonusScore(float, i32, i32, float, i32 *) {
}

void DrawBoxMtx_Now(_vum_s *, _vuv_s *, i32, i32) {
}

void DrawBuildUpBar(float, float, i32, i32, float, float, float, u16) {
}

void DrawCodeMenu3D() {
}

void DrawCutBorders(i32) {
}

void DrawExplosions() {
}

void DrawItemMenu2D() {
}

void DrawMessageBox(i32, float, float, float, float) {
}

void DrawRopeCurved(nuvec_s *, nuvec_s *, i32, i32, numtl_s *) {
}

void DrawRopeSingle(nuvec_s *, nuvec_s *, float, numtl_s *, float, float, float, float) {
}

void DrawStatusText(char *, u16, float, float, float, u32, i32) {
}

void DrawWallSpline(float) {
}

void Draw3DObjectMtx(WORLDINFO_s *, i32, numtx_s *) {
}

void DrawGameObjects() {
}

void DrawPaintLights() {
}

void DrawShopPrompts() {
}

void DrawStatusIcons(STATUSPACKET_s *, float, float) {
}

void DrawStillScreen(i32) {
}

void DrawTouchPrompt(char *, char *, bool, bool) {
}

void Draw_LOADFAILED() {
}

void DrawAreaCylinder(nuvec_s *, nuvec_s *, i32) {
}

void DrawCameraTarget(nuvec_s *) {
}

void DrawGameMessages() {
}

void DrawMeleeTargets(i16 *, char *, float *, i32) {
}

void DrawMiniKitCount(float, float, i32, i32) {
}

void DrawStatusBG_LSW(STATUSPACKET_s *) {
}

void DrawStatusScreen(WORLDINFO_s *) {
}

void Draw_LOADCORRUPT() {
}

void Draw3DObjectAlpha(WORLDINFO_s *, i32, nuvec_s *, u16, u16, u16, float, float,
                       float, i32, float) {
}

void DrawBossHitPoints(GameObject_s *) {
}

void DrawCameraTarget2(nuvec_s *) {
}

void DrawPanel3DObject(float, float, float, float, float, float, u16, u16, u16,
                       nuhspecial_s *, i32, float) {
}

void DrawStatusMiniKit(float, float, float, float, float, i32, STATUSPACKET_s *, float) {
}

void DrawSubItemMenu2D() {
}

void DrawSubItemMenu3D() {
}

void Draw_NOMEMORYCARD() {
}

void DrawFadeScreenWipe() {
}

void DrawMessageBoxRGBA(float, float, float, float, u32, u32, u32, u32, numtl_s *,
                        i32, float) {
}

void DrawSuperStoryTime(float, float, float, i32, i32) {
}

void DrawForceBackEffect(nuhspecial_s *) {
}

void DrawForceGlowSprite(nuvec_s *, float, i32, float, GameObject_s *) {
}

void DrawGameObjectsDraw(i32) {
}

void DrawPauseScreenWipe() {
}

void Draw_AUTOSAVECANCEL() {
}

void DrawMeleeTargetsRows(i16 *, char *, float *, i32) {
}

void DrawMiniSnowTroopers(WORLDINFO_s *) {
}

void DrawPanel3DObjectMtx(nuhspecial_s *, numtx_s *, float) {
}

void Draw_AUTOSAVEWARNING() {
}

void Draw_NODATAAVAILABLE() {
}

void DrawInDoubleScoreZone(float) {
}

void DrawObjectOnCharacter(WORLDINFO_s *, GameObject_s *, i32, nuhspecial_s *, i32, i32, numtx_s *, i32, u32,
                           numtx_s *, nuvec_s *, float, float) {
}

void DrawPlayerIconPrompts(i32, i32, float, i32, i32, i32, i32, i32, i32, float, i32, i32, i32, i32) {
}

void DrawGameObjectsProcess() {
}

void DrawMeleeTargetsNumber(i16 *, unsigned char *, i32, unsigned char, nuhspecial_s *) {
}

void DrawStatusTextFraction(i32, i32, float, float, u16, float, u32, float, float) {
}

void DrawGameMessage_Targets(GAMEMESSAGE_s *, nuvec_s *, float) {
}

void DrawTorpedoTargetSprite(void *, unsigned char, float) {
}

void DrawPanel3DObjectNoAlpha(float, float, float, float, float, float, u16, u16, u16,
                              nuhspecial_s *, i32) {
}

void DrawPanel3DObjectMtxNoAlpha(nuhspecial_s *, numtx_s *) {
}

void Draw_OK(MENU_s *) {
}

void DrawItem(nuhspecial_s *, nuvec_s *, float, float, float, u16, u16, u16) {
}

void DrawAABox(_vuv_s *, _vuv_s *, i32) {
}

void DrawArrow(nuhspecial_s *, float) {
}

void DrawCross(nuvec_s *, float, numtl_s *, i32) {
}

void DrawMSitu(i32) {
}

void DrawPanel() {
}

void DrawTimer(i32, i32, i32) {
}

void SwipeDecalRenderer::Process(float) {
}

void SwipeDecalRenderer::Render() {
}

SwipeDecalRenderer::SwipeDecalRenderer(TouchHolder &, i32, SwipeDecalRenderer::Style) {
}
