#ifndef LEGOAPI_RENDER_CORE_RENDER_H
#define LEGOAPI_RENDER_CORE_RENDER_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"

// Core rendering helpers (module legoapi/render/core, render.cpp).

void DrawPaintLights(void);
void DrawTimer(i32 mins, i32 secs, i32 unknown);
void DrawBossHitPoints(GameObject_s *obj);
void DrawMiniSnowTroopers(WORLDINFO_s *world);
void DrawForceBackEffect(nuhspecial_s *special);
#ifdef __cplusplus
extern "C" {
#endif
    void Text3DEx(char *, i32, float, float, float, float, float, i32, i32, i32, i32, i32); // C-linkage symbol
#ifdef __cplusplus
}
#endif
void DrawPanel3DObject(float, float, float, float, float, float, u16, u16, u16, nuhspecial_s *, i32, float);

#endif
