#include "nu2api/nu3d/android/nutex_ios_ex.h"

// These host definitions intentionally retain the platform API names declared
// in nutex_ios_ex.h. Internal host-only state and helpers use a host prefix.
i32 g_currentTexUnit = -1;
i32 g_loadDefaultTexture;
GLuint g_lastBound2DTexIds[16];
GLuint g_lastBoundCubeTexIds[16];
GLuint g_earlyColorFramebuffer;
GLuint g_defaultFramebuffer;
GLuint g_currentFramebuffer;
GLuint g_earlyColorMSAAFramebuffer;
GLuint g_earlyColorTexture = 0;
