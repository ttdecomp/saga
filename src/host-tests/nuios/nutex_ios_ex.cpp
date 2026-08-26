#include "nu2api/nu3d/android/nutex_ios_ex.h"

i32 g_currentTexUnit = -1;
i32 g_loadDefaultTexture;
GLuint g_lastBound2DTexIds[16];
GLuint g_lastBoundCubeTexIds[16];
GLuint g_earlyColorFramebuffer;
GLuint g_defaultFramebuffer;
GLuint g_currentFramebuffer;
GLuint g_earlyColorMSAAFramebuffer;
GLuint g_earlyColorTexture = 0;
u8 g_hostReadbackRGBA[1280 * 720 * 4] = {0};
bool g_hostReadbackReady = false;
