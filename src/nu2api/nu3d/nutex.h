#pragma once

#include <pthread.h>
#include <stddef.h>

#include "decomp.h"
#include "nu2api/nucore/common.h"

#ifdef ANDROID
#include "nu2api/nu3d/android/nutex_android.h"
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

enum NUTEXFORMAT : i32 {
    NUTEX_UNKNOWN = 0,
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

i32 NuDDSGetTextureDescription(const char *dds_data, NUTEXFORMAT &out_format, i32 &out_width, i32 &out_height,
                               i32 &out_depth, i32 &out_mip_count, bool &out_is_cube_map, bool *out_has_four_cc);
void GetNativeTextureFormat(NUTEXFORMAT inFormat, i32 &outBpp, u32 &outInternalFormat, u32 &outType, u32 &outFormat,
                            bool &outIsCompressed, NUTEXFORMAT &outFormatEnum);