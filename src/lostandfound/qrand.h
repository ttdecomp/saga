#pragma once

#include "decomp.h"

#define QRAND_FLOAT() (qrand() * (1.0f / 65535.0f))

#ifdef __cplusplus

i32 qrand(void);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
