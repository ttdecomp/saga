#include "decomp.h"
#include "gameapi/gui/apimenu.h"
#include "gamelib/crc/crc.h"
#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"
#include "legoapi/core/input/gamepads.h"

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
shopitem_s CutItems[128] = {};
nuhspecial_s extrasils[44] = {};
nuhspecial_s atoz0to9icon[36] = {};

u32 codelist[144] = {};
i32 SHOPCHARCOUNT = 0;
i32 SHOPHINTCOUNT = 0;
i32 SHOPEXTRACOUNT = 0;
i32 CutScenePlayCount = 0;
i16 HintTab[24] = {-1};

i32 CharShelfIds[7] = {};
i32 HintShelfIds[7] = {};
i32 ExtraShelfIds[7] = {};
i32 BrickShelfIds[7] = {};
i32 CutShelfIds[7] = {};
NUVEC CharCurPos[7] = {};
NUVEC HintCurPos[7] = {};
NUVEC ExtraCurPos[7] = {};
NUVEC BrickCurPos[7] = {};
NUVEC CutCurPos[7] = {};

__attribute__((visibility("hidden"))) NUGSPLINE *splshelf = NULL;
__attribute__((visibility("hidden"))) NUGSPLINE *splcharshelf = NULL;
__attribute__((visibility("hidden"))) NUGSPLINE *splcodes = NULL;
NUVEC ShelfPos[6] = {};
NUVEC SubShelfPos[7] = {};
NUVEC CodePos[7] = {};
u16 shelfang = 0;

nuhspecial_s iconback = {};
nuhspecial_s infoblank = {};
nuhspecial_s cutblank = {};
nuhspecial_s cutfilm_unlocked = {};
nuhspecial_s cutfilm_locked = {};
nuhspecial_s toolblank = {};
nuhspecial_s codeblank = {};
nuhspecial_s question = {};
nuhspecial_s arrow1 = {};
nuhspecial_s arrow2 = {};
nuhspecial_s arrow3 = {};
nuhspecial_s arrow4 = {};
NUGSPLINE *shopcamspline = NULL;
NUVEC *shopcampos = NULL;
NUVEC *shopcamlookat = NULL;
__attribute__((visibility("hidden"))) f32 TopShelfScale[6] = {};
__attribute__((visibility("hidden"))) f32 TopBigScale[6] = {};
__attribute__((visibility("hidden"))) f32 topscale[6] = {};
__attribute__((visibility("hidden"))) f32 TopShelfPush[6] = {};
__attribute__((visibility("hidden"))) f32 TopBigPush[6] = {};
__attribute__((visibility("hidden"))) f32 toppush[6] = {};
i32 oldpicked = 1;
i32 picked = 1;
i32 subpicked = 0;
i32 subitemselected = 0;
__attribute__((visibility("hidden"))) void (*drawptr)() = NULL;

__attribute__((visibility("hidden"))) f32 scale2 = 0.0f;
__attribute__((visibility("hidden"))) f32 scale3 = 0.0f;
__attribute__((visibility("hidden"))) f32 scale4 = 0.0f;
__attribute__((visibility("hidden"))) f32 subpush[3] = {};
__attribute__((visibility("hidden"))) i32 moveitems = 0;
NUVEC selectedoff = {};
void *shopcutsceneplayer = NULL;

f32 ShopLockedScale = 0.0f;
f32 ShopNameAlpha = 0.0f;
i32 enteredshop = 0;
i32 SHOPACTIVE = 0;
i32 shopmenu = 0;
i32 col = 0;
char usercode[6] = {'A', 'A', 'A', 'A', 'A', 'A'};
extern i32 shop_from_cutsceneplayer;

__attribute__((visibility("hidden"))) f32 SubBigCharPush = 0.0f;
__attribute__((visibility("hidden"))) f32 SubNormCharPush = 0.0f;
__attribute__((visibility("hidden"))) ShopMenuCallback menuptr = NULL;
__attribute__((visibility("hidden"))) ShopMenuCallback oldmenuptr = NULL;
__attribute__((visibility("hidden"))) ShopMenuCallback menuparent[3] = {};
__attribute__((visibility("hidden"))) ShopMenuCallback oldmenuparent[3] = {};
__attribute__((visibility("hidden"))) ShopDrawCallback drawparent[3] = {};
__attribute__((visibility("hidden"))) ShopDrawCallback olddrawparent[3] = {};
__attribute__((visibility("hidden"))) ShopDrawCallback drawpanelptr = NULL;
__attribute__((visibility("hidden"))) ShopDrawCallback olddrawpanelptr = NULL;
__attribute__((visibility("hidden"))) ShopDrawCallback olddrawptr = NULL;
__attribute__((visibility("hidden"))) i32 currentmenulevel = 0;
__attribute__((visibility("hidden"))) i32 oldcurrentmenulevel = 0;
__attribute__((visibility("hidden"))) i32 currentdrawlevel = 0;
__attribute__((visibility("hidden"))) i32 oldcurrentdrawlevel = 0;
__attribute__((visibility("hidden"))) f32 oldpickedscale = 0.0f;
__attribute__((visibility("hidden"))) f32 oldpickedpush = 0.0f;
__attribute__((visibility("hidden"))) f32 slidetimer = 0.0f;
__attribute__((visibility("hidden"))) f32 scaleoverride[7] = {};
__attribute__((visibility("hidden"))) i32 ExitMenu = 0;
__attribute__((visibility("hidden"))) i32 lastitem = 0;
__attribute__((visibility("hidden"))) i32 itemchanged = 0;
__attribute__((visibility("hidden"))) f32 hintdrawwait = 0.0f;
__attribute__((visibility("hidden"))) i32 movesfxlock = 0;
__attribute__((visibility("hidden"))) i32 SubMenu = 0;
__attribute__((visibility("hidden"))) i32 easesubin = 0;
__attribute__((visibility("hidden"))) f32 inoutscale = 0.0f;
__attribute__((visibility("hidden"))) i32 cheatname = 0;

extern i32 ItemMenu(MENU_s *menu);
extern void DrawItemMenu2D();
extern void InitAlphaList();
extern void InitExtraList();
extern HINT_s *Hint_FindHint(i32 hint_id);
extern i16 HintTab[24];
extern void GameCam_Blend(GAMECAMERA_s *camera, f32 duration, f32 curve, i32 mode);

i32 DoShopMenu(MENU_s *menu) {
    i32 result = 0;
    if (menuptr != NULL) {
        result = menuptr(menu);
        if (menu->cancel_pressed != 0) {
            result = currentmenulevel < 1 ? 5 : 6;
            for (i32 i = 4; i < 13; ++i) {
                menu->item_width[i] = 0.0f;
            }
            subitemselected = 0;
        }

        if (result == 5) {
            return 1;
        }
        if (result == 6) {
            GameCam_Blend(GameCam, 0.6f, 0.0f, 1);
            drawptr = NULL;

            menuptr = menuparent[currentmenulevel];
            menuparent[currentmenulevel] = NULL;
            --currentmenulevel;
            if (currentmenulevel < 0) {
                currentmenulevel = 0;
            }

            drawpanelptr = drawparent[currentdrawlevel];
            drawparent[currentdrawlevel] = NULL;
            --currentdrawlevel;
            if (currentdrawlevel < 0) {
                currentdrawlevel = 0;
            }
        }
    }
    return 0;
}

i32 UpdateShop(MENU_s *menu) {
    UpdateCharacterIDs();

    scaleoverride[0] = 0.0f;
    scaleoverride[1] = 0.4f;
    scaleoverride[2] = 0.8f;
    scaleoverride[4] = 0.8f;
    scaleoverride[5] = 0.4f;
    scaleoverride[6] = 0.0f;

    if (1.0f > ShopNameAlpha) {
        ShopNameAlpha += FRAMETIME + FRAMETIME;
        if (ShopNameAlpha > 1.0f) {
            ShopNameAlpha = 1.0f;
        }
    }

    ShopLockedScale = SeekLinearF(ShopLockedScale, 1.0f, FRAMETIME * 3.0f);
    SubNormCharPush = 0.02f;
    SubBigCharPush = 0.05f;

    if (menuptr == NULL) {
        if (shop_from_cutsceneplayer != 0) {
            shop_from_cutsceneplayer = 0;
            topscale[5] = oldpickedscale;
            menuptr = oldmenuptr;
            toppush[5] = oldpickedpush;
            drawparent[0] = olddrawparent[0];
            menuparent[0] = oldmenuparent[0];
            drawparent[1] = olddrawparent[1];
            menuparent[1] = oldmenuparent[1];
            drawparent[2] = olddrawparent[2];
            menuparent[2] = oldmenuparent[2];
            drawpanelptr = olddrawpanelptr;
            drawptr = olddrawptr;
            currentmenulevel = oldcurrentmenulevel;
            currentdrawlevel = oldcurrentdrawlevel;
            picked = oldpicked;
        } else {
            menuptr = ItemMenu;
            ExitMenu = 0;
            slidetimer = 0.125f;
            enteredshop = 1;
            currentmenulevel = 0;
            currentdrawlevel = 0;
            menuparent[0] = NULL;
            drawparent[0] = NULL;
            picked = oldpicked;
            drawpanelptr = DrawItemMenu2D;
        }
    }

    return DoShopMenu(menu) != 0;
}

