#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/render.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuprim.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" void SetQFont2D(void);
extern "C" i32 NuRndrBeginScene(i32 flags);
extern "C" void NuRndrClear(i32 flags, i32 colour, f32 alpha);
extern "C" void NuRndrEndScene(void);
extern FadeSystem FadeSys;
extern NUCAMERA *pNuCam;
extern "C" f32 NuIOS_GetAspectRatio(void);
void SetPanelLights(f32 intensity);
void TimingBars(void);
void Arcade_ResetPanel(void);
void TBOPENFN(char *name, i32 bar);
void TBCLOSEFN(char *name, i32 bar);

extern NUMTL *FadeMtl2;
extern i32 TimingBarSet;

extern f32 statstime;
extern f32 cointotaltime;
extern f32 goldbricktime;

static f32 redbrickslidetime;

void PanelRender(WORLDINFO_s *) {
    NuRndrBeginScene(-1);
    SetQFont2D();
    SetPanelLights(1.0f);
    NuRndrClear(0xe00, 0xff000000, 1.0f);
    DrawPauseFade();
    if (TimingBarSet == 5) {
        TBOPENFN("Panel", 5);
    }
    DrawPanel();
    if (TimingBarSet == 5) {
        TBCLOSEFN("Panel", 5);
    }
    TimingBars();
    FadeSys.Update();
    static_cast<ThingManager *>(theGameThings)->DisplayThings(NULL);
    NuRndrEndScene();
    FadeSys.Draw();

    if (WORLD != NULL && WORLD->current_level == TITLES_LDATA) {
        i32 colour = static_cast<i32>((1.0f - newgamealpha) * 255.0f);

        NuRndrBeginScene(-1);
        colour <<= 24;
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(NUPRIM_SCALEMODE_ABSOLUTE);
        NuPrim2DBegin(4, 5, FadeMtl2);

        struct PanelFadeVertex {
            f32 x;
            f32 y;
            f32 z;
            u32 colour;
        };

        PanelFadeVertex *vertex = reinterpret_cast<PanelFadeVertex *>(g_NuPrim_StreamBufferPtr->void_ptr);
        if (g_NuPrim_NeedsOverbrightening == 0) {
            vertex->colour = colour & 0xff000000u;
        } else {
            vertex->colour = colour;
        }
        NuPrim2DAddXYZ(0.0f, 0.0f, 0.0f);

        vertex = reinterpret_cast<PanelFadeVertex *>(g_NuPrim_StreamBufferPtr->void_ptr);
        if (g_NuPrim_NeedsOverbrightening == 0) {
            vertex->colour = colour & 0xff000000u;
        } else {
            vertex->colour = colour;
        }
        NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);

        NuPrim2DEnd();
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
        NuRndrEndScene();
    }
}

void Panel_Clear() {
    statstime = 0.0f;
    DrawMiniKitTime = 0.0f;
    MiniKitScale = 1.0f;
    DrawBuildUpTime = 0.0f;
    builduptime = 0.0f;
    BuildUpScale = 1.0f;
    DrawRedBrickTime = 0.0f;
    RedBrickScale = 1.0f;
    DrawCoinTotalTime = 0.0f;
    cointotaltime = 0.0f;
    CoinTotalScale = 1.0f;
    redbrickslidetime = 0.0f;
    goldbricktime = 0.0f;
    Arcade_ResetPanel();
}

void AddCoinsToPanel(i32, nuvec_s *, i32, float, GameObject_s *, i32) {
}

void CoinsGoToMainTotal() {
}

void InitPanel(i32) {
    const f32 panel_fov = pNuCam->fov / 0.75f;
    const f32 aspect_ratio = NuIOS_GetAspectRatio();
    const f32 divisor = (1.0f - panel_fov) * 0.22f + 2.545f;
    PANEL3DMULX = aspect_ratio * panel_fov / divisor;
    PANEL3DMULY = panel_fov / divisor;
}
