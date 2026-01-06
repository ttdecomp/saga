#pragma once

#include <stddef.h>
#include <stdint.h>

#include "decomp.h"

void NuStrCat(char *dest, const char *src);
int32_t NuStrNICmp(const char *a, const char *b, size_t n);
size_t NuStrLen(const char *str);
char NuToUpper(char c);
size_t NuStrCpy(char *dest, const char *src);