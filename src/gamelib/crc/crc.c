#include "gamelib/crc/crc.h"

#include "nu2api.saga/nucore/nustring.h"

static i32 g_crc_initialised = 0;
static i32 *g_crc_table = NULL;

#define CRC32_POLY 0x04C11DB7

void CRC_Init(VARIPTR *buffer_start) {
    if (g_crc_initialised) {
        return;
    }

    g_crc_table = BUFFER_ALLOC_ARRAY(&buffer_start->void_ptr, 0x100, i32);

    for (u32 i = 0; i < 0x100; i++) {
        u32 crc = i << 24;

        for (i32 bit = 0; bit < 8; bit++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ CRC32_POLY;
            } else {
                crc <<= 1;
            }
        }

        g_crc_table[i] = crc;
    }

    g_crc_initialised = 1;
}

u32 CRC_ProcessStringIgnoreCase(const char *str) {
    u32 hash = 0;

    for (char c = *str; c != '\0'; c = *(str++)) {
        u32 value = (u32)NuToUpper(c) ^ (hash >> 24);
        hash = (hash << 8) ^ g_crc_table[value];
    }

    return hash;
}
