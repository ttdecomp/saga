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
    tree->numCodes[0] = 0;

    for (uint32_t len = 1; len < 16; len++) {
        tree->numCodes[len] = tree->numCodes[len - 1] + lengthCount[len - 1];
        tree->firstCode[len] = nextCode[len];
        tree->baseCode[len - 1] = (nextCode[len] + lengthCount[len]) << (0x10 - len);

        nextCode[len + 1] = (nextCode[len] + lengthCount[len]) << 1;
    }

    tree->baseCode[15] = 0x10000;
    tree->firstCode[16] = 0;

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

    ctx.readBuffer = readBuffer;
    ctx.readBufferEnd = readBuffer + readBufferSize;

    if (Inflate(&ctx, buffer, decodedSize) == 0) {
        return -1;
    } else {
        return (size_t)ctx.currentPos - (size_t)ctx.startPos;
    }
}

int32_t ExplodeBufferSize(char *buf) {
    char magic[] = "LZ2K";

    for (int32_t i = 0; i < 4; i++) {
        if (buf[i] != magic[i])
            return 0;
    }

    return ImplodeGetI(buf, 4);
}

int32_t ExplodeCompressedSize(char *buffer) {
    char magic[] = "LZ2K";

    for (int32_t i = 0; i < 4; i++) {
        if (buffer[i] != magic[i])
            return 0;
    }

    int32_t size = ImplodeGetI((int32_t *)(buffer + 4), 4);
    if (size != 0) {
        size += 0xc;
    }

    return size;
}

