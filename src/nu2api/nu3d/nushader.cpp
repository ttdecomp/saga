#include "decomp.h"
#include "nu2api/nu3d/nu2api_nu3d_types.h"
#include "nu2api/nu3d/nushader.h"

#include "nu2api/numath/numtx.h"

#include <ctype.h>
#include <string.h>

// Weak COMDAT helper emitted by the original shader implementation. Keeping
// the real C++ tag (`GLSLParameter`) is ABI-significant: the tag, not a typedef
// alias, is encoded in the symbol name.
void __attribute__((weak)) GLSLParameter::setElementsMatrix(i32 first_element, i32 count, const f32 *values) {
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

#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nustring.h"

struct nushaderuniform_e {
    i32 name_kind;
};

struct GLSLTypeInfo {
    GLenum gl_type;
    u32 parameter_type;
    u32 setter_class;
    u32 element_count;
};

extern "C" {
    extern i32 g_semanticMaskCount;
    extern NUSHADERUSAGEMASK g_semanticMasks[128];
}

extern u32 g_boundShader;
extern void (*g_glConstantSetterTable[4])(u32 location, i32 count, const void *values);

static f32 water_theta_step = 0.26666668f;

namespace {
    static const GLSLTypeInfo kGLSLTypeInfo[] = {
        {GL_FLOAT, 1, 0, 1},      {GL_FLOAT_VEC2, 1, 1, 1}, {GL_FLOAT_VEC3, 1, 2, 1},
        {GL_FLOAT_VEC4, 1, 3, 1}, {GL_FLOAT_MAT2, 3, 1, 2}, {GL_FLOAT_MAT3, 3, 2, 3},
        {GL_FLOAT_MAT4, 3, 3, 4}, {GL_SAMPLER_2D, 4, 0, 1}, {GL_SAMPLER_CUBE, 4, 0, 1},
    };

    static const GLSLTypeInfo *GetGLSLTypeInfo(GLenum type) {
        for (u32 i = 0; i < sizeof(kGLSLTypeInfo) / sizeof(kGLSLTypeInfo[0]); ++i) {
            if (kGLSLTypeInfo[i].gl_type == type) {
                return &kGLSLTypeInfo[i];
            }
        }
        return NULL;
    }

    static NUSHADERUSAGEMASK *GetUsageMask(const NUSHADERUSAGEMASK *mask) {
        for (i32 i = 0; i < g_semanticMaskCount; ++i) {
            if (memcmp(&g_semanticMasks[i], mask, sizeof(*mask)) == 0) {
                return &g_semanticMasks[i];
            }
        }
        NUSHADERUSAGEMASK *result = &g_semanticMasks[g_semanticMaskCount++];
        *result = *mask;
        return result;
    }
} // namespace

extern "C" {
    i32 g_semanticMaskCount;
    NUSHADERUSAGEMASK g_semanticMasks[128];
}

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

    for (i32 i = 0; i < NUSHADEROBJECT_PARAMETERS_COUNT; ++i) {
        shader->parameters[i].semantic = i;
        shader->parameters[i].location = -1;
    }
}

