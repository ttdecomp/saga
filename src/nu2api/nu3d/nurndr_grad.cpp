#include "nu2api/nucore/common.h"

extern "C" void NuRndrClear(i32 clear_flags, i32 background_colour, f32 alpha);

extern "C" void NuRndrGradClear(i32 clear_flags, i32 top_colour, i32, f32 alpha) {
    NuRndrClear(clear_flags, top_colour, alpha);
}
