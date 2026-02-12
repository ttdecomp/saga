#pragma once

#include <GLES2/gl2.h>

#ifdef __cplusplus
extern "C" {
#endif
    extern int g_currentTexUnit;
    extern GLuint g_lastBound2DTexIds[16];
    extern GLuint g_lastBoundCubeTexIds[16];
    extern GLuint g_earlyColorFramebuffer;
    extern GLuint g_defaultFramebuffer;
    extern GLuint g_currentFramebuffer;
    extern GLuint g_earlyColorMSAAFramebuffer;
#ifdef __cplusplus
}
#endif

GLuint NuIOS_CreateGLTexFromFile(const char *filename);
GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, int *width, int *height, bool is_pvrtc);
GLuint loadDefaultTexture(GLuint texture, GLint level, GLsizei size, GLenum texture_type, GLenum target);
