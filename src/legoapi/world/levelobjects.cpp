#include "legoapi/world/level_shared.h"

#include <string.h>

void *LevObj_FindByPlatID(WORLDINFO_s *world, i32 platID) {
    i32 count = LEVELOBJECTCOUNT;
    u8 *obj;
    i32 i;

    if (count <= 0) {
        return NULL;
    }
    obj = *(u8 **)((char *)world + 0x2ac0);
    if (*(i16 *)(obj + 0xc) == platID) {
        return obj;
    }
    for (i = 1; i < count; i++) {
        obj += 0x10;
        if (*(i16 *)(obj + 0xc) == platID) {
            return obj;
        }
    }
    return NULL;
}

void LevelObjects_InitForGame(LEVELOBJECT *, variptr_u *, variptr_u *, i32, i32) {
}

void LevObj_FixUpPlatIDs(WORLDINFO_s *world) {
    i32 i;
    u8 *obj;

    if (ObjTabList == NULL || LEVELOBJECTCOUNT <= 0) {
        return;
    }
    for (i = 0; i < LEVELOBJECTCOUNT; i++) {
        obj = *(u8 **)((char *)world + 0x2ac0) + i * 0x10;
        *(i16 *)(obj + 0xc) = -1;
        if (*(void **)((char *)world + 0x295c) != NULL) {
            if (NuSpecialExistsFn(obj)) {
                if (*(u8 *)((char *)ObjTabList + i * 8) == 1) {
                    i++;
                    *(i16 *)(obj + 0xc) = FindPlatInst(NuSpecialGetInstanceix(obj));
                }
            }
        }
    }
}
