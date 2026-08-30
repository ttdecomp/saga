// Target display-list callbacks are portable. Host overrides in this file
// only expose their state for bounded render-test diagnostics.

#include "decomp.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"

void NuIOSDLSkinMtxCallback(void *data) {
    i32 *packet = static_cast<i32 *>(data);
    const i32 matrix_count = *packet++;
    const i32 shader_address = NuShaderManagerGetCurrentShader();

    static bool logged;
    if (!logged) {
        const NUSHADEROBJECT *shader = reinterpret_cast<const NUSHADEROBJECT *>(static_cast<uintptr_t>(shader_address));
        const GLSLParameter *parameter = shader != NULL ? &shader->parameters[0x5a] : NULL;
        LOG_INFO("skin callback: matrices=%d shader=%p program=%u semantic=0x5a "
                 "location=%d type=%u elements=%u array=%u",
                 matrix_count, shader, shader != NULL ? shader->glsl.program : 0,
                 parameter != NULL ? parameter->location : -1, parameter != NULL ? parameter->type_and_flags & 0x0f : 0,
                 parameter != NULL ? parameter->element_count_and_setter >> 2 : 0,
                 parameter != NULL ? parameter->array_size : 0);
        logged = true;
    }

    if (shader_address != 0) {
        NuShaderObjectSetElementsfv(shader_address, 0x5a, 0, matrix_count * 4, reinterpret_cast<const f32 *>(packet));
    }
}
