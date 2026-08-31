#include "nu2api/nucore/common.h"

// Host implementation of compressed formats unavailable from host GLES.

#include <GLES2/gl2.h>

#include <algorithm>
#include <string.h>
#include <string>
#include <vector>

// Compatibility for compressed formats supported by the original device but
// not guaranteed by a desktop GLES implementation. The target texture parser,
// registry, and upload decisions remain in the reconstructed game sources.

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

static bool host_pvr_is_pow2(u32 n) {
    return n != 0 && (n & (n - 1)) == 0;
}

// Interleave the x/y block coordinates into a "twiddled" (Morton) data index.
static u32 host_pvr_twiddle_uv(u32 xsize, u32 ysize, u32 xpos, u32 ypos) {
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

static void host_pvr_get_color_a(u32 cd, u8 rgb[4]) {
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

static void host_pvr_get_color_b(u32 cd, u8 rgb[4]) {
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
static void host_pvr_interpolate_colors(const i32 P[4], const i32 Q[4], const i32 R[4], const i32 S[4], i32 bpp,
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
static void host_pvr_unpack_modulations(u32 mod_data, u32 cd, i32 offset_x, i32 offset_y, u8 mod_vals[16][16]) {
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
static void host_pvr_get_pixels(const PvrWordPair &P, const PvrWordPair &Q, const PvrWordPair &R, const PvrWordPair &S,
                                i32 bpp, u8 out[16][4]) {
    i32 W = 4;
    i32 H = 4;
    if (bpp == 2) {
        W = 8;
        H = 8;
    }
    u8 mod_vals[16][16];
    memset(mod_vals, 0, sizeof(mod_vals));
    host_pvr_unpack_modulations(P[0], P[1], 0, 0, mod_vals);
    host_pvr_unpack_modulations(Q[0], Q[1], W, 0, mod_vals);
    host_pvr_unpack_modulations(R[0], R[1], 0, H, mod_vals);
    host_pvr_unpack_modulations(S[0], S[1], W, H, mod_vals);

    i32 pa[4], qa[4], ra[4], sa[4], pb[4], qb[4], rb[4], sb[4];
    u8 tmp[4];
    host_pvr_get_color_a(P[1], tmp);
    for (int i = 0; i < 4; i++)
        pa[i] = tmp[i];
    host_pvr_get_color_a(Q[1], tmp);
    for (int i = 0; i < 4; i++)
        qa[i] = tmp[i];
    host_pvr_get_color_a(R[1], tmp);
    for (int i = 0; i < 4; i++)
        ra[i] = tmp[i];
    host_pvr_get_color_a(S[1], tmp);
    for (int i = 0; i < 4; i++)
        sa[i] = tmp[i];
    host_pvr_get_color_b(P[1], tmp);
    for (int i = 0; i < 4; i++)
        pb[i] = tmp[i];
    host_pvr_get_color_b(Q[1], tmp);
    for (int i = 0; i < 4; i++)
        qb[i] = tmp[i];
    host_pvr_get_color_b(R[1], tmp);
    for (int i = 0; i < 4; i++)
        rb[i] = tmp[i];
    host_pvr_get_color_b(S[1], tmp);
    for (int i = 0; i < 4; i++)
        sb[i] = tmp[i];

    u8 A[16][4], B[16][4];
    host_pvr_interpolate_colors(pa, qa, ra, sa, bpp, A);
    host_pvr_interpolate_colors(pb, qb, rb, sb, bpp, B);

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
static void host_pvr_map_data(u8 *out, i32 width, u8 word[16][4], u32 py, u32 px, u32 qy, u32 qx, u32 ry, u32 rx,
                              u32 sy, u32 sx, i32 bpp) {
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
static void host_pvr_decode_4bpp(const u8 *data, usize data_size, i32 width, i32 height, u8 *outRGBA) {
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
            u32 offs[4] = {host_pvr_twiddle_uv(nxw, nyw, Px, Py) * 2, host_pvr_twiddle_uv(nxw, nyw, Qx, Qy) * 2,
                           host_pvr_twiddle_uv(nxw, nyw, Rx, Ry) * 2, host_pvr_twiddle_uv(nxw, nyw, Sx, Sy) * 2};
            if (offs[0] + 1 >= word_count || offs[1] + 1 >= word_count || offs[2] + 1 >= word_count ||
                offs[3] + 1 >= word_count) {
                continue;
            }
            PvrWordPair PW = {words[offs[0]], words[offs[0] + 1]};
            PvrWordPair QW = {words[offs[1]], words[offs[1] + 1]};
            PvrWordPair RW = {words[offs[2]], words[offs[2] + 1]};
            PvrWordPair SW = {words[offs[3]], words[offs[3] + 1]};
            u8 block[16][4];
            host_pvr_get_pixels(PW, QW, RW, SW, bpp, block);
            host_pvr_map_data(out.data(), width, block, Py, Px, Qy, Qx, Ry, Rx, Sy, Sx, bpp);
        }
    }
    memcpy(outRGBA, out.data(), (usize)width * height * 4);
}

// --- ETC1 (host software decode) --------------------------------------------
//
// The "mob" platform texture blobs are DDS containers with an 'ETC1' fourcc.
// Desktop GL on the host machine does not guarantee the OES ETC1
// extension, so decode to RGBA on the CPU. Algorithm per the Khronos ETC1
// specification; verified against the first-boot legal texture.

static const i32 host_etc1_modifier_table[8][4] = {
    {2, 8, -2, -8},     {5, 17, -5, -17},   {9, 29, -9, -29},     {13, 42, -13, -42},
    {18, 60, -18, -60}, {24, 80, -24, -80}, {33, 106, -33, -106}, {47, 183, -47, -183},
};

static u8 host_etc1_clamp(i32 v) { // signed: modifier deltas go negative
    return (u8)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

static void host_etc1_decode_block(const u8 *blk, u8 out[16][3]) {
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
            i32 m = host_etc1_modifier_table[tbl][(msb << 1) | lsb];
            u8 *o = out[y * 4 + x];
            o[0] = host_etc1_clamp(base[0] + m);
            o[1] = host_etc1_clamp(base[1] + m);
            o[2] = host_etc1_clamp(base[2] + m);
        }
    }
}

#ifndef _WIN32
#include <dlfcn.h>

namespace {
    typedef void (*HostCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *);
    typedef const GLubyte *(*HostGlGetString)(GLenum);

    HostGlGetString host_real_gl_get_string() {
        static HostGlGetString host_real = reinterpret_cast<HostGlGetString>(dlsym(RTLD_NEXT, "glGetString"));
        return host_real;
    }

    HostCompressedTexImage2D host_real_compressed_tex_image_2d() {
        static HostCompressedTexImage2D host_real =
            reinterpret_cast<HostCompressedTexImage2D>(dlsym(RTLD_NEXT, "glCompressedTexImage2D"));
        return host_real;
    }

    void host_upload_etc1(GLenum target, GLint level, GLsizei width, GLsizei height, GLsizei image_size,
                          const void *data) {
        const usize block_width = (static_cast<usize>(width) + 3) / 4;
        const usize block_height = (static_cast<usize>(height) + 3) / 4;
        const usize required_size = block_width * block_height * 8;
        if (data == nullptr || image_size < 0 || static_cast<usize>(image_size) < required_size) {
            return;
        }

        std::vector<u8> rgba(static_cast<usize>(width) * static_cast<usize>(height) * 4);
        const u8 *source = static_cast<const u8 *>(data);
        for (usize by = 0; by < block_height; ++by) {
            for (usize bx = 0; bx < block_width; ++bx) {
                u8 decoded[16][3];
                host_etc1_decode_block(source + (by * block_width + bx) * 8, decoded);
                for (i32 y = 0; y < 4; ++y) {
                    for (i32 x = 0; x < 4; ++x) {
                        const usize dst_x = bx * 4 + static_cast<usize>(x);
                        const usize dst_y = by * 4 + static_cast<usize>(y);
                        if (dst_x >= static_cast<usize>(width) || dst_y >= static_cast<usize>(height)) {
                            continue;
                        }
                        u8 *pixel = rgba.data() + (dst_y * static_cast<usize>(width) + dst_x) * 4;
                        pixel[0] = decoded[y * 4 + x][0];
                        pixel[1] = decoded[y * 4 + x][1];
                        pixel[2] = decoded[y * 4 + x][2];
                        pixel[3] = 0xff;
                    }
                }
            }
        }
        glTexImage2D(target, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }
} // namespace

extern "C" const GLubyte *glGetString(GLenum name) {
    HostGlGetString host_real = host_real_gl_get_string();
    const GLubyte *value = host_real != nullptr ? host_real(name) : nullptr;
    if (name != GL_EXTENSIONS || value == nullptr) {
        return value;
    }

    // Expose precisely the compressed format emulated below. This keeps the
    // reconstructed capability selection and texture format validation intact.
    static std::string host_extensions;
    host_extensions.assign(reinterpret_cast<const char *>(value));
    if (host_extensions.find("GL_OES_compressed_ETC1_RGB8_texture") == std::string::npos) {
        host_extensions.append(" GL_OES_compressed_ETC1_RGB8_texture");
    }
    return reinterpret_cast<const GLubyte *>(host_extensions.c_str());
}

// Host drivers commonly lack the Android device's ETC1/PVRTC formats. Wrap the
// external GLES upload itself so all target parsing, mip selection, texture
// registry, and scene-loading code remains unchanged.
extern "C" void glCompressedTexImage2D(GLenum target, GLint level, GLenum internal_format, GLsizei width,
                                       GLsizei height, GLint border, GLsizei image_size, const void *data) {
    if (internal_format == 0x8d64) { // GL_ETC1_RGB8_OES
        host_upload_etc1(target, level, width, height, image_size, data);
        return;
    }

    if (internal_format == 0x8c00 || internal_format == 0x8c02) { // PVRTC1 RGB/RGBA 4bpp
        std::vector<u8> rgba(static_cast<usize>(width) * static_cast<usize>(height) * 4);
        host_pvr_decode_4bpp(static_cast<const u8 *>(data), static_cast<usize>(image_size), width, height, rgba.data());
        glTexImage2D(target, level, GL_RGBA, width, height, border, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
        return;
    }

    HostCompressedTexImage2D host_real = host_real_compressed_tex_image_2d();
    if (host_real != nullptr) {
        host_real(target, level, internal_format, width, height, border, image_size, data);
    }
}
#endif
