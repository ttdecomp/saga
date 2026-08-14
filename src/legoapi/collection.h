#pragma once

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
    float field_18;         /* 0x18 */
};

#ifdef __cplusplus
extern "C" {
#endif

    extern i32 COLLECTION_COMPLETIONCOUNT;

#ifdef __cplusplus
}
#endif

void Collection_Configure(char *file, VARIPTR *buf, VARIPTR *buf_end);
