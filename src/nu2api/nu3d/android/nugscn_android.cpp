#include "nu2api/nu3d/android/nugscn_android.h"

#include <GLES2/gl2.h>

#include "legoapi/characters/core/character.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numath.h"

i32 g_vaoLifetimeMutex = -1;
u32 g_lastBoundVAO;
