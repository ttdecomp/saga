#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"

struct COLLECTID;

struct COLLECTION_s {
    struct COLLECTID *list; /* 0x0 */
    u16 count_x;            /* 0x4 */
    u16 count_y;            /* 0x6 */
    void *field_8;          /* 0x8 */
    void *field_c;          /* 0xc */
    float field_10;         /* 0x10 */
    float field_14;         /* 0x14 */
};
DECOMP_ASSERT(sizeof(COLLECTION_s) == 0x18, "COLLECTION_s ABI");

#ifdef __cplusplus
extern "C" {
#endif

    extern i32 COLLECTION_COMPLETIONCOUNT;

#ifdef __cplusplus
}
#endif

void Collection_Configure(char *file, VARIPTR *buf, VARIPTR *buf_end);
i32 InCollectList_Index(i32 id, COLLECTID *list, i32 count);
i32 Collection_Got(i32 id);
i32 Collection_GetIDList(COLLECTION_s *collection, u32 model_flag_mask, u32 required_model_flags, i16 *ids,
                         i32 *first_id, i32 *second_id, i32 unused);
COLLECTION_s *GetFreePlayCollection(i32 area);
