#include "nu2api/nu3d/nutex.h"

#include <pthread.h>
#include <string.h>
#include "decomp.h"
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"

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
    NUTEX_DX1A = 2,       // DXT1 with Alpha (A1XD)
    NUTEX_DXT2 = 3,
    NUTEX_DXT3 = 4,
    NUTEX_DXT4 = 5,
    NUTEX_DXT5 = 6,
    NUTEX_RGBA32 = 7,     // Calculated from (bVar3 * 8 + 7) for 32-bit
    NUTEX_FLOAT16 = 8,    // 0x71 is D3DFMT_A16B16G16R16F
    NUTEX_FLOAT32 = 9,    // 0x74 is D3DFMT_A32B32G32R32F
    NUTEX_PAL8 = 12,      // 0xC  (PAL8)
    NUTEX_PAL4 = 13,      // 0xD  (PAL4)
    NUTEX_BANN = 14,      // 0xE  (BANN - likely a custom banner format)
    NUTEX_RGB24 = 15,     // Calculated from (bVar3 * 8 + 7) for 24-bit
    NUTEX_ETC1 = 17,      // 0x11 (ETC1)
    NUTEX_ETCA = 18,      // 0x12 (ETC with Alpha)
    NUTEX_PVRTC2 = 20,    // 0x14 (PT21)
    NUTEX_PVRTC2A = 21,   // 0x15 (PT2A)
    NUTEX_PVRTC4 = 22,    // 0x16 (PTC1)
    NUTEX_PVRTC4A = 23,   // 0x17 (PTCA)
    NUTEX_ATCA = 24,      // 0x18 (ACTA)
    NUTEX_ATC = 25        // 0x19 (ATC)
};

// _Z26NuDDSGetTextureDescriptionPKcR11NUTEXFORMATRiS3_S3_S3_RbPb
/* NuDDSGetTextureDescription(char const*, NUTEXFORMAT&, int&, int&, int&, int&, bool&, bool*) */

i32 NuDDSGetTextureDescription(const char *dds_buffer,NUTEXFORMAT &param_2,int &param_3,int &param_4,int &param_5,int &param_6,bool &param_7,bool *is_compressed)

{
  i32 bitfield;
  i32 uVar2;
  bool bVar3;

  if (dds_buffer[0] != 'D' && dds_buffer[1] != 'D' && dds_buffer[2] != 'S') {

    return 0;
  }
  uVar2 = dds_buffer[0x54];
  *is_compressed = uVar2 != 0;
  bitfield = dds_buffer[0x50];
  if ((bitfield & 0x40) == 0) {
    if ((bitfield & 0x20) != 0) {
        param_2 = NUTEX_PAL8;
    }
    if ((bitfield & 8) != 0) {
        param_2 = NUTEX_PAL4;
    }
    if (uVar2 == '3TXD') {
        param_2 = NUTEX_DXT3;
    }
    if ('3TXD' < uVar2) {
        if (uVar2 == 'A1XD') {
            param_2 = NUTEX_DX1A;
        } 
        else if (uVar2 < 'A1XE') {
            if (uVar2 == '4TXD') {
            param_2 = NUTEX_DXT4;
            } 
            else if (uVar2 < '4TXE') {
                if (uVar2 == '4LAP') {
                    param_2 = NUTEX_PAL4;
                }
            }
            else if (uVar2 == '5TXD') {
                param_2 = NUTEX_DXT5;
            }
            else if (uVar2 == '8LAP') {
                param_2 = NUTEX_PAL8;
            }
        }
        else if (uVar2 == 'ACTE') {
                param_2 = NUTEX_ETCA;
            }
    }
    


}

  return 1;
}

