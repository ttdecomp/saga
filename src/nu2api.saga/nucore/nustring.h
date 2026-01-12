#pragma once

#include <stddef.h>
#include <stdint.h>

#include "decomp.h"

C_API_START

typedef unsigned short NUWCHAR;

void NuStrCat(char *str, const char *ext);
int NuStrCmp(const char *a, const char *b);
char *NuStrChr(char *src, char c);
ssize_t NuStrCpy(char *dest, const char *src);
void NuStrFixExtPlatform(char *dst, char *src, char *ext, int dst_size, char *platform_string);
int NuStrICmp(const char *a, const char *b);
char * NuStrIStr(char *str, char *sub);
ssize_t NuStrLen(const char *str);
ssize_t NuStrLenW(const NUWCHAR *str);
int NuStrNCmp(const char *a, const char *b, ssize_t n);
int NuStrNICmp(const char *a, const char *b, ssize_t n);
ssize_t NuStrNCpy(char *dest, const char *src, ssize_t n);
char *NuStrRChr(char *src, char c);
unsigned char NuToLower(unsigned char c);
unsigned char NuToUpper(unsigned char c);

C_API_END
