#pragma once

#include "nu2api/nu3d/numtl.h"

typedef struct NuVertexFormatPS {
} NuVertexFormatPS;

#ifdef __cplusplus
extern "C" {
#endif
    NuVertexFormatPS *NuGetVertexDeclaration(NUVERTEXDESCRIPTOR vtx_desc);
#ifdef __cplusplus
}
#endif
