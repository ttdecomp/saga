#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/render.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" void SetQFont2D(void);
extern "C" i32 NuRndrBeginScene(i32 flags);
extern "C" void NuRndrClear(i32 flags, i32 colour, f32 alpha);
extern "C" void NuRndrEndScene(void);
extern FadeSystem FadeSys;
void SetPanelLights(f32 intensity);
void TimingBars(void);

void PanelRender(WORLDINFO_s *) {
    NuRndrBeginScene(-1);
    SetQFont2D();
    SetPanelLights(1.0f);
    NuRndrClear(0xe00, 0xff000000, 1.0f);
    DrawPauseFade();
    DrawPanel();
    TimingBars();
    FadeSys.Update();
    NuRndrEndScene();
    FadeSys.Draw();
}

void Panel_Clear() {
}

void AddCoinsToPanel(i32, nuvec_s *, i32, float, GameObject_s *, i32) {
}

void CoinsGoToMainTotal() {
}

void InitPanel(i32) {
}
