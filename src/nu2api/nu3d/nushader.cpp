#include "nu2api/nu3d/nushader.h"

#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nustring.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static f32 water_theta_step = 0.26666668f;



NUSHADERPROGRAM programPool[64];
unsigned char g_shaderProgramBitmask[8];
int g_shaderProgramCursor;
NUUNIFORMRECORD g_uniformParameterRecordStorage[10];
int g_uniformParameterRecordAllocator;
GLuint g_boundShader;
char BuildRegisterIndexToUniformLocationMapping_uniformName[256];

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

int NuShaderObjectBindAttributeLocationsGLSL(GLuint program) {
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




bool LinkShaderProgram(unsigned int a1)
{
  int v2;
  glLinkProgram(a1);
  glGetProgramiv(a1, 35714, &v2);
  return v2 != 0;
}



GLSLTYPEINFO typeInfoTable[9]; 

GLSLTYPEINFO* GetGLSLTypeInfo(GLenum glType) {
    if (glType == GL_FLOAT)        return &typeInfoTable[0];
    if (glType == GL_FLOAT_VEC2)   return &typeInfoTable[1];
    if (glType == GL_FLOAT_VEC3)   return &typeInfoTable[2];
    if (glType == GL_FLOAT_VEC4)   return &typeInfoTable[3];
    if (glType == GL_FLOAT_MAT2)   return &typeInfoTable[4];
    if (glType == GL_FLOAT_MAT3)   return &typeInfoTable[5];
    if (glType == GL_FLOAT_MAT4)   return &typeInfoTable[6];
    if (glType == GL_SAMPLER_2D)   return &typeInfoTable[7];
    return (glType == GL_SAMPLER_CUBE) ? &typeInfoTable[8] : (GLSLTYPEINFO*)0;
}



static int GetHLSLRegisterIndex(const char *param_1, const char *param_2, bool param_3) {
    char searchBuffer[264];
    
    char *globalUniformName = (char *)0x11ad9a1; 

    sprintf(searchBuffer, "_%s ", globalUniformName);

    char *constantsHeader = strstr((char*)param_1, "//NU2API CONSTANTS :");
    char *uniformMatch = strstr((char*)param_1, searchBuffer);
    char *attribsHeader = strstr((char*)param_1, "//NU2API ATTRIBS :");

    if (constantsHeader != NULL && uniformMatch != NULL && 
       (attribsHeader == NULL || uniformMatch < attribsHeader)) {
        
        int searchLen = NuStrLen(searchBuffer);
        unsigned char *haystack = (unsigned char *)(uniformMatch + searchLen);

        if (*param_2 == '\0') {
            if (NuToUpper(*haystack) == 'C') {
                haystack++;
            }
        } else {
            int texLen = NuStrLen("TEXUNIT");
            if (NuStrNICmp((char *)haystack, "TEXUNIT", texLen) == 0) {
                haystack += texLen;
            }
        }

        if (*haystack != 0xFF && isdigit(*haystack)){
            int regIndex = NuAToI((char *)haystack);

            unsigned char *nameMatch = (unsigned char *)strstr((char *)haystack, globalUniformName + 1);

            if (nameMatch != NULL && haystack < nameMatch) {
                unsigned char *cursor = nameMatch - 1;
                

                while (*cursor != '\n') { 
                    int varLen = NuStrLen("//var ");
                    
                    if (NuStrNCmp((char *)cursor, "//var ", varLen) != 0) {
                        return regIndex; 
                    }
                    
                    if (cursor == haystack) {
                        return -1;
                    }
                    
                    cursor--;
                }
            }
        }
    }
    
    return -1; 
}


NUSHADERPROGRAM* NuShaderProgramCreateIOS(const GLchar *vtxSource, const GLchar *fragSource) {
    GLuint vtxShader, fragShader, programId;
    int compileStatus;
    const char *precisionStr;
    NUSHADERPROGRAM *prog;

    vtxShader = glCreateShader(GL_VERTEX_SHADER);
    precisionStr = strstr(vtxSource, "precision mediump float;");
    if (precisionStr != NULL) {
        memcpy((void*)precisionStr, "precision highp float;  ", NuStrLen("precision highp float;  "));
    }
    glShaderSource(vtxShader, 1, &vtxSource, NULL);
    glCompileShader(vtxShader);
    
    compileStatus = 0;
    glGetShaderiv(vtxShader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == 0) glDeleteShader(vtxShader);

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    NuStrLen(fragSource);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    
    compileStatus = 0;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == 0) {
        glDeleteShader(fragShader);
        return NULL;
    }

    programId = glCreateProgram();
    glAttachShader(programId, vtxShader);
    glAttachShader(programId, fragShader);
    LinkShaderProgram(programId);
    NuShaderObjectBindAttributeLocationsGLSL(programId);

    int poolIndex = -1;
    if (g_shaderProgramCursor < 64) {
        int checkIdx = g_shaderProgramCursor;
        int byteIdx = checkIdx >> 3;
        int bitIdx = checkIdx & 7;
        unsigned int bits = g_shaderProgramBitmask[byteIdx] >> bitIdx;
        
        while ((bits & 1) != 0) {
            checkIdx++;
            if (checkIdx == 64) {
                if (g_shaderProgramCursor > 0) goto fallback_search;
                goto alloc_fail;
            }
            byteIdx = checkIdx >> 3;
            bitIdx = checkIdx & 7;
            bits = g_shaderProgramBitmask[byteIdx] >> bitIdx;
        }
        g_shaderProgramBitmask[byteIdx] |= (1 << bitIdx);
        g_shaderProgramCursor = (checkIdx + 1) % 64;
        poolIndex = checkIdx;
    } else {
    fallback_search:
        if ((g_shaderProgramBitmask[0] & 1) == 0) {
            g_shaderProgramBitmask[0] |= 1;
            g_shaderProgramCursor = 1 % 64;
            poolIndex = 0;
        } else {
            int checkIdx = 0;
            unsigned int bits;
            do {
                checkIdx++;
                if (checkIdx == g_shaderProgramCursor) goto alloc_fail;
                bits = g_shaderProgramBitmask[checkIdx >> 3] >> (checkIdx & 7);
            } while ((bits & 1) != 0);
            
            g_shaderProgramBitmask[checkIdx >> 3] |= (1 << (checkIdx & 7));
            g_shaderProgramCursor = (checkIdx + 1) % 64;
            poolIndex = checkIdx;
        }
    }

    if (poolIndex == -1) {
    alloc_fail:
        return NULL; 
    }

    prog = &programPool[poolIndex];
    prog->vertexShader = vtxShader;
    prog->fragmentShader = fragShader;
    prog->program = programId;

    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &prog->numUniforms);
    
    if (prog->numUniforms == 0) {
        prog->uniforms = NULL;
    } else {
        int allocStart = g_uniformParameterRecordAllocator;
        g_uniformParameterRecordAllocator += prog->numUniforms;
        prog->uniforms = &g_uniformParameterRecordStorage[allocStart];
        
        int skippedUniforms = 0;
        
        for (int i = 0; i < prog->numUniforms; i++) {
            GLsizei length; GLint size; GLenum type;
            char *uniformName = BuildRegisterIndexToUniformLocationMapping_uniformName;
            
            glGetActiveUniform(prog->program, i, 0x100, &length, &size, &type, uniformName);
            
            char *bracket = strchr(uniformName, '[');
            if (bracket != NULL) *bracket = '\0';

            NUUNIFORMRECORD *record = &prog->uniforms[i - skippedUniforms];
            unsigned short shaderFlag = 0;
            
            int hlslReg = GetHLSLRegisterIndex((const char*)vtxSource, (const char*)0, false);
            if (hlslReg == -1) {
                hlslReg = GetHLSLRegisterIndex((const char*)fragSource, (const char*)0, true);
                shaderFlag = 0x8000;
            }
            
            record->registerInfo = shaderFlag | (unsigned short)hlslReg;
            
            int glLoc = glGetUniformLocation(prog->program, uniformName);
            record->locationInfo = (record->locationInfo & 0xF000) | (glLoc & 0x0FFF);
            
            GLSLTYPEINFO *typeInfo = GetGLSLTypeInfo(type);
            
            if (typeInfo != NULL) {
                if (typeInfo->typeClass == 4) { 
                    int texReg = GetHLSLRegisterIndex((const char*)fragSource, (const char*)1, true);
                    if (texReg != -1) {
                        glUseProgram(prog->program);
                        glUniform1i(glLoc, texReg);
                        glUseProgram(0);
                        g_boundShader = 0;
                    }
                    skippedUniforms++;
                    continue;
                }
                

                ((unsigned char*)record)[3] = (((unsigned char*)record)[3] & 0x0F) | (typeInfo->typeEnum << 4);
            }
        }
        prog->numUniforms -= skippedUniforms;
        g_uniformParameterRecordAllocator -= skippedUniforms;
    }

    return prog;
}