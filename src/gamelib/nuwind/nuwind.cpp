#include "gamelib/nuwind/nuwind.h"
#include "nu2api/numath/nufloat.h"
#include <emmintrin.h>

void NuWindInitialise(NUWIND *wind) {
    if (wind != NULL) {
        wind->unk2.x = 1.0f;
        wind->unk2.y = 1.0f;
        wind->unk2.z = 0.0f;
        wind->unk2.w = 0.0f;
        wind->unk3 = 0.0f;
        wind->unk1 = -1;

        // i HIGHLY HIGHLY doubt they actually used explicit SSE here, but this matches the original 100%
        __m128i ones = _mm_set1_epi32(-1);
        _mm_storeu_si128((__m128i *)&wind->unk0[0], ones);
        _mm_storeu_si128((__m128i *)&wind->unk0[4], ones);
    }
}


void NuWindAnimate(NUWIND *param_1, float param_2) {
    float fVar2;
    if (param_1 != NULL) { 
        fVar2 = param_1->unk2.y * 0.00390625f * param_2 + param_1->unk2.z;
        param_1->unk2.z = fVar2;
        if (1.0f <= fVar2) {
            param_1->unk2.z = NuFmod(fVar2, 1.0f); 
        }
        param_1->unk2.w += param_2;
        param_1->unk3 += param_2 * 5.0f;
    }
}
