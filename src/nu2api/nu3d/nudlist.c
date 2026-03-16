#include "nu2api/nu3d/nudlist.h"
#include "decomp.h"
VARIPTR *display_list_buffer;

void NuDisplayListAnimateMtls(float param_1) {
    UNIMPLEMENTED();
};

void NuDisplayListReset(NUDISPLAYLIST *list) {
    NUDISPLAYLIST_STATE *state;
    state = list->state;
    state->unk08 = 0;
    state->unk14 = 0xffffffff;
    state->unk20 = 0xffffffff;
    state->unk24 = 0xffff;
    state->unk2C = 0xffff;
    state->unk2E = 0xffff;
    state->unk1E = 0xffff;
    state->unk32 = 0xffff;
    list->current = list->head;
}