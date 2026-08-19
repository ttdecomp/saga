#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void UpdateGameMenu(GAMEPAD_s *, i32) {
}

void GetParentMenuID() {
}

void GetMenuActiveChild(eduimenu_s *) {
}

void ResizePauseScreenTexture(i32, i32) {
}

void CodeMenu(MENU_s *) {
}

void ItemMenu(MENU_s *) {
}

void GetMenuID() {
}

extern "C" {

    void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
        (void)menu_id;
        (void)menu_y;
        (void)param3;
    }

} // extern "C"
