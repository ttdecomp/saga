#pragma once

#include <stddef.h>
#include <stdint.h>

void NuStrCat(char *str, const char *ext);
size_t NuStrCpy(char *dest, const char *src);
size_t NuStrLen(const char *str);
int32_t NuStrNICmp(const char *a, const char *b, size_t n);
unsigned char NuToUpper(unsigned char c);
