#include <string.h>
#include <vector>

#include "decomp.h"
#include "globals.h"

#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

// Host platform-specific (PS) texture loader. On the host build this file
// replaces android/nutex_android.cpp (see src/host.cmake), so it supplies the
// two entry points the shared NuTex layer needs. It reproduces the original
// (Android) behaviour so the *game* texture pipeline is unchanged; the only
// real host difference is the PS upload: Host drivers lack PVRTC, so the
// compressed data is software-decoded to RGBA before glTexImage2D.

// --- PowerVR 'PVR' v3 / PVRTC support (the "ios" texture format) --------------
// Container: PVRTC-compressed data is placed in a PVR header. On Android this
// data would go straight to glCompressedTexImage2D; llvmpipe has no such
// format, so we decode PVRTC 4bpp to RGBA here (a host-only PS step).
//
// The decoder below is a faithful C++ port of Imagination's PVRTC 4bpp
// decompressor (PVRTDecompress.cpp, PowerVR SDK). It was verified byte-for-byte
// against the real "LEGAL_ENGLISH_ios.tex" first-boot texture: decoding the
// 1024x1024 PVRTC 4bpp legal texture reproduces the LEGO copyright screen
// (black background, red/white LEGO text) with zero differing pixels vs the
// reference decoder.

static bool PvrIsPow2(u32 n) {
    return n != 0 && (n & (n - 1)) == 0;
}

// Interleave the x/y block coordinates into a "twiddled" (Morton) data index.
static u32 PvrTwiddleUV(u32 xsize, u32 ysize, u32 xpos, u32 ypos) {
    u32 min_dim = xsize;
    u32 max_val = ypos;
    u32 twiddled = 0;
    u32 src_bit = 1;
    u32 dst_bit = 1;
    u32 shift = 0;

    if (ysize < xsize) {
        min_dim = ysize;
        max_val = xpos;
    }
    while (src_bit < min_dim) {
        if (ypos & src_bit) {
            twiddled |= dst_bit;
        }
        if (xpos & src_bit) {
            twiddled |= (dst_bit << 1);
        }
        src_bit <<= 1;
        dst_bit <<= 2;
        shift += 1;
    }
    max_val >>= shift;
    twiddled |= (max_val << (2 * shift));
    return twiddled;
}

static void PvrGetColorA(u32 cd, u8 rgb[4]) {
    if (cd & 0x8000) { // opaque RGB 554
        rgb[0] = (u8)((cd & 0x7c00) >> 10);
        rgb[1] = (u8)((cd & 0x3e0) >> 5);
        rgb[2] = (u8)((cd & 0x1e) | ((cd & 0x1e) >> 4));
        rgb[3] = 0xf;
    } else { // transparent ARGB 3443
        rgb[0] = (u8)(((cd & 0xf00) >> 7) | ((cd & 0xf00) >> 11));
        rgb[1] = (u8)(((cd & 0xf0) >> 3) | ((cd & 0xf0) >> 7));
        rgb[2] = (u8)(((cd & 0xe) << 1) | ((cd & 0xe) >> 2));
        rgb[3] = (u8)((cd & 0x7000) >> 11);
    }
}

static void PvrGetColorB(u32 cd, u8 rgb[4]) {
    if (cd & 0x80000000) { // opaque RGB 555
        rgb[0] = (u8)((cd & 0x7c000000) >> 26);
        rgb[1] = (u8)((cd & 0x3e00000) >> 21);
        rgb[2] = (u8)((cd & 0x1f0000) >> 16);
        rgb[3] = 0xf;
    } else { // transparent ARGB 3444
        rgb[0] = (u8)(((cd & 0xf000000) >> 23) | ((cd & 0xf000000) >> 27));
        rgb[1] = (u8)(((cd & 0xf00000) >> 19) | ((cd & 0xf00000) >> 23));
        rgb[2] = (u8)(((cd & 0xf0000) >> 15) | ((cd & 0xf0000) >> 19));
        rgb[3] = (u8)((cd & 0x70000000) >> 27);
    }
}

