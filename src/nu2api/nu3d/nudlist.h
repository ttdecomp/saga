#pragma once

#include "nu2api/nucore/common.h"

typedef struct nudisplaylistitem_s {
    u8 type;
    u8 id;
    i16 size;
    void *next;
    u32 p[2];
} NUDISPLAYLISTITEM;

typedef struct nudisplaylist_s {
    char filler[0x24]; // list header state 0x00-0x23

    struct nudisplaylistitem_s *items; // 0x24: next free item cursor (see NuDisplayListAddItem)
} NUDISPLAYLIST;

#ifdef __cplusplus
extern "C" {
#endif
    extern VARIPTR *display_list_buffer;

    static VARIPTR *NuDisplayListGetBuffer(void) {
        display_list_buffer->addr = ALIGN(display_list_buffer->addr, 0x10);

        return display_list_buffer;
    }
#ifdef __cplusplus
}
#endif
