#include "deflate/deflate.h"

int32_t ExplodeBufferNoHeader(uint8_t *param_1, uint8_t *param_2, int32_t param_3, int32_t param_4) {
    UNIMPLEMENTED();
}

static uint32_t getmasktbl[5] = {0, 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF};

int32_t ImplodeGetI(uint8_t *buffer, int32_t size) {
    return getmasktbl[size] & *(int32_t *)buffer;
}
