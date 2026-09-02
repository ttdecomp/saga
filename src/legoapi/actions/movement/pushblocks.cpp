#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

enum PushBlockCompletionFlags {
    PUSH_BLOCK_FIRST_OUTPUT_FLAG = 1 << 3,
    PUSH_BLOCK_ANY_OUTPUT_MASK = 0x7f8,
};

void KnockPushBlock(pushblock_s *, nuvec_s *) {
}

void NewBlockAction(GameObject_s *) {
}

void NearestPushBlock(WORLDINFO_s *, nuvec_s *, float) {
}

void PushSeekComplete(pushblock_s *, i32) {
}

void OtherBlockInRange(WORLDINFO_s *, pushblock_s *, nuvec_s *, i32) {
}

void ResetSinglePushBlock(WORLDINFO_s *, pushblock_s *, i32) {
}

void NearestFacingPushBlock(WORLDINFO_s *, GameObject_s *, float) {
}

void GizmoPushBlockInitAndReset(WORLDINFO_s *, void *) {
}

void ResetSinglePushBlockHeight(WORLDINFO_s *, pushblock_s *, i32) {
}

i32 GizPushBlock_EndFrameCompleted(pushblock_s *push_block, i32 output_index) {
    if (push_block == NULL) {
        return -1;
    }

    if (output_index == 0) {
        return (push_block->completion_flags & PUSH_BLOCK_ANY_OUTPUT_MASK) != 0;
    }
    const u8 completed_outputs = push_block->completion_flags / PUSH_BLOCK_FIRST_OUTPUT_FLAG;
    return (completed_outputs >> output_index) & 1;
}

void PushBlock(GameObject_s *) {
}
