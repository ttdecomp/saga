#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nugscn.h"

#include "decomp.h"
#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include <pthread.h>
#include <string.h>

struct nutexanimprog_s;
struct nutextureformat_e {};
struct nutexanim_s;

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

void NuTexHiresFilename(i32 tex_id, char *filename) {
    NUNATIVETEX *tex;
    char checksum_hex[33];

    tex = NuTexGetNative(tex_id);

    NuStrCpy(filename, "c:\\temp\\stream\\textures\\");
    NuChecksumAsHex(tex->checksum, checksum_hex);
    NuStrCat(filename, checksum_hex);
    NuStrCat(filename, ".tex");
}

i32 NuTexSwapHires(i32 tex_id_lo, i32 tex_id_hi) {
    return 0;
}

void NuTexLoadHires(i32 tex_id) {
    char hires_path[2048];
    i32 tex_id_hi;

    NuTexHiresFilename(tex_id, hires_path);
    tex_id_hi = NuTexRead(hires_path, NULL, NULL);
    NuTexSwapHires(tex_id, tex_id_hi);
}

void NuTexUnloadHires(i32 tex_id) {
}

void NuTexAddReference(i32 tex_id, NUGSCN *) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        tex->ref_count++;
    }
}

void NuTexRemoveReference(i32 tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        tex->ref_count--;
    }
}

i32 NuTexGetRefCount(i32 tex_id) {
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

i32 NuTexCreateNative(NUNATIVETEX *tex, bool is_pvrtc) {
    i32 i;

    if (tex == NULL) {
        return 0;
    }

    pthread_mutex_lock(&criticalSection);

    for (i32 i = 0; i < max_textures; i++) {
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

NUNATIVETEX *NuTexGetNative(i32 tex_id) {
    if (tex_id > 0) {
        return texture_list[tex_id - 1];
    }

    return NULL;
}

extern "C" i32 NuTexResolveReference(NUGSCN *scene, i32 tex_id) {
    if ((tex_id & 0x4000) == 0) {
        return scene->texture_ids[tex_id];
    }
    if (max_textures == 0) {
        return 0;
    }

    const i32 reference_index = tex_id & 0x3fff;
    NUNATIVETEX *reference = scene->textures[reference_index];
    i32 newest_texture_id = 0;
    i32 newest_texture_order = 0;

    for (i32 index = 0; index < max_textures; ++index) {
        NUNATIVETEX *texture = texture_list[index];
        if (texture == NULL || texture->ref_count < 0) {
            continue;
        }

        bool checksum_matches = true;
        for (i32 byte = 0; byte < 16; ++byte) {
            if (texture->checksum[byte] != reference->checksum[byte]) {
                checksum_matches = false;
                break;
            }
        }
        if (checksum_matches && static_cast<u32>(texture_order[index]) > static_cast<u32>(newest_texture_order)) {
            newest_texture_id = index + 1;
            newest_texture_order = texture_order[index];
        }
    }

    if (newest_texture_id != 0) {
        NuTexAddReference(newest_texture_id, scene);
        scene->texture_ids[reference_index] = newest_texture_id;
    }
    return newest_texture_id;
}

i32 NuTexWidth(i32 tex_id) {
    return texture_list[tex_id - 1]->width;
}

i32 NuTexHeight(i32 tex_id) {
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
        mipmap_count = (header->dw_flags & 0x20000) == 0;
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

void NuTexRemap(i32, i32) {
}

void NuTexReadTex() {
}

void NuTexAssignAddr(i32, i32) {
}

void NuTexGetManager() {
}

void NuTexReadBitmap(char *) {
}

void NuTexManagerInit(variptr_u *, variptr_u) {
}

void NuTexAnimProgInit(nutexanimprog_s *) {
}

void NuTextureCreate3D(i32, i32, i32, i32, i32, nutextureformat_e) {
}

void NuTexAnimResetList(nutexanim_s *) {
}

void NuTexManagerStream(nugscn_s *) {
}

void NuTexAnimProgParseFile(i32, variptr_u *, variptr_u, i32) {
}

i32 NuTexGetUnresolvedTextureTIDPS() {
    return 0;
}
