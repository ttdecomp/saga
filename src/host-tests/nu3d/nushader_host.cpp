#include "nu2api/nu3d/nushader.h"
#include "nu2api/numath/numtx.h"

// The Android object references this platform shader helper weakly. The host
// supplies the GLES implementation so matrix uniforms can be exercised there.
void GLSLParameter::setElementsMatrix(i32 first_element, i32 count, const f32 *values) {
    NUMTX transposed[32];
    for (i32 i = 0; i < count; ++i) {
        NuMtxTranspose(&transposed[i], const_cast<NUMTX *>(reinterpret_cast<const NUMTX *>(values) + i));
    }

    i32 vector_count = count * 4;
    const i32 vectors_remaining = (element_count_and_setter >> 2) - first_element * 4;
    if (vector_count > vectors_remaining) {
        vector_count = vectors_remaining;
    }
    glUniform4fv(location + first_element * 4, vector_count, reinterpret_cast<const f32 *>(transposed));
}
