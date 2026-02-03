#include <stdio.h>
#include <string.h>

#include "nu2api.saga/nucore/nustring.h"

void NuStrFormatAddress(char *buf, u32 buf_len, void *ptr) {
    char address[21];

    snprintf(address, sizeof(address), "0x%p", ptr);

    // Shift the address in case the native formatting includes `0x`.
    if (address[2] == '0' && address[3] == 'x') {
        memmove(address + 2, address + 4, sizeof(address) - 4);
    }

    strcpy(buf, address);
}

void NuStrFormatSize(char *buf, u32 buf_len, u32 size, bool align_left) {
    u32 remainder;
    u32 gb;
    u32 mb;
    u32 kb;
    u32 b;
    usize len;

    gb = size / 1000000000;
    remainder = size % 1000000000;

    mb = remainder / 1000000;
    remainder %= 1000000;

    kb = remainder / 1000;
    b = remainder % 1000;

    if (gb != 0) {
        snprintf(buf, buf_len, "%u,%03u,%03u,%03u", gb, mb, kb, b);
    } else if (mb != 0) {
        snprintf(buf, buf_len, "%u,%03u,%03u", mb, kb, b);
    } else if (kb != 0) {
        snprintf(buf, buf_len, "%u,%03u", kb, b);
    } else {
        snprintf(buf, buf_len, "%u", b);
    }

    len = strlen(buf);

    if (!align_left) {
        usize remaining = buf_len - len;

        memmove(buf + remaining - 1, buf, len + 1);
        memset(buf, 0x20, remaining - 1);
    } else {
        memset(buf + len, 0x20, buf_len - len - 1);
        buf[buf_len - 1] = '\0';
    }
}

const char *NuStrStripPath(const char *string) {
    const char *cursor;

    do {
        cursor = string;

        while (*cursor != '\\' && *cursor != '/') {
            if (*cursor == '\0') {
                return string;
            }

            cursor++;
        }

        string = cursor + 1;
    } while (*string != '\0');

    return string;
}
