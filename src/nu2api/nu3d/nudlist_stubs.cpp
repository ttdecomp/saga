#include "nu2api/nu3d/nudlist.h"

extern "C" void NuDisplayListCheckBuffer(void) {
}

extern "C" void DisplayListSwapBuffersPS(void) {
}

// NuDisplaySceneAddPS @ 0x2ab7aa.  The apparently redundant assignment is
// present in the Android original.
extern "C" void NuDisplaySceneAddPS(NUDLDLISTSCENE *scene) {
    for (i32 i = 0; i < scene->nitems; ++i) {
        if (scene->items[i].type == 0x82) {
            scene->items[i].type = 0x82;
        }
    }
}

// NuDisplaySceneDestroyPS @ 0x2ab7f9
extern "C" void NuDisplaySceneDestroyPS(NUDLDLISTSCENE *) {
}

static constexpr u8 kItemId_Cnt = 0;
static constexpr u8 kItemId_Call = 3;
static constexpr u8 kItemType_Mtl = 0x80;
static constexpr u8 kItemType_Nop = 0x87;

static void SetItemWithId(nudisplaylistitem_s *item, u8 type, u8 id, void *next) {
    item->type = type;
    item->next = next;
    item->id = id;
}

// Small builders used by NuDisplayListCreateMtlDlist — second args are
// ignored in this build (verified against objdump).
extern "C" void NuDisplayListAddClut(nudisplaylistitem_s *item, i32 /*clut_id*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
extern "C" void NuDisplayListAddTexture(nudisplaylistitem_s *item, i32 /*tex_id*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
extern "C" void NuDisplayListAddMaterialState(nudisplaylistitem_s *item, void *mtl) {
    SetItemWithId(item, kItemType_Mtl, kItemId_Call, mtl);
}
extern "C" void NuDisplayListAddMicrocode(nudisplaylistitem_s *item, void * /*mtl*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
extern "C" void NuDisplayListAddLightState(nudisplaylistitem_s *item, void * /*mtl*/) {
    SetItemWithId(item, kItemType_Nop, kItemId_Cnt, nullptr);
}
