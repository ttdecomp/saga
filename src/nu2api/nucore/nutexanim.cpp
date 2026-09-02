#include "globals.h"

static u32 nta_sig_old;
static u32 nta_sig_off;
static u32 nta_sig_on;

extern "C" void NuTexAnimSetMask(i32 mask) {
    script_mask = static_cast<u16>(mask);
}

extern "C" void NuTexAnimSetSignals(u32 signals) {
    const u32 previous_signals = nta_sig_old;
    nta_sig_off = (previous_signals | signals) ^ signals;
    nta_sig_on = ~previous_signals & signals;
    nta_sig_old = signals;
}
