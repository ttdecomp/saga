#pragma once

#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/common.h"

// Text system (module legoapi/menus/core, text.cpp).

void Text_InitLanguageList(LANGUAGEDATA *list);
void Text_SetLanguage(i32 lang);
void Text_LocaliseDecimalPoint(char *text);

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 MenuStopDraw;
    void SmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                     u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, u32 alpha);
    void SmartTextEx2(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                      u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, u32 alpha);
    void MenuSmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                         u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw,
                         u32 alpha);
    void MenuText3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u8 alignment, u8 red,
                      u8 green, u8 blue, i32 alpha);
    void SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale);
    void SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale);
#ifdef __cplusplus
}
#endif
