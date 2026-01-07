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

char *NuStrChr(char *src, char c) {
    do {
        if (*src == '\0') {
            return NULL;
        }

        src += 1;
    } while (*src != c);

    return src;
}

size_t NuStrCpy(char *dest, const char *src) {
    char *orig = dest;

    if (src != NULL) {
        for (; *src != '\0'; src = src + 1) {
            *dest = *src;
            dest = dest + 1;
        }
    }

    *dest = '\0';

    return (size_t)dest - (size_t)orig;
}

size_t NuStrLen(const char *str) {
    size_t i = 0;

    for (; *str != '\0'; str = str + 1) {
        i++;
    }

    return i;
}

int32_t NuStrNICmp(const char *a, const char *b, size_t n) {
    char c1;
    char c2;

    if (a == NULL) {
        return -1;
    } else if (b == NULL) {
        return 1;
    } else if (n == 0) {
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

        if (c1 == '\0' || c2 == '\0') {
            break;
        }
    } while (n != 0);

    return 0;
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
