#pragma once

#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuspline.h"

extern shopitem_s TopShelf[6];
extern shopitem_s *CharItems;
extern shopitem_s *HintItems;
extern shopitem_s *ExtraItems;
extern shopitem_s *CodeItems;
extern shopitem_s *BrickItems;
extern shopitem_s CutItems[128];
extern nuhspecial_s extrasils[44];
extern nuhspecial_s atoz0to9icon[36];

extern u32 codelist[144];
extern i32 SHOPCHARCOUNT;
extern i32 SHOPHINTCOUNT;
extern i32 SHOPEXTRACOUNT;
extern i32 CutScenePlayCount;
extern i32 SHOPACTIVE;
extern f32 ShopNameAlpha;

extern i32 CharShelfIds[7];
extern i32 HintShelfIds[7];
extern i32 ExtraShelfIds[7];
extern i32 BrickShelfIds[7];
extern i32 CutShelfIds[7];
extern NUVEC CharCurPos[7];
extern NUVEC HintCurPos[7];
extern NUVEC ExtraCurPos[7];
extern NUVEC BrickCurPos[7];
extern NUVEC CutCurPos[7];

extern __attribute__((visibility("hidden"))) NUGSPLINE *splshelf;
extern __attribute__((visibility("hidden"))) NUGSPLINE *splcharshelf;
extern __attribute__((visibility("hidden"))) NUGSPLINE *splcodes;
extern NUVEC ShelfPos[6];
extern NUVEC SubShelfPos[7];
extern NUVEC CodePos[7];
extern u16 shelfang;

extern nuhspecial_s iconback;
extern nuhspecial_s infoblank;
extern nuhspecial_s cutblank;
extern nuhspecial_s cutfilm_unlocked;
extern nuhspecial_s cutfilm_locked;
extern nuhspecial_s toolblank;
extern nuhspecial_s codeblank;
extern nuhspecial_s question;
extern nuhspecial_s arrow1;
extern nuhspecial_s arrow2;
extern nuhspecial_s arrow3;
extern nuhspecial_s arrow4;
extern NUGSPLINE *shopcamspline;
extern NUVEC *shopcampos;
extern NUVEC *shopcamlookat;
extern __attribute__((visibility("hidden"))) f32 TopShelfScale[6];
extern __attribute__((visibility("hidden"))) f32 topscale[6];
extern __attribute__((visibility("hidden"))) f32 TopShelfPush[6];
extern __attribute__((visibility("hidden"))) f32 toppush[6];
extern i32 oldpicked;
extern i32 picked;
extern i32 subpicked;
extern i32 subitemselected;
extern __attribute__((visibility("hidden"))) void (*drawptr)();

void InitShop(WORLDINFO_s *world);
void LoadShelfSplines();
void UpdateCharacterIDs();
int DoShopMenu(MENU_s *menu);
int UpdateShop(MENU_s *menu);
void EndShopMenu(i32 exit_mode);
void DrawTopShelf(i32 picked_item);
