#include "nu2api.saga/nucore/nustring.h"

void NuStrCat(char *str, const char *ext) {
    while (*str != '\0') {
        str += 1;
    }

    if (ext != NULL) {
        while (1) {
            *str = *ext;
            str += 1;

            if (*ext == '\0') {
                break;
            }

            ext += 1;
        }
    }
}

int NuStrCmp(const char *a, const char *b) {
    char c1;
    char c2;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        c1 = *a;
        c2 = *b;

        if (c1 > c2) {
            return 1;
        }

        if (c1 < c2) {
            return -1;
        }

        a += 1;
        b += 1;
    } while (c1 != '\0' && c2 != '\0');

    return 0;
}

char *NuStrChr(char *src, char c) {
    while (*src != '\0') {
        if (*src == c) {
            return src;
        }

        src += 1;
    }

    return NULL;
}

size_t NuStrCpy(char *dst, const char *src) {
    char *orig = dst;

    if (src != NULL) {
        for (; *src != '\0'; src += 1) {
            *dst = *src;
            dst = dst + 1;
        }
    }

    *dst = '\0';

    return dst - orig;
}

void NuStrFixExtPlatform(char *dst, char *src, char *ext, int dst_size, char *platform_string) {
    char *p;
    char *sep;
    char *sep2;

    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }

    NuStrCpy(dst, src);

    p = NuStrRChr(dst, '.');
    sep = NuStrRChr(dst, '/');
    sep2 = NuStrRChr(dst, '\\');

    if (sep2 > sep) {
        sep = sep2;
    }

    if (sep >= p) {
        NuStrCat(dst, "_");

        if (platform_string != NULL) {
            NuStrCat(dst, platform_string);
        }

        NuStrCat(dst, ".");
        NuStrCat(dst, ext);
    } else {
        *p = '\0';
        NuStrCat(p, "_");

        if (platform_string != NULL) {
            NuStrCat(p, platform_string);
        }

        NuStrCat(p, ".");
        NuStrCat(p, ext);
    }

    NuStrLen(dst);
}

int NuStrICmp(const char *a, const char *b) {
    char c1;
    char c2;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        c1 = NuToUpper(*a);
        c2 = NuToUpper(*b);

        if (c1 > c2) {
            return 1;
        }

        if (c1 < c2) {
            return -1;
        }

        a += 1;
        b += 1;
    } while (c1 != '\0' && c2 != '\0');

    return 0;
}

size_t NuStrLen(const char *str) {
    size_t i = 0;

    for (; *str != '\0'; str += 1) {
        i++;
    }

    return i;
}

int NuStrNCmp(const char *a, const char *b, size_t n) {
    char c1;
    char c2;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        c1 = *a;
        c2 = *b;

        if (c1 > c2) {
            return 1;
        }

        if (c1 < c2) {
            return -1;
        }

        a++;
        b++;
        n--;
    } while (c1 != '\0' && c2 != '\0' && n != 0);

    return 0;
}

int NuStrNICmp(const char *a, const char *b, size_t n) {
    char c1;
    char c2;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        c1 = NuToUpper(*a);
        c2 = NuToUpper(*b);

        if (c1 > c2) {
            return 1;
        }

        if (c1 < c2) {
            return -1;
        }

        a++;
        b++;
        n--;
    } while (c1 != '\0' && c2 != '\0' && n != 0);

    return 0;
}

int NuStrNCpy(char *dst, const char *src, int n) {
    int count = 0;

    if (src == NULL) {
        *dst = '\0';
        return 0;
    }

    for (; *src != '\0'; src += 1) {
        n -= 1;
        if (n < 1) {
            *dst = '\0';
            count += 1;

            break;
        }

        *dst = *src;
        dst = dst + 1;

        count += 1;
    }

    return count - 1;
}

char *NuStrRChr(char *src, char c) {
    char *ptr = src;

    while (*ptr != '\0') {
        ptr += 1;
    }

    while (ptr >= src) {
        if (*ptr == c) {
            return ptr;
        }

        ptr -= 1;
    }

    return NULL;
}

unsigned char NuToLower(unsigned char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 0x20;
    } else if (c > 0xbf && c < 0xe0) {
        c += 0x20;
    }

    return c;
}

unsigned char NuToUpper(unsigned char c) {
    if (c >= 'a' && c <= 'z') {
        c -= 0x20;
    } else if (c > 0xdf) {
        c -= 0x20;
    }

    return c;
}
