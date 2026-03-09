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



enum DDSCAPS : u32 {
    DDSCAPS2_CUBEMAP = 0x200, // Required for a cubemap
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000
};

struct __attribute__((packed)) dds_pixelformat_s {
    u32 dw_size;
    u32 dw_flags;
    u32 dw_four_cc;
    u32 dw_rgb_bit_count;
    u32 dw_r_bit_mask;
    u32 dw_g_bit_mask;
    u32 dw_b_bit_mask;
    u32 dw_a_bit_mask;
};

struct __attribute__((packed)) dds_header_s {
    char magic[4];
    u32 dw_size;
    u32 dw_flags;
    u32 dw_height;
    u32 dw_width;
    u32 dw_pitch_or_linear_size;
    u32 dw_depth;
    u32 dw_mip_map_count;
    u32 dw_reserved1[11];
    struct dds_pixelformat_s ddspf;
    u32 dw_caps;
    u32 dw_caps2;
    u32 dw_caps3;
    u32 dw_caps4;
    u32 dw_reserved2;
};

i32 NuDDSGetTextureDescription(const char *dds_data, NUTEXFORMAT &out_format, i32 &out_width, i32 &out_height,
                               i32 &out_depth, i32 &out_mip_count, bool &out_is_cube_map, bool *out_has_four_cc)

{
    dds_header_s *header = (dds_header_s *)dds_data;

    if (header->magic[0] != 'D')
        return 0;
    if (header->magic[1] != 'D')
        return 0;
    if (header->magic[2] != 'S')
        return 0;

    u32 four_cc = header->ddspf.dw_four_cc;
    *out_has_four_cc = (four_cc != 0);

    u32 flags = header->ddspf.dw_flags;

    if ((flags & 0x40) != 0) {
        out_format = (NUTEXFORMAT)((header->ddspf.dw_four_cc == 0x18) * 8 + 7);
    } else if ((flags & 0x20) != 0) {
        out_format = NUTEX_PAL8;
    } else if ((flags & 0x8) != 0) {
        out_format = NUTEX_PAL4;
    } else {
        switch (four_cc) {
            case 0x31545844:
                out_format = NUTEX_DXT1;
                break;
            case 0x41315844:
                out_format = NUTEX_DX1A;
                break;
            case 0x32545844:
                out_format = NUTEX_DXT2;
                break;
            case 0x33545844:
                out_format = NUTEX_DXT3;
                break;
            case 0x34545844:
                out_format = NUTEX_DXT4;
                break;
            case 0x35545844:
                out_format = NUTEX_DXT5;
                break;
            case 0x00000071:
                out_format = NUTEX_FLOAT16;
                break;
            case 0x00000074:
                out_format = NUTEX_FLOAT32;
                break;
            case 0x344c4150:
                out_format = NUTEX_PAL4;
                break;
            case 0x384c4150:
                out_format = NUTEX_PAL8;
                break;
            case 0x4e4e4142:
                out_format = NUTEX_BANN;
                break;
            case 0x31435445:
                out_format = NUTEX_ETC1;
                break;
            case 0x41435445:
                out_format = NUTEX_ETCA;
                break;
            case 0x31325450:
                out_format = NUTEX_PVRTC2;
                break;
            case 0x41325450:
                out_format = NUTEX_PVRTC2A;
                break;
            case 0x31435450:
                out_format = NUTEX_PVRTC4;
                break;
            case 0x41435450:
                out_format = NUTEX_PVRTC4A;
                break;
            case 0x41435441:
                out_format = NUTEX_ATCA;
                break;
            case 0x20435441:
                out_format = NUTEX_ATC;
                break;
            case 0:
                out_format = (NUTEXFORMAT)((header->ddspf.dw_rgb_bit_count != 0x20) * 8 + 7);
                break;
            default:
                break;
        }
    }

    out_is_cube_map = false;
    out_width = 0;
    out_height = 0;
    out_depth = 0;

    out_width = header->dw_width;
    out_height = header->dw_height;

    if ((header->dw_flags & 0x800000) != 0) {
        out_depth = header->dw_depth;
    }

    u32 mipmap_count = header->dw_mip_map_count;
    if (mipmap_count == 0) {
        mipmap_count = header->dw_flags & 0x20000 ? 1 : 0;
    }
    out_mip_count = mipmap_count;

    if ((header->dw_caps & 8) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP_POSITIVEY) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP_POSITIVEX) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ) == 0)
        return 1;
    if ((header->dw_caps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) == 0)
        return 1;

    out_is_cube_map = true;
    return 1;
}
