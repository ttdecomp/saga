#include "nu2api/numath/numtx.h"

extern "C" void NuMtxSetRotationXYVU0(NUMTX *matrix, i32 *angles) {
    NuMtxSetRotationX(matrix, angles[0]);
    NuMtxRotateY(matrix, angles[1]);
}
