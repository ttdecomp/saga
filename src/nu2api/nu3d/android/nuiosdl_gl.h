// GLES2 display-list callbacks and immediate GL state — see nuiosdl_gl.cpp.
#pragma once

#include "decomp.h"

struct numtl_s;
struct nunativetex_s;

typedef u32 NuVertexFormatPS;

// Globals defined in nuiosdl_gl.cpp (original bss 0x99b440..).
extern u32 g_boundShader;
extern i32 g_currentShaderProgram;
extern numtl_s *g_boundMaterial;
extern numtl_s *g_renderContext_materialInUse;
extern numtl_s *g_LastMtl;
extern u32 g_boundVertexFormat;
extern u32 g_activeAttributes;
extern i32 g_renderContext_zFunc;
extern u32 g_alphaRef;
extern u32 g_alphaFunc;
extern i32 g_alphaTestEnabled;
extern u32 g_lastAlphaRef;
extern u32 g_lastAlphaBlend;
extern i32 g_DebrisGlassDistortTID;

void NuIOS_SetCullMode(i32 mode);
extern "C" void NuMtlSetRenderStatesPS(numtl_s *mtl);
extern "C" void NuRenderContextSetZFunc(i32 zfunc);
extern "C" void NuIOS_SetVertexFormat(u32 fmt);
void NuIOSDLMtlCallback(void *arg);
void NuIOSDLPreWarmGeomCallback(void *arg);
void NuIOSDLGeom2DCallback(void *arg);