int32_t Inflate(DEFLATECONTEXT *ctx, char *buffer, int32_t size) {
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

static int32_t LengthBase[29] = {3,  4,  5,  6,  7,  8,  9,  10, 11,  13,  15,  17,  19,  23, 27,
                                 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
static uint8_t LengthExtraBits[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
                                      2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
static int32_t DistanceBase[30] = {1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
                                   33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
                                   1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static uint8_t DistanceExtraBits[30] = {0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
                                        6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

int32_t DecodeDeflatedBlock(DEFLATECONTEXT *ctx) {
    unsigned int *piVar1;
    uint8_t uVar2;
    uint16_t uVar3;
    uint16_t uVar4;
    char *pbVar5;
    uint uVar6;
    uint8_t *puVar7;
    uint8_t bVar8;
    uint8_t *puVar9;
    uint uVar10;
    uint uVar11;
    uint8_t *puVar12;
    int distance;
    uint local_18;
    uint local_14;
    char *pbVar4;
    int distanceExtra;

    do {
        uVar11 = ctx->bitBuffer;
        if ((int)uVar11 < 0x10)
            goto LAB_00325b8d;
    LAB_00325b3a:
        uVar6 = ctx->numBitsAvailable;
        while (true) {
            uVar10 = (uint)(uint16_t)(ctx->lengthTree).fastLookup[uVar6 & 0x1ff];
            if (uVar10 == 0xffff) {
                uVar10 = (uVar6 & 0x5555) * 2 | (int)(uVar6 & 0xaaaa) >> 1;
                uVar10 = (uVar10 & 0x3333) << 2 | (int)(uVar10 & 0xcccc) >> 2;
                uVar10 = (uVar10 & 0xf0f) << 4 | (int)(uVar10 & 0xf0f0) >> 4;
                uVar10 = (uVar10 & 0xff) << 8 | uVar10 >> 8;
                if ((int)uVar10 < (ctx->lengthTree).baseCode[9]) {
                    bVar8 = 6;
                    distance = 10;
                } else {
                    distanceExtra = 10;
                    do {
                        distance = distanceExtra + 1;
                        piVar1 = (ctx->lengthTree).baseCode + distanceExtra;
                        distanceExtra = distance;
                    } while (*piVar1 <= (int)uVar10);
                    bVar8 = 0x10 - (char)distance;
                }
                uVar11 = uVar11 - distance;
                uVar3 = (ctx->lengthTree).firstCode[distance];
                uVar4 = (ctx->lengthTree).numCodes[distance];
                ctx->bitBuffer = uVar11;
                uVar6 = uVar6 >> ((uint8_t)distance & 0x1f);
                ctx->numBitsAvailable = uVar6;
                uVar3 =
                    *(uint16_t *)((int)ctx +
                                  ((((int)uVar10 >> (bVar8 & 0x1f)) - (uint)uVar3) + 0x2d0 + (uint)uVar4) * 2 + 0x20);
            } else {
                bVar8 = (ctx->lengthTree).symbols[uVar10];
                uVar3 = (ctx->lengthTree).symbolIndex[uVar10];
                uVar11 = uVar11 - bVar8;
                uVar6 = uVar6 >> (bVar8 & 0x1f);
                ctx->bitBuffer = uVar11;
                ctx->numBitsAvailable = uVar6;
            }
            uVar10 = (uint)uVar3;
            if (0xff < uVar10)
                break;
            puVar9 = (uint8_t *)ctx->currentPos;
            *puVar9 = (char)uVar3;
            uVar11 = ctx->bitBuffer;
            ctx->currentPos = puVar9 + 1;
            if (0xf < (int)uVar11)
                goto LAB_00325b3a;
        LAB_00325b8d:
            do {
                pbVar5 = ctx->readBuffer;
                uVar6 = 0;
                if (pbVar5 < ctx->readBufferEnd) {
                    uVar6 = (uint)*pbVar5;
                    ctx->readBuffer = (char *)pbVar5 + 1;
                }
                bVar8 = (uint8_t)uVar11;
                uVar11 = uVar11 + 8;
                uVar6 = uVar6 << (bVar8 & 0x1f) | ctx->numBitsAvailable;
                ctx->numBitsAvailable = uVar6;
                ctx->bitBuffer = uVar11;
            } while ((int)uVar11 < 0x19);
        }
        if (uVar10 == 0x100) {
            return 1;
        }
        local_18 = LengthBase[uVar10 - 0x101];
        uVar10 = LengthExtraBits[uVar10 - 0x101];
        if (uVar10 != 0) {
            if ((int)uVar11 < (int)uVar10) {
                uVar11 = ctx->bitBuffer;
                do {
                    pbVar5 = ctx->readBuffer;
                    uVar6 = 0;
                    if (pbVar5 < ctx->readBufferEnd) {
                        uVar6 = (uint)*pbVar5;
                        ctx->readBuffer = pbVar5 + 1;
                    }
                    bVar8 = (uint8_t)uVar11;
                    uVar11 = uVar11 + 8;
                    uVar6 = uVar6 << (bVar8 & 0x1f) | ctx->numBitsAvailable;
                    ctx->numBitsAvailable = uVar6;
                    ctx->bitBuffer = uVar11;
                } while ((int)uVar11 < 0x19);
            }
            uVar11 = uVar11 - uVar10;
            ctx->bitBuffer = uVar11;
            ctx->numBitsAvailable = uVar6 >> ((uint8_t)uVar10 & 0x1f);
            local_18 = local_18 + ((1 << ((uint8_t)uVar10 & 0x1f)) - 1U & uVar6);
        }
        if ((int)uVar11 < 0x10) {
            uVar11 = ctx->bitBuffer;
            do {
                pbVar4 = ctx->readBuffer;
                uVar6 = 0;
                if (pbVar4 < ctx->readBufferEnd) {
                    uVar6 = (uint)*pbVar4;
                    ctx->readBuffer = pbVar4 + 1;
                }
                bVar8 = (uint8_t)uVar11;
                uVar11 = uVar11 + 8;
                uVar6 = uVar6 << (bVar8 & 0x1f) | ctx->numBitsAvailable;
                ctx->numBitsAvailable = uVar6;
                ctx->bitBuffer = uVar11;
            } while ((int)uVar11 < 0x19);
        } else {
            uVar6 = ctx->numBitsAvailable;
        }
        uVar10 = (uint)(uint16_t)(ctx->distanceTree).fastLookup[uVar6 & 0x1ff];
        if (uVar10 == 0xffff) {
            uVar10 = (uVar6 & 0x5555) * 2 | (int)(uVar6 & 0xaaaa) >> 1;
            uVar10 = (uVar10 & 0x3333) << 2 | (int)(uVar10 & 0xcccc) >> 2;
            uVar10 = (uVar10 & 0xf0f) << 4 | (int)(uVar10 & 0xf0f0) >> 4;
            uVar10 = (uVar10 & 0xff) << 8 | uVar10 >> 8;
            if ((int)uVar10 < (ctx->distanceTree).baseCode[9]) {
                bVar8 = 6;
                distance = 10;
            } else {
                distanceExtra = 10;
                do {
                    distance = distanceExtra + 1;
                    piVar1 = (ctx->distanceTree).baseCode + distanceExtra;
                    distanceExtra = distance;
                } while (*piVar1 <= (int)uVar10);
                bVar8 = 0x10 - (char)distance;
            }
            uVar3 = (ctx->distanceTree).firstCode[distance];
            uVar4 = (ctx->distanceTree).numCodes[distance];
            ctx->bitBuffer = uVar11 - distance;
            uVar6 = uVar6 >> ((uint8_t)distance & 0x1f);
            ctx->numBitsAvailable = uVar6;
            uVar3 = *(uint16_t *)((int)ctx +
                                  ((((int)uVar10 >> (bVar8 & 0x1f)) - (uint)uVar3) + 0x2d0 + (uint)uVar4) * 2 + 0x804);
        } else {
            bVar8 = (ctx->distanceTree).symbols[uVar10];
            uVar3 = (ctx->distanceTree).symbolIndex[uVar10];
            uVar6 = uVar6 >> (bVar8 & 0x1f);
            ctx->bitBuffer = uVar11 - bVar8;
            ctx->numBitsAvailable = uVar6;
        }
        distance = DistanceBase[uVar3];
        distanceExtra = DistanceExtraBits[uVar3];
        if (distanceExtra != 0) {
            local_14 = ctx->bitBuffer;
            if ((int)local_14 < distanceExtra) {
                do {
                    pbVar5 = ctx->readBuffer;
                    uVar11 = 0;
                    if (pbVar5 < ctx->readBufferEnd) {
                        uVar11 = (uint)*pbVar5;
                        ctx->readBuffer = pbVar5 + 1;
                    }
                    bVar8 = (uint8_t)local_14;
                    local_14 = local_14 + 8;
                    uVar6 = uVar11 << (bVar8 & 0x1f) | ctx->numBitsAvailable;
                    ctx->numBitsAvailable = uVar6;
                    ctx->bitBuffer = local_14;
                } while ((int)local_14 < 0x19);
            }
            ctx->numBitsAvailable = uVar6 >> ((uint8_t)distanceExtra & 0x1f);
            ctx->bitBuffer = local_14 - distanceExtra;
            distance = distance + ((1 << ((uint8_t)distanceExtra & 0x1f)) - 1U & uVar6);
        }
        puVar7 = (uint8_t *)ctx->currentPos;
        puVar9 = puVar7 + -distance;
        if (local_18 != 0) {
            puVar12 = puVar9 + local_18;
            do {
                uVar2 = *puVar9;
                puVar9 = puVar9 + 1;
                *puVar7 = uVar2;
                puVar7 = puVar7 + 1;
                ctx->currentPos = puVar7;
            } while (puVar9 != puVar12);
        }
    } while (true);
}

int32_t DecodeDeflated(DEFLATECONTEXT *ctx) {
    LOG_DEBUG("ctx=%p", ctx);

    uint uVar1;
    int iVar2;
    uint uVar3;
    uint8_t bVar4;
    uint uVar5;
    char *buf;

    uVar5 = 0;
    ctx->bitBuffer = 0;
    ctx->numBitsAvailable = 0;

    while (true) {
        if ((int)uVar5 < 1) {
            uVar5 = ctx->bitBuffer;
            do {
                buf = ctx->readBuffer;
                uVar3 = 0;
                if (buf < ctx->readBufferEnd) {
                    uVar3 = (uint)*buf;
                    ctx->readBuffer = buf + 1;
                }
                bVar4 = (uint8_t)uVar5;
                uVar5 = uVar5 + 8;
                uVar3 = uVar3 << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                ctx->numBitsAvailable = uVar3;
                ctx->bitBuffer = uVar5;
            } while ((int)uVar5 < 25);
        } else {
            uVar3 = ctx->numBitsAvailable;
        }

        uVar5 = uVar5 - 1;
        uVar1 = uVar3 >> 1;
        ctx->numBitsAvailable = uVar1;
        ctx->bitBuffer = uVar5;

        if ((int)uVar5 < 2) {
            do {
                buf = ctx->readBuffer;
                uVar1 = 0;
                if (buf < ctx->readBufferEnd) {
                    uVar1 = (uint)*buf;
                    ctx->readBuffer = buf + 1;
                }
                bVar4 = (uint8_t)uVar5;
                uVar5 = uVar5 + 8;
                uVar1 = uVar1 << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                ctx->numBitsAvailable = uVar1;
                ctx->bitBuffer = uVar5;
            } while ((int)uVar5 < 0x19);
        }

        ctx->numBitsAvailable = uVar1 >> 2;
        ctx->bitBuffer = uVar5 - 2;

        LOG_DEBUG("block type: %u, final: %u", uVar1 & 3, uVar3 & 1);

        if ((uVar1 & 3) == 2) {
            // iVar2 = DecodeUncompressedBlock(ctx);
            UNIMPLEMENTED("uncompressed block");
        } else {
            if ((uVar1 & 3) == 1) {
                LOG_DEBUG("static huffman trees");

                if (DefaultDistances[0x1f] == '\0') {
                    InitHuffmanDefaults();
                }

                if (BuildHuffmanTree(&ctx->lengthTree, DefaultLengths, 0x120) == 0) {
                    return 0;
                }

                iVar2 = BuildHuffmanTree(&ctx->distanceTree, DefaultDistances, 0x20);
            } else {
                LOG_DEBUG("dynamic huffman trees");

                iVar2 = DecompressHuffmanTrees(ctx);
            }

            if (iVar2 == 0) {
                return 0;
            }

            iVar2 = DecodeDeflatedBlock(ctx);
            UNIMPLEMENTED("deflated block decoding");
        }

        if (iVar2 == 0) {
            return 0;
        }

        if ((uVar3 & 1) != 0)
            break;

        uVar5 = ctx->bitBuffer;
    }

    return 1;
}

static inline uint32_t CtxPeekBits(DEFLATECONTEXT *ctx, int32_t numBits) {
    uint32_t bitBuffer = ctx->bitBuffer;
    uint32_t bitsAvailable = ctx->numBitsAvailable;
    char *buf;
    uint32_t uint8_t;

    if ((int)bitBuffer < numBits) {
        do {
            buf = ctx->readBuffer;
            uint8_t = 0;

            if (buf < ctx->readBufferEnd) {
                uint8_t = (uint32_t)*buf;
                ctx->readBuffer = buf + 1;
            }

            bitsAvailable = (uint8_t << (bitBuffer & 0x1f)) | bitsAvailable;
            bitBuffer += 8;

        } while (bitBuffer < 25);

        ctx->numBitsAvailable = bitsAvailable;
    }

    return bitsAvailable >> numBits;
}

static inline uint32_t CtxReadBits(DEFLATECONTEXT *ctx, int32_t numBits) {
    uint32_t result = CtxPeekBits(ctx, numBits);

    ctx->bitBuffer -= numBits;
    ctx->numBitsAvailable = result;

    return result;
}

static inline int32_t CtxReadHuffmanSymbol(DEFLATECONTEXT *ctx, DEFHUFFMAN *tree) {
    // ensure at least 16 bits are available
    while (ctx->numBitsAvailable < 16) {
        if (ctx->readBuffer < ctx->readBufferEnd) {
            ctx->bitBuffer |= (*ctx->readBuffer++) << ctx->numBitsAvailable;
            ctx->numBitsAvailable += 8;
        }
    }

    uint32_t bits = ctx->bitBuffer;

    // fast path: 9-bit lookup
    int32_t symbolIndex = tree->fastLookup[bits & 0x1ff];
    if (symbolIndex != -1) {
        int32_t symbolLength = tree->symbols[symbolIndex];
        ctx->bitBuffer >>= symbolLength;
        ctx->numBitsAvailable -= symbolLength;
        return symbolIndex;
    }

    // slow path: bit-by-bit traversal
    uint16_t rev = ReverseBits(bits & 0xFFFF);

    int len;
    if (rev < tree->baseCode[9]) {
        len = 10;
    } else {
        for (len = 10; len < 16; len++) {
            if (rev < tree->baseCode[len])
                break;
        }
    }

    uint16_t canonical = rev >> (16 - len);
    uint16_t code = canonical - tree->firstCode[len];
    uint16_t index = tree->numCodes[len] + code;

    uint16_t symbol = tree->symbolIndex[index];

    ctx->bitBuffer >>= len;
    ctx->numBitsAvailable -= len;

    return symbol;
}

static uint8_t LengthDeZigZag[19] = {0x10, 0x11, 0x12, 0x00, 0x08, 0x07, 0x09, 0x06, 0x0a, 0x05,
                                     0x0b, 0x04, 0x0c, 0x03, 0x0d, 0x02, 0x0e, 0x01, 0x0f};

/*bool DecompressHuffmanTrees(DEFLATECONTEXT *ctx) {
    uint32_t hlit = (CtxReadBits(ctx, 5) & 0x1f) + 257;
    uint32_t hdist = (CtxReadBits(ctx, 5) & 0x1f) + 1;
    uint32_t hclen = (CtxReadBits(ctx, 4) & 0x0f) + 4;
    LOG_DEBUG("hlit=%d, hdist=%d, hclen=%d", hlit, hdist, hclen);

    uint8_t codeLengths[19];
    memset(codeLengths, 0, sizeof(codeLengths));
    for (int32_t i = 0; i < hclen; i++) {
        codeLengths[LengthDeZigZag[i]] = CtxReadBits(ctx, 3) & 0b111;
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
        if (symbol <= 15) {
            // Literal code length
            LOG_DEBUG("code length symbol: %d", symbol);
            allCodeLengths[j++] = symbol;
        } else if (symbol == 16) {
            // Repeat previous code length 3-6 times
            uint32_t repeatCount = (CtxReadBits(ctx, 2) & 0x3) + 3;
            LOG_DEBUG("repeat previous code length %d times", repeatCount);
            uint8_t prevCodeLength = allCodeLengths[j - 1];
            memset(allCodeLengths + j, prevCodeLength, repeatCount);
            j += repeatCount;
        } else if (symbol == 17) {
            // Repeat code length 0 for 3-10 times
            uint32_t repeatCount = (CtxReadBits(ctx, 3) & 0x7) + 3;
            LOG_DEBUG("repeat code length 0 %d times", repeatCount);
            memset(allCodeLengths + j, 0, repeatCount);
            j += repeatCount;
        } else if (symbol == 18) {
            // Repeat code length 0 for 11-138 times
            uint32_t repeatCount = (CtxReadBits(ctx, 7) & 0x7f) + 11;
            LOG_DEBUG("repeat code length 0 %d times", repeatCount);
            memset(allCodeLengths + j, 0, repeatCount);
            j += repeatCount;
        } else {
            LOG_WARN("invalid code length symbol: %d", symbol);
            return false;
        }
    }

    if (!BuildHuffmanTree(&ctx->lengthTree, allCodeLengths, hlit)) {
        return false;
    }
    if (!BuildHuffmanTree(&ctx->distanceTree, allCodeLengths + hlit, hdist)) {
        return false;
    }

    return true;
}
*/

bool DecompressHuffmanTrees(DEFLATECONTEXT *ctx) {
    int *piVar1;
    uint16_t uVar2;
    uint16_t uVar3;
    int hlit;
    int hdist;
    uint32_t word;
    uint8_t bVar4;
    uint16_t index;
    uint32_t uVar5;
    int iVar6;
    uint32_t hclen;
    int i;
    size_t sVar7;
    uint32_t uVar8;
    bool bVar9;
    int j;
    uint32_t local_210;
    uint8_t codeLengths[19];
    uint8_t lengths[257];
    uint8_t distances[206];
    uint8_t symbol;
    uint8_t *position;
    uint32_t bitBuffer;
    char *buf;

    bitBuffer = ctx->bitBuffer;
    if ((int)bitBuffer < 5) {
        do {
            buf = ctx->readBuffer;
            uVar8 = 0;
            if (buf < ctx->readBufferEnd) {
                uVar8 = (uint)*buf;
                ctx->readBuffer = buf + 1;
            }
            bVar4 = (uint8_t)bitBuffer;
            bitBuffer = bitBuffer + 8;
            uVar8 = uVar8 << (bVar4 & 0x1f) | ctx->numBitsAvailable;
            ctx->numBitsAvailable = uVar8;
            ctx->bitBuffer = bitBuffer;
        } while ((int)bitBuffer < 25);
    } else {
        uVar8 = ctx->numBitsAvailable;
    }
    local_210 = uVar8 >> 5;
    bitBuffer = bitBuffer - 5;
    ctx->numBitsAvailable = local_210;
    ctx->bitBuffer = bitBuffer;
    if ((int)bitBuffer < 5) {
        do {
            buf = ctx->readBuffer;
            word = 0;
            if (buf < ctx->readBufferEnd) {
                word = (uint)*buf;
                ctx->readBuffer = buf + 1;
            }
            bVar4 = (uint8_t)bitBuffer;
            bitBuffer = bitBuffer + 8;
            local_210 = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
            ctx->numBitsAvailable = local_210;
            ctx->bitBuffer = bitBuffer;
        } while ((int)bitBuffer < 25);
    }
    bitBuffer = bitBuffer - 5;
    hclen = local_210 >> 5;
    ctx->numBitsAvailable = hclen;
    ctx->bitBuffer = bitBuffer;
    if ((int)bitBuffer < 4) {
        do {
            buf = ctx->readBuffer;
            word = 0;
            if (buf < ctx->readBufferEnd) {
                word = (uint)*buf;
                ctx->readBuffer = buf + 1;
            }
            bVar4 = (uint8_t)bitBuffer;
            bitBuffer = bitBuffer + 8;
            hclen = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
            ctx->numBitsAvailable = hclen;
            ctx->bitBuffer = bitBuffer;
        } while ((int)bitBuffer < 0x19);
    }
    bitBuffer = bitBuffer - 4;
    ctx->bitBuffer = bitBuffer;
    ctx->numBitsAvailable = hclen >> 4;
    word = 0x13;
    buf = (char *)codeLengths;
    bVar9 = ((uint)buf & 2) != 0;
    if (bVar9) {
        codeLengths[0] = 0;
        codeLengths[1] = 0;
        buf = (char *)codeLengths + 2;
        word = 0x11;
    }
    for (word = word >> 2; word != 0; word = word - 1) {
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        buf = buf + 4;
    }
    if (!bVar9) {
        buf[0] = 0;
        buf[1] = 0;
        buf = buf + 2;
    }
    *buf = 0;
    i = 0;
    do {
        if ((int)bitBuffer < 3) {
            bitBuffer = ctx->bitBuffer;
            do {
                buf = ctx->readBuffer;
                word = 0;
                if (buf < ctx->readBufferEnd) {
                    word = (uint)*buf;
                    ctx->readBuffer = buf + 1;
                }
                bVar4 = (uint8_t)bitBuffer;
                bitBuffer = bitBuffer + 8;
                word = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                ctx->numBitsAvailable = word;
                ctx->bitBuffer = bitBuffer;
            } while ((int)bitBuffer < 0x19);
        } else {
            word = ctx->numBitsAvailable;
        }
        bitBuffer = bitBuffer - 3;
        ctx->numBitsAvailable = word >> 3;
        ctx->bitBuffer = bitBuffer;
        position = LengthDeZigZag + i;
        i = i + 1;
        codeLengths[*position] = (uint8_t)word & 7;
    } while (i < (int)((hclen & 0xf) + 4));

    LOG_DEBUG("hlit: %d, hdist: %d, hclen: %d", (uVar8 & 0x1f) + 0x101, (local_210 & 0x1f) + 1, (hclen & 0xf) + 4);

    i = BuildHuffmanTree(&ctx->tempCodeLength, codeLengths, 19);

    if (i != 0) {
        j = 0;
        hlit = (uVar8 & 0x1f) + 0x101;
        hdist = (local_210 & 0x1f) + 1;
        do {
            while (true) {
                bitBuffer = ctx->bitBuffer;
                if ((int)bitBuffer < 16) {
                    do {
                        buf = ctx->readBuffer;
                        word = 0;
                        if (buf < ctx->readBufferEnd) {
                            word = (uint)*buf;
                            ctx->readBuffer = buf + 1;
                        }
                        bVar4 = (uint8_t)bitBuffer;
                        bitBuffer = bitBuffer + 8;
                        word = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                        ctx->numBitsAvailable = word;
                        ctx->bitBuffer = bitBuffer;
                    } while ((int)bitBuffer < 0x19);
                } else {
                    word = ctx->numBitsAvailable;
                }
                uVar5 = (uint)(uint16_t)(ctx->tempCodeLength).fastLookup[word & 0x1ff];
                if (uVar5 != 0xffff)
                    break;
                uVar5 = (word & 0x5555) * 2 | (int)(word & 0xaaaa) >> 1;
                uVar5 = (uVar5 & 0x3333) << 2 | (int)(uVar5 & 0xcccc) >> 2;
                uVar5 = (uVar5 & 0xf0f) << 4 | (int)(uVar5 & 0xf0f0) >> 4;
                uVar5 = (uVar5 & 0xff) << 8 | uVar5 >> 8;
                if ((int)uVar5 < (ctx->tempCodeLength).baseCode[9]) {
                    bVar4 = 6;
                    i = 10;
                } else {
                    iVar6 = 10;
                    do {
                        i = iVar6 + 1;
                        piVar1 = (int32_t *)(ctx->tempCodeLength).baseCode + iVar6;
                        iVar6 = i;
                    } while (*piVar1 <= (int)uVar5);
                    bVar4 = 16 - (char)i;
                }
                uVar2 = (ctx->tempCodeLength).firstCode[i];
                uVar3 = (ctx->tempCodeLength).numCodes[i];
                ctx->bitBuffer = bitBuffer - i;
                word = word >> ((uint8_t)i & 0x1f);
                ctx->numBitsAvailable = word;

                index =
                    *(uint16_t *)((int)ctx +
                                  ((((int)uVar5 >> (bVar4 & 0x1f)) - (uint)uVar2) + 0x2d0 + (uint)uVar3) * 2 + 0xfe8);

                if (15 < index)
                    goto LAB_003261f3;
            LAB_003260f3:
                lengths[j] = (uint8_t)index;
                j = j + 1;
            LAB_00326102:
                if (hlit + hdist <= j)
                    goto LAB_00326257;
            }
            symbol = (ctx->tempCodeLength).symbols[uVar5];
            index = (ctx->tempCodeLength).symbolIndex[uVar5];
            word = word >> (symbol & 0x1f);
            ctx->numBitsAvailable = word;
            ctx->bitBuffer = bitBuffer - symbol;
            if (index < 0x10)
                goto LAB_003260f3;
        LAB_003261f3:
            bitBuffer = ctx->bitBuffer;
            if (index == 0x10) {
                if ((int)bitBuffer < 2) {
                    do {
                        buf = ctx->readBuffer;
                        word = 0;
                        if (buf < ctx->readBufferEnd) {
                            word = (uint)*buf;
                            ctx->readBuffer = buf + 1;
                        }
                        bVar4 = (uint8_t)bitBuffer;
                        bitBuffer = bitBuffer + 8;
                        word = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                        ctx->numBitsAvailable = word;
                        ctx->bitBuffer = bitBuffer;
                    } while ((int)bitBuffer < 0x19);
                }
                ctx->bitBuffer = bitBuffer - 2;
                ctx->numBitsAvailable = word >> 2;
                sVar7 = (word & 3) + 3;
                memset(lengths + j, (uint)codeLengths[j + 0x12], sVar7);
                j = j + sVar7;
                goto LAB_00326102;
            }
            if (index == 0x11) {
                if ((int)bitBuffer < 3) {
                    do {
                        buf = ctx->readBuffer;
                        word = 0;
                        if (buf < ctx->readBufferEnd) {
                            word = (uint)*buf;
                            ctx->readBuffer = buf + 1;
                        }
                        bVar4 = (uint8_t)bitBuffer;
                        bitBuffer = bitBuffer + 8;
                        word = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                        ctx->numBitsAvailable = word;
                        ctx->bitBuffer = bitBuffer;
                    } while ((int)bitBuffer < 0x19);
                }
                ctx->numBitsAvailable = word >> 3;
                ctx->bitBuffer = bitBuffer - 3;
                sVar7 = (word & 7) + 3;
            } else {
                if ((int)bitBuffer < 7) {
                    do {
                        buf = ctx->readBuffer;
                        word = 0;
                        if (buf < ctx->readBufferEnd) {
                            word = (uint)*buf;
                            ctx->readBuffer = buf + 1;
                        }
                        bVar4 = (uint8_t)bitBuffer;
                        bitBuffer = bitBuffer + 8;
                        word = word << (bVar4 & 0x1f) | ctx->numBitsAvailable;
                        ctx->numBitsAvailable = word;
                        ctx->bitBuffer = bitBuffer;
                    } while ((int)bitBuffer < 25);
                }
                ctx->bitBuffer = bitBuffer - 7;
                ctx->numBitsAvailable = word >> 7;
                sVar7 = (word & 0x7f) + 0xb;
            }
            memset(lengths + j, 0, sVar7);
            j = j + sVar7;
        } while (j < hlit + hdist);
    LAB_00326257:
        i = BuildHuffmanTree(&ctx->lengthTree, lengths, hlit);
        if (i != 0) {
            i = BuildHuffmanTree(&ctx->distanceTree, distances + (uVar8 & 0x1f), hdist);
            return i != 0;
        }
    }
    return false;
}