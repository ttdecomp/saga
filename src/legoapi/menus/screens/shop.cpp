#include "decomp.h"
#include "gamelib/crc/crc.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

shopitem_s TopShelf[6] = {};
shopitem_s *CharItems = NULL;
shopitem_s *HintItems = NULL;
shopitem_s *ExtraItems = NULL;
shopitem_s *CodeItems = NULL;
shopitem_s *BrickItems = NULL;

u32 codelist[144] = {};
i32 SHOPCHARCOUNT = 0;

__attribute__((visibility("hidden"))) NUGSPLINE *splshelf = NULL;
__attribute__((visibility("hidden"))) NUGSPLINE *splcharshelf = NULL;
__attribute__((visibility("hidden"))) NUGSPLINE *splcodes = NULL;
NUVEC ShelfPos[6] = {};
NUVEC SubShelfPos[7] = {};
NUVEC CodePos[7] = {};
u16 shelfang = 0;

nuhspecial_s iconback = {};
__attribute__((visibility("hidden"))) f32 TopShelfScale[6] = {};
__attribute__((visibility("hidden"))) f32 topscale[6] = {};
__attribute__((visibility("hidden"))) f32 TopShelfPush[6] = {};
__attribute__((visibility("hidden"))) f32 toppush[6] = {};
i32 oldpicked = 1;
i32 picked = 1;
i32 subpicked = 0;
i32 subitemselected = 0;
__attribute__((visibility("hidden"))) void (*drawptr)() = NULL;

void DoShopMenu(MENU_s *) {
}

void UpdateShop(MENU_s *) {
}

void BuyShopItem(shopitem_s *, i32, i32) {
}

void EndShopMenu(i32) {
}

void SelectSubItem() {
}

void SelectShopItem(shopitem_s *, i32) {
}

void Shop_UpdateHint(HINT_s *) {
}

void BuyAllShopExtras() {
}

void GetShopCamLookPos(nuvec_s *) {
}

void Shop_CollectAllCharacters(i32) {
}

void InitShop(WORLDINFO_s *world) {
    CharItems =
        static_cast<shopitem_s *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, 100 * sizeof(shopitem_s)));
    HintItems =
        static_cast<shopitem_s *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, 64 * sizeof(shopitem_s)));
    ExtraItems =
        static_cast<shopitem_s *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, 64 * sizeof(shopitem_s)));
    CodeItems =
        static_cast<shopitem_s *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, 64 * sizeof(shopitem_s)));
    BrickItems =
        static_cast<shopitem_s *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, 14 * sizeof(shopitem_s)));

    if (CharItems == NULL || HintItems == NULL || ExtraItems == NULL || CodeItems == NULL || BrickItems == NULL) {
        return;
    }

    TopShelf[0].type = 0;
    NuStrCpy(TopShelf[0].name, "Hint");
    NuStrCpy(TopShelf[0].special_name, "info");
    NuSpecialFind(things_scene, &TopShelf[0].special, "info", 1);

    TopShelf[1].type = 1;
    TopShelf[1].name[0] = '\0';
    TopShelf[1].special_name[0] = '\0';
    memset(&TopShelf[1].special, 0, sizeof(TopShelf[1].special));

    TopShelf[2].type = 2;
    NuStrCpy(TopShelf[2].name, "Extra");
    NuStrCpy(TopShelf[2].special_name, "tool_box");
    NuSpecialFind(world->current_gscn, &TopShelf[2].special, "tool_box", 1);

    TopShelf[3].type = 3;
    NuStrCpy(TopShelf[3].name, "Code");
    NuStrCpy(TopShelf[3].special_name, "shop_question");
    NuSpecialFind(world->current_gscn, &TopShelf[3].special, "shop_question", 1);

    TopShelf[4].type = 4;
    NuStrCpy(TopShelf[4].name, "Gold Bricks");
    NuStrCpy(TopShelf[4].special_name, "gold_brick");
    NuSpecialFind(things_scene, &TopShelf[4].special, "gold_brick", 1);

    TopShelf[5].type = 5;
    NuStrCpy(TopShelf[5].name, "Cut Scenes");
    NuStrCpy(TopShelf[5].special_name, "FMV");
    NuSpecialFind(world->current_gscn, &TopShelf[5].special, "fmv", 1);

    memset(codelist, 0, sizeof(codelist));
    SHOPCHARCOUNT = 0;
    i32 code_count = 0;
    for (i32 i = 0; i < ShopCollection.count_y; ++i) {
        COLLECTID *collect = &ShopCollection.list[i];
        shopitem_s *item = &CharItems[SHOPCHARCOUNT];
        item->unlocked = 0;
        item->type = 1;

        u32 *unlocked_bits = reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(&Game) + 0x7be0);
        if (((unlocked_bits[SHOPCHARCOUNT >> 5] >> (SHOPCHARCOUNT & 0x1f)) & 1) != 0) {
            item->unlocked = 1;
        }

        const i32 character_id = collect->id;
        if (CDataList[character_id].name_id == -1) {
            NuStrCpy(item->name, CDataList[character_id].dir);
        } else {
            NuStrCpy(item->name, TTab[CDataList[character_id].name_id]);
        }
        memset(&item->special, 0, sizeof(item->special));

        if (code_count <= 143) {
            codelist[code_count] = CRC_ProcessStringIgnoreCase(collect->cheat_code);
            ++code_count;
        }
        ++SHOPCHARCOUNT;
    }

    UpdateCharacterIDs();
    NuSpecialFind(things_scene, &iconback, "icon_back_neutral", 1);
    LoadShelfSplines();

    const f32 shelf_scale[6] = {0.9f, 0.28f, 0.9f, 0.95f, 0.8f, 0.9f};
    const f32 shelf_push[6] = {0.045f, 0.045f, 0.045f, 0.05f, 0.015f, 0.045f};
    const f32 current_push[6] = {0.0f, 0.0f, -0.005f, 0.0f, -0.01f, 0.0f};
    memcpy(TopShelfScale, shelf_scale, sizeof(TopShelfScale));
    memcpy(topscale, shelf_scale, sizeof(topscale));
    memcpy(TopShelfPush, shelf_push, sizeof(TopShelfPush));
    memcpy(toppush, current_push, sizeof(toppush));
    picked = oldpicked;
    subpicked = 0;
}

void CheckCash(shopitem_s *, i32) {
}
