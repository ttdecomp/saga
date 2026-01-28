#pragma once

#include <GLES2/gl2.h>

#include "decomp.h"

typedef struct nunativetexps_s {
    GLuint gl_tex;
} NUNATIVETEXPS;

C_API_START

extern int g_textureLoadBufferCriticalSection;

extern GLuint g_LegoEnvTexture;
extern GLuint g_PhongEnvTexture;
extern GLuint g_whiteTexture;

C_API_END