// Original 0x30ba90. Parameter metadata is populated by the GLSL semantic
// probe; the low nibble selects scalar/vector/matrix upload behaviour.
extern "C" void NuShaderObjectSetElementsfv(i32 shader_object, i32 semantic, i32 first_element, i32 count,
                                            const f32 *values) {
    NUSHADEROBJECT *shader = reinterpret_cast<NUSHADEROBJECT *>(static_cast<uintptr_t>(shader_object));
    GLSLParameter &parameter = shader->parameters[semantic];
    if (parameter.location < 0) {
        return;
    }

    switch (parameter.type_and_flags & 0x0f) {
        case 2:
            glUniform4fv(parameter.location + first_element, count, values);
            break;
        case 1:
            g_glConstantSetterTable[parameter.element_count_and_setter & 3](parameter.location + first_element, count,
                                                                            values);
            break;
        case 3:
            parameter.setElementsMatrix(first_element, count, values);
            break;
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

void NuShaderObjectBaseInit(NUSHADEROBJECTBASE *shader, NUSHADEROBJECTKEY *key, i32 unk) {
    shader->key = (usize) * (NUSHADEROBJECTKEY **)key; // it only matches if you do this noop cast/dereference
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

i32 NuShaderObjectBindAttributeLocationsGLSL(GLuint program) {
    static GLchar infoLog[0x2000];

    GLint params;

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

    glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
    return 0;
}

i32 NuShaderObjectCombineGLSLShadersIntoProgram(GLuint *program_dest, GLuint vertex_shader, GLuint fragment_shader) {
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
    i32 bind_result = NuShaderObjectBindAttributeLocationsGLSL(*program_dest);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 239);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    return bind_result;
}

i32 NuShaderObjectGenerateGLSLShader(GLuint *shader_dest, GLenum shader_type, const GLchar *shader_source,
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

// original 0x30b970 — Init(obj,key,i,uint,uint,version) forwards to
// InitGLSL(obj,key,i,vshader,pshader) and then probes semantics.
void NuShaderObjectInit(nushaderobject_s *obj, nushaderobjectkey_s const *key, i32 param, u32 vshader, u32 pshader,
                        eSHADERVERSION) {
    NuShaderObjectInitGLSL((nushaderobjectglsl_s *)obj, (nushaderobjectkey_s const *)key, param, vshader, pshader);
    NuShaderObjectGLSLProbeSemantics(obj);
}

// original 0x30b050 — BaseInit, stash both shader objects, then build and
// link the GL program; destroy the object when linking failed.
void NuShaderObjectInitGLSL(nushaderobjectglsl_s *obj, nushaderobjectkey_s const *key, i32 param, u32 vshader,
                            u32 pshader) {
    NuShaderObjectBaseInit(&obj->base, (NUSHADEROBJECTKEY *)key, param);
    obj->vertex_shader = vshader;   // +0x14
    obj->fragment_shader = pshader; // +0x18
    if (!NuShaderObjectCombineGLSLShadersIntoProgram(&obj->program, vshader, pshader)) {
        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp",
                               298);
        NuShaderObjectGLSLDestroy(obj);
        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp", 300);
    }
}

void NuShaderObjectInitGLSL(nushaderobjectglsl_s *, nushaderobjectkey_s const *, i32, char const *, i32, char const *,
                            i32) {
}

void NuShaderObjectInitGLSL(nushaderobjectglsl_s *, nushaderobjectkey_s const *, i32, char const *, i32, u32) {
}

void NuShaderObjectInitGLSL(nushaderobjectglsl_s *, nushaderobjectkey_s const *, i32, u32, char const *, i32) {
}

// Original registry entry 90 in g_shaderUniforms. This is the branch required
// by smooth skins; the remaining semantic-name registry is still pending.
i32 NuShaderObjectGLSLGetSemanticIndex(const char *name, nushaderuniform_e &uniform) {
    if (NuStrCmp(name + 1, "skinMatrix") == 0) {
        uniform.name_kind = 1;
        return 0x5a;
    }
    return -1;
}

// Original 0x30b560, retaining the complete active-uniform walk and the exact
// metadata construction for the transcribed skinMatrix registry entry.
extern "C" void NuShaderObjectGLSLProbeSemantics(NUSHADEROBJECT *shader) {
    if (shader->glsl.program == 0) {
        return;
    }

    static const char *source_path =
        "i:/SagaTouch-Android_9176564/nu2api.saga/shaderbuilder/android/nushaderobject.cpp";
    BeginCriticalSectionGL(source_path, 582);

    GLint uniform_count = 0;
    glGetProgramiv(shader->glsl.program, GL_ACTIVE_UNIFORMS, &uniform_count);
    NUSHADERUSAGEMASK usage_mask = {};
    for (GLint i = 0; i < uniform_count; ++i) {
        char uniform_name[256];
        GLint array_size = 0;
        GLenum type = 0;
        glGetActiveUniform(shader->glsl.program, i, sizeof(uniform_name), NULL, &array_size, &type, uniform_name);
        char *array_suffix = strchr(uniform_name, '[');
        if (array_suffix != NULL) {
            *array_suffix = '\0';
        }

        nushaderuniform_e uniform;
        const i32 semantic = NuShaderObjectGLSLGetSemanticIndex(uniform_name, uniform);
        if (semantic < 0) {
            continue;
        }

        GLSLParameter &parameter = shader->parameters[semantic];
        parameter.element_count_and_setter = (parameter.element_count_and_setter & 3) | 4;
        usage_mask.semantics[semantic >> 5] |= 1u << (semantic & 31);

        const GLSLTypeInfo *type_info = GetGLSLTypeInfo(type);
        if (type_info != NULL) {
            parameter.type_and_flags = (parameter.type_and_flags & 0xf0) | (type_info->parameter_type & 0x0f);
            parameter.element_count_and_setter = (parameter.element_count_and_setter & 0xfc) |
                                                 (type_info->setter_class & 3) | (type_info->element_count << 2);
        }

        parameter.location = glGetUniformLocation(shader->glsl.program, uniform_name);
        parameter.array_size = array_size;
        const u8 setter_class = parameter.element_count_and_setter & 3;
        const u8 element_count = parameter.element_count_and_setter >> 2;
        parameter.element_count_and_setter = setter_class | (element_count * array_size << 2);
    }

    shader->usage_mask = GetUsageMask(&usage_mask);
    EndCriticalSectionGL(source_path, 676);
    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }
}

namespace {
    static NUSHADERPROGRAM program_pool[64];
    static NUSHADERPROGRAMPARAMETER parameter_pool[2048];
    static i32 next_program;
    static i32 next_parameter;

    static i32 FindShaderRegister(const char *source, const char *uniform_name, const char *section) {
        const char *header = strstr(source, section);
        if (header == NULL) {
            return -1;
        }
        const char *header_end = strstr(header, "\n\n");
        if (header_end == NULL) {
            header_end = source + strlen(source);
        }

        const usize name_length = strlen(uniform_name);
        for (const char *match = strstr(header, uniform_name); match != NULL && match < header_end;
             match = strstr(match + 1, uniform_name)) {
            const char before = match == header ? ' ' : match[-1];
            const char after = match[name_length];
            if ((isalnum((unsigned char)before) || before == '_') || (isalnum((unsigned char)after) || after == '_')) {
                continue;
            }
            const char *number = match + name_length;
            while (number < header_end && isspace((unsigned char)*number)) {
                ++number;
            }
            if (number < header_end && isdigit((unsigned char)*number)) {
                i32 value = 0;
                while (number < header_end && isdigit((unsigned char)*number)) {
                    value = value * 10 + (*number++ - '0');
                }
                return value;
            }
        }
        return -1;
    }

    static u16 UniformSetterClass(GLenum type) {
        switch (type) {
            case GL_FLOAT:
                return 0;
            case GL_FLOAT_VEC2:
                return 1;
            case GL_FLOAT_VEC3:
                return 2;
            default:
                return 3;
        }
    }

    static bool IsSampler(GLenum type) {
        return type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE;
    }

    static void SetProgramParam(NUSHADERPROGRAM *program, u16 wanted_register, const f32 *values, i32 component_count) {
        for (i32 i = 0; i < program->parameter_count; ++i) {
            const NUSHADERPROGRAMPARAMETER &parameter = program->parameters[i];
            if (parameter.register_index != wanted_register) {
                continue;
            }
            const u32 location = parameter.location_and_setter & 0x0fff;
            const u32 setter = parameter.location_and_setter >> 12;
            const i32 vector_count = (component_count + 3) / 4;
            g_glConstantSetterTable[setter](location, vector_count, values);
            return;
        }
    }
} // namespace

extern "C" NUSHADERPROGRAM *NuShaderProgramCreateIOS(const char *vertex_source, const char *fragment_source) {
    if (next_program >= (i32)(sizeof(program_pool) / sizeof(program_pool[0]))) {
        return NULL;
    }

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    GLint compiled = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == 0) {
        glDeleteShader(vertex_shader);
        return NULL;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == 0) {
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        return NULL;
    }

    NUSHADERPROGRAM *result = &program_pool[next_program++];
    memset(result, 0, sizeof(*result));
    result->vertex_shader = vertex_shader;
    result->fragment_shader = fragment_shader;
    result->program = glCreateProgram();
    glAttachShader(result->program, vertex_shader);
    glAttachShader(result->program, fragment_shader);
    if (!NuShaderObjectBindAttributeLocationsGLSL(result->program)) {
        return NULL;
    }

    GLint active_uniforms = 0;
    glGetProgramiv(result->program, GL_ACTIVE_UNIFORMS, &active_uniforms);
    result->parameters = &parameter_pool[next_parameter];
    for (i32 i = 0; i < active_uniforms; ++i) {
        char uniform_name[256];
        GLsizei length = 0;
        GLint count = 0;
        GLenum type = 0;
        glGetActiveUniform(result->program, i, sizeof(uniform_name), &length, &count, &type, uniform_name);
        char *array_suffix = strchr(uniform_name, '[');
        if (array_suffix != NULL) {
            *array_suffix = '\0';
        }

        const GLint location = glGetUniformLocation(result->program, uniform_name);
        if (IsSampler(type)) {
            const i32 texture_unit = FindShaderRegister(fragment_source, uniform_name, "//NU2API TEXTURES");
            if (texture_unit >= 0) {
                glUseProgram(result->program);
                glUniform1i(location, texture_unit);
                glUseProgram(0);
                g_boundShader = 0;
            }
            continue;
        }

        i32 register_index = FindShaderRegister(vertex_source, uniform_name, "//NU2API CONSTANTS");
        u16 stage_flag = 0;
        if (register_index < 0) {
            register_index = FindShaderRegister(fragment_source, uniform_name, "//NU2API CONSTANTS");
            stage_flag = 0x8000;
        }
        if (register_index < 0 || next_parameter >= (i32)(sizeof(parameter_pool) / sizeof(parameter_pool[0]))) {
            continue;
        }

        NUSHADERPROGRAMPARAMETER &parameter = parameter_pool[next_parameter++];
        parameter.register_index = stage_flag | (u16)register_index;
        parameter.location_and_setter = (UniformSetterClass(type) << 12) | (location & 0x0fff);
        result->parameter_count++;
    }
    return result;
}

