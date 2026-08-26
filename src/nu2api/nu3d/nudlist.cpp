#include "nu2api/nu3d/nudlist.h"

extern VARIPTR rndrstream_end;
extern VARIPTR rndrstream_free;

extern "C" VARIPTR *display_list_buffer = 0;

// Bulk display-list manager (original bss @0x11a0080, 0x604 bytes). Opaque:
// only byte offsets within it are meaningful (see NuDisplayListInit and
// NuDisplayListGet2dList).
extern "C" u8 global_dlist_manager[0x604] = {0};
extern "C" VARIPTR *display_list_buffer_end = 0;

// Scratch space backing the static 2D list's item cursor. The original
// initialises the matching area of the manager bss elsewhere; a dedicated
// buffer keeps NuDisplayListAddItem safe regardless.
static u8 nudlist_2d_item_scratch[0x100] = {0};

static void NuDisplayListSetID_CALL(nudisplaylistitem_s *item) {
    item->id = 3;
}

static void NuDisplayListSetNext(nudisplaylistitem_s *item, void *next) {
    item->next = next;
}

extern "C" void NuDisplayListCheckBuffer(void) {
}

extern "C" nudisplaylist_s *NuDisplayListGet2dList(void) {
    // The 2D list is embedded in the manager at offset 0x4B8; its item cursor
    // (nudisplaylist_s+0x24, manager+0x4DC) is pointed at scratch space here
    // so AddItem is always safe.
    nudisplaylist_s *list = (nudisplaylist_s *)(global_dlist_manager + 0x4B8);
    if (list->items == NULL) {
        list->items = (nudisplaylistitem_s *)nudlist_2d_item_scratch;
    }
    return list;
}

extern "C" void NuDisplayListResetBuffer(void) {
    display_list_buffer = (VARIPTR *)&rndrstream_free;
    display_list_buffer_end = (VARIPTR *)rndrstream_end.addr;
}

static void NuDisplayListSetID_CNT(nudisplaylistitem_s *item) {
    item->id = 0;
}

static void NuDisplayListSetID_RET(nudisplaylistitem_s *item) {
    item->id = 4;
}

static void NuDisplayListSetID_NEXT(nudisplaylistitem_s *item) {
    item->id = 1;
}

static void NuDisplayListSetID(nudisplaylistitem_s *item, u8 id) {
    switch (id) {
        case 0:
            NuDisplayListSetID_CNT(item);
            break;
        case 1:
            NuDisplayListSetID_NEXT(item);
            break;
        case 3:
            NuDisplayListSetID_CALL(item);
            break;
        case 4:
            NuDisplayListSetID_RET(item);
            break;
        default:
            break;
    }
}

static nudisplaylistitem_s *NuDisplayListAddItem(nudisplaylist_s *list, u8 type, void *next) {
    nudisplaylistitem_s *item = list->items;

    item->type = type;
    NuDisplayListSetID_CALL(item);
    NuDisplayListSetNext(item, next);

    list->items = (nudisplaylistitem_s *)((u8 *)list->items + 0x10);

    return (nudisplaylistitem_s *)((u8 *)list->items - 0x10);
}

static void NuDisplayListSetItem(nudisplaylistitem_s *item, u8 type, u8 id, void *next) {
    item->type = type;
    NuDisplayListSetNext(item, next);
    NuDisplayListSetID(item, id);
}
