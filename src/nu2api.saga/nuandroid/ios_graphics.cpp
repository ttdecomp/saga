#include "nu2api.saga/nuandroid/ios_graphics.h"

#include <GLES2/gl2.h>
#include <string.h>

#include "decomp.h"
#include "nu2api.saga/nu3d/NuRenderDevice.h"
#include "nu2api.saga/nuandroid/nuios.h"

void NuIOSInitOpenGLES(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 308);
    NuIOS_AllocateSystemFramebuffers();
    glFrontFace(GL_CW);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 312);
}

GLuint g_lastBound2DTexIds[16];
GLuint g_lastBoundCubeTexIds[16];
GLuint g_earlyColorFramebuffer;
GLuint g_defaultFramebuffer;
GLuint g_currentFramebuffer;
GLuint g_earlyColorMSAAFramebuffer;

void NuIOS_AllocateSystemFramebuffers(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 106);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 108);

    memset(g_lastBound2DTexIds, 0, sizeof(g_lastBound2DTexIds));
    memset(g_lastBoundCubeTexIds, 0, sizeof(g_lastBoundCubeTexIds));

    g_earlyColorFramebuffer = 0;
    if (NuIOS_ShouldUseMSAA()) {
        glGenFramebuffers(1, &g_earlyColorMSAAFramebuffer);
    }
    g_defaultFramebuffer = 0;
    g_currentFramebuffer = 0;
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 260);
}

i32 NuCheckGLErrorsFL(char *file, i32 line) {
    return 0; // nice
}
