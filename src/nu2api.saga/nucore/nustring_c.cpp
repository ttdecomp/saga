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

float NuAToF(char *string) {
    float dividend = 0.0f;
    float divisor = 1.0f;

    char c = *string;
    string++;

    if (c == '-') {
        divisor = -1.0f;

        c = *string;
        string++;
    }

    while (c >= '0' && c <= '9') {
        dividend *= 10.0f;
        dividend += (float)(c - 0x30);

        c = *string;
        string++;
    }

    if (c == '.') {
        c = *string;
        string++;

        while (c >= '0' && c <= '9') {
            divisor *= 10.0f;
            dividend *= 10.0f;
            dividend += (float)(c - 0x30);

            c = *string;
            string++;
        }
    }

    return dividend / divisor;
}

int NuAToI(char *string) {
    int value = 0;
    int sign = 0;

    char c = *string;
    string++;

    if (c == '-') {
        sign = -1;

        c = *string;
        string++;
    }

    while (c >= '0' && c <= '9') {
        value = (value << 3) + 2 * value;
        value = value + (int)c - 0x30;

        c = *string;
        string++;
    }

    if (sign != 0) {
        return value * sign;
    }

    return value;
}

int NuHexStringToI(char *string) {
    int value = 0;

    for (; *string != '\0'; string++) {
        value <<= 4;
        int upper = (int)NuToUpper(*string);

        if (upper <= '9' && upper >= '0') {
            value |= upper - 0x30;
        } else if (upper <= 'F' && upper >= 'A') {
            value |= upper - 0x37;
        } else {
            return 0;
        }
    }

    return value;
}

void NuUnicodeToAscii(char *dst, NUWCHAR16 *src) {
    int pos = 0;

    if (src == NULL) {
        return;
    }

    if (dst == NULL) {
        return;
    }

    *dst = '\0';

    for (; src[pos] != '\0'; pos++) {
        if ((src[pos] & 0xff00) != 0) {
            switch (src[pos]) {
                // U+2018 LEFT SINGLE QUOTATION MARK
                case 0x2018:
                    dst[pos] = 0x91;
                    break;
                // U+2019 RIGHT SINGLE QUOTATION MARK
                case 0x2019:
                    dst[pos] = 0x92;
                    break;
                // U+2013 EN DASH
                case 0x2013:
                    dst[pos] = 0x96;
                    break;
                // U+2026 HORIZONTAL ELLIPSIS
                case 0x2026:
                    dst[pos] = 0x85;
                    break;
                // U+2122 TRADE MARK SIGN
                case 0x2122:
                    dst[pos] = 0x99;
                    break;
                default:
                    dst[pos] = '?';
            }
        } else {
            dst[pos] = (char)src[pos];
        }
    }

    dst[pos] = '\0';
}

void NuStrUpr(char *dest, char *src) {
    char c;

    for (; *src != '\0'; src = src + 1) {
        c = NuToUpper(*src);
        *dest = c;
        dest = dest + 1;
    }
    *dest = *src;
}
