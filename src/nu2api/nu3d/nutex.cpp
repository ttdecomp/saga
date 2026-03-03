#include "nu2api/nu3d/nutex.h"

#include "decomp.h"
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include <pthread.h>
#include <string.h>

void NuChecksumAsHex(u8 *checksum, char *out) {
    i32 i;
    char hex_digits[] = "0123456789abcdef";

    for (i = 0; i < 16; i++) {
        u8 check_digit = checksum[i];
        i32 least_sig_digit = check_digit >> 4;

        out[i * 2] = hex_digits[least_sig_digit];
        out[i * 2 + 1] = hex_digits[(u8)(check_digit - (least_sig_digit << 4))];
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

i32 max_textures;
static NUNATIVETEX **texture_list;
static i32 *texture_order;
static i32 gTextureLoadCount;

void NuTexInitEx(VARIPTR *buf, i32 max_tex_count) {
    max_textures = max_tex_count;

    texture_list = (NUNATIVETEX **)ALIGN(buf->addr, 0x4);
    buf->addr = (usize)texture_list + max_tex_count * sizeof(NUNATIVETEX *);
    memset(texture_list, 0, max_tex_count * sizeof(NUNATIVETEX *));

    texture_order = (i32 *)ALIGN(buf->addr, 0x4);
    buf->addr = (usize)texture_order + max_tex_count * sizeof(i32);
    memset(texture_order, 0, max_tex_count * sizeof(i32));

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

enum NUTEXFORMAT : unsigned int {
    NUTEX_DXT1 = 1,
    NUTEX_DX1A = 2, // DXT1 with Alpha (A1XD)
    NUTEX_DXT2 = 3,
    NUTEX_DXT3 = 4,
    NUTEX_DXT4 = 5,
    NUTEX_DXT5 = 6,
    NUTEX_RGBA32 = 7,   // Calculated from (bVar3 * 8 + 7) for 32-bit
    NUTEX_FLOAT16 = 8,  // 0x71 is D3DFMT_A16B16G16R16F
    NUTEX_FLOAT32 = 9,  // 0x74 is D3DFMT_A32B32G32R32F
    NUTEX_PAL8 = 12,    // 0xC  (PAL8)
    NUTEX_PAL4 = 13,    // 0xD  (PAL4)
    NUTEX_BANN = 14,    // 0xE  (BANN - likely a custom banner format)
    NUTEX_RGB24 = 15,   // Calculated from (bVar3 * 8 + 7) for 24-bit
    NUTEX_ETC1 = 17,    // 0x11 (ETC1)
    NUTEX_ETCA = 18,    // 0x12 (ETC with Alpha)
    NUTEX_PVRTC2 = 20,  // 0x14 (PT21)
    NUTEX_PVRTC2A = 21, // 0x15 (PT2A)
    NUTEX_PVRTC4 = 22,  // 0x16 (PTC1)
    NUTEX_PVRTC4A = 23, // 0x17 (PTCA)
    NUTEX_ATCA = 24,    // 0x18 (ACTA)
    NUTEX_ATC = 25      // 0x19 (ATC)
};


i32 NuDDSGetTextureDescription(const char *ddsData, NUTEXFORMAT &outFormat, int &outWidth, int &outHeight, int &outPitchOrLinearSize, int &outMipCount, bool &outIsCubeMap, bool *outHasFourCC)

{
    if (ddsData[0] != 'D')
        return 0;
    if (ddsData[1] != 'D')
        return 0;
    if (ddsData[2] != 'S')
        return 0;

    uint32_t fourCC = *(uint32_t *)(ddsData + 0x54);
    *outHasFourCC = (fourCC != 0);

    if ((ddsData[0x50] & 0x40) == 0) {
        if ((ddsData[0x50] & 0x20) != 0) {
            outFormat = NUTEX_PAL8;
        } else if ((ddsData[0x50] & 0x8) != 0) {
            outFormat = NUTEX_PAL4;
        } else {
            switch (fourCC) {
                case 0x31545844:
                    outFormat = NUTEX_DXT1;
                    break;
                case 0x41315844:
                    outFormat = NUTEX_DX1A;
                    break;
                case 0x32545844:
                    outFormat = NUTEX_DXT2;
                    break;
                case 0x33545844:
                    outFormat = NUTEX_DXT3;
                    break;
                case 0x34545844:
                    outFormat = NUTEX_DXT4;
                    break;
                case 0x35545844:
                    outFormat = NUTEX_DXT5;
                    break;
                case 0x00000071:
                    outFormat = NUTEX_FLOAT16;
                    break;
                case 0x00000074:
                    outFormat = NUTEX_FLOAT32;
                    break;
                case 0x344c4150:
                    outFormat = NUTEX_PAL4;
                    break;
                case 0x384c4150:
                    outFormat = NUTEX_PAL8;
                    break;
                case 0x4e4e4142:
                    outFormat = NUTEX_BANN;
                    break;
                case 0x31435445:
                    outFormat = NUTEX_ETC1;
                    break;
                case 0x41435445:
                    outFormat = NUTEX_ETCA;
                    break;
                case 0x31325450:
                    outFormat = NUTEX_PVRTC2;
                    break;
                case 0x41325450:
                    outFormat = NUTEX_PVRTC2A;
                    break;
                case 0x31435450:
                    outFormat = NUTEX_PVRTC4;
                    break;
                case 0x41435450:
                    outFormat = NUTEX_PVRTC4A;
                    break;
                case 0x41435441:
                    outFormat = NUTEX_ATCA;
                    break;
                case 0x20435441:
                    outFormat = NUTEX_ATC;
                    break;
                case 0:
                    outFormat = (NUTEXFORMAT)((*(uint32_t *)(ddsData + 0x58) != 0x20) * 8 + 7);
                    break;
                default:
                    break;
            }
        }
    } else {
        outFormat = (NUTEXFORMAT)((*(uint32_t *)(ddsData + 0x58) == 0x18) * 8 + 7);
    }

    outIsCubeMap = false;
    outWidth = 0;
    outHeight = 0;
    outPitchOrLinearSize = 0;

    outWidth = *(int *)(ddsData + 0x10);
    outHeight = *(int *)(ddsData + 0x0C);

    if ((ddsData[10] & 0x80) != 0) {
        outPitchOrLinearSize = *(int *)(ddsData + 0x18);
    }

    uint32_t mipCountVal = *(uint32_t *)(ddsData + 0x1C);
    uint32_t caps2 = *(uint32_t *)(ddsData + 0x70);

    if (mipCountVal == 0) {
        mipCountVal = (ddsData[10] & 2) == 0;
    }
    outMipCount = mipCountVal;

    if ((ddsData[0x6C] & 8) == 0)
        return 1;
    if ((caps2 & 0x200) == 0)
        return 1;
    if ((caps2 & 0x1000) == 0)
        return 1;
    if ((caps2 & 0x400) == 0)
        return 1;
    if ((caps2 & 0x800) == 0)
        return 1;
    if ((caps2 & 0x4000) == 0)
        return 1;
    if ((caps2 & 0x8000) == 0)
        return 1;
    if ((caps2 & 0x2000) == 0)
        return 1;

    outIsCubeMap = true;
    return 1;
}