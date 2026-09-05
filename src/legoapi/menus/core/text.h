#pragma once

#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/common.h"

// Text system (module legoapi/menus/core, text.cpp).

struct vufnt_s;

void Text_InitLanguageList(LANGUAGEDATA *list);
void Text_SetLanguage(i32 lang);
void Text_SetLanguage_Game(i32 language);
extern void (*Text_GameSetLanguageFn)(i32 language);
void Text_LocaliseDecimalPoint(char *text);
i32 Text_GetMaxOverallStrings();
void Text_MakeTime(f32 time, i32 show_hours, i32 show_minutes, i32 show_centiseconds, char *text);
void TextCrawl_Init(TEXTCRAWL_s *crawl, i32 id, i32 unk);
void TextCrawl_Draw(f32 dt, i32 paragraphs, f32 alpha, char *text);

#ifdef __cplusplus
extern "C" {
#endif
    void SetQFont2D(void);
    void Set3DGameFont(vufnt_s *font);
    void SetGameFont(vufnt_s *font);
    extern i32 MenuStopDraw;
    extern i32 smarttextex_drawmessagebox;
    void SmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                     u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, u32 alpha);
    void SmartTextEx2(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                      u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, u32 alpha);
    void MenuSmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                         u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw,
                         u32 alpha);
    void MenuText3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u8 alignment, u8 red,
                      u8 green, u8 blue, i32 alpha);
    void Text3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u8 alignment, u8 red,
                  u8 green, u8 blue, i32 alpha);
    void Text3DEx2(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u8 alignment, u8 red,
                   u8 green, u8 blue, i32 alpha);
    void SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale);
    void SmartTextSetFWNMode(i32 mode);
    void SmartTextSetFont(vufnt_s *font);
    void SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale);
    void TextRegisterButtonMapFn(void (*fn)(char *, char *));
    void TextRegisterPulseTimerFn(f32 (*fn)(f32));
#ifdef __cplusplus
}
#endif