extern "C" void NuShaderProgramSetVertexParamfv(NUSHADERPROGRAM *program, u32 register_index, const f32 *values,
                                                i32 component_count) {
    SetProgramParam(program, (u16)register_index, values, component_count);
}

extern "C" void NuShaderProgramSetFragmentParamfv(NUSHADERPROGRAM *program, u32 register_index, const f32 *values,
                                                  i32 component_count) {
    SetProgramParam(program, (u16)(register_index | 0x8000), values, component_count);
}

// Additional overloads present in original (char* shader sources)
void NuShaderObjectInit(nushaderobject_s *obj, nushaderobjectkey_s const *key, i32 param, const char *vshader,
                        i32 vsize, u32 pshader, eSHADERVERSION version) {
    (void)vshader;
    (void)vsize;
    NuShaderObjectInit(obj, key, param, 0, pshader, version);
}

void NuShaderObjectInit(nushaderobject_s *obj, nushaderobjectkey_s const *key, i32 param, u32 vshader,
                        const char *pshader, i32 psize, eSHADERVERSION version) {
    (void)pshader;
    (void)psize;
    NuShaderObjectInit(obj, key, param, vshader, 0, version);
}

asm(".globl NuShaderObjectInit\n"
    ".set NuShaderObjectInit, _Z18NuShaderObjectInitP16nushaderobject_sPK19nushaderobjectkey_sijj14eSHADERVERSION\n");
