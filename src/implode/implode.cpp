#include <cstdint>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "implode/implode.h"

#include "nu2api.saga/nucore/common.h"

static int gExplodeInitialised = 1;

char *implode_inbuffer;
char *implode_outbuffer;
unsigned int implode_origsize;
unsigned int implode_compsize;

static int bytes_to_copy;

static void decode_start() {
    ImplodeHufDecodeStart();

    bytes_to_copy = 0;
}

static void decode(unsigned int size, unsigned char *buf) {
    static unsigned int i;

    unsigned int n;

    n = 0;
    unsigned int c;

    while (--bytes_to_copy >= 0) {
        buf[n] = buf[i];
        i = (i + 1) & 0x1fff;

        if (++n == size) {
            return;
        }
    }

    while (true) {
        c = ImplodeDecodeC();

        if (c < 0x100) {
            buf[n] = c;

            if (++n == size) {
                return;
            }
        } else {
            bytes_to_copy = c - 0xfd;
            i = (n - ImplodeDecodeOffset() - 1) & 0x1fff;

            while (--bytes_to_copy >= 0) {
                buf[n] = buf[i];
                i = (i + 1) & 0x1fff;

                if (++n == size) {
                    return;
                }
            }
        }
    }
}

static unsigned char buffer[0x2000];

ssize_t ExplodeBufferNoHeader(char *in_buf, char *out_buf, ssize_t compressed_size, ssize_t orig_size) {
    int total_read;
    unsigned int read_size;

    if (gExplodeInitialised == 0) {
        return 0;
    }

    implode_inbuffer = in_buf;
    implode_outbuffer = out_buf;
    implode_origsize = orig_size;
    implode_compsize = compressed_size;

    decode_start();

    total_read = 0;
    while (implode_origsize != 0) {
        read_size = MIN(implode_origsize, 0x2000);

        decode(read_size, buffer);

        memcpy(implode_outbuffer, buffer, read_size);

        implode_outbuffer += read_size;
        implode_origsize -= read_size;
        total_read += read_size;
    }

    return total_read;
}

ssize_t ExplodeBufferSize(char *buf) {
    char magic[] = "LZ2K";

    for (int32_t i = 0; i < 4; i++) {
        int does_not_match;
        if ((does_not_match = *(buf++) != magic[i]) != 0) {
            return 0;
        }
    }

    int32_t size = ImplodeGetI(buf, 4);
    return size;
}

ssize_t ExplodeCompressedSize(char *buf) {
    char magic[] = "LZ2K";

    for (int32_t i = 0; i < 4; i++) {
        int does_not_match;
        if ((does_not_match = *(buf++) != magic[i]) != 0) {
            return 0;
        }
    }

    buf += 4;
    int32_t size = ImplodeGetI(buf, sizeof(int32_t));
    if (size != 0) {
        size += 0xc;
    }

    return size;
}

static uint32_t getmasktbl[5] = {0, 0xff, 0xffff, 0xffffff, 0xffffffff};

int32_t ImplodeGetI(void *buf, int32_t size) {
    unsigned char *char_ptr;
    unsigned int buf_reversed;

    char_ptr = (unsigned char *)buf;
    buf_reversed = *char_ptr | *(char_ptr + 1) << 0x8 | *(char_ptr + 2) << 0x10 | *(char_ptr + 3) << 0x18;

    return buf_reversed & getmasktbl[size];
}

#define SELECT(dst, table, shift, count, initial_mask, len)                                                            \
    ({                                                                                                                 \
        (dst) = (table)[implode_bitbuf >> shift];                                                                      \
                                                                                                                       \
        if ((dst) > (count)) {                                                                                         \
            uint32_t mask = (initial_mask);                                                                            \
                                                                                                                       \
            do {                                                                                                       \
                if ((implode_bitbuf & mask) != 0) {                                                                    \
                    (dst) = implode_right[(dst)];                                                                      \
                } else {                                                                                               \
                    (dst) = implode_left[(dst)];                                                                       \
                }                                                                                                      \
                                                                                                                       \
                mask >>= 1;                                                                                            \
            } while ((dst) > (count));                                                                                 \
        }                                                                                                              \
                                                                                                                       \
        ImplodeFillBuf((len)[(dst)]);                                                                                  \
    })

static uint32_t blocksize;

static uint16_t offset_table[0x100];
static uint8_t offset_len[0x13];

static void read_offset_len(int32_t table_size, int32_t bit_count, int32_t zero_value) {
    int32_t n;
    int32_t i;
    int32_t value;

    n = ImplodeGetBits(bit_count);
    if (n == 0) {
        value = ImplodeGetBits(bit_count);

        for (i = 0; i < table_size; i++) {
            offset_len[i] = 0;
        }

        for (i = 0; i < 0x100; i++) {
            offset_table[i] = value;
        }

        return;
    }

    i = 0;
    while (i < n) {
        value = implode_bitbuf >> 0x1d;

        if (value == 7) {
            for (uint32_t k = 0x10000000; (implode_bitbuf & k) != 0; k >>= 1, value++) {
            }
        }

        ImplodeFillBuf(value >= 7 ? value - 3 : 3);

        offset_len[i] = value;
        i++;

        if (i == zero_value) {
            value = ImplodeGetBits(2);

            while (--value >= 0) {
                offset_len[i] = 0;
                i++;
            }
        }
    }

    for (; i < table_size; i++) {
        offset_len[i] = 0;
    }

    ImplodeMakeTable(table_size, offset_len, 8, offset_table);
}

static uint16_t literal_table[0x1000];
static uint8_t literal_len[0x1fe];

