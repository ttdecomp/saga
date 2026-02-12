#include "nu2api.saga/nu3d/nuprim.h"

int NuPrimCSPos;
NUPRIMSCALEMODE NuPrimCoordSystemStack[16];
float NuPrim_XScale = 1.0f;
float NuPrim_YScale = 1.0f;
float NuPrim_XBias;
float NuPrim_YBias;

void NuPrimSetCoordinateSystem(NUPRIMSCALEMODE scale_mode) {
    NuPrimCoordSystemStack[NuPrimCSPos] = scale_mode;

    switch (scale_mode) {
        case NUPRIM_SCALEMODE_PS2:
            NuPrim_XScale = 0.003125f;
            NuPrim_YScale = -0.008928572f;
            NuPrim_XBias = -1.0f;
            NuPrim_YBias = 1.0f;
            break;
        case NUPRIM_SCALEMODE_NORMALISED:
            NuPrim_XScale = 1.0f;
            NuPrim_YScale = -1.0f;
            NuPrim_XBias = 0.0f;
            NuPrim_YBias = 0.0f;
            break;
        case NUPRIM_SCALEMODE_ABSOLUTE:
            NuPrim_XScale = 2.0f;
            NuPrim_YScale = -2.0f;
            NuPrim_XBias = -1.0f;
            NuPrim_YBias = 1.0f;
            break;
    }
}

void NuPrimPushCoordSystem(NUPRIMSCALEMODE scale_mode) {
    NuPrimCSPos++;
    NuPrimSetCoordinateSystem(scale_mode);
}

void NuPrimInit(VARIPTR *buffer, VARIPTR buffer_end) {
    NuPrimCSPos = -1;
    NuPrimPushCoordSystem(NUPRIM_SCALEMODE_PS2);
}

void NuPrimReset(void) {
}
