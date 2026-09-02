#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern u8 MainFrameCounters[5];

void TimingBars() {
}

void getCurrentTime() {
}

void SetFramesToWait(u32) {
}

void ResetFrameCounters() {
    MainFrameCounters[0] = 0;
    MainFrameCounters[1] = 0xff;
    MainFrameCounters[2] = 0xff;
    MainFrameCounters[3] = 0xff;
    MainFrameCounters[4] = 0xff;
}

void UpdateFrameCounters() {
}
