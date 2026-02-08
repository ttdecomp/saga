#include <pthread.h>
#include <string.h>

#include "nu2api.saga/nu3d/nutex.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"

void NuChecksumAsHex(u8 *checksum, char *out) {
    char hex_digits[] = "0123456789abcdef";

    for (int i = 0; i < 16; i++) {
        u8 check_digit = checksum[i];
        i32 least_sig_digit = check_digit >> 4;

        out[i * 2] = hex_digits[least_sig_digit];
        out[i * 2 + 1] = hex_digits[(check_digit - (least_sig_digit << 4)) & 0xff];
    }

    out[32] = '\0';
}

void NuTexHiresFilename(int tex_id, char *filename) {
    NUNATIVETEX *tex;
    char checksum_hex[33];

    tex = NuTexGetNative(tex_id);

    NuStrCpy(filename, "c:\\temp\\stream\\textures\\");
    NuChecksumAsHex(tex->checksum, checksum_hex);
    NuStrCat(filename, checksum_hex);
    NuStrCat(filename, ".tex");
}

int NuTexSwapHires(int tex_id_lo, int tex_id_hi) {
    return 0;
}

void NuTexLoadHires(int tex_id) {
    char hires_path[2048];
    int tex_id_hi;

    NuTexHiresFilename(tex_id, hires_path);
    tex_id_hi = NuTexRead(hires_path, NULL, NULL);
    NuTexSwapHires(tex_id, tex_id_hi);
}

void NuTexUnloadHires(int tex_id) {
}

void NuTexAddReference(int tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        tex->ref_count++;
    }
}

void NuTexRemoveReference(int tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        tex->ref_count--;
    }
}

int NuTexGetRefCount(int tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        return tex->ref_count;
    }

    return 0;
}

int max_textures;
static NUNATIVETEX **texture_list;
static int *texture_order;
static int gTextureLoadCount;

void NuTexInitEx(VARIPTR *buf, int max_tex_count) {
    max_textures = max_tex_count;

    texture_list = (NUNATIVETEX **)ALIGN(buf->addr, 0x4);
    buf->void_ptr = (void *)(texture_list + max_tex_count);
    memset(texture_list, 0, max_tex_count * sizeof(NUNATIVETEX *));

    texture_order = (int *)ALIGN(buf->addr, 0x4);
    buf->void_ptr = (void *)(texture_order + max_tex_count);
    memset(texture_order, 0, max_tex_count * sizeof(int));

    gTextureLoadCount = 0;
}

pthread_mutex_t criticalSection = PTHREAD_MUTEX_INITIALIZER;

int NuTexCreateNative(NUNATIVETEX *tex, bool is_pvrtc) {
    int i;

    if (tex == NULL) {
        return 0;
    }

    pthread_mutex_lock(&criticalSection);

    for (int i = 0; i < max_textures; i++) {
        if (texture_list[i] == NULL) {
            texture_list[i] = tex;
            texture_order[i] = gTextureLoadCount++;

            pthread_mutex_unlock(&criticalSection);

            NuTexCreatePS(tex, is_pvrtc);

            return i + 1;
        }
    }

    pthread_mutex_unlock(&criticalSection);

    return 0;
}

NUNATIVETEX *NuTexGetNative(int tex_id) {
    if (tex_id > 0) {
        return texture_list[tex_id - 1];
    }

    return NULL;
}

int NuTexWidth(int tex_id) {
    return texture_list[tex_id - 1]->width;
}

int NuTexHeight(int tex_id) {
    return texture_list[tex_id - 1]->height;
}
