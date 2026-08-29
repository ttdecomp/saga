#pragma once

#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/common.h"

// Text system (module legoapi/menus/core, text.cpp).

void Text_InitLanguageList(LANGUAGEDATA *list);
void Text_SetLanguage(i32 lang);

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 MenuStopDraw;
    void SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale);
    void SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale);
#ifdef __cplusplus
}
#endif
