#include "decomp.h"

#include "nu2api.saga/nu3d/nutex.h"

C_API_START

extern int g_currentTexUnit;

void NuTexSetTextureWithStagePS(NUNATIVETEX *tex, GLuint stage);

C_API_END

GLuint NuIOS_CreateGLTexFromFile(const char *filename);
GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, int *width, int *height, bool is_pvrtc);
