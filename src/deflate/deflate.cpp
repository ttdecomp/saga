#include "deflate.h"

#include <stdint.h>
#include <string.h>

#include "decomp.h"

static inline int32_t ReverseBits(int32_t x) {
    x = (x & 0xaaaa) >> 1 | (x & 0x5555) << 1;
    x = (x & 0xcccc) >> 2 | (x & 0x3333) << 2;
    x = (x & 0xf0f0) >> 4 | (x & 0xf0f) << 4;
    x = (x & 0xff00) >> 8 | (x & 0xff) << 8;
    return x;
}

uint32_t BuildHuffmanTree(DEFHUFFMAN *tree, uint8_t *codeLengths, int32_t symbolCount) {
    uint32_t nextCode[16];
    uint32_t lengthCount[17];
    uint32_t symbolTableIndex;
    int32_t code;

    LOG_DEBUG("tree=%p, codeLengths=%p, symbolCount=%d", tree, codeLengths, symbolCount);

    memset(lengthCount, 0, sizeof(lengthCount));
    memset(tree->fastLookup, -1, sizeof(tree->fastLookup));

    for (int32_t i = 0; i < symbolCount; i++) {
        lengthCount[codeLengths[i]]++;
    }

    lengthCount[0] = 0;

    nextCode[1] = 0;

    tree->firstCode[1] = 0;
    tree->numCodes[1] = 0;
    tree->baseCode[0] = lengthCount[1] << 15;

    for (uint32_t len = 2; len < 16; len++) {
        nextCode[len] = (nextCode[len - 1] + lengthCount[len - 1]) << 1;

        tree->firstCode[len] = nextCode[len];
        tree->numCodes[len] = tree->numCodes[len - 1] + lengthCount[len - 1];
        tree->baseCode[len - 1] = (nextCode[len] + lengthCount[len]) << (0x10 - len);
    }

    tree->baseCode[15] = 0x10000;

    for (int32_t i = 0; i < symbolCount; i++) {
        int32_t value = codeLengths[i];

        if (value != 0) {
            code = nextCode[value];
            symbolTableIndex = (code - tree->firstCode[value]) + tree->numCodes[value];

            tree->symbols[symbolTableIndex] = value;
            tree->symbolIndex[symbolTableIndex] = i;

            if (value < 10) {
                int32_t j = ReverseBits(code) >> (0x10 - value);

                do {
                    tree->fastLookup[j] = (int16_t)symbolTableIndex;
                    j = j + (1 << value);
                } while (j < 512);
            }

            nextCode[value] = code + 1;
        }
    }

    return 1;
}

int32_t InflateBuffer(char *buffer, int decodedSize, char *readBuffer, int32_t readBufferSize) {
    DEFLATECONTEXT ctx;

    LOG_DEBUG("buffer=%p, decodedSize=%d, readBuffer=%p, readBufferSize=%d", buffer, decodedSize, readBuffer,
              readBufferSize);

    ctx.readBuffer = (uint8_t *)readBuffer;
    ctx.readBufferEnd = (uint8_t *)readBuffer + readBufferSize;

    if (Inflate(&ctx, buffer, decodedSize) == 0) {
        return -1;
    } else {
        return (size_t)ctx.currentPos - (size_t)ctx.startPos;
    }
}

int32_t ExplodeBufferSize(uint8_t *buf) {
    char magic[] = "LZ2K";

    for (int32_t i = 0; i < 4; i++) {
        if (buf[i] != magic[i])
            return 0;
    }

    return ImplodeGetI(buf, 4);
}

int32_t ExplodeCompressedSize(uint8_t *buffer) {
    char magic[] = "LZ2K";

    for (int32_t i = 0; i < 4; i++) {
        if (buffer[i] != magic[i])
            return 0;
    }

    int32_t size = ImplodeGetI(buffer + 4, 4);
    if (size != 0) {
        size += 0xc;
    }

    return size;
}

int32_t Inflate(DEFLATECONTEXT *ctx, char *buffer, int32_t size) {
    LOG_DEBUG("ctx=%p, buffer=%p, size=%d", ctx, buffer, size);

    ctx->startPos = buffer;
    ctx->currentPos = buffer;
    ctx->endPos = buffer + size;
    return DecodeDeflated(ctx);
}

static uint8_t DefaultLengths[288] = {0};
static uint8_t DefaultDistances[32] = {0};

