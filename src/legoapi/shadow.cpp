#include "legoapi/gameobject.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

bool (*MatrixReflection_CanOverrideFn)();

f32 GameShadow(GameObject_s *, NUVEC *, f32, i32) {
    return 2000000.0f;
}

f32 FindReflectionNoPlatforms(NUVEC *) {
    return 2000000.0f;
}

extern "C" u32 ShadowInfo() {
    return 0;
}

extern "C" i32 EShadowInfo() {
    return 0;
}

extern "C" i32 NewShadowOnPlatform() {
    return -1;
}

i32 MatrixReflectionVU0_AXISY(NUMTX *matrix, f32 plane_y, f32 level_y, NUMTX *reflection) {
    if (matrix == NULL || reflection == NULL) {
        return 0;
    }
    *reflection = *matrix;
    reflection->m31 = plane_y + level_y - matrix->m31;
    return 1;
}