void BuyShopItem(shopitem_s *, i32, i32) {
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

    NuSpecialFind(WORLD->current_gscn, &infoblank, "info_blank", 1);
    NuSpecialFind(WORLD->current_gscn, &cutblank, "fmv_blank", 1);
    NuSpecialFind(WORLD->current_gscn, &cutfilm_unlocked, "shop_film1", 1);
    NuSpecialFind(WORLD->current_gscn, &cutfilm_locked, "shop_film1b", 1);
    NuSpecialFind(WORLD->current_gscn, &toolblank, "tool_blank", 1);
    NuSpecialFind(WORLD->current_gscn, &codeblank, "code_blank", 1);
    NuSpecialFind(things_scene, &question, "question_icon", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow1, "shop_arrow1", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow2, "shop_arrow2", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow3, "shop_arrow3", 1);
    NuSpecialFind(WORLD->current_gscn, &arrow4, "shop_arrow4", 1);
    NuSpecialSetVisibility(&arrow1, 0);
    NuSpecialSetVisibility(&arrow2, 0);
    NuSpecialSetVisibility(&arrow3, 0);
    NuSpecialSetVisibility(&arrow4, 0);

    InitExtraList();
    InitAlphaList();

    for (i32 i = 0; i < SHOPHINTCOUNT; ++i) {
        HINT_s *hint = Hint_FindHint(HintTab[i]);
        shopitem_s *item = &HintItems[i];
        item->item_id = i;
        item->type = 0;
        item->price = hint != NULL ? *reinterpret_cast<i32 *>(reinterpret_cast<u8 *>(hint) + 8) : 0;
        item->unlocked = (reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(&Game) + 0x7bd4)[i >> 5] >> (i & 0x1f)) & 1;
        if (item->price == 0) {
            item->unlocked = 1;
        }
        item->special = atoz0to9icon[i];
    }

    shopcamspline = NuSplineFind(WORLD->current_gscn, const_cast<char *>("shop_cam"));
    if (shopcamspline == NULL) {
        return;
    }
    shopcampos = shopcamspline->pts;
    shopcamlookat = shopcamspline->pts + 1;
    LoadShelfSplines();

    if (SHOPCHARCOUNT > 0) {
        for (i32 i = 0; i < 7; ++i) {
            CharShelfIds[i] = (SHOPCHARCOUNT + i - 3) % SHOPCHARCOUNT;
            CharCurPos[i] = SubShelfPos[i];
        }
    }
    if (SHOPHINTCOUNT > 0) {
        for (i32 i = 0; i < 7; ++i) {
            HintShelfIds[i] = (SHOPHINTCOUNT + i - 3) % SHOPHINTCOUNT;
            HintCurPos[i] = SubShelfPos[i];
        }
    }
    if (SHOPEXTRACOUNT > 0) {
        for (i32 i = 0; i < 7; ++i) {
            ExtraShelfIds[i] = (SHOPEXTRACOUNT + i - 3) % SHOPEXTRACOUNT;
            ExtraCurPos[i] = SubShelfPos[i];
        }
    }

    const i32 brick_shelf_ids[7] = {11, 12, 13, 0, 1, 2, 3};
    for (i32 i = 0; i < 7; ++i) {
        BrickShelfIds[i] = brick_shelf_ids[i];
        BrickCurPos[i] = SubShelfPos[i];
    }

    for (i32 i = 0; i < SHOPGOLDBRICKS; ++i) {
        shopitem_s *item = &BrickItems[i];
        item->item_id = i;
        item->type = 4;
        item->price = 10000 + i * 5000;
        item->unlocked = (reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(&Game) + 0x7bf8)[i >> 5] >> (i & 0x1f)) & 1;
        item->special = TopShelf[4].special;
    }

    for (i32 i = 0; i < CutScenePlayCount && i < 128; ++i) {
        shopitem_s *item = &CutItems[i];
        item->item_id = i;
        item->type = 5;
        item->price = 0;
        item->unlocked = 0;
        item->special = TopShelf[5].special;
    }

    if (shop_from_cutsceneplayer == 0 && CutScenePlayCount > 0) {
        for (i32 i = 0; i < 7; ++i) {
            CutShelfIds[i] = (CutScenePlayCount + i - 3) % CutScenePlayCount;
            CutCurPos[i] = SubShelfPos[i];
        }
    }

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

i32 CheckCash(shopitem_s *items, i32 item) {
    return Game.coins >= items[item].price;
}
