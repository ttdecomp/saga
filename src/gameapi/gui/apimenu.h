#pragma once

typedef struct MENU_s {
    float menu_time;
    float unk;
    char filler1[0xc];
    short menu;
    char filler2[0x2662];
} MENU;

typedef struct MENUFNINFO_s {
    int id;
    void *enter_fn;
    void *draw_fn;
    void *update_fn;
    void *exit_fn;
    short memory_x;
    short memory_y;
    char wrap;
} MENUFNINFO;

#ifdef __cplusplus

void APIMenuDrawMemCardSlots(MENU* menu, float y);
void APIMenuDrawGameState(float x, float y, int highlight, int slot);

extern "C" {
#endif

    void MenuReset(void);
    void MenuInitialiseEx(MENUFNINFO* menu_infos, int menu_infos_count, int language_count, void(*draw_save_slots_info_fn)(float, float, int, int), int fade_enabled, int shadow_enabled);
    void MenuInitialise(MENUFNINFO* menu_infos, int menu_infos_count, int language_count, void(*draw_save_slots_fn)(MENU*, float), int fade_enabled, int shadow_enabled);

#ifdef __cplusplus
}
#endif
