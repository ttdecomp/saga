#include "decomp.h"

// Editor-UI variadic font-print helpers (C++ / Itanium-mangled). These are
// static so they reproduce the original binary's local `t` symbols exactly:
//   _ZL14eduiFntPrintExPviiiPcz       == static void eduiFntPrintEx(void*, int, int, int, char*, ...)
//   _ZL18eduiFntPrintClipExPvffiffPcz == static void eduiFntPrintClipEx(void*, float, float, int, float, float, char*,
//   ...)
// __used__ keeps them emitted and lint-clean.

static __used__ void eduiFntPrintEx(void *, int, int, int, char *, ...) {
}

static __used__ void eduiFntPrintClipEx(void *, float, float, int, float, float, char *, ...) {
}
