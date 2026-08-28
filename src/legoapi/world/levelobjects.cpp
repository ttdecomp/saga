#include "legoapi/world/level.h"
#include "globals.h"

#include <string.h>

extern "C" i32 NuSpecialExistsFn(void *);
extern "C" void *NuSpecialGetInstanceix(void *);
extern "C" i16 FindPlatInst(void *);

void *LevObj_FindByPlatID(WORLDINFO_s *world, i32 platID) {
    i32 count = LEVELOBJECTCOUNT;
    u8 *obj;
    i32 i;

    if (count <= 0) {
        return NULL;
    }
    obj = (u8 *)world->lev_objs;
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

// LevelObjects_InitForGame @0x475400. Registers the perm-loaded object table
// (ObjTab: 8-byte {kind, pad, ref-flag, name} entries terminated by kind 0xff)
// as ObjTabList, counts the entries into LEVELOBJECTCOUNT, remembers the range
// of reference entries (ref-flag == 1) for LevelObject_GetReflection, and
// reserves the extra-object name table from the perm buffer. The original also
// resets four unnamed scratch slots to -1 (last-ref bookkeeping state consumed
// by the level-editor paths); our three named LevObjRef_* globals carry the
// same information the game reads back.
void LevelObjects_InitForGame(LEVELOBJECT *tab, VARIPTR *buf, VARIPTR *buf_end, i32 max, i32 name_table_size) {
    (void)buf_end;
    i32 count;
    i32 first = -1;
    i32 last = -1;
    LEVELOBJECT *entry;

    ExtraLevelObject_NameTableIndex = 0;
    ObjTabList = tab;

    // Walk the table from the start until the 0xff terminator, tracking
    // ref-flag entries; the counter continues from LEVELOBJECTCOUNT.
    count = LEVELOBJECTCOUNT;
    entry = tab;
    if ((u8)entry->kind != 0xff) {
        while ((u8)entry->kind != 0xff) {
            if (*(u16 *)&entry->pad_02 == 1) {
                if (first == -1) {
                    first = count;
                }
                last = count;
            }
            entry++;
            count++;
        }
    }
    LEVELOBJECTCOUNT = count;
    LevObjRef_FirstObj = first;
    LevObjRef_LastObj = last;

    // Carve the extra-object name table from the perm buffer.
    if (name_table_size > 0) {
        ExtraLevelObject_NameTable = (char *)buf->u8_ptr;
        ExtraLevelObject_NameTableSize = name_table_size;
        buf->u32_ptr += name_table_size;
    }
}

void LevObj_FixUpPlatIDs(WORLDINFO_s *world) {
    i32 i;
    u8 *obj;

    if (ObjTabList == NULL || LEVELOBJECTCOUNT <= 0) {
        return;
    }
    for (i = 0; i < LEVELOBJECTCOUNT; i++) {
        obj = (u8 *)world->lev_objs + i * 0x10;
        *(i16 *)(obj + 0xc) = -1;
        if (world->terrain != NULL) {
            if (NuSpecialExistsFn(obj)) {
                if (*(u8 *)((char *)ObjTabList + i * 8) == 1) {
                    i++;
                    *(i16 *)(obj + 0xc) = FindPlatInst(NuSpecialGetInstanceix(obj));
                }
            }
        }
    }
}
