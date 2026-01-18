#pragma once

#include "decomp.h"

#include "nu2api.saga/nucore/common.h"

typedef struct nufilepakhdrv0_s {
    unsigned int type;
    unsigned int item_count;
    unsigned int file_size;
} NUFILEPAKHDR_V0;

typedef struct nufilepakhdrv1_s {
    unsigned int type;
    unsigned int item_count;
    unsigned int file_size;
    unsigned int checksum;
    unsigned int flags;
} NUFILEPAKHDR_V1;

typedef struct nufilepakhdrv2_s {
    unsigned int type;
    unsigned int item_count;
    unsigned int file_size;
    unsigned int checksum;
    unsigned int flags;
    unsigned int decrypted_checksum;
} NUFILEPAKHDR;

typedef struct nufilepakitemattr_s {
    unsigned int required : 1;
    unsigned int removed : 1;
} NUFILEPAKITEMATTR;

// There are no known uses of functions of this type, so their parameters are
// unknown.
typedef int NuFilePakMemMoveFn();

typedef struct nufilepakitem_s {
    int name_offset;
    int data_offset;
    int size;
    int alignment;
    NUFILEPAKITEMATTR attr;
    NuFilePakMemMoveFn *pre_move_fn;
    NuFilePakMemMoveFn *post_move_fn;
} NUFILEPAKITEM;

typedef enum {
    NUFILEPAK_ERROR_NONE,
    NUFILEPAK_ERROR_FILENOTFOUND,
    NUFILEPAK_ERROR_FILEIO,
    NUFILEPAK_ERROR_CHECKSUM,
    NUFILEPAK_ERROR_UNSUPPORTEDTYPE,
    NUFILEPAK_ERROR_ALIGNMENT,
    NUFILEPAK_ERROR_KEYMISMATCH,
    NUFILEPAK_ERROR_CNT,
} NUFILEPAK_ERROR;

C_API_START

void *NuFilePakLoad(char *filepath, VARIPTR *buf, VARIPTR buf_end, int alignment);
void *NuFilePakLoadKey(char *filepath, VARIPTR *buf, VARIPTR buf_end, int alignment, unsigned char *key,
                       unsigned int key_len);

int NuFilePakGetItem(void *hdr, char *item_name);
int NuFilePakGetItemInfo(void *hdr, int item_handle, void **addr, int *size);

C_API_END
