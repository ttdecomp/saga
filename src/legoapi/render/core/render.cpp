#include "legoapi/world/world_shared.h"
struct starfighter_s;
struct rtl_s;
struct rtlidata_s;

#include "legoapi/render/core/SwipeDecalRenderer.h"
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
    NUGSCN *NuGScnRead(VARIPTR *buf, VARIPTR buf_end, char *path) {
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

i32 NuSpecialFind(NUGSCN *scene, void **dest, char *name, i32 flags) {
    (void)scene;
    (void)dest;
    (void)name;
    (void)flags;
    return 0;
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

void Draw3DObject(WORLDINFO_s *, i32, nuvec_s *, u16, u16, u16, float, float, float, i32) {
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

void Draw3DObjectAlpha(WORLDINFO_s *, i32, nuvec_s *, u16, u16, u16, float, float, float, i32, float) {
}

void DrawBossHitPoints(GameObject_s *) {
}

void DrawCameraTarget2(nuvec_s *) {
}

void DrawPanel3DObject(float, float, float, float, float, float, u16, u16, u16, nuhspecial_s *, i32, float) {
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

void DrawMessageBoxRGBA(float, float, float, float, u32, u32, u32, u32, numtl_s *, i32, float) {
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

void DrawObjectOnCharacter(WORLDINFO_s *, GameObject_s *, i32, nuhspecial_s *, i32, i32, numtx_s *, i32, u32, numtx_s *,
                           nuvec_s *, float, float) {
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

void DrawPanel3DObjectNoAlpha(float, float, float, float, float, float, u16, u16, u16, nuhspecial_s *, i32) {
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

static __used__ void PauseRenderOff() {
}

static __used__ bool MatrixReflection_CanOverride() {
    return false;
}

static __used__ void DrawStarFighter(starfighter_s *) {
}

static __used__ void DrawParaphernalia(GameObject_s *) {
}

static __used__ void DrawFalconSpotLights(GameObject_s *) {
}

static __used__ double ApplyAntilights(rtl_s *, rtlidata_s *, float) {
    return {};
}

static __used__ void DisplayListMaterialClipUpdate(nudisplayscene_s *) {
}

static __used__ void SelectNextFog() {
}

static __used__ void SelectPrevFog() {
}

static __used__ void PreWarmGeomsAndBakeVAOs(nudisplayscene_s *, nunativegscene_s *) {
}

#include "legoapi/legoapi_types.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nu3d/nurndr.h"

extern "C" {
    void NuRndrGradClear(i32 a, i32 b, i32 c, f32 d);
    void NuRndrClear(u32 flags, u32 colour, f32 alpha);
    i32 NuSpecialFind(NUGSCN *scene, void **dest, char *name, i32 flags);
    void NuSpecialGetMtx(nuhspecial_s *hs, NUMTX *out);
    void NuSpecialDrawAtAlpha(nuhspecial_s *hs, NUMTX *mtx, f32 alpha);
}
extern i32 qrand(void);

static NUGSCN *s_backdrop_scene = nullptr;

struct BackdropEntry {
    nuhspecial_s *hs = nullptr;
    u32 pad0 = 0;
    u32 pad1 = 0;
};
static BackdropEntry s_backdrop_hspecial[4];

static f32 s_backdrop_back_wait = 0.0f;

f32 backdrop_top_r = 0.0f;
f32 backdrop_top_g = 0.0f;
f32 backdrop_top_b = 0.0f;
f32 backdrop_bot_r = 0.0f;
f32 backdrop_bot_g = 0.0f;
f32 backdrop_bot_b = 0.0f;

f32 backdrop_top_tr = 0.0f;
f32 backdrop_top_tg = 0.0f;
f32 backdrop_top_tb = 0.0f;
f32 backdrop_bot_tr = 0.0f;
f32 backdrop_bot_tg = 0.0f;
f32 backdrop_bot_tb = 0.0f;

i32 backdrop_black = 0;

void (*BackDrop_AlphaFn)(float *) = nullptr;

static __used__ void BackDrop_Alpha(float *alpha) {
    if (alpha == nullptr)
        return;
    if (backdrop_black) {
        *alpha *= 0.0f;
    } else if (s_backdrop_back_wait > 0.0f) {
        *alpha *= 0.5f;
    }
}

void BackDrop_Init(char *path, variptr_u *buf, variptr_u *buf_end) {
    for (auto &e : s_backdrop_hspecial) {
        e.hs = nullptr;
        e.pad0 = 0;
        e.pad1 = 0;
    }
    if (s_backdrop_scene != nullptr) {
    } else {
        NUGSCN *scn = nullptr;
        if (buf != nullptr && buf_end != nullptr) {
            scn = NuGScnRead(buf, *buf_end, path);
        }
        if (scn == nullptr) {
            static NUGSCN dummy{};
            scn = &dummy;
        }
        s_backdrop_scene = scn;
    }
    if (s_backdrop_scene == nullptr)
        return;
    NuSpecialFind(s_backdrop_scene, (void **)&s_backdrop_hspecial[0].hs, (char *)"ball1", 1);
    NuSpecialFind(s_backdrop_scene, (void **)&s_backdrop_hspecial[1].hs, (char *)"ball2", 1);
    NuSpecialFind(s_backdrop_scene, (void **)&s_backdrop_hspecial[2].hs, (char *)"ball3", 1);
    NuSpecialFind(s_backdrop_scene, (void **)&s_backdrop_hspecial[3].hs, (char *)"ball4", 1);
    for (auto &e : s_backdrop_hspecial) {
        if (e.hs == nullptr) {
            static nuhspecial_s dummyHs{};
            e.hs = &dummyHs;
        }
    }
}

void BackDrop_Dump() {
    s_backdrop_scene = nullptr;
    for (auto &e : s_backdrop_hspecial)
        e.hs = nullptr;
}

void BackDrop_Update(float dt) {
    if (s_backdrop_scene == nullptr)
        return;
    NuGScnUpdate(s_backdrop_scene, (i32)(dt * 1000.0f));
    if (s_backdrop_back_wait > 0.0f) {
        s_backdrop_back_wait -= dt;
        if (s_backdrop_back_wait < 0.0f)
            s_backdrop_back_wait = 0.0f;
    }
}

void BackDrop_ResetColours() {
    backdrop_top_r = 0.0f;
    backdrop_top_g = 0.0f;
    backdrop_top_b = 0.0f;
    backdrop_bot_r = 0.0f;
    backdrop_bot_g = 0.0f;
    backdrop_bot_b = 0.0f;
    backdrop_top_tr = 0.0f;
    backdrop_top_tg = 0.0f;
    backdrop_top_tb = 0.0f;
    backdrop_bot_tr = 0.0f;
    backdrop_bot_tg = 0.0f;
    backdrop_bot_tb = 0.0f;
    s_backdrop_back_wait = 1.0f;
    backdrop_black = 0;
}

void BackDrop_UpdateColours(i32 instant) {
    const f32 lerp = (instant != 0) ? 1.0f : 0.05f;
    auto seek = [&](f32 &cur, f32 tgt) { cur += (tgt - cur) * lerp; };
    if (backdrop_black) {
        seek(backdrop_top_r, 0.0f);
        seek(backdrop_top_g, 0.0f);
        seek(backdrop_top_b, 0.0f);
        seek(backdrop_bot_r, 0.0f);
        seek(backdrop_bot_g, 0.0f);
        seek(backdrop_bot_b, 0.0f);
        return;
    }
    seek(backdrop_top_r, backdrop_top_tr);
    seek(backdrop_top_g, backdrop_top_tg);
    seek(backdrop_top_b, backdrop_top_tb);
    seek(backdrop_bot_r, backdrop_bot_tr);
    seek(backdrop_bot_g, backdrop_bot_tg);
    seek(backdrop_bot_b, backdrop_bot_tb);
}

void BackDrop_SetTint(f32 r, f32 g, f32 b) {
    backdrop_top_r = r;
    backdrop_top_g = g;
    backdrop_top_b = b;
    backdrop_bot_r = r * 0.6f;
    backdrop_bot_g = g * 0.6f;
    backdrop_bot_b = b * 0.6f;
}

void BackDrop_Draw(float alpha, i32 flags) {
    if (s_backdrop_scene == nullptr)
        return;
    float a = alpha;
    if (flags == 0 && BackDrop_AlphaFn != nullptr) {
        BackDrop_AlphaFn(&a);
        alpha = a;
    }
    if (alpha <= 0.001f)
        return;
    NuRndrGradClear(0xf00, 0x80000000, 0x80000000, 1.0f);
    for (int i = 0; i < 4; i++) {
        nuhspecial_s *hs = s_backdrop_hspecial[i].hs;
        if (hs == nullptr)
            continue;
        float layerAlpha = alpha * (0.7f + 0.075f * (float)i);
        if (layerAlpha > 1.0f)
            layerAlpha = 1.0f;
        NUMTX mtx = numtx_identity;
        float jitterX = (float)(qrand() & 0x7fff) * (1.0f / 32767.0f) * 2.0f - 1.0f;
        float jitterY = (float)(qrand() & 0x7fff) * (1.0f / 32767.0f) * 2.0f - 1.0f;
        mtx.m30 += jitterX * 0.02f;
        mtx.m31 += jitterY * 0.02f;
        NuSpecialDrawAtAlpha(hs, &mtx, layerAlpha);
    }
}
