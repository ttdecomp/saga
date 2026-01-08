#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void NuStrCat(char *str, const char *ext);
char *NuStrChr(char *src, char c);
size_t NuStrCpy(char *dest, const char *src);
size_t NuStrLen(const char *str);
int32_t NuStrNICmp(const char *a, const char *b, size_t n);
unsigned char NuToLower(unsigned char c);
unsigned char NuToUpper(unsigned char c);

#ifdef __cplusplus
}
#endif
