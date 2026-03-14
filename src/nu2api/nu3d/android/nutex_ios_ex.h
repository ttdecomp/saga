#pragma once

#include <GLES2/gl2.h>
#include "decomp.h"
#include "nu2api/nu3d/nutex.h"
#ifdef __cplusplus
extern "C" {
#endif
    extern int g_currentTexUnit;
    extern GLuint g_lastBound2DTexIds[16];
    extern GLuint g_lastBoundCubeTexIds[16];
    extern GLuint g_earlyColorFramebuffer;
    extern GLuint g_defaultFramebuffer;
    extern GLuint g_currentFramebuffer;
    extern GLuint g_earlyColorMSAAFramebuffer;
#ifdef __cplusplus
}
#endif

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

struct NuHardwareTexture {    
    NUTEXFORMAT format;           
    void* lp_raw_data;              
    void* lp_device_resource;      
    u32 data_size;       
    u32 ref_count;       
    u32 checksum;       
    u16 unknown;      
    u16 width;        
    u16 height;       
    u16 depth;        
    u8  mips;         
    u8  type;         
    u8  flags; // 50/50 on this being a enum or a bitfield idk tbh        
    u8  pad;          
};


// clang-format off
static const u8 FormatIsCompressedTable[128] = {
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const u8 FormatBytesPerElementTable[128] = {
    8, 8, 16, 16, 16, 16, 4, 8, 16, 0, 0, 0, 0, 0, 0, 1, 
    8, 0, 16, 8,  8,  8,  8, 16, 8, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0,  0,  0,  4,  4, 0,  4, 0, 0, 0, 0, 8, 0, 0, 
    0, 4, 0,  4,  8,  8,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0 
};

static const u8 FormatBlockWidthTable[128] = {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  
};

static const u8 FormatMinBlocksYTable[128] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  
};

// clang-format on

GLuint NuIOS_CreateGLTexFromFile(const char *filename);
GLuint NuIOS_CreateGLTexFromPlatformInMemory(void *data, int *width, int *height, bool is_pvrtc);
GLuint loadDefaultTexture(GLuint texture, GLint level, GLsizei size, GLenum texture_type, GLenum target);
GLuint CreateTexturePS(void);
void GetNativeTextureFormat(NUTEXFORMAT inFormat, int &outBpp, u32 &outInternalFormat, u32 &outType, u32 &outFormat, bool &outIsCompressed, NUTEXFORMAT &outFormatEnum);
void GetTextureFormatInfo(NUTEXFORMAT texture, u32 &param_2, u32 &param_3);
int GetMipLevelSize(NUTEXFORMAT param_1, int unclamped_width, int unclamped_height);
int GetMipOffset(NuHardwareTexture *tex, int targetMip, int targetSlice, NUTEXFORMAT overrideFormat);
void DecompressTextureToRGBA(u8 *ddsData, u32 size, u8 *&outBuffer);
void UnlockTexturePS(u32 texID, void *pixels, int width, int height, int depth, bool isCubemap, int mips, NUTEXFORMAT format, u32 &glFormat, u32 &glInternalFormat, u32 glType, bool isCompressed);