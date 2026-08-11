#include "legoapi/bolts.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"

struct nuhspecial_s;

static NUMTX special_matrix;

extern "C" i32 NuSpecialExistsFn(nuhspecial_s *) {
    return 0;
}

extern "C" void NuSpecialSetVisibility(nuhspecial_s *, i32) {}

extern "C" NUMTX *NuSpecialGetDrawMtx(nuhspecial_s *) {
    return &special_matrix;
}

extern "C" void NuSpecialSetDrawMtx(nuhspecial_s *, NUMTX *matrix) {
    if (matrix != NULL) {
        special_matrix = *matrix;
    }
}

extern "C" void NuSpecialUpdate(nuhspecial_s *) {}

extern "C" void NuSpecialGetRadius(nuhspecial_s *, NUVEC *center, f32 *radius) {
    if (center != NULL) {
        center->x = 0.0f;
        center->y = 0.0f;
        center->z = 0.0f;
    }
    if (radius != NULL) {
        *radius = 0.0f;
    }
}

extern "C" i32 NuSpecialFind(NUGSCN *, BOLTSPECIAL *, char *) {
    return 0;
}

extern "C" i32 NuSpecialDrawAt(BOLTSPECIAL *, NUMTX *) {
    return 0;
}
