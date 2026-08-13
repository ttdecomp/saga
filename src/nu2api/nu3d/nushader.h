#pragma once

#include <GLES2/gl2.h>

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

struct glslparamter_s {
    i16 unk1;
    i16 unk2;
    char unk3;
    char unk4[3];
};

typedef struct glslparamter_s GLSLParameter;

#define NUSHADEROBJECT_PARAMETERS_COUNT 91

struct nushaderobject_s {
    NUSHADEROBJECTGLSL glsl;
    char unk[0x10];
    GLSLParameter parameters[NUSHADEROBJECT_PARAMETERS_COUNT];
};

typedef nushaderobject_s NUSHADEROBJECT;

#ifdef __cplusplus
i32 NuShaderObjectBindAttributeLocationsGLSL(GLuint program);
i32 NuShaderObjectCombineGLSLShadersIntoProgram(GLuint *program_dest, GLuint vertex_shader, GLuint fragment_shader);
i32 NuShaderObjectGenerateGLSLShader(GLuint *shader_dest, GLenum shader_type, const GLchar *shader_source,
                                     GLint shader_source_length);

extern "C" {
#endif
    void NuShaderObjectBaseCreate(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectGLSLCreate(NUSHADEROBJECTGLSL *shader);
    void NuShaderObjectCreate(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseDestroy(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectGLSLDestroy(NUSHADEROBJECTGLSL *shader);
    void NuShaderObjectDestroy(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseInit(NUSHADEROBJECTBASE *shader, NUSHADEROBJECTKEY *key, i32 unk);
    void NuShaderObjectUnInit(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseUnInit(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectBaseSetWaterSpeed(f32 speed);
#ifdef __cplusplus
}
#endif
