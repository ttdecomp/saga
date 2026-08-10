#include "gamelib/crc/crc.h"

#include "nu2api/nucore/nustring.h"

static i32 g_crc_initialised = 0;
i32 *g_crc_table = NULL;

#define CRC32_POLY 0x04C11DB7

static u32 CRC_CalculateTableValue(u32 crc) {
    if (crc & 0x80000000) {
        return (crc << 1) ^ CRC32_POLY;
    }
    return crc << 1;
}

void CRC_Init(VARIPTR *buffer_start, VARIPTR) {
    if (g_crc_initialised) {
        return;
    }

    i32 *table = (i32 *)ALIGN(buffer_start->addr, alignof(i32));
    g_crc_table = table;
    buffer_start->addr = (usize)(table + 0x100);

    for (u32 i = 0; i < 0x100; i++) {
        u32 crc = i << 24;

        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);
        crc = CRC_CalculateTableValue(crc);

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
