#include "gamelib/nuwind/nuwind.h"

#ifndef __EMSCRIPTEN__
#include <emmintrin.h>
#endif

void NuWindInitialise(NUWIND *wind) {
    if (wind != NULL) {
        wind->unk2.x = 1.0f;
        wind->unk2.y = 1.0f;
        wind->unk2.z = 0.0f;
        wind->unk2.w = 0.0f;
        wind->unk3 = 0.0f;
        wind->unk1 = -1;

#ifdef __EMSCRIPTEN__
        for (usize i = 0; i < 8; ++i) {
            wind->unk0[i] = -1;
        }
#else
        // i HIGHLY HIGHLY doubt they actually used explicit SSE here, but this matches the original 100%
        __m128i ones = _mm_set1_epi32(-1);
        _mm_storeu_si128((__m128i *)&wind->unk0[0], ones);
        _mm_storeu_si128((__m128i *)&wind->unk0[4], ones);
#endif
    }
}
