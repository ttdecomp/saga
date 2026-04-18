#pragma once

#include <GLES2/gl2.h>

#include "nu2api/nucore/common.h"

struct nushaderobjectkey_s {};

typedef struct nushaderobjectkey_s NUSHADEROBJECTKEY;

struct nushaderobjectbase_s {
    int field0;
    int field1;
    // this is a NUSHADEROBJECTKEY*, but for some reason they store it as an int
    // if it isn't an int then SSE instructions aren't generated and it doesn't match
    int key;
    int field3;
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



typedef struct NuUniformRecord_s {
    unsigned short registerInfo; 
    unsigned short locationInfo; 
} NUUNIFORMRECORD;

typedef struct nushaderprogram_s {
    GLuint vertexShader;        
    GLuint fragmentShader;      
    GLuint program;            
    GLint numUniforms;          
    NUUNIFORMRECORD *uniforms;  
    int unk14;                  
    int unk18;                  
    int unk1C;               
} NUSHADERPROGRAM;



typedef struct GLSLTypeInfo_s {
    int unk00;         
    int typeClass;     
    int typeEnum;     
    int unk0C;         
} GLSLTYPEINFO;

#ifdef __cplusplus
int NuShaderObjectBindAttributeLocationsGLSL(GLuint program);
int NuShaderObjectCombineGLSLShadersIntoProgram(GLuint *program_dest, GLuint vertex_shader, GLuint fragment_shader);
int NuShaderObjectGenerateGLSLShader(GLuint *shader_dest, GLenum shader_type, const GLchar *shader_source,
                                     GLint shader_source_length);

extern "C" {
#endif
    void NuShaderObjectBaseCreate(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectGLSLCreate(NUSHADEROBJECTGLSL *shader);
    void NuShaderObjectCreate(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseDestroy(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectGLSLDestroy(NUSHADEROBJECTGLSL *shader);
    void NuShaderObjectDestroy(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseInit(NUSHADEROBJECTBASE *shader, NUSHADEROBJECTKEY *key, int unk);
    void NuShaderObjectUnInit(NUSHADEROBJECT *shader);
    void NuShaderObjectBaseUnInit(NUSHADEROBJECTBASE *shader);
    void NuShaderObjectBaseSetWaterSpeed(f32 speed);
    NUSHADERPROGRAM* NuShaderProgramCreateIOS(const GLchar *vtxSource, const GLchar *fragSource);
    GLSLTYPEINFO* GetGLSLTypeInfo(GLenum glType);
#ifdef __cplusplus
}
#endif
