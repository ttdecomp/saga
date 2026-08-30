#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <stdio.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <vector>

#include "decomp.h"
#include "globals.h"

#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nugscn.h"
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
static void PvrDecode4bpp(const u8 *data, usize data_size, i32 width, i32 height, u8 *outRGBA) {
    i32 bpp = 4;
    i32 W = 4;
    i32 H = 4;
    u32 word_count = (u32)((usize)width * height / 2 / 4);
    std::vector<u32> words(width * height / 2 / 4, 0);
    memcpy(words.data(), data, std::min(data_size, (usize)width * height / 2));
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

// --- ETC1 (host software decode) --------------------------------------------
//
// The "mob" platform texture blobs are DDS containers with an 'ETC1' fourcc.
// Desktop GL on the host test machine does not guarantee the OES ETC1
// extension, so decode to RGBA on the CPU. Algorithm per the Khronos ETC1
// specification; verified against the first-boot legal texture.

static const i32 Etc1ModifierTable[8][4] = {
    {2, 8, -2, -8},     {5, 17, -5, -17},   {9, 29, -9, -29},     {13, 42, -13, -42},
    {18, 60, -18, -60}, {24, 80, -24, -80}, {33, 106, -33, -106}, {47, 183, -47, -183},
};

static u8 Etc1Clamp(i32 v) { // signed: modifier deltas go negative
    return (u8)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

static void Etc1DecodeBlock(const u8 *blk, u8 out[16][3]) {
    // The blob serializes each 64-bit block in spec-canonical MSB-first byte
    // order: control/colour word first, texel-index word second, both
    // big-endian. (A PKM file would hold the same bits as two little-endian
    // words in the opposite order.) Layout verified against the ios-platform
    // PVR variant of the same texture; mean per-channel deviation 3/255.
    u32 ctrl = ((u32)blk[0] << 24) | ((u32)blk[1] << 16) | ((u32)blk[2] << 8) | blk[3];
    u32 idxw = ((u32)blk[4] << 24) | ((u32)blk[5] << 16) | ((u32)blk[6] << 8) | blk[7];

    bool flip = (ctrl & 1) != 0;
    bool diff = ((ctrl >> 1) & 1) != 0;
    u32 cw1 = (ctrl >> 5) & 7;
    u32 cw2 = (ctrl >> 2) & 7;

    u8 c1[3], c2[3];
    if (diff) {
        i32 r1 = (ctrl >> 27) & 0x1f, dr = (ctrl >> 24) & 7;
        i32 g1 = (ctrl >> 19) & 0x1f, dg = (ctrl >> 16) & 7;
        i32 b1 = (ctrl >> 11) & 0x1f, db = (ctrl >> 8) & 7;
        auto sx3 = [](i32 v) -> i32 { return (v & 4) ? v - 8 : v; };
        auto e5 = [](i32 c) -> u8 {
            if (c < 0) {
                c = 0;
            } else if (c > 31) {
                c = 31;
            }
            return (u8)((c << 3) | (c >> 2));
        };
        c1[0] = e5(r1);
        c1[1] = e5(g1);
        c1[2] = e5(b1);
        c2[0] = e5(r1 + sx3(dr));
        c2[1] = e5(g1 + sx3(dg));
        c2[2] = e5(b1 + sx3(db));
    } else {
        c1[0] = ((ctrl >> 28) & 0xf) * 17;
        c1[1] = ((ctrl >> 20) & 0xf) * 17;
        c1[2] = ((ctrl >> 12) & 0xf) * 17;
        c2[0] = ((ctrl >> 24) & 0xf) * 17;
        c2[1] = ((ctrl >> 16) & 0xf) * 17;
        c2[2] = ((ctrl >> 8) & 0xf) * 17;
    }

    for (i32 y = 0; y < 4; y++) {
        for (i32 x = 0; x < 4; x++) {
            i32 bit = x * 4 + y; // column-major within the block
            u32 lsb = (idxw >> bit) & 1;
            u32 msb = (idxw >> (bit + 16)) & 1;
            u32 tbl;
            u32 pi;
            if (!flip) {
                tbl = (x < 2) ? cw1 : cw2;
                pi = (x < 2) ? 0 : 1;
            } else {
                tbl = (y < 2) ? cw1 : cw2;
                pi = (y < 2) ? 0 : 1;
            }
            const u8 *base = (pi == 0) ? c1 : c2;
            i32 m = Etc1ModifierTable[tbl][(msb << 1) | lsb];
            u8 *o = out[y * 4 + x];
            o[0] = Etc1Clamp(base[0] + m);
            o[1] = Etc1Clamp(base[1] + m);
            o[2] = Etc1Clamp(base[2] + m);
        }
    }
}

// The original device can create the font atlas texture on the loading
// thread.  The host EGL context belongs to the render thread, so retain that
// inline texture blob until NuTexCreateNative associates it with a native
// texture object and the render thread can perform the upload.
static std::vector<std::vector<u8>> g_hostUnownedTextureBlobs;

static u32 PvrBitsPerPixel(const u8 *src) {
    const u32 channel_bits = *reinterpret_cast<const u32 *>(src + 0x0c);
    if (channel_bits == 0) {
        const u32 compressed_format = *reinterpret_cast<const u32 *>(src + 0x08);
        return compressed_format < 2 ? 2 : 4;
    }
    return (src[0x0c] + src[0x0d] + src[0x0e] + src[0x0f]);
}

static usize PlatformTextureBlobSize(const void *data) {
    const u8 *src = static_cast<const u8 *>(data);
    if (src[0] == 'D' && src[1] == 'D' && src[2] == 'S' && src[3] == ' ') {
        const usize width = *reinterpret_cast<const u32 *>(src + 0x0c);
        const usize height = *reinterpret_cast<const u32 *>(src + 0x10);
        return 128 + ((width + 3) / 4) * ((height + 3) / 4) * 8;
    }
    if (src[0] == 'P' && src[1] == 'V' && src[2] == 'R') {
        u32 width = *reinterpret_cast<const u32 *>(src + 0x1c);
        u32 height = *reinterpret_cast<const u32 *>(src + 0x18);
        const usize headerSize = 0x34 + *reinterpret_cast<const u32 *>(src + 0x30);
        const u32 depth = *reinterpret_cast<const u32 *>(src + 0x20);
        const u32 surfaces = *reinterpret_cast<const u32 *>(src + 0x24);
        const u32 faces = *reinterpret_cast<const u32 *>(src + 0x28);
        const u32 mip_count = *reinterpret_cast<const u32 *>(src + 0x2c);
        const u32 bits_per_pixel = PvrBitsPerPixel(src);
        usize payload_size = 0;
        for (u32 level = 0; level < mip_count; ++level) {
            usize level_size = static_cast<usize>(width) * height * bits_per_pixel / 8;
            if (level_size < 0x20) {
                level_size = 0x20;
            }
            payload_size += level_size * depth * surfaces * faces;
            width = width > 1 ? width >> 1 : 1;
            height = height > 1 ? height >> 1 : 1;
        }
        return headerSize + payload_size;
    }
    return 0;
}

// Creates a GL texture from a PVR or DDS texture blob, returning the texture id
// and setting *width/*height. Host-only: software decodes PVRTC to RGBA.
static GLuint CreateGLTexFromPlatformInMemory(void *data, usize data_size, i32 *width, i32 *height, bool is_pvrtc) {
    const u8 *src = (const u8 *)data;

    // DDS container ("mob" platform blobs): here it wraps a full ETC1 mip
    // chain (fourcc 'ETC1'). HOST-ONLY software decode to RGBA.
    if (src[0] == 'D' && src[1] == 'D' && src[2] == 'S' && src[3] == ' ') {
        i32 w = *(const i32 *)(src + 0x0c);
        i32 h = *(const i32 *)(src + 0x10);
        *width = w;
        *height = h;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // HOST-ONLY: upload mip level 0 only and sample with GL_LINEAR. The
        // original GLES2 path would upload the full chain, but Mesa/llvmpipe
        // silently produced a black-sampling object for it.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        u32 pixel_flags = *(const u32 *)(src + 0x50);
        u32 fourcc = *(const u32 *)(src + 0x54);
        if (pixel_flags == 0x41 && *(const u32 *)(src + 0x58) == 32 && *(const u32 *)(src + 0x5c) == 0x00ff0000 &&
            *(const u32 *)(src + 0x60) == 0x0000ff00 && *(const u32 *)(src + 0x64) == 0x000000ff &&
            *(const u32 *)(src + 0x68) == 0xff000000) {
            // NuDDSGetTextureDescription classifies this as NUTEX_RGBA32 and
            // the original upload path passes the payload straight to
            // glTexImage2D(..., GL_RGBA, GL_UNSIGNED_BYTE, ...).  In
            // particular it does not reinterpret the legacy DDS masks.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, src + 128);
        } else if (fourcc == 0x31435445) { // 'ETC1'
            if (w <= 0 || h <= 0 || data_size < 128) {
                glDeleteTextures(1, &tex);
                return 0;
            }
            const usize block_width = ((usize)w + 3) / 4;
            const usize block_height = ((usize)h + 3) / 4;
            const usize payload_size = block_width * block_height * 8;
            if (payload_size > data_size - 128) {
                LOG_WARN("[tex] truncated ETC1 texture %dx%d: have=%u need=%u", w, h, (u32)(data_size - 128),
                         (u32)payload_size);
                glDeleteTextures(1, &tex);
                return 0;
            }
            const u8 *cur = src + 128;
            u8 *rgba = (u8 *)malloc((usize)w * h * 4);
            for (usize by = 0; by < block_height; by++) {
                for (usize bx = 0; bx < block_width; bx++) {
                    u8 px[16][3];
                    Etc1DecodeBlock(cur + (by * block_width + bx) * 8, px);
                    for (i32 y = 0; y < 4; y++) {
                        for (i32 x = 0; x < 4; x++) {
                            const usize dst_x = bx * 4 + x;
                            const usize dst_y = by * 4 + y;
                            if (dst_x >= (usize)w || dst_y >= (usize)h) {
                                continue;
                            }
                            u8 *d = rgba + (dst_y * (usize)w + dst_x) * 4;
                            d[0] = px[y * 4 + x][0];
                            d[1] = px[y * 4 + x][1];
                            d[2] = px[y * 4 + x][2];
                            d[3] = 255;
                        }
                    }
                }
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
            free(rgba);
        } else {
            glDeleteTextures(1, &tex);
            return 0;
        }
        return tex;
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

    const usize header_size = (usize)(data_ptr - src);
    const usize payload_size = data_size > header_size ? data_size - header_size : 0;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const u32 channel_order = *reinterpret_cast<const u32 *>(src + 0x08);
    const u32 channel_bits = *reinterpret_cast<const u32 *>(src + 0x0c);
    if (channel_order == 0x61626772 && channel_bits == 0x04040404) { // "rgba", 4 bits each
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data_ptr);
    } else if (channel_order == 0x61626772 && channel_bits == 0x08080808) { // "rgba", 8 bits each
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_ptr);
    } else if (channel_bits == 0) {
        u8 *rgba = (u8 *)malloc((usize)w * h * 4);
        PvrDecode4bpp(data_ptr, payload_size, w, h, rgba); // host software PVRTC 4bpp -> RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
        free(rgba);
    } else {
        glDeleteTextures(1, &tex);
        return 0;
    }
    return tex;
}

GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, i32 *width, i32 *height, bool is_pvrtc) {
    if (eglGetCurrentContext() == EGL_NO_CONTEXT) {
        const u8 *src = static_cast<const u8 *>(data);
        const usize size = PlatformTextureBlobSize(data);
        if (size == 0) {
            return 0;
        }
        if (src[0] == 'D') {
            *width = *reinterpret_cast<const i32 *>(src + 0x0c);
            *height = *reinterpret_cast<const i32 *>(src + 0x10);
        } else {
            *width = *reinterpret_cast<const i32 *>(src + 0x1c);
            *height = *reinterpret_cast<const i32 *>(src + 0x18);
        }
        g_hostUnownedTextureBlobs.emplace_back(src, src + size);
        return 0;
    }
    return CreateGLTexFromPlatformInMemory(data, PlatformTextureBlobSize(data), width, height, is_pvrtc);
}

// --- Shared NuTex layer entry points (host) ----------------------------------

i32 NuTexRead(char *name, VARIPTR *buf, VARIPTR *buf_end) {
    char filename[1024];

    // HOST-ONLY: the wad ships one texture blob per compression platform with
    // different naming schemes ("legal_english_ios.tex",
    // "legal_english_mob.android_etc1_tex"). Try each candidate until one
    // opens instead of duplicating the original's platform switch.
    struct {
        const char *platform;
        const char *ext;
    } candidates[] = {
        {"mob", "android_etc1_tex"},
        {"ios", "tex"},
    };

    char base[512];
    NUFILE file_handle = 0;
    for (usize i = 0; i < sizeof(candidates) / sizeof(candidates[0]) && file_handle == 0; i++) {
        snprintf(base, sizeof(base), "%s", name);
        NuStrFixExtPlatform(filename, base, (char *)candidates[i].ext, sizeof(filename),
                            (char *)candidates[i].platform);
        file_handle = NuFileOpen(filename, NUFILE_READ);
    }

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

// HOST-ONLY: texture blobs land in a shared scratch arena that the engine
// recycles within a few frames, while the GL upload can only run once some
// thread owns an EGL context (the loader thread does not). Until then keep a
// private snapshot of each pending blob here, keyed by NUNATIVETEX*, so the
// deferred upload decodes real pixels instead of recycled arena bytes.
static std::map<NUNATIVETEX *, std::vector<u8>> g_hostStagedTextures;
// Retain the scene hash after the deferred blob has been consumed.  This is
// host-test diagnostics only: it lets debugger/capture tooling identify the
// source asset behind a live GL texture without changing the game texture
// registry or the original render path.
static std::map<const NUNATIVETEX *, u32> g_hostTextureHashes;

extern "C" u32 NuTexHostGetHash(const NUNATIVETEX *texture) {
    auto it = g_hostTextureHashes.find(texture);
    return it != g_hostTextureHashes.end() ? it->second : 0;
}

// NuGScnReadTexturesPS uploads through a stack-local NUNATIVETEX in the
// original.  That is safe there because the Android loading thread owns a GL
// context and NuTexCreatePS completes synchronously.  Keep the embedded blobs
// by scene-texture slot until NuGScnCreatePS supplies their persistent native
// texture objects on the host.
static std::vector<std::vector<u8>> g_hostSceneTextureBlobs;

static bool HostReadHashedTexture(u32 hash, std::vector<u8> &data) {
    char path[0x10c];
    const char *extensions[] = {"ETC1", "PVRNC"};

    NUFILE file = 0;
    for (usize i = 0; i < sizeof(extensions) / sizeof(extensions[0]); ++i) {
        snprintf(path, sizeof(path), "SHAREDTEXTURES/0X%08X.%s", hash, extensions[i]);
        file = NuFileOpen(path, NUFILE_READ);
        if (file != 0) {
            break;
        }
    }
    if (file == 0) {
        return false;
    }

    const i32 size = NuFileOpenSize(file);
    data.resize(size);
    NuFileRead(file, data.data(), size);
    NuFileClose(file);
    return true;
}

static bool HostStageHashedTexture(NUNATIVETEX *texture, u32 hash) {
    std::vector<u8> data;
    if (!HostReadHashedTexture(hash, data)) {
        LOG_WARN("[tex-scene] missing hash 0x%08x", hash);
        return false;
    }
    g_hostStagedTextures[texture] = std::move(data);
    g_hostTextureHashes[texture] = hash;
    return true;
}

GLuint NuIOS_CreateGLTexFromHash(u32 hash) {
    std::vector<u8> data;
    if (!HostReadHashedTexture(hash, data)) {
        return 0;
    }
    i32 width = 0;
    i32 height = 0;
    return CreateGLTexFromPlatformInMemory(data.data(), data.size(), &width, &height, false);
}

bool NuTexHostTakeStaged(NUNATIVETEX *tex, std::vector<u8> &out) {
    auto it = g_hostStagedTextures.find(tex);
    if (it == g_hostStagedTextures.end()) {
        return false;
    }
    out = std::move(it->second);
    g_hostStagedTextures.erase(it);
    return true;
}

i32 NuGScnReadTexturesPS(i32 file, VARIPTR *buf, VARIPTR) {
    i32 bytes_read = NuFileRead(file, &g_VideoResHeader.ntextures, sizeof(g_VideoResHeader.ntextures));

    g_VideoResHeader.texture_hashes = g_VideoResHeader.ntextures & 0x8000;
    g_VideoResHeader.ntextures &= 0x7fff;
    g_VideoResHeader.textures = buf->u32_ptr;
    buf->u32_ptr += g_VideoResHeader.ntextures;
    memset(g_VideoResHeader.textures, 0, (usize)g_VideoResHeader.ntextures * sizeof(u32));

    g_hostSceneTextureBlobs.clear();
    g_hostSceneTextureBlobs.resize(g_VideoResHeader.ntextures);
    LOG_INFO("[tex-scene] read count=%u hashes=%d", (u32)g_VideoResHeader.ntextures,
             g_VideoResHeader.texture_hashes != 0);
    if (g_VideoResHeader.texture_hashes != 0) {
        bytes_read += NuFileRead(file, g_VideoResHeader.textures, (i32)g_VideoResHeader.ntextures * (i32)sizeof(u32));
        return bytes_read;
    }

    for (u32 i = 0; i < g_VideoResHeader.ntextures; ++i) {
        i32 texture_header[6];
        bytes_read += NuFileRead(file, texture_header, sizeof(texture_header));
        const u32 size = (u32)texture_header[5];
        if (size == 0) {
            if (texture_header[0] < 0) {
                i += 5;
            }
            continue;
        }
        if (texture_header[0] == 0) {
            continue;
        }

        void *scratch = buf->void_ptr;
        buf->addr += size;
        bytes_read += NuFileRead(file, scratch, size);
        const u8 *bytes = static_cast<const u8 *>(scratch);
        g_hostSceneTextureBlobs[i].assign(bytes, bytes + size);
        LOG_INFO("[tex-scene] slot=%u size=%u pvrtc=%d", i, size, texture_header[0] < 0);
        buf->addr -= size;

        if (texture_header[0] < 0) {
            i += 5;
        }
    }
    return bytes_read;
}

void NuGScnCreatePS(NUGSCN *scene, VARIPTR *, VARIPTR *) {
    NUNATIVETEX **textures = scene->textures;
    i32 staged_count = 0;
    for (i32 i = 0; i < scene->ntextures; ++i) {
        NUNATIVETEX *texture = textures[i];
        texture->image_data = nullptr;
        texture->size = 0;
        texture->platform.gl_tex = 0;

        if (g_VideoResHeader.texture_hashes != 0) {
            const u32 hash = g_VideoResHeader.textures[i];
            texture->size = hash;
            if (hash != 0 && HostStageHashedTexture(texture, hash)) {
                staged_count++;
            }
        } else if ((usize)i < g_hostSceneTextureBlobs.size() && !g_hostSceneTextureBlobs[i].empty()) {
            g_hostStagedTextures[texture] = std::move(g_hostSceneTextureBlobs[i]);
            staged_count++;
        }
    }
    LOG_INFO("[tex-scene] create scene=%p textures=%d blobs=%u staged=%d", scene, scene->ntextures,
             (u32)g_hostSceneTextureBlobs.size(), staged_count);
    g_hostSceneTextureBlobs.clear();
}

// original 0x29d701 — bind a native texture on a texture unit. The Android
// original lives in nutex_android.cpp which the host build excludes; this is
// its exact body against the host GL context.
extern i32 g_currentTexUnit; // nutex_ios_ex.cpp
void NuTexSetTextureWithStagePS(NUNATIVETEX *tex, u32 stage) {
    glActiveTexture(GL_TEXTURE0 + stage);
    g_currentTexUnit = (i32)stage;
    if (tex != NULL && tex->platform.gl_tex == 0) {
        std::vector<u8> staged;
        if (NuTexHostTakeStaged(tex, staged)) {
            void *data_ptr = staged.data();
            GLuint created = 0;
            {
                i32 w = tex->width, h = tex->height;
                created = CreateGLTexFromPlatformInMemory(data_ptr, staged.size(), &w, &h, true);
                if (created == 0) {
                    created = CreateGLTexFromPlatformInMemory(data_ptr, staged.size(), &w, &h, false);
                }
                tex->width = w;
                tex->height = h;
            }
            if (created != 0) {
                tex->platform.gl_tex = created;
                tex->image_data = NULL;
                tex->size = 0;
            } else {
                LOG_WARN("[tex] staged upload failed");
            }
        } else if (tex->image_data != NULL && tex->size != 0) {
            NuTexCreatePS(tex, true);
            if (tex->platform.gl_tex == 0)
                NuTexCreatePS(tex, false);
        }
    }
    glBindTexture(GL_TEXTURE_2D, tex != NULL ? tex->platform.gl_tex : 0);
}

i32 NuTexGenTexture(NUNATIVETEX *tex) {
    // The original stores the supplied native texture in the first free
    // texture-list slot.  The host registry is owned by NuTexCreateNative,
    // whose extra upload step is a no-op for these initially empty render
    // targets, so use it to preserve both the slot and pointer association.
    return NuTexCreateNative(tex, false);
}

void NuTexCreatePS(NUNATIVETEX *tex, bool is_pvrtc) {
    if (tex == NULL) {
        return;
    }

    if (tex->image_data == NULL || tex->size == 0) {
        if (tex->platform.gl_tex == 0 && !g_hostUnownedTextureBlobs.empty()) {
            g_hostStagedTextures[tex] = std::move(g_hostUnownedTextureBlobs.front());
            g_hostUnownedTextureBlobs.erase(g_hostUnownedTextureBlobs.begin());
        }
        return;
    }

    // No current context on this thread: GL calls would silently no-op while
    // consuming image_data, losing the texture forever. Keep the bytes alive
    // for the present-side upload instead.
    std::vector<u8> staged;
    void *data_ptr = tex->image_data;
    if (eglGetCurrentContext() == EGL_NO_CONTEXT) {
        if (!NuTexHostTakeStaged(tex, staged)) {
            g_hostStagedTextures[tex] = std::vector<u8>((u8 *)tex->image_data, (u8 *)tex->image_data + tex->size);
            // NuTexCreatePS on the original always replaces this arena field
            // with glGenTextures' result before returning.  The host defers
            // that operation to the render thread, so establish the value the
            // pending operation must have instead of retaining arena garbage.
            tex->platform.gl_tex = 0;
            return;
        }
        // A previous pass staged these bytes; fall through and upload them.
        data_ptr = staged.data();
    }

    const usize data_size = staged.empty() ? tex->size : staged.size();
    tex->platform.gl_tex = CreateGLTexFromPlatformInMemory(data_ptr, data_size, &tex->width, &tex->height, is_pvrtc);
    if (tex->platform.gl_tex != 0) {
        tex->image_data = NULL;
        tex->size = 0;
    }
}
