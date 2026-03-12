#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nutex.h"
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <squish.h>
#include <stdlib.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/numemory.h"

int g_currentTexUnit = -1;
int g_loadDefaultTexture;
GLuint g_lastBound2DTexIds[16];
GLuint g_lastBoundCubeTexIds[16];
GLuint g_earlyColorFramebuffer;
GLuint g_defaultFramebuffer;
GLuint g_currentFramebuffer;
GLuint g_earlyColorMSAAFramebuffer;

GLuint NuIOS_CreateGLTexFromFile(const char *filename) {
    UNIMPLEMENTED();
}

GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, int *width, int *height, bool is_pvrtc) {
    UNIMPLEMENTED();
}

GLuint loadDefaultTexture(GLuint texture, GLint level, GLsizei size, GLenum texture_type, GLenum target) {
    isize pixel_count = size * size;
    u8 *pixels = (u8 *)malloc(pixel_count * 4);
    u8 *p1 = pixels + 8;
    u8 *p2 = pixels + 4;
    for (i32 i = 0; i < pixel_count; i += 2) {
        i32 row = i / size;

        if (row & 1) {
            p1[-8] = 0x52;
            p1[-7] = 0x52;
            p1[-6] = 0x99;
            p1[-5] = 0xFF;
            p2[3] = 0xFF;
            p2[0] = 0x7A;
            p2[1] = 0x7A;
            p2[2] = 0x7A;
        } else {
            p1[-8] = 0x7A;
            p1[-7] = 0x7A;
            p1[-6] = 0x7A;
            p1[-5] = 0xFF;
            p2[3] = 0xFF;
            p2[0] = 0x52;
            p2[1] = 0x52;
            p2[2] = 0x99;
        }
        p1 += 8;
        p2 += 8;
    }

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 552);

    GLuint textures[3];
    textures[0] = texture;
    if (texture == 0) {
        glGenTextures(1, textures);
    }

    if (texture_type == GL_TEXTURE_2D) {
        glActiveTexture(GL_TEXTURE0);
        g_currentTexUnit = 0;
        glBindTexture(GL_TEXTURE_2D, textures[0]);
    } else {
        if (g_currentTexUnit != 0) {
            glActiveTexture(GL_TEXTURE0);
            g_currentTexUnit = 0;
        }
        if (g_lastBoundCubeTexIds[0] != textures[0]) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0]);
            g_lastBoundCubeTexIds[0] = textures[0];
        }
    }

    glTexImage2D(target, level, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 574);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    NuCheckGLErrorsFL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 583);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 585);
    free(pixels);

    return textures[0];
}

GLuint NuIOS_CreateGLTexFromMemoryDDS(void *ddsPointer, int *out_width, int *out_height) {
    GLuint texID = 0;
    unsigned char *decompressedBuffer = nullptr;
    unsigned char *pixelData = nullptr;

    NUTEXFORMAT format;
    int depth = 0;
    int mipCount = 0;
    bool isCubemap = false;
    bool hasFourCC = false;

    bool success = NuDDSGetTextureDescription((char *)ddsPointer, format, *out_width, *out_height, depth, mipCount,
                                              isCubemap, &hasFourCC);

    if (success && (*out_width != 0 || *out_height != 0 || mipCount > 1)) {

        texID = CreateTexturePS();
        int bpp;
        u32 glInternalFormat;
        u32 glType;
        u32 glFormat;
        bool isCompressed;
        NUTEXFORMAT nativeFormat;

        GetNativeTextureFormat(format, bpp, glInternalFormat, glType, glFormat, isCompressed, nativeFormat);

        pixelData = (unsigned char *)ddsPointer + 128;

        if (format == NUTEX_DXT5) {
            decompressedBuffer = nullptr;
            DecompressTextureToRGBA((unsigned char *)ddsPointer, 0, decompressedBuffer);

            format = NUTEX_RGBA32;
            isCompressed = false;
            glInternalFormat = GL_RGBA;
            glFormat = GL_RGBA;

            pixelData = decompressedBuffer;
        }
        UnlockTexturePS(texID, pixelData, *out_width, *out_height, depth, isCubemap, mipCount, format, glFormat,
                        glInternalFormat, glType, isCompressed);
    }

    return texID;
}

GLuint CreateTexturePS(void) {
    GLuint local_10;

    local_10 = 0;
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 0x1ad);
    glGenTextures(1, &local_10);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 0x1b1);
    return local_10;
}

void GetNativeTextureFormat(NUTEXFORMAT inFormat, int &outBpp, u32 &outInternalFormat, u32 &outType, u32 &outFormat,
                            bool &outIsCompressed, NUTEXFORMAT &outFormatEnum) {

    int formatToCheck = inFormat;

    if (inFormat == NUTEX_DXT1) {
        if (g_renderDevice.enabled_extensions[NUTEX_DXT1] != '\0') {
            outBpp = 0;
        } else if (g_renderDevice.enabled_extensions[NUTEX_ETC1] != '\0') {
            inFormat = NUTEX_ETC1;
            formatToCheck = NUTEX_ETC1;
            outBpp = 0;
        } else {
            outIsCompressed = false;
            inFormat = NUTEX_RGBA32;
            outBpp = 0;
        }
    } else {
        outIsCompressed = false;
        outBpp = 0;
    }

    switch (inFormat) {
        case NUTEX_DXT1:                                         // 1
            outInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; // 0x83F0
            outIsCompressed = true;
            outFormat = GL_RGB;         // 0x1907
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_DX1A:                                          // 2
            outInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; // 0x83F1
            outIsCompressed = true;
            outFormat = GL_RGBA;        // 0x1908
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_DXT5:                 // 6
            outInternalFormat = GL_RGBA; // 0x1908
            outIsCompressed = true;
            outFormat = GL_RGBA;        // 0x1908
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case 0x10:
            outInternalFormat = GL_LUMINANCE; // 0x1909
            outBpp = 8;
            outFormat = GL_LUMINANCE;   // 0x1909
            outType = GL_UNSIGNED_BYTE; // 0x1401
            outFormatEnum = inFormat;
            return;

        case NUTEX_ETC1:                          // 0x11
            outInternalFormat = GL_ETC1_RGB8_OES; // 0x8D64
            outIsCompressed = true;
            outFormat = GL_RGB;         // 0x1907
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_PVRTC2:                                          // 0x14
            outInternalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG; // 0x8C01
            outIsCompressed = true;
            outFormat = GL_RGB;         // 0x1907
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_PVRTC2A:                                          // 0x15
            outInternalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG; // 0x8C03
            outIsCompressed = true;
            outFormat = GL_RGBA;        // 0x1908
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_PVRTC4:                                          // 0x16
            outInternalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; // 0x8C00
            outIsCompressed = true;
            outFormat = GL_RGB;         // 0x1907
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_PVRTC4A:                                          // 0x17
            outInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; // 0x8C02
            outIsCompressed = true;
            outFormat = GL_RGBA;        // 0x1908
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_ATCA:                                            // 0x18
            outInternalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD; // 0x87EE
            outIsCompressed = true;
            outFormat = GL_RGBA;        // 0x1908
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case NUTEX_ATC:                         // 0x19
            outInternalFormat = GL_ATC_RGB_AMD; // 0x8C92
            outIsCompressed = true;
            outFormat = GL_RGB;         // 0x1907
            outType = GL_UNSIGNED_BYTE; // 0x1401
            break;

        case 0x66:
            outInternalFormat = GL_RGB; // 0x1907
            outBpp = 24;                // 0x18
            outFormat = GL_RGB;         // 0x1907
            outType = GL_UNSIGNED_BYTE; // 0x1401
            outFormatEnum = inFormat;
            return;

        case 0x69:
        case 0x72:
            outFormatEnum = inFormat;
            return;

        default:
            outInternalFormat = GL_RGBA;
            outBpp = 32;
            outFormat = GL_RGBA;
            outType = GL_UNSIGNED_BYTE;
            outFormatEnum = inFormat;
            return;
    }

    if (g_renderDevice.enabled_extensions[formatToCheck] == false) {
        while (true) {
        }
    }

    outFormatEnum = inFormat;
}

void GetTextureFormatInfo(NUTEXFORMAT texture, u32 &param_2, u32 &param_3) {
    // I need to figure out these cases before I put enums
    param_2 = 1;
    switch (texture) {
        case 1:
        case 2:
            param_2 = 4;
            param_3 = 4;
            return;
        case 3:
        case 4:
        case 5:
            param_2 = 4;
            param_3 = 8;
            return;
        case 6:
            param_2 = 4;
            param_3 = 8;
            return;
        case 7:
            param_3 = 0x20;
            return;
        case 8:
            param_3 = 0x40;
            return;
        case 9:
            param_3 = 0x80;
            return;
        case 0x10:
            param_3 = 8;
            return;
        case 0x14:
        case 0x15:
            param_2 = 8;
            param_3 = 2;
            return;
        case 0x16:
        case 0x17:
            param_2 = 8;
            param_3 = 4;
            return;
        case 0x6c:
        case 0x76:
            param_3 = 0x10;
    }
    return;
}

int GetMipLevelSize(NUTEXFORMAT param_1, int unclamped_width, int unclamped_height) {
    u32 block_size;
    u32 bpp;

    GetTextureFormatInfo(param_1, block_size, bpp);
    u32 width = 1;
    if (unclamped_width > 0) {
        width = unclamped_width;
    }
    u32 height = unclamped_height;
    if (unclamped_height <= 0) {
        height = 1;
    }
    u32 div_w = width / block_size;
    u32 multiplier = (block_size * block_size * bpp) >> 3;
    u32 m_w = 1;
    if (div_w != 0) {
        m_w = div_w;
    }
    u32 div_h = height / block_size;
    u32 m_h = 1;
    if (div_h != 0) {
        m_h = div_h;
    }
    u32 base_area = m_w * m_h;
    return base_area * multiplier;
}

int GetMipOffset(NuHardwareTexture *tex, int targetMip, int targetSlice, NUTEXFORMAT overrideFormat) {
    if (tex->depth == 0) {
        tex->depth = 1;
    }

    NUTEXFORMAT format = (overrideFormat != 0) ? overrideFormat : tex->format;
    bool isAutoMode = (targetMip < 0 && targetSlice < 0);
    int currentMipLimit = targetMip;
    int currentSliceLimit = targetSlice;

    if (isAutoMode) {
        currentMipLimit = tex->mips;
        currentSliceLimit = (tex->flags & 1) ? 5 : tex->depth;
    }

    int totalOffset = 0;
    int currentSlice = 0;

    while (true) {
        int innerMipLimit = currentMipLimit;

        if ((tex->flags & 1) && (currentSlice < targetSlice)) {
            innerMipLimit = tex->mips;
        }

        int currentMip = 0;
        while (true) {
            int w = tex->width >> currentMip;
            int h = tex->height >> currentMip;

            int levelSize = GetMipLevelSize(format, w, h);

            if (currentMip == innerMipLimit) {
                if (!isAutoMode && tex->depth > 1) {
                    totalOffset += levelSize * targetSlice;
                }
                break;
            }

            int d = tex->depth >> currentMip;
            if (d < 1)
                d = 1;

            totalOffset += d * levelSize;
            currentMip++;
        }

        if (!(tex->flags & 1) || currentSlice == currentSliceLimit) {
            break;
        }

        currentSlice++;
    }

    return totalOffset;
}

#define SQUISH_KDXT1 (1 << 0) // 1
#define SQUISH_KDXT3 (1 << 1) // 2
#define SQUISH_KDXT5 (1 << 2) // 4

static const int CSWTCH_249[] = {SQUISH_KDXT1, SQUISH_KDXT3, SQUISH_KDXT5};

void DecompressTextureToRGBA(unsigned char *ddsData, u32 size, unsigned char *&outBuffer) {
    int squishFlags;
    NuMemory *v8;
    NuMemoryManager *threadMem;
    static unsigned char *buffer = nullptr;

    NuHardwareTexture tex;

    NUTEXFORMAT format;
    int width, height, depth;
    int mips;
    bool isCubemap;
    bool unkDesc;

    NuDDSGetTextureDescription((char *)ddsData, format, width, height, depth, mips, isCubemap, &unkDesc);

    squishFlags = 1;
    if ((u32)(format - 4) < 3) {
        squishFlags = CSWTCH_249[format - 4];
    }

    tex.format = format;
    tex.width = width;
    tex.height = height;
    tex.depth = depth;

    tex.flags = (tex.flags & 0x80) | (isCubemap & 1);

    tex.lp_raw_data = nullptr;
    tex.lp_device_resource = nullptr;
    tex.data_size = 0;
    tex.ref_count = 0;
    tex.checksum = 0;
    tex.unknown = 0;
    tex.mips = mips;
    tex.type = 0;

    ddsData += 128;

    GetMipOffset(&tex, -1, -1, (NUTEXFORMAT)7);

    if (buffer == nullptr) {
        v8 = NuMemoryGet();
        threadMem = v8->GetThreadMem();
        buffer = (unsigned char *)threadMem->_BlockAlloc(0x6000000, 4, 1, "DecompressBuffer", 0);
    }

    outBuffer = buffer;

    int faceLimit = (-(int)(isCubemap == false) & 0xFFFFFFFB) + 6;

    int face = 0;
    do {
        if (tex.mips != 0) {
            int mip = 0;
            do {
                int mipOffset = GetMipOffset(&tex, mip, face, (NUTEXFORMAT)0);
                int rgbaOffset = GetMipOffset(&tex, mip, face, (NUTEXFORMAT)7);
                squish::DecompressImage(outBuffer + rgbaOffset, tex.width >> mip, tex.height >> mip,
                                        ddsData + mipOffset, squishFlags);
                mip++;
            } while (mip < tex.mips);
        }
        face++;
    } while (face < faceLimit);
}

int GetMipOffset(int width, int height, NUTEXFORMAT format, int depth, bool isCubemap, int mips, int targetMip,
                 int targetSlice) {
    int totalOffset = 0;
    int currentSlice = 0;
    int mipLimit;
    int sliceLimit;
    char isAuto;

    if (depth == 0) {
        depth = 1;
    }

    isAuto = (char)((targetMip >> 31) & (targetSlice >> 31));

    if (isAuto != 0) {
        sliceLimit = 5;
        mipLimit = mips;
        if (!isCubemap) {
            sliceLimit = depth;
        }
    } else {
        mipLimit = targetMip;
        sliceLimit = targetSlice;
    }

    bool isCompressed;
    int minBlocks;
    int bytesPerBlockOrPixel;
    int blockWidth;

    if (format - 1 < 118) {
        isCompressed = FormatIsCompressedTable[format];
        minBlocks = FormatMinBlocksYTable[format];
        bytesPerBlockOrPixel = FormatBytesPerElementTable[format];
        blockWidth = FormatBlockWidthTable[format];
    } else {
        isCompressed = false;
        minBlocks = 1;
        bytesPerBlockOrPixel = 0;
        blockWidth = 4;
    }

    while (true) {
        int innerMipLimit = mipLimit;

        if (isCubemap && (currentSlice < targetSlice)) {
            innerMipLimit = mips;
        }

        int m = 0;

        while (true) {
            int w = width >> m;
            if (w < 1)
                w = 1;

            int h = height >> m;
            if (h < 1)
                h = 1;

            int levelSize;

            if (!isCompressed) {
                levelSize = w * h * bytesPerBlockOrPixel;
            } else {
                int blocksY = h >> 2;
                if (blocksY < minBlocks)
                    blocksY = minBlocks;

                int blocksX = w / blockWidth;
                if (blocksX < minBlocks)
                    blocksX = minBlocks;

                levelSize = blocksX * blocksY * bytesPerBlockOrPixel;
            }

            if (m == innerMipLimit) {
                if (isAuto == 0 && depth > 1) {
                    totalOffset += levelSize * targetSlice;
                }
                break;
            }

            int d = depth >> m;
            if (d <= 0)
                d = 1;

            totalOffset += d * levelSize;
            m++;
        }

        if (!isCubemap || (currentSlice == sliceLimit)) {
            break;
        }

        currentSlice++;
    }

    return totalOffset;
}

void UnlockTexturePS(u32 texID, void *pixels, int width, int height, int depth, bool isCubemap, int mips,
                     NUTEXFORMAT format, u32 &glFormat, u32 &glInternalFormat, u32 glType, bool isCompressed) {
    u32 faceTarget = 0x8515;

    int loopCounter;

    do {
        if (mips != 0) {

            u32 texTarget = 0x0DE1;
            if (isCubemap) {
                texTarget = faceTarget;
            }

            int mip = 0;
            int mipWidth;
            int hLimit;

            do {
                mipWidth = width >> mip;
                if (mipWidth < 1) {
                    mipWidth = 1;
                }

                int mipHeight = height >> mip;
                hLimit = 1;
                if (mipHeight > 0) {
                    hLimit = mipHeight;
                }

                int currentOffset =
                    GetMipOffset(width, height, format, depth, isCubemap, mips, mip, faceTarget - 0x8515);

                unsigned char *mipData = (unsigned char *)pixels + currentOffset;

                if (!isCompressed) {
                    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                           0x58e);

                    u32 bindTarget = (-(int)(isCubemap == false) & 0xFFFF88CE) + 0x8513;
                    glBindTexture(bindTarget, texID);

                    if (g_loadDefaultTexture == 0) {
                        glTexImage2D(texTarget, mip, glInternalFormat, mipWidth, hLimit, 0, glFormat, glType, mipData);
                    } else {
                        loadDefaultTexture(texID, mip, mipWidth, 0x0DE1, 0x0DE1);
                    }
                    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                         0x5b3);

                } else {
                    int nextOffset;
                    int sizeOffset;
                    int targetSlice;

                    if (mips - 1 == mip) {
                        nextOffset = GetMipOffset(width, height, format, depth, isCubemap, mips, -1, -1);
                        sizeOffset = mips - 1;
                        targetSlice = ~-(int)(isCubemap == false) & 5;
                    } else {
                        nextOffset = GetMipOffset(width, height, format, depth, isCubemap, mips, mip + 1, 0);
                        sizeOffset = mip;
                        targetSlice = 0;
                    }

                    sizeOffset = GetMipOffset(width, height, format, depth, isCubemap, mips, sizeOffset, targetSlice);
                    int mipSize = nextOffset - sizeOffset;

                    if (!isCubemap) {
                        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                               0x56e);
                        glBindTexture(0x0DE1, texID);
                        glTexParameteri(0x0DE1, 0x2801, 0x2601);
                        glTexParameteri(0x0DE1, 0x2800, 0x2601);

                        if (g_loadDefaultTexture == 0) {
                            glCompressedTexImage2D(0x0DE1, mip, glInternalFormat, mipWidth, hLimit, 0, mipSize,
                                                   mipData);
                        } else {
                            loadDefaultTexture(texID, mip, mipWidth, 0x0DE1, 0x0DE1);
                        }
                        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                             0x589);
                    } else {
                        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                               0x54b);
                        glBindTexture(0x8513, texID);
                        glTexParameteri(0x8513, 0x2801, 0x2601);
                        glTexParameteri(0x8513, 0x2800, 0x2601);

                        if (g_loadDefaultTexture == 0) {
                            glCompressedTexImage2D(faceTarget, mip, glInternalFormat, mipWidth, hLimit, 0, mipSize,
                                                   mipData);
                        } else {
                            loadDefaultTexture(texID, mip, mipWidth, 0x8513, faceTarget);
                        }
                        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",
                                             0x566);

                        g_loadDefaultTexture = 0;
                    }
                }

            } while (((hLimit != 1) || (mipWidth != 1)) && (++mip != mips));
        }

        loopCounter = faceTarget - (0x8515 - 1);
        faceTarget++;

    } while (loopCounter < (-(int)(isCubemap == false) & 0xFFFFFFFB) + 6);

    if (!isCompressed) {
        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 0x5bf);
        u32 bindTarget = (-(int)(isCubemap == false) & 0xFFFF88CE) + 0x8513;
        glGenerateMipmap(bindTarget);
        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp", 0x5c1);
    }

    glFlush();
    g_loadDefaultTexture = 0;
}