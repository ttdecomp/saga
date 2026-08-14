#include "nu2api/nu3d/nudlist.h"

extern "C" VARIPTR *display_list_buffer = 0;
extern "C" {
static void NuDisplayListCheckBuffer(void) {
}

static nudisplaylist_s *NuDisplayListGet2dList(void) {
    return 0;
}

static void NuDisplayListResetBuffer(void) {
}
}

static void NuDisplayListSetID(nudisplaylistitem_s *, u8) {
}

static void NuDisplayListSetID_CNT(nudisplaylistitem_s *) {
}

static void NuDisplayListSetID_RET(nudisplaylistitem_s *) {
}

static void NuDisplayListSetID_CALL(nudisplaylistitem_s *) {
}

static void NuDisplayListSetID_NEXT(nudisplaylistitem_s *) {
}

static nudisplaylistitem_s *NuDisplayListAddItem(nudisplaylist_s *, u8, void *) {
    return 0;
}

static void NuDisplayListSetItem(nudisplaylistitem_s *, u8, u8, void *) {
}

static void NuDisplayListSetNext(nudisplaylistitem_s *, void *) {
}