void InitHuffmanDefaults() {
    for (int32_t i = 0; i < 144; i++) {
        DefaultLengths[i] = 8;
    }

    for (int32_t i = 144; i < 256; i++) {
        DefaultLengths[i] = 9;
    }

    for (int32_t i = 256; i < 280; i++) {
        DefaultLengths[i] = 7;
    }

    for (int32_t i = 280; i < 288; i++) {
        DefaultLengths[i] = 8;
    }

    for (int32_t i = 0; i < 32; i++) {
        DefaultDistances[i] = 5;
    }
}

#define DROPBITS(ctx, n)                                                                                               \
    ({                                                                                                                 \
        (ctx)->bitBuffer >>= (n);                                                                                      \
        (ctx)->numBitsAvailable -= (n);                                                                                \
    })

#define PEEKBITS(ctx, n)                                                                                               \
    ({                                                                                                                 \
        if ((ctx)->numBitsAvailable < (n)) {                                                                           \
            do {                                                                                                       \
                uint32_t byte = 0;                                                                                     \
                if ((ctx)->readBuffer < (ctx)->readBufferEnd) {                                                        \
                    byte = *ctx->readBuffer++;                                                                         \
                }                                                                                                      \
                                                                                                                       \
                (ctx)->bitBuffer |= byte << (ctx)->numBitsAvailable;                                                   \
                (ctx)->numBitsAvailable += 8;                                                                          \
            } while ((ctx)->numBitsAvailable < 25);                                                                    \
        }                                                                                                              \
                                                                                                                       \
        (ctx)->bitBuffer & ((1 << (n)) - 1);                                                                           \
    })

#define READBITS(ctx, n)                                                                                               \
    ({                                                                                                                 \
        uint32_t bits = PEEKBITS(ctx, n);                                                                              \
        DROPBITS(ctx, n);                                                                                              \
        bits;                                                                                                          \
    })

static inline int32_t CtxReadHuffmanSymbol(DEFLATECONTEXT *ctx, DEFHUFFMAN *tree) {
    uint32_t bits = PEEKBITS(ctx, 16);

    uint32_t lookupIndex = tree->fastLookup[bits & 0x1ff];

    if (lookupIndex != 0xffff) {
        // fast path: 9-bit lookup
        int32_t symbolLength = tree->symbols[lookupIndex];
        int32_t symbolIndex = tree->symbolIndex[lookupIndex];

        DROPBITS(ctx, symbolLength);

        return symbolIndex;
    }

    // slow path: bit-by-bit traversal
    int32_t rev = ReverseBits(bits);

    uint32_t len = 10;
    while (rev >= tree->baseCode[len - 1]) {
        len++;
    }

    DROPBITS(ctx, len);

    uint16_t firstCode = tree->firstCode[len];
    uint16_t numCodes = tree->numCodes[len];
    int32_t symbolIndex = tree->symbolIndex[(rev >> (16 - len)) + numCodes - firstCode];

    return symbolIndex;
}

static int32_t LengthBase[31] = {3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
                                 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0};
static int32_t LengthExtraBits[31] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                                      3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0};
static int32_t DistanceBase[32] = {1,    2,    3,    4,    5,    7,     9,     13,    17,  25,   33,
                                   49,   65,   97,   129,  193,  257,   385,   513,   769, 1025, 1537,
                                   2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0,   0};
static int32_t DistanceExtraBits[32] = {0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,  6, 6,
                                        7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0};

int DecodeDeflatedBlock(DEFLATECONTEXT *ctx) {
    while (true) {
        int32_t symbol = CtxReadHuffmanSymbol(ctx, &ctx->lengthTree);

        if (symbol < 256) {
            // Literal byte
            *ctx->currentPos++ = symbol;
        } else if (symbol > 256) {
            // Length/Distance pair
            symbol -= 257;

            int32_t length = LengthBase[symbol];
            if (LengthExtraBits[symbol] != 0) {
                length += READBITS(ctx, LengthExtraBits[symbol]);
            }

            int32_t distanceSymbol = CtxReadHuffmanSymbol(ctx, &ctx->distanceTree);
            int32_t distance = DistanceBase[distanceSymbol];
            if (DistanceExtraBits[distanceSymbol] != 0) {
                distance += READBITS(ctx, DistanceExtraBits[distanceSymbol]);
            }

            char *src = ctx->currentPos - distance;
            for (int32_t i = 0; i < length; i++) {
                *ctx->currentPos++ = *src++;
            }
        } else {
            // End of block
            LOG_DEBUG("end of deflated block");
            return true;
        }
    }
}