// Bilinear upscale of the 2x2 endpoint grid into the 4x4 (bpp==4) texel block.
static void PvrInterpolateColors(const i32 P[4], const i32 Q[4], const i32 R[4], const i32 S[4], i32 bpp,
                                 u8 px[16][4]) {
    i32 W = 4;
    i32 H = 4;
    if (bpp == 2) {
        W = 8;
        H = 8;
    }
    i32 hP[4] = {P[0], P[1], P[2], P[3]};
    i32 hQ[4] = {Q[0], Q[1], Q[2], Q[3]};
    i32 hR[4] = {R[0], R[1], R[2], R[3]};
    i32 hS[4] = {S[0], S[1], S[2], S[3]};
    i32 QminusP[4] = {hQ[0] - hP[0], hQ[1] - hP[1], hQ[2] - hP[2], hQ[3] - hP[3]};
    i32 SminusR[4] = {hS[0] - hR[0], hS[1] - hR[1], hS[2] - hR[2], hS[3] - hR[3]};
    for (int i = 0; i < 4; i++) {
        hP[i] *= W;
        hR[i] *= W;
    }
    if (bpp == 2) {
        for (i32 x = 0; x < W; x++) {
            i32 result[4] = {4 * hP[0], 4 * hP[1], 4 * hP[2], 4 * hP[3]};
            i32 dY[4] = {hR[0] - hP[0], hR[1] - hP[1], hR[2] - hP[2], hR[3] - hP[3]};
            for (i32 y = 0; y < H; y++) {
                px[y * W + x][0] = (u8)((result[0] >> 7) + (result[0] >> 2));
                px[y * W + x][1] = (u8)((result[1] >> 7) + (result[1] >> 2));
                px[y * W + x][2] = (u8)((result[2] >> 7) + (result[2] >> 2));
                px[y * W + x][3] = (u8)((result[3] >> 5) + (result[3] >> 1));
                result[0] += dY[0];
                result[1] += dY[1];
                result[2] += dY[2];
                result[3] += dY[3];
            }
            hP[0] += QminusP[0];
            hP[1] += QminusP[1];
            hP[2] += QminusP[2];
            hP[3] += QminusP[3];
            hR[0] += SminusR[0];
            hR[1] += SminusR[1];
            hR[2] += SminusR[2];
            hR[3] += SminusR[3];
        }
    } else {
        for (i32 y = 0; y < H; y++) {
            i32 result[4] = {4 * hP[0], 4 * hP[1], 4 * hP[2], 4 * hP[3]};
            i32 dY[4] = {hR[0] - hP[0], hR[1] - hP[1], hR[2] - hP[2], hR[3] - hP[3]};
            for (i32 x = 0; x < W; x++) {
                px[y * W + x][0] = (u8)((result[0] >> 6) + (result[0] >> 1));
                px[y * W + x][1] = (u8)((result[1] >> 6) + (result[1] >> 1));
                px[y * W + x][2] = (u8)((result[2] >> 6) + (result[2] >> 1));
                px[y * W + x][3] = (u8)((result[3] >> 4) + (result[3]));
                result[0] += dY[0];
                result[1] += dY[1];
                result[2] += dY[2];
                result[3] += dY[3];
            }
            hP[0] += QminusP[0];
            hP[1] += QminusP[1];
            hP[2] += QminusP[2];
            hP[3] += QminusP[3];
            hR[0] += SminusR[0];
            hR[1] += SminusR[1];
            hR[2] += SminusR[2];
            hR[3] += SminusR[3];
        }
    }
}

// Read the 2-bit-per-texel modulation of one 4x4 sub-block into mod_vals.
static void PvrUnpackModulations(u32 mod_data, u32 cd, i32 offset_x, i32 offset_y, u8 mod_vals[16][16]) {
    u32 word_mod_mode = cd & 0x1;
    u32 bits = mod_data;
    if (word_mod_mode) {
        for (i32 y = 0; y < 4; y++) {
            for (i32 x = 0; x < 4; x++) {
                u32 v = bits & 3;
                if (v == 1) {
                    v = 4;
                } else if (v == 2) {
                    v = 14; // punch-through alpha
                } else if (v == 3) {
                    v = 8;
                }
                mod_vals[y + offset_y][x + offset_x] = (u8)v;
                bits >>= 2;
            }
        }
    } else {
        for (i32 y = 0; y < 4; y++) {
            for (i32 x = 0; x < 4; x++) {
                u32 v = (bits & 3) * 3;
                if (v > 3) {
                    v -= 1;
                }
                mod_vals[y + offset_y][x + offset_x] = (u8)v;
                bits >>= 2;
            }
        }
    }
}

