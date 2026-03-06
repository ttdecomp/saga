#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nutex.h"
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"

int g_currentTexUnit = -1;
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


GLuint NuIOS_CreateGLTexFromMemoryDDS(void* ddsPointer, int* out_width, int* out_height) {
  char cVar1;
  uchar *puVar2;
  GLuint uVar3;
  bool out_is_cube_map;
  bool out_has_four_cc;
  bool local_a0;
  NUTEXFORMAT local_90;
  i32 out_depth;
  i32 out_mip_count;
  u32 local_60;
  u32 local_50;
  u32 local_40;
  int local_30;
  NUTEXFORMAT local_20;
  uchar *local_14;
  
  uVar3 = 0;
  local_14 = (uchar *)0x2afcfd;
  out_depth = 0;
  out_mip_count = 0;
  out_is_cube_map = false;
  out_has_four_cc = false;
  cVar1 = NuDDSGetTextureDescription((const char*)ddsPointer,local_90,*out_width,*out_height,out_depth,out_mip_count,out_is_cube_map,&out_has_four_cc);
  if ((cVar1 != '\0') && (((*out_width != 0 || (*out_height != 0)) || (1 < out_mip_count)))) {
    uVar3 = CreateTexturePS();
    GetNativeTextureFormat(local_90,local_30,local_60,local_40,local_50,local_a0,local_20);
    puVar2 = (uchar *)((int)ddsPointer + 0x80);
    if (local_90 == NUTEX_DXT5) {
      local_14 = (uchar *)0x0;
      //DecompressTextureToRGBA(param_1,0,&local_14);
      local_90 = NUTEX_RGBA32;
      local_a0 = 0;
      local_60 = 0x1908;
      local_50 = 0x1908;
      puVar2 = local_14;
    }
    //UnlockTexturePS(uVar3,puVar2,*param_2,*param_3,local_80[0],local_c0[0],local_70[0],local_90[0],local_50,local_60,local_40[0],local_a0[0]);
  }
  return uVar3;
}



GLuint CreateTexturePS(void)
{
  GLuint local_10;
  
  local_10 = 0;
  BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",0x1ad);
  glGenTextures(1,&local_10);
  EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nutex_ios_ex.cpp",0x1b1);
  return local_10;
}

/* GetNativeTextureFormat(NUTEXFORMAT, int&, unsigned int&, unsigned int&, unsigned int&, bool&, NUTEXFORMAT&) */

void GetNativeTextureFormat(
    NUTEXFORMAT inFormat,
    int& outBpp,
    unsigned int& outInternalFormat,
    unsigned int& outType,
    unsigned int& outFormat,
    bool& outIsCompressed,
    NUTEXFORMAT& outFormatEnum) 
{
    // Declaring as int forces the compiler to allocate a unique local stack slot ([esp+0x4])
    // preventing it from fusing with the inFormat enum parameter.
    int formatToCheck = inFormat;

    if (inFormat == NUTEX_DXT1) {
        if (g_renderDevice.enabled_extensions[NUTEX_DXT1] != '\0') {
            outBpp = 0;
        } 
        else if (g_renderDevice.enabled_extensions[NUTEX_ETC1] != '\0') {
            inFormat = NUTEX_ETC1;
            formatToCheck = NUTEX_ETC1;
            outBpp = 0;
        } 
        else {
            outIsCompressed = false;
            inFormat = (NUTEXFORMAT)7; // NUTEX_RGBA32
            outBpp = 0;
        }
    } 
    else {
        outIsCompressed = false;
        outBpp = 0;
    }

    // Map NUTEXFORMAT to OpenGL native constants
    switch (inFormat) {
        case NUTEX_DXT1: // 1
            outInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;  // 0x83F0
            outIsCompressed = true;
            outFormat = GL_RGB;                                   // 0x1907
            outType = GL_UNSIGNED_BYTE;                           // 0x1401
            break;
            
        case NUTEX_DX1A: // 2
            outInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; // 0x83F1
            outIsCompressed = true;
            outFormat = GL_RGBA;                                  // 0x1908
            outType = GL_UNSIGNED_BYTE;                           // 0x1401
            break;
            
        case NUTEX_DXT5: // 6
            outInternalFormat = GL_RGBA;                          // 0x1908
            outIsCompressed = true;
            outFormat = GL_RGBA;                                  // 0x1908
            outType = GL_UNSIGNED_BYTE;                           // 0x1401
            break;
            
        case 0x10: // Likely NUTEX_LUMINANCE
            outInternalFormat = GL_LUMINANCE;                     // 0x1909
            outBpp = 8;
            outFormat = GL_LUMINANCE;                             // 0x1909
            outType = GL_UNSIGNED_BYTE;                           // 0x1401
            outFormatEnum = inFormat;
            return;
            
        case NUTEX_ETC1: // 0x11
            outInternalFormat = GL_ETC1_RGB8_OES;                 // 0x8D64
            outIsCompressed = true;
            outFormat = GL_RGB;                                   // 0x1907
            outType = GL_UNSIGNED_BYTE;                           // 0x1401
            break;
            
        case NUTEX_PVRTC2: // 0x14
            outInternalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;  // 0x8C01
            outIsCompressed = true;
            outFormat = GL_RGB;                                      // 0x1907
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            break;
            
        case NUTEX_PVRTC2A: // 0x15
            outInternalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG; // 0x8C03
            outIsCompressed = true;
            outFormat = GL_RGBA;                                     // 0x1908
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            break;
            
        case NUTEX_PVRTC4: // 0x16
            outInternalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;  // 0x8C00
            outIsCompressed = true;
            outFormat = GL_RGB;                                      // 0x1907
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            break;
            
        case NUTEX_PVRTC4A: // 0x17
            outInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; // 0x8C02
            outIsCompressed = true;
            outFormat = GL_RGBA;                                     // 0x1908
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            break;
            
        case NUTEX_ATCA: // 0x18
            outInternalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;  // 0x87EE
            outIsCompressed = true;
            outFormat = GL_RGBA;                                     // 0x1908
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            break;
            
        case NUTEX_ATC: // 0x19
            outInternalFormat = GL_ATC_RGB_AMD;                      // 0x8C92
            outIsCompressed = true;
            outFormat = GL_RGB;                                      // 0x1907
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            break;
            
        case 0x66: // Unknown hex extension format
            outInternalFormat = GL_RGB;                              // 0x1907
            outBpp = 24;                                             // 0x18
            outFormat = GL_RGB;                                      // 0x1907
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            outFormatEnum = inFormat;
            return;
            
        case 0x69:
        case 0x72:
            outFormatEnum = inFormat;
            return;
            
        default:
            // Unhandled formats default to uncompressed RGBA
            outInternalFormat = GL_RGBA;                             // 0x1908
            outBpp = 32;                                             // 0x20
            outFormat = GL_RGBA;                                     // 0x1908
            outType = GL_UNSIGNED_BYTE;                              // 0x1401
            outFormatEnum = inFormat;
            return;
    }

    // Capability check for compressed formats
    if (g_renderDevice.enabled_extensions[formatToCheck] == '\0') {
        // FATAL ERROR: Format not supported! Engine hangs intentionally.
        while (true) {} 
    }

    outFormatEnum = inFormat;
}