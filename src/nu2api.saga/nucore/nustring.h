#pragma once

#include <stddef.h>

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef u16 NUWCHAR;
    typedef u16 NUWCHAR16;

    void NuStrCat(char *str, const char *ext);
    int NuStrCmp(const char *a, const char *b);
    char *NuStrChr(char *src, char c);
    int NuStrCpy(char *dest, const char *src);
    void NuStrFixExtPlatform(char *dst, char *src, char *ext, int dst_size, char *platform_string);
    int NuStrICmp(const char *a, const char *b);
    char *NuStrIStr(char *str, char *sub);
    int NuStrLen(const char *str);
    int NuStrLenW(const NUWCHAR *str);
    int NuStrNCmp(const char *a, const char *b, int n);
    int NuStrNICmp(const char *a, const char *b, int n);
    int NuStrNCpy(char *dest, const char *src, int n);
    char *NuStrRChr(char *src, char c);
    void NuStrUpr(char *dest, const char *src);

    unsigned char NuToLower(unsigned char c);
    unsigned char NuToUpper(unsigned char c);

    f32 NuAToF(char *string);
    int NuAToI(char *string);
    int NuHexStringToI(char *string);

    void NuUnicodeToAscii(char *dst, NUWCHAR16 *src);

    int NuIsAlNum(char c);
#ifdef __cplusplus
}

void NuStrFormatAddress(char *buf, u32 buf_len, void *ptr);
void NuStrFormatSize(char *buf, u32 buf_len, u32 size, bool align_left);
const char *NuStrStripPath(const char *string);

#endif