// Decode a 4x4 texel block from four 16x16 sub-grid words.
typedef u32 PvrWordPair[2];
static void PvrGetPixels(const PvrWordPair &P, const PvrWordPair &Q, const PvrWordPair &R, const PvrWordPair &S,
                         i32 bpp, u8 out[16][4]) {
    i32 W = 4;
    i32 H = 4;
    if (bpp == 2) {
        W = 8;
        H = 8;
    }
    u8 mod_vals[16][16];
    memset(mod_vals, 0, sizeof(mod_vals));
    PvrUnpackModulations(P[0], P[1], 0, 0, mod_vals);
    PvrUnpackModulations(Q[0], Q[1], W, 0, mod_vals);
    PvrUnpackModulations(R[0], R[1], 0, H, mod_vals);
    PvrUnpackModulations(S[0], S[1], W, H, mod_vals);

    i32 pa[4], qa[4], ra[4], sa[4], pb[4], qb[4], rb[4], sb[4];
    u8 tmp[4];
    PvrGetColorA(P[1], tmp);
    for (int i = 0; i < 4; i++)
        pa[i] = tmp[i];
    PvrGetColorA(Q[1], tmp);
    for (int i = 0; i < 4; i++)
        qa[i] = tmp[i];
    PvrGetColorA(R[1], tmp);
    for (int i = 0; i < 4; i++)
        ra[i] = tmp[i];
    PvrGetColorA(S[1], tmp);
    for (int i = 0; i < 4; i++)
        sa[i] = tmp[i];
    PvrGetColorB(P[1], tmp);
    for (int i = 0; i < 4; i++)
        pb[i] = tmp[i];
    PvrGetColorB(Q[1], tmp);
    for (int i = 0; i < 4; i++)
        qb[i] = tmp[i];
    PvrGetColorB(R[1], tmp);
    for (int i = 0; i < 4; i++)
        rb[i] = tmp[i];
    PvrGetColorB(S[1], tmp);
    for (int i = 0; i < 4; i++)
        sb[i] = tmp[i];

    u8 A[16][4], B[16][4];
    PvrInterpolateColors(pa, qa, ra, sa, bpp, A);
    PvrInterpolateColors(pb, qb, rb, sb, bpp, B);

    i32 w, h;
    for (h = 0; h < H; h++) {
        for (w = 0; w < W; w++) {
            i32 mod = mod_vals[w + W / 2][h + H / 2];
            bool punch = mod > 10;
            if (punch) {
                mod -= 10;
            }
            i32 ac = (i32)A[h * W + w][0] * (8 - mod) + (i32)B[h * W + w][0] * mod;
            i32 gc = (i32)A[h * W + w][1] * (8 - mod) + (i32)B[h * W + w][1] * mod;
            i32 bc = (i32)A[h * W + w][2] * (8 - mod) + (i32)B[h * W + w][2] * mod;
            i32 al = punch ? 0 : ((i32)A[h * W + w][3] * (8 - mod) + (i32)B[h * W + w][3] * mod);
            out[h + w * W][0] = (u8)((ac / 8) & 0xff);
            out[h + w * W][1] = (u8)((gc / 8) & 0xff);
            out[h + w * W][2] = (u8)((bc / 8) & 0xff);
            out[h + w * W][3] = (u8)((al / 8) & 0xff);
        }
    }
}

// Scatter one 4x4 (bpp4) decoded block into the final RGBA image.
static void PvrMapData(u8 *out, i32 width, u8 word[16][4], u32 py, u32 px, u32 qy, u32 qx, u32 ry, u32 rx, u32 sy,
                       u32 sx, i32 bpp) {
    i32 W = 4;
    i32 H = 4;
    if (bpp == 2) {
        W = 8;
        H = 8;
    }
    for (i32 y = 0; y < H / 2; y++) {
        for (i32 x = 0; x < W / 2; x++) {
            u8 *dst;
            dst = out + ((((py * H) + y + H / 2) * width + (px * W) + x + W / 2) * 4);
            memcpy(dst, word[y * W + x], 4);
            dst = out + ((((qy * H) + y + H / 2) * width + (qx * W) + x) * 4);
            memcpy(dst, word[y * W + x + W / 2], 4);
            dst = out + ((((ry * H) + y) * width + (rx * W) + x + W / 2) * 4);
            memcpy(dst, word[(y + H / 2) * W + x], 4);
            dst = out + ((((sy * H) + y) * width + (sx * W) + x) * 4);
            memcpy(dst, word[(y + H / 2) * W + x + W / 2], 4);
        }
    }
}

