#pragma once

#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuspline.h"

extern shopitem_s TopShelf[6];
extern shopitem_s *CharItems;
extern shopitem_s *HintItems;
extern shopitem_s *ExtraItems;
extern shopitem_s *CodeItems;
extern shopitem_s *BrickItems;

extern u32 codelist[144];
extern i32 SHOPCHARCOUNT;

extern __attribute__((visibility("hidden"))) NUGSPLINE *splshelf;
extern __attribute__((visibility("hidden"))) NUGSPLINE *splcharshelf;
extern __attribute__((visibility("hidden"))) NUGSPLINE *splcodes;
extern NUVEC ShelfPos[6];
extern NUVEC SubShelfPos[7];
extern NUVEC CodePos[7];
extern u16 shelfang;

extern nuhspecial_s iconback;
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
void DrawTopShelf(i32 picked_item);