int DecodeUncompressedBlock(DEFLATECONTEXT *ctx) {
    UNIMPLEMENTED("DecodeUncompressedBlock");
}

int DecodeDeflated(DEFLATECONTEXT *ctx) {
    uint32_t final;
    int32_t btype;

    LOG_DEBUG("ctx=%p", ctx);

    ctx->numBitsAvailable = 0;
    ctx->bitBuffer = 0;

    do {
        final = READBITS(ctx, 1);
        btype = READBITS(ctx, 2);

        LOG_DEBUG("block type: %u, final: %u", btype, final);

        if (btype == 2) {
            LOG_DEBUG("uncompressed block");

            if (!DecodeUncompressedBlock(ctx)) {
                return false;
            }
        } else {
            if (btype == 1) {
                LOG_DEBUG("static huffman trees");

                if (DefaultDistances[0x1f] == '\0') {
                    InitHuffmanDefaults();
                }

                if (!BuildHuffmanTree(&ctx->lengthTree, DefaultLengths, 0x120)) {
                    return false;
                }

                if (!BuildHuffmanTree(&ctx->distanceTree, DefaultDistances, 0x20)) {
                    return false;
                }
            } else {
                LOG_DEBUG("dynamic huffman trees");

                if (!DecompressHuffmanTrees(ctx)) {
                    return false;
                }
            }

            if (!DecodeDeflatedBlock(ctx)) {
                LOG_WARN("failed to decode deflated block");
                return false;
            }
        }
    } while (!final);

    return true;
}

static uint8_t LengthDeZigZag[19] = {0x10, 0x11, 0x12, 0x00, 0x08, 0x07, 0x09, 0x06, 0x0a, 0x05,
                                     0x0b, 0x04, 0x0c, 0x03, 0x0d, 0x02, 0x0e, 0x01, 0x0f};

bool DecompressHuffmanTrees(DEFLATECONTEXT *ctx) {
    int32_t hlit = READBITS(ctx, 5) + 257;
    int32_t hdist = READBITS(ctx, 5) + 1;
    int32_t hclen = READBITS(ctx, 4) + 4;

    LOG_DEBUG("hlit=%d, hdist=%d, hclen=%d", hlit, hdist, hclen);

    uint8_t codeLengths[19];
    memset(codeLengths, 0, sizeof(codeLengths));

    for (int32_t i = 0; i < hclen; i++) {
        codeLengths[LengthDeZigZag[i]] = READBITS(ctx, 3);
    }

    if (!BuildHuffmanTree(&ctx->tempCodeLength, codeLengths, 19)) {
        LOG_WARN("failed to build code length huffman tree");
        return false;
    }

    uint8_t allCodeLengths[288 + 32];

    int32_t j = 0;
    while (j < hlit + hdist) {
        int32_t symbol = CtxReadHuffmanSymbol(ctx, &ctx->tempCodeLength);

        // Process the decoded symbol
        if (symbol < 16) {
            // Literal code length
            allCodeLengths[j++] = symbol;
        } else {
            uint32_t repeatCount;

            if (symbol == 16) {
                // Repeat previous code length 3-6 times
                repeatCount = READBITS(ctx, 2) + 3;
                uint8_t prevCodeLength = allCodeLengths[j - 1];
                memset(allCodeLengths + j, prevCodeLength, repeatCount);
            } else {
                if (symbol == 17) {
                    // Repeat code length 0 for 3-10 times
                    repeatCount = READBITS(ctx, 3) + 3;
                } else {
                    // Repeat code length 0 for 11-138 times
                    repeatCount = READBITS(ctx, 7) + 11;
                }

                memset(allCodeLengths + j, 0, repeatCount);
            }

            j += repeatCount;
        }
    }

    if (!BuildHuffmanTree(&ctx->lengthTree, allCodeLengths, hlit)) {
        LOG_WARN("failed to build length huffman tree");
        return 0;
    }

    if (!BuildHuffmanTree(&ctx->distanceTree, allCodeLengths + hlit, hdist)) {
        LOG_WARN("failed to build distance huffman tree");
        return 0;
    }

    return true;
}
