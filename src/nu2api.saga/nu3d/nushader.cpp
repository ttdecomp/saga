#include "nu2api.saga/nu3d/nushader.h"

#include "nu2api.saga/nu3d/NuRenderDevice.h"
#include "nu2api.saga/nuandroid/nuios.h"
#include "nu2api.saga/nuthread/nuthread.h"

static f32 water_theta_step = 0.26666668f;

void NuShaderObjectBaseCreate(NUSHADEROBJECTBASE *shader) {
    shader->field0 = -1;
    shader->field1 = 0;
    shader->key = 0;
    shader->field3 = 0;
}

void NuShaderObjectGLSLCreate(NUSHADEROBJECTGLSL *shader) {
    NuShaderObjectBaseCreate(&shader->base);
}

void NuShaderObjectCreate(NUSHADEROBJECT *shader) {
    NuShaderObjectGLSLCreate(&shader->glsl);

    for (int i = 0; i < NUSHADEROBJECT_PARAMETERS_COUNT; ++i) {
        shader->parameters[i].unk3 = i;
        shader->parameters[i].unk1 = 0xffff;
    }
}

void NuShaderObjectBaseDestroy(NUSHADEROBJECTBASE *shader) {
    shader->field1 = 0;
}

void NuShaderObjectGLSLDestroy(NUSHADEROBJECTGLSL *shader) {
    if (shader->fragment_shader != 0) {
        glDeleteShader(shader->fragment_shader);
        shader->fragment_shader = 0;
    }

    if (shader->vertex_shader != 0) {
        glDeleteShader(shader->vertex_shader);
        shader->vertex_shader = 0;
    }

    if (shader->program != 0) {
        glDeleteProgram(shader->program);
        shader->program = 0;
    }
}

void NuShaderObjectDestroy(NUSHADEROBJECT *shader) {
}

void NuShaderObjectBaseInit(NUSHADEROBJECTBASE *shader, NUSHADEROBJECTKEY *key, int unk) {
    shader->key = (isize) * (NUSHADEROBJECTKEY **)key; // it only matches if you do this noop cast/dereference
    shader->field0 = unk;
}

void NuShaderObjectUnInit(NUSHADEROBJECT *shader) {
}

void NuShaderObjectBaseUnInit(NUSHADEROBJECTBASE *shader) {
    shader->field0 = -1;
}

void NuShaderObjectBaseSetWaterSpeed(f32 speed) {
    water_theta_step = speed * 0.1f;
}

int NuShaderObjectBindAttributeLocationsGLSL(GLuint program) {
    GLint params;
    static GLchar info_log[8192];

    glBindAttribLocation(program, 0, "cg_Vertex");
    glBindAttribLocation(program, 1, "COLOR");
    glBindAttribLocation(program, 2, "SPECULAR");
    glBindAttribLocation(program, 3, "NORMAL");
    glBindAttribLocation(program, 4, "TANGENT");
    glBindAttribLocation(program, 5, "TEXCOORD4");
    glBindAttribLocation(program, 6, "TEXCOORD0");
    glBindAttribLocation(program, 7, "TEXCOORD1");
    glBindAttribLocation(program, 8, "TEXCOORD6");
    glBindAttribLocation(program, 9, "TEXCOORD7");
    glBindAttribLocation(program, 10, "BLENDWEIGHT0");
    glBindAttribLocation(program, 11, "BLENDINDICES0");
    glBindAttribLocation(program, 12, "TEXCOORD5");

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &params);
    if (params) { // weird register swap issue with the matching here...
        return 1;
    }

    glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
    return 0;
}

int NuShaderObjectCombineGLSLShadersIntoProgram(GLuint *program_dest, GLuint vertex_shader, GLuint fragment_shader) {
    // these were most definitely macros
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 228);
    GLuint program = glCreateProgram();
    *program_dest = program;
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 230);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 232);
    glAttachShader(*program_dest, vertex_shader);
    glAttachShader(*program_dest, fragment_shader);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 235);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 237);
    int bind_result = NuShaderObjectBindAttributeLocationsGLSL(*program_dest);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 239);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    return bind_result;
}

int NuShaderObjectGenerateGLSLShader(GLuint *shader_dest, GLenum shader_type, const GLchar *shader_source,
                                     GLint shader_source_length) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 197);
    GLuint shader = glCreateShader(shader_type);
    *shader_dest = shader;
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 200);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    GLint params = 0;
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 204);
    glShaderSource(shader, 1, &shader_source, &shader_source_length);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 208);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    return 1;
}