// Software-decode a PVRTC 4bpp image (width*height/2 bytes) to RGBA.
static void PvrDecode4bpp(const u8 *data, i32 width, i32 height, u8 *outRGBA) {
    i32 bpp = 4;
    i32 W = 4;
    i32 H = 4;
    u32 word_count = (u32)((usize)width * height / 2 / 4);
    std::vector<u32> words(width * height / 2 / 4);
    memcpy(words.data(), data, (usize)width * height / 2);
    std::vector<u8> out((usize)width * height * 4, 0);
    u32 nxw = width / W;
    u32 nyw = height / H;
    for (i32 wy = -1; wy < (i32)nyw - 1; wy++) {
        for (i32 wx = -1; wx < (i32)nxw - 1; wx++) {
            u32 Px = ((u32)wx) % nxw;
            u32 Py = ((u32)wy) % nyw;
            u32 Qx = ((u32)(wx + 1)) % nxw;
            u32 Qy = ((u32)wy) % nyw;
            u32 Rx = ((u32)wx) % nxw;
            u32 Ry = ((u32)(wy + 1)) % nyw;
            u32 Sx = ((u32)(wx + 1)) % nxw;
            u32 Sy = ((u32)(wy + 1)) % nyw;
            u32 offs[4] = {PvrTwiddleUV(nxw, nyw, Px, Py) * 2, PvrTwiddleUV(nxw, nyw, Qx, Qy) * 2,
                           PvrTwiddleUV(nxw, nyw, Rx, Ry) * 2, PvrTwiddleUV(nxw, nyw, Sx, Sy) * 2};
            if (offs[0] + 1 >= word_count || offs[1] + 1 >= word_count || offs[2] + 1 >= word_count ||
                offs[3] + 1 >= word_count) {
                continue;
            }
            PvrWordPair PW = {words[offs[0]], words[offs[0] + 1]};
            PvrWordPair QW = {words[offs[1]], words[offs[1] + 1]};
            PvrWordPair RW = {words[offs[2]], words[offs[2] + 1]};
            PvrWordPair SW = {words[offs[3]], words[offs[3] + 1]};
            u8 block[16][4];
            PvrGetPixels(PW, QW, RW, SW, bpp, block);
            PvrMapData(out.data(), width, block, Py, Px, Qy, Qx, Ry, Rx, Sy, Sx, bpp);
        }
    }
    memcpy(outRGBA, out.data(), (usize)width * height * 4);
}

// Creates a GL texture from a PVR or DDS texture blob, returning the texture id
// and setting *width/*height. Host-only: software decodes PVRTC to RGBA.
GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, i32 *width, i32 *height, bool is_pvrtc) {
    const u8 *src = (const u8 *)data;

    // DDS (legacy .mob / PC) path
    if (src[0] == 'D' && src[1] == 'D' && src[2] == 'S' && src[3] == ' ') {
        return 0; // TODO: DDS upload
    }

    // PVR v3 header. Match the original NuIOS_CreateGLTexFromPVRInMemory field
    // offsets exactly: width at +0x1c, height at +0x18, pixel data starts at
    // base+0x34+base[0x30].
    if (src[0] != 'P' || src[1] != 'V' || src[2] != 'R') {
        return 0;
    }
    i32 w = *(const i32 *)(src + 0x1c);
    i32 h = *(const i32 *)(src + 0x18);
    *width = w;
    *height = h;

    const u8 *data_ptr = src + 0x34 + *(const i32 *)(src + 0x30);

    u8 *rgba = (u8 *)malloc((usize)w * h * 4);
    PvrDecode4bpp(data_ptr, w, h, rgba); // host software PVRTC 4bpp -> RGBA

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    free(rgba);
    return tex;
}

// --- Shared NuTex layer entry points (host) ----------------------------------

i32 NuTexRead(char *name, VARIPTR *buf, VARIPTR *buf_end) {
    char filename[1024];

    // `stuff\legal\LEGAL_ENGLISH` -> `stuff\legal\LEGAL_ENGLISH_ios.tex`
    const char *platform =
        NuPlatform::Get() != NULL && (NuPlatform::Get()->GetCurrentPlatform() == IOS_PLATFORM ||
                                      NuPlatform::Get()->GetCurrentPlatform() == ANDROID_PVRTC_PLATFORM)
            ? "ios"
            : "MOB";
    NuStrFixExtPlatform(filename, name, "tex", sizeof(filename), (char *)platform);

    NUFILE file_handle = NuFileOpen(filename, NUFILE_READ);
    if (file_handle == 0) {
        return 0;
    }
    i32 file_size = NuFileOpenSize(file_handle);

    NUNATIVETEX *tex = (NUNATIVETEX *)ALIGN(buf->addr, 0x4);
    buf->addr = ALIGN(buf->addr, 0x4) + sizeof(NUNATIVETEX);

    char *data = (char *)ALIGN(buf->addr, 0x4);
    buf->addr = ALIGN(buf->addr, 0x4) + file_size;

    tex->image_data = data;
    tex->size = file_size;

    NuFileRead(file_handle, data, file_size);
    NuFileClose(file_handle);

    return NuTexCreateNative(tex, false);
}

void NuTexCreatePS(NUNATIVETEX *tex, bool is_pvrtc) {
    if (tex == NULL || tex->image_data == NULL || tex->size == 0) {
        return;
    }
    tex->platform.gl_tex = NuIOS_CreateGLTexFromPlatformInMemory(tex->image_data, &tex->width, &tex->height, is_pvrtc);
    tex->image_data = NULL;
    tex->size = 0;
}
