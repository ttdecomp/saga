#include "nu2api.saga/nu3d/android/nutex_ios_ex.h"

#include "decomp.h"

#include "nu2api.saga/nu3d/android/nutex_android.h"

int g_currentTexUnit = -1;

void NuTexSetTextureWithStagePS(NUNATIVETEX *tex, GLuint stage) {
    GLuint gl_tex;

    glActiveTexture(GL_TEXTURE0 + stage);

    g_currentTexUnit = stage;

    glBindTexture(GL_TEXTURE_2D, tex != NULL ? tex->platform.gl_tex : 0);
}

GLuint NuIOS_CreateGLTexFromFile(const char *filename) {
    UNIMPLEMENTED();
}

GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, int *width, int *height, bool is_pvrtc) {
    UNIMPLEMENTED();
}
