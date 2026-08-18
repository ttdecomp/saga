#include "legoapi/legoapi_types.h"
#include "decomp.h"
struct spacelevel_s;

HudRadarPulse::HudRadarPulse(VuVec const &) {
}

void HudRadarPulse::IsFinished() {
}

void HudRadarPulse::Process(float) {
}

void HudRadarPulse::Render() {
}

static __used__ void RefreshUI() {
}

static __used__ void DrawCoinTotal(int, int) {}

static __used__ void DrawPackButton(GAMEMESSAGE_s*, nuvec_s*, float) {}

static __used__ void DrawSpaceLevel(spacelevel_s*) {}

static __used__ void DrawEpisodesMenu(int, float) {}

static __used__ void InitUI() {
}

static __used__ i32 SubItemMenu(MENU_s *) {
    return 0;
}

extern "C" {

    void _NuTimeBarSlotBegin(void) {
    }

    void _NuTimeBarSlotEnd(void) {
    }

} // extern "C"
