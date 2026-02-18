#pragma once

#include <stddef.h>

#include "nu2api/nucore/common.h"

typedef u16 NUWCHAR;
typedef u8 NUWCHAR8;
typedef u16 NUWCHAR16;

typedef struct nustring_s {
    char *id;

    union {
        NUWCHAR *str_utf16;
        NUWCHAR8 *str_utf8;
    };
} NUSTRING;

typedef struct nustringbank_s {
    NUSTRING *strings;
    i32 string_count;
    i32 max_strings;
} NUSTRINGBANK;

#ifdef __cplusplus
extern "C" {
#endif
    void NuStrCat(char *str, const char *ext);
    i32 NuStrCmp(const char *a, const char *b);
    char *NuStrChr(char *src, char c);
    i32 NuStrCpy(char *dst, const char *src);
    void NuStrFixExtPlatform(char *dst, char *src, char *ext, i32 dst_size, char *platform_string);
    i32 NuStrICmp(const char *a, const char *b);
    char *NuStrIStr(char *str, char *sub);
    i32 NuStrLen(const char *str);
    i32 NuStrNCmp(const char *a, const char *b, i32 n);
    i32 NuStrNICmp(const char *a, const char *b, i32 n);
    i32 NuStrNCpy(char *dst, const char *src, i32 n);
    char *NuStrRChr(char *src, char c);
    void NuStrUpr(char *dst, const char *src);

    i32 NuStrCpyW(NUWCHAR *dst, NUWCHAR *src);
    i32 NuStrICmpW(NUWCHAR *a, NUWCHAR *b);
    i32 NuStrLenW(const NUWCHAR *str);

    u8 NuToLower(u8 c);
    u8 NuToUpper(u8 c);

    NUWCHAR NuToLowerW(NUWCHAR c);
    NUWCHAR NuToUpperW(NUWCHAR c);

    f32 NuAToF(char *string);
    i32 NuAToI(char *string);
    i32 NuHexStringToI(char *string);

    void NuAsciiToUnicode(NUWCHAR16 *dst, char *src);
    void NuUnicodeToAscii(char *dst, NUWCHAR16 *src);
    void NuUnicodeToUTF8(NUWCHAR8 *dst, NUWCHAR16 *src);

    i32 NuIsAlNum(char c);

    i32 NuStringTableLoadCSV(char *filepath, VARIPTR *buf, VARIPTR buf_end, char *label, char *language);

    void NuStringTableSetBank(i32 bank);
    void NuStringTableSetFormat(i32 format);

    NUWCHAR *NuStringTableGetByName(char *name);
#ifdef __cplusplus
}

void NuStrFormatAddress(char *buf, u32 buf_len, void *ptr);
void NuStrFormatSize(char *buf, u32 buf_len, u32 size, bool align_left);
const char *NuStrStripPath(const char *string);

#endif
