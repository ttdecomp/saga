#include "nu2api.saga/nustr/nustr.h"

void NuStrCat(char *dest, const char *src) {
    for (; *dest != '\0'; dest = dest + 1) {
    }

    if (src != NULL) {
        char c;

        do {
            *dest = *src;
            dest = dest + 1;
            c = *src;
            src = src + 1;
        } while (c != '\0');
    }
}

int32_t NuStrNICmp(const char *a, const char *b, size_t n) {
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

    while (*a != '\0' && *b != '\0' && n > 0) {
        char c1 = NuToUpper(*a);
        char c2 = NuToUpper(*b);

        if (c2 < c1) {
            return 1;
        }

        if (c1 < c2) {
            return -1;
        }

        a++;
        b++;
        n--;
    }

    return 0;
}

size_t NuStrLen(const char *str) {
    size_t i = 0;
    for (; *str != '\0'; str = str + 1) {
        i++;
    }
    return i;
}

char NuToUpper(char c) {
    if (c < 'a' || 'z' < c) {
        if (0xdf < c) {
            return c - 0x20;
        } else {
            return c;
        }
    } else {
        return c - 0x20;
    }
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
