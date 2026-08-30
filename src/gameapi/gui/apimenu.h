#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"

typedef struct MENU_s {
    f32 menu_time;            // 0x0000
    f32 unk;                  // 0x0004
    i16 selected_column;      // 0x0008
    i16 selected_row;         // 0x000a
    i16 first_column;         // 0x000c
    i16 first_row;            // 0x000e
    i16 last_column;          // 0x0010
    i16 last_row;             // 0x0012
    i16 menu;                 // 0x0014
    i8 previous_menu;         // 0x0016
    i8 flags_17;              // 0x0017
    f32 transition_time;      // 0x0018
    f32 transition_duration;  // 0x001c
    f32 repeat_up_time;       // 0x0020
    f32 repeat_down_time;     // 0x0024
    f32 repeat_left_time;     // 0x0028
    f32 repeat_right_time;    // 0x002c
    u8 repeat_up_count;       // 0x0030
    u8 repeat_down_count;     // 0x0031
    u8 repeat_left_count;     // 0x0032
    u8 repeat_right_count;    // 0x0033
    i32 selected_item;        // 0x0034 (selected row after navigation)
    i32 selected_item_column; // 0x0038 (selected column after navigation)
    i32 previous_item;        // 0x003c
    u32 buttons_held;         // 0x0040
    u32 buttons_pressed;      // 0x0044
    i32 column_count;         // 0x0048
    i32 row_count;            // 0x004c
    i32 field_50;             // 0x0050
    i32 up_pressed;           // 0x0054
    i32 down_pressed;         // 0x0058
    i32 left_pressed;         // 0x005c
    i32 right_pressed;        // 0x0060
    i32 confirm_pressed;      // 0x0064
    i32 cancel_pressed;       // 0x0068
    i32 up_held;              // 0x006c
    i32 down_held;            // 0x0070
    i32 left_held;            // 0x0074
    i32 right_held;           // 0x0078
    u32 start_pressed;        // 0x007c
    u32 select_pressed;       // 0x0080
    u32 action_04_pressed;    // 0x0084
    u32 action_08_pressed;    // 0x0088
    i32 draw_item;            // 0x008c
    f32 item_scale;           // 0x0090
    f32 draw_x;               // 0x0094
    f32 draw_y;               // 0x0098
    f32 draw_z;               // 0x009c
    f32 field_a0;             // 0x00a0
    f32 centre_offset;        // 0x00a4
    u8 filler_a8[4];
    i32 paused;            // 0x00ac
    i32 field_b0;          // 0x00b0
    i32 field_b4;          // 0x00b4
    i32 field_b8;          // 0x00b8
    i32 field_bc;          // 0x00bc
    i32 field_c0;          // 0x00c0
    f32 *item_offsets;     // 0x00c4
    f32 item_offset_scale; // 0x00c8
    f32 field_cc;          // 0x00cc
    f32 item_scale_x;      // 0x00d0
    f32 item_spacing;      // 0x00d4
    i32 input_disabled;    // 0x00d8
    i32 state;             // 0x00dc
    i32 queued_item;       // 0x00e0
    i32 queued_column;     // 0x00e4
    i32 queued_row;        // 0x00e8
    u8 move_left;          // 0x00ec
    u8 move_right;         // 0x00ed
    u8 move_up;            // 0x00ee
    u8 move_down;          // 0x00ef
    u8 close_requested;    // 0x00f0
    u8 input_activity;     // 0x00f1
    u8 filler_f2[6];
    f32 item_x[400];      // 0x00f8
    f32 item_y[400];      // 0x0738
    f32 item_width[400];  // 0x0d78
    f32 item_height[400]; // 0x13b8
    i32 item_column[400]; // 0x19f8
    i32 item_row[400];    // 0x2038
} MENU;

DECOMP_ASSERT(sizeof(MENU) == 0x2678, "MENU size");

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
extern "C" {
#endif
    extern MENUFNINFO GameMenuInfo[33];
    extern MENUFNINFO MenuInfo[100];
    extern MENU GameMenu[10];
    extern i32 GameMenuLevel;
    extern i32 MenusUsed;
    extern i32 MenuValidated;
    extern i32 MenuResult;
    extern i32 CurrentMenuId;
    extern f32 MenuAlpha;
    extern i32 MenuA;
    extern f32 MENUTEXTSCALE;
    extern f32 MENUDY;
    extern f32 MENUTOPY;
    extern f32 dme_sx;
    extern f32 dme_sy;
    extern i32 dme_align;
    extern i32 dme_rgb;
    extern u8 dme_r;
    extern u8 dme_g;
    extern u8 dme_b;
    extern f32 menu_pulse;
    extern f32 menu_pulse_speed;
    extern i32 menu_pulse_angle;
    extern f32 menu_pulsate;
    extern f32 menu_pulsate_speed;
    extern i32 menu_pulsate_angle;
    extern i32 MenuFadeEnabled;
    extern char MenuHeader[64];
    extern u8 MENUHEADERR;
    extern u8 MENUHEADERG;
    extern u8 MENUHEADERB;
#ifdef __cplusplus
}
#endif

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
    void DrawMenuEntry(MENU *menu, char *text);
    void DrawMenuEntryEx(MENU *menu, char *text, i32 alpha);
    void DrawMenuHeader(void);
    void Draw_CANCEL(MENU *menu);
    void MenuRememberCursor(MENU *menu);
    i32 UpdateMenu(u32 buttons_held, u32 buttons_pressed, u32 alternate_held, u32 alternate_pressed, f32 elapsed,
                   u32 confirm_mask, u32 cancel_mask, u32 start_mask, u32 select_mask);
#ifdef __cplusplus
}
#endif
