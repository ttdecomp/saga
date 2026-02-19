#pragma once

#include "nu2api/nucore/common.h"

typedef struct MENU_s {
    f32 menu_time;
    f32 unk;
    u8 filler1[0xc];
    i16 menu;
    u8 filler2[0x2662];
} MENU;

typedef struct MENUFNINFO_s {
    i32 id;
    void (*enter_fn)(MENU *);
    void (*draw_fn)(MENU *);
    void (*update_fn)(MENU *);
    void (*exit_fn)(MENU *);
    i16 memory_x;
    i16 memory_y;
    char wrap;
} MENUFNINFO;

#ifdef __cplusplus

void APIMenuDrawMemCardSlots(MENU *menu, f32 y);
void APIMenuDrawGameState(f32 x, f32 y, i32 highlight, i32 slot);

extern "C" {
#endif
    void MenuReset(void);
    void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_info_count, i32 language_count,
                          void (*draw_save_slots_info_fn)(f32, f32, i32, i32), i32 is_fade_enabled,
                          i32 is_shadow_enabled);
    void MenuInitialise(MENUFNINFO *menu_info, i32 menu_info_count, i32 language_count,
                        void (*draw_save_slots_fn)(MENU *, f32), i32 is_fade_enabled, i32 is_shadow_enabled);

    void MenuLoadTechnicalStrings(char *filepath, char *language, VARIPTR *buf, VARIPTR buf_end);
#ifdef __cplusplus
}
#endif
