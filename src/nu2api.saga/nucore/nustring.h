#pragma once

#include <stddef.h>
#include <stdint.h>

#include "decomp.h"

C_API_START

void NuStrCat(char *str, const char *ext);
int NuStrCmp(const char *a, const char *b);
char *NuStrChr(char *src, char c);
size_t NuStrCpy(char *dest, const char *src);
void NuStrFixExtPlatform(char *dst, char *src, char *ext, int dst_size, char *platform_string);
int NuStrICmp(const char *a, const char *b);
size_t NuStrLen(const char *str);
int NuStrNCmp(const char *a, const char *b, size_t n);
int NuStrNICmp(const char *a, const char *b, size_t n);
int NuStrNCpy(char *dest, const char *src, int n);
char *NuStrRChr(char *src, char c);
unsigned char NuToLower(unsigned char c);
unsigned char NuToUpper(unsigned char c);

C_API_END