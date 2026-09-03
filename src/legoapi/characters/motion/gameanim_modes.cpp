#include "legoapi/legoapi_types.h"

static i32 AnimBlendMode;

extern "C" i32 GetAnimBlendMode(void) {
    return AnimBlendMode;
}

extern "C" void SetAnimBlendMode(i32 mode) {
    AnimBlendMode = mode;
}
