#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuIOSInitOpenGLES(void);
    void NuIOS_AllocateSystemFramebuffers(void);
    i32 NuCheckGLErrorsFL(char *file, i32 line);
#ifdef __cplusplus
}
#endif
