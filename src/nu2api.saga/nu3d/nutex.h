#pragma once

#include <pthread.h>
#include <stddef.h>

#include "nu2api.saga/nucore/common.h"

#if 1
// Isolate platform-specific definitions to make it easier to add platform
// support in future.
#include "nu2api.saga/nu3d/nutex_android.h"
#endif

typedef enum nutextype_e {
    NUTEX_RTT24 = 15,
} NUTEXTYPE;

typedef struct nutex_s {
    NUTEXTYPE type;
    int width;
    int height;
} NUTEX;

typedef struct nunativetex_s {
    int width;
    int height;
    unsigned char checksum[16];
    int ref_count;
    void *image_data;
    u32 size;
    NUNATIVETEXPS platform;
} NUNATIVETEX;

#ifdef __cplusplus
extern "C" {
#endif
    extern pthread_mutex_t criticalSection;
    extern int max_textures;

    void NuTexInitEx(VARIPTR *buf, int max_tex_count);

    int NuTexRead(char *name, VARIPTR *buf, VARIPTR *buf_end);

    int NuTexCreate(NUTEX *tex);
    int NuTexCreateNative(NUNATIVETEX *tex, bool is_pvrtc);

    void NuTexDestroy(int tex_id);

    NUNATIVETEX *NuTexGetNative(int tex_id);

    void NuTexAddReference(int tex_id);
    void NuTexRemoveReference(int tex_id);
    int NuTexGetRefCount(int tex_id);

    int NuTexWidth(int tex_id);
    int NuTexHeight(int tex_id);

    void NuTexDisplayTexturePage(int page, float depth, int alpha);
#ifdef __cplusplus
}
#endif

void NuTexInitExPS(VARIPTR *buf);

void NuTexCreatePS(NUNATIVETEX *tex, bool is_pvrtc);
void NuTexDestroyPS(NUNATIVETEX *tex);

void NuChecksumAsHex(u8 *checksum, char *out);
void NuTexHiresFilename(int tex_id, char *filename);
int NuTexSwapHires(int tex_id_lo, int tex_id_hi);

void NuTexLoadHires(int tex_id);
void NuTexUnloadHires(int tex_id);

int NuTexGetReqSize(int tex_id, int level);

int NuTexReserve(int size);
void NuTexUnReserve();
