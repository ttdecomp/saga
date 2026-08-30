#pragma once

#include <GLES2/gl2.h>

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"

struct nushaderobjectkey_s;

typedef struct nushaderobjectkey_s NUSHADEROBJECTKEY;

struct nushaderobjectbase_s {
    i32 field0;
    i32 field1;
    // this is a NUSHADEROBJECTKEY*, but for some reason they store it as an i32
    // if it isn't an i32 then SSE instructions aren't generated and it doesn't match
    i32 key;
    i32 field3;
};

typedef struct nushaderobjectbase_s NUSHADEROBJECTBASE;

struct nushaderobjectglsl_s {
    NUSHADEROBJECTBASE base;
    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;
};

typedef struct nushaderobjectglsl_s NUSHADEROBJECTGLSL;

// original 0x30b050 — BaseInit + stash shaders + combine/link program.
void NuShaderObjectInitGLSL(nushaderobjectglsl_s *obj, nushaderobjectkey_s const *key, i32 param, u32 vshader,
                            u32 pshader);

struct glslparamter_s {
    i16 location;
    u8 element_count_and_setter;
    u8 array_size;
    u8 semantic;
    u8 type_and_flags;
    u8 reserved[2];

#ifdef __cplusplus
    void setElementsMatrix(i32 first_element, i32 count, const f32 *values) __attribute__((weak));
#endif
};

typedef struct glslparamter_s GLSLParameter;

typedef struct nushaderusagemask_s {
    u32 semantics[4];
} NUSHADERUSAGEMASK;

DECOMP_ASSERT(sizeof(GLSLParameter) == 8, "GLSL parameter metadata size");
DECOMP_ASSERT(offsetof(GLSLParameter, semantic) == 4, "GLSL parameter semantic offset");

#define NUSHADEROBJECT_PARAMETERS_COUNT 91

struct nushaderobject_s {
    NUSHADEROBJECTGLSL glsl;
    NUSHADERUSAGEMASK *usage_mask;
    char unk[0x0c];
    GLSLParameter parameters[NUSHADEROBJECT_PARAMETERS_COUNT];
};

typedef nushaderobject_s NUSHADEROBJECT;

struct nushaderprogramparameter_s {
    u16 register_index;
    u16 location_and_setter;
};

typedef nushaderprogramparameter_s NUSHADERPROGRAMPARAMETER;

struct nushaderprogram_s {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    i32 parameter_count;
    nushaderprogramparameter_s *parameters;
    u32 unused[3];
};

typedef nushaderprogram_s NUSHADERPROGRAM;

#ifdef __cplusplus
i32 NuShaderObjectBindAttributeLocationsGLSL(GLuint program);
i32 NuShaderObjectCombineGLSLShadersIntoProgram(GLuint *program_dest, GLuint vertex_shader, GLuint fragment_shader);
i32 NuShaderObjectGenerateGLSLShader(GLuint *shader_dest, GLenum shader_type, const GLchar *shader_source,
                                     GLint shader_source_length);

extern "C" {
#endif
    void NuShaderObjectBaseCreate(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectGLSLCreate(NUSHADEROBJECTGLSL *shader);
    void NuShaderObjectGLSLProbeSemantics(NUSHADEROBJECT *shader);
    void NuShaderObjectCreate(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseDestroy(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectGLSLDestroy(NUSHADEROBJECTGLSL *shader);
    void NuShaderObjectDestroy(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseInit(NUSHADEROBJECTBASE *shader, NUSHADEROBJECTKEY *key, i32 unk);
    void NuShaderObjectUnInit(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseUnInit(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectBaseSetWaterSpeed(f32 speed);
    void NuShaderObjectSetElementsfv(i32 shader_object, i32 semantic, i32 first_element, i32 count, const f32 *values);
    NUSHADERPROGRAM *NuShaderProgramCreateIOS(const char *vertex_source, const char *fragment_source);
    void NuShaderProgramSetVertexParamfv(NUSHADERPROGRAM *program, u32 register_index, const f32 *values,
                                         i32 component_count);
    void NuShaderProgramSetFragmentParamfv(NUSHADERPROGRAM *program, u32 register_index, const f32 *values,
                                           i32 component_count);
#ifdef __cplusplus
}
#endif
