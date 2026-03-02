#pragma once

#include <GLES2/gl2.h>

struct nunativetex_s;

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

    void NuTexSetTextureWithStagePS(struct nunativetex_s *tex, GLuint stage);
#ifdef __cplusplus
}
#endif
