#include "nu2api.saga/nucore/nustring.h"

void NuStrCat(char *str, const char *ext) {
    while (*str != '\0') {
        str++;
    }

    if (ext != NULL) {
        do {
            *str = *ext;
            str++;
        } while (*(ext++) != '\0');
    }
}

int NuStrCmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = *a;
        b_cursor = *b;

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

char *NuStrChr(char *src, char c) {
    while (*src != '\0') {
        if (*src == c) {
            return src;
        }

        src++;
    }

    return NULL;
}

ssize_t NuStrCpy(char *dst, const char *src) {
    char *dst_start = dst;

    if (src != NULL) {
        while (*src != '\0') {
            *dst = *src;
            dst++;
            src++;
        }
    }

    *dst = '\0';

    return dst - dst_start;
}

void NuStrFixExtPlatform(char *dst, char *src, char *ext, int dst_size, char *platform_string) {
    char *period;
    char *sep;
    char *win_sep;

    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }

    NuStrCpy(dst, src);

    period = NuStrRChr(dst, '.');

    sep = NuStrRChr(dst, '/');
    win_sep = NuStrRChr(dst, '\\');
    if (win_sep > sep) {
        sep = win_sep;
    }

    if (sep >= period) {
        NuStrCat(dst, "_");

        if (platform_string != NULL) {
            NuStrCat(dst, platform_string);
        }

        NuStrCat(dst, ".");
        NuStrCat(dst, ext);
    } else {
        *period = '\0';
        NuStrCat(period, "_");

        if (platform_string != NULL) {
            NuStrCat(period, platform_string);
        }

        NuStrCat(period, ".");
        NuStrCat(period, ext);
    }

    if (NuStrLen(dst) == dst_size - 1) {
        // Nothing here. Presumably some gated code in the original.
    }
}

int NuStrICmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = NuToUpper(*a);
        b_cursor = NuToUpper(*b);

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

char *NuStrIStr(char *str, char *sub) {
    char *str_cursor;
    char *sub_cursor;

    while (*str != '\0') {
        str_cursor = str;
        sub_cursor = sub;

        while (*sub_cursor != '\0') {
            if (*str_cursor == '\0') {
                break;
            }

            if (NuToUpper(*str_cursor) != NuToUpper(*sub_cursor)) {
                break;
            }

            str_cursor++;
            sub_cursor++;
        }

        if (*sub_cursor == '\0') {
            return str;
        }

        str++;
    }

    return NULL;
}

ssize_t NuStrLen(const char *str) {
    ssize_t count = 0;

    while (*str != '\0') {
        count++;
        str++;
    }

    return count;
}

ssize_t NuStrLenW(const NUWCHAR *str) {
    ssize_t count = 0;

    while (*str != 0) {
        count++;
        str++;
    }

    return count;
}

int NuStrNCmp(const char *a, const char *b, ssize_t n) {
    char a_cursor;
    char b_cursor;

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
        a_cursor = *a;
        b_cursor = *b;

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

int NuStrNICmp(const char *a, const char *b, ssize_t n) {
    char a_cursor;
    char b_cursor;

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
        a_cursor = NuToUpper(*a);
        b_cursor = NuToUpper(*b);

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

ssize_t NuStrNCpy(char *dst, const char *src, ssize_t n) {
    ssize_t count = 0;

    if (src == NULL) {
        *dst = '\0';
        return 0;
    }

    do {
        n--;
        if (n < 1) {
            *dst = '\0';
            count++;

            break;
        }

        *dst = *src;
        dst++;

        count++;
    } while (*(src++) != '\0');

    return count - 1;
}

char *NuStrRChr(char *src, char c) {
    char *ptr = src;

    while (*ptr != '\0') {
        ptr++;
    }

    while (ptr >= src) {
        if (*ptr == c) {
            return ptr;
        }

        ptr--;
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