static void read_literal_len() {
    int32_t n;
    int32_t i;
    int32_t value;

    n = ImplodeGetBits(9);
    if (n == 0) {
        value = ImplodeGetBits(9);

        for (i = 0; i < sizeof(literal_len); i++) {
            literal_len[i] = 0;
        }

        for (i = 0; i < 0x1000; i++) {
            literal_table[i] = value;
        }

        return;
    }

    i = 0;
    while (i < n) {
        SELECT(value, offset_table, 0x18, 0x12, 0x800000, offset_len);

        if (value < 3) {
            if (value == 0) {
                value = 1;
            } else if (value == 1) {
                value = ImplodeGetBits(4) + 0x3;
            } else {
                value = ImplodeGetBits(9) + 0x14;
            }

            while (--value >= 0) {
                literal_len[i] = 0;
                i++;
            }
        } else {
            literal_len[i] = value - 2;
            i++;
        }
    }

    for (; i < sizeof(literal_len); i++) {
        literal_len[i] = 0;
    }

    ImplodeMakeTable(sizeof(literal_len), literal_len, 0xc, literal_table);
}

uint16_t implode_left[0x3fb];
uint16_t implode_right[0x3fb];

uint32_t ImplodeDecodeC() {
    uint32_t value;

    if (blocksize == 0) {
        blocksize = ImplodeGetBits(0x10);

        read_offset_len(0x13, 5, 3);
        read_literal_len();
        read_offset_len(0xe, 4, -1);
    }

    blocksize--;

    SELECT(value, literal_table, 0x14, sizeof(literal_len) - 1, 0x80000, literal_len);

    return value;
}

uint32_t ImplodeDecodeOffset() {
    uint32_t offset;

    SELECT(offset, offset_table, 0x18, 0xd, 0x800000, offset_len);

    if (offset != 0) {
        offset = (1u << (offset - 1)) + ImplodeGetBits(offset - 1);
    }

    return offset;
}

void ImplodeMakeTable(int32_t size, uint8_t *len, int32_t unknown, uint16_t *table) {
    int32_t shift;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t i;
    uint32_t j;
    uint16_t symbol_count[17];
    uint16_t masks[17];
    uint16_t offsets[18];

    for (i = 1; i < 0x11; i++) {
        symbol_count[i] = 0;
    }

    for (i = 0; i < size; i++) {
        symbol_count[len[i]]++;
    }

    offsets[1] = 0;

    for (i = 1; i < 0x11; i++) {
        offsets[i + 1] = offsets[i] + (symbol_count[i] << (0x10 - i));
    }

    if (offsets[0x11] != 0) {
        ImplodeError("Bad table");
    }

    shift = 0x10 - unknown;

    for (i = 1; i <= unknown; i++) {
        offsets[i] >>= shift;
        masks[i] = 1 << (unknown - i);
    }

    for (; i < 0x11; i++) {
        masks[i] = 1 << (0x10 - i);
    }

    i = (int32_t)offsets[unknown + 1] >> shift;
    if (i != 0) {
        for (j = 1 << unknown; i != j; i++) {
            table[i] = 0;
        }
    }

    unknown2 = size;
    unknown3 = 1 << (0xf - unknown);

    for (uint32_t symbol = 0; symbol < size; symbol++) {
        uint32_t length;
        uint32_t unknown4;

        if ((length = len[symbol]) == 0) {
            continue;
        }

        unknown4 = offsets[length] + masks[length];
        if (unknown >= length) {
            for (i = offsets[length]; i < unknown4; i++) {
                table[i] = symbol;
            }
        } else {
            uint16_t *cursor;

            j = offsets[length];
            cursor = table + (j >> shift);

            for (uint32_t i = length - unknown; i != 0; i--) {
                if (*cursor == 0) {
                    implode_left[unknown2] = 0;
                    implode_right[unknown2] = implode_left[unknown2];
                    *cursor = unknown2;

                    unknown2++;
                }

                if ((j & unknown3) != 0) {
                    cursor = implode_right + *cursor;
                } else {
                    cursor = implode_left + *cursor;
                }

                j <<= 1;
            }

            *cursor = symbol;
        }

        offsets[length] = unknown4;
    }
}

void ImplodeHufDecodeStart() {
    ImplodeInitGetBits();

    blocksize = 0;
}

unsigned int implode_bitbuf;
static unsigned int subbitbuf;
static unsigned int bitcount;

void ImplodeInitGetBits() {
    implode_bitbuf = 0;
    subbitbuf = 0;
    bitcount = 0;

    ImplodeFillBuf(0x20);
}

uint32_t ImplodeGetBits(int32_t count) {
    uint32_t value;

    if (count == 0) {
        return 0;
    }

    value = implode_bitbuf >> (0x20u - count);

    ImplodeFillBuf(count);

    return value;
}

void ImplodeFillBuf(int32_t count) {
    implode_bitbuf <<= count;

    while (bitcount < count) {
        implode_bitbuf |= subbitbuf << (count -= bitcount);

        if (implode_compsize != 0) {
            implode_compsize--;

            subbitbuf = ImplodeGetByteFromMem() & 0xff;
        } else {
            subbitbuf = 0;
        }

        bitcount = 8;
    }

    implode_bitbuf |= subbitbuf >> (bitcount -= count);
}

unsigned char ImplodeGetByteFromMem() {
    return *implode_inbuffer++;
}

void ImplodeError(char *msg, ...) {
    va_list args;
    va_start(args, msg);

    putc('\n', stderr);
    vfprintf(stderr, msg, args);
    putc('\n', stderr);

    exit(1);
}
