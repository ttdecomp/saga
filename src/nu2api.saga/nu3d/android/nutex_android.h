#pragma once

#include <GLES2/gl2.h>

typedef struct nunativetex_s NUNATIVETEX;

typedef struct nunativetexps_s {
    GLuint gl_tex;
} NUNATIVETEXPS;

#ifdef __cplusplus
extern "C" {
#endif
    extern int g_textureLoadBufferCriticalSection;

    extern GLuint g_LegoEnvTexture;
    extern GLuint g_PhongEnvTexture;
    extern GLuint g_whiteTexture;

    void NuTexSetTextureWithStagePS(NUNATIVETEX *tex, GLuint stage);
#ifdef __cplusplus
}
#endif
