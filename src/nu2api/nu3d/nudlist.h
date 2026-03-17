#pragma once

#include "nu2api/nucore/common.h"

typedef struct nudisplaylistitem_s {
    u8 type;
    u8 id;
    i16 size;
    void *next;
    u32 p[2];
} NUDISPLAYLISTITEM;

typedef struct nudisplaylist_state_s {
    u8 pad00[0x08];
    u32 unk08;
    u8 pad0C[0x08];
    u32 unk14;
    u8 pad18[0x06];
    u16 unk1E;
    u32 unk20;
    u16 unk24;
    u8 pad26[0x06];
    u16 unk2C;
    u16 unk2E;
    u8 pad30[0x02];
    u16 unk32;
} NUDISPLAYLIST_STATE;

typedef struct nudisplaylist_s {
    u32 unk00[2];
    NUDISPLAYLIST_STATE *state;
    u32 unk0C;
    NUDISPLAYLISTITEM *head;
    NUDISPLAYLISTITEM *current;
} NUDISPLAYLIST;

#ifdef __cplusplus
extern "C" {
#endif
    extern VARIPTR *display_list_buffer;
    void NuDisplayListAnimateMtls(float param_1);
    void NuDisplayListReset(NUDISPLAYLIST *list);
    int NuDisplayListAddRenderScene(void);
    static VARIPTR *NuDisplayListGetBuffer(void) {
        display_list_buffer->addr = ALIGN(display_list_buffer->addr, 0x10);

        return display_list_buffer;
    }
#ifdef __cplusplus
}
#endif
