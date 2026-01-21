#include "deflate.h"

#include <stdint.h>
#include <string.h>

#include "decomp.h"

static inline uint32_t ReverseBits(uint32_t in, uint8_t byteValue) {
    uint32_t out = 0;
    out = (in & 0x5555) * 2 | (int32_t)(in & 0xaaaa) >> 1;
    out = (int32_t)(out & 0xcccc) >> 2 | (out & 0x3333) << 2;
    out = (int32_t)(out & 0xf0f0) >> 4 | (out & 0xf0f) << 4;
    out = (out & 0xff) << 8 | (int32_t)(out >> 8) >> (0x10 - byteValue & 0x1f);
    return out;
}

uint32_t BuildHuffmanTree(DEFHUFFMAN *tree, uint8_t *codeLengths, int32_t symbolCount) {
    uint32_t nextCode[16];
    int32_t lengthCount[17];
    int32_t symbolTableIndex;
    uint32_t code;

    memset(lengthCount, 0, sizeof(lengthCount));
    memset(tree->fastLookup, -1, sizeof(tree->fastLookup));

    for (int32_t i = 0; i < symbolCount; i++) {
        lengthCount[codeLengths[i]]++;
    }

    tree->numCodes[2] = (int16_t)lengthCount[1];
    lengthCount[0] = 0;
    tree->baseCode[0] = lengthCount[1] << 15;
    nextCode[1] = 0;

    for (int32_t len = 2; len < 16; len++) {
        nextCode[len] = (nextCode[len - 1] + lengthCount[len - 1]) << 1;
        tree->firstCode[len] = (int16_t)nextCode[len];
        tree->baseCode[len - 1] = (nextCode[len] + lengthCount[len]) << (16 - len);
        tree->numCodes[len] = tree->numCodes[len - 1] + (int16_t)lengthCount[len - 1];
    }

    tree->baseCode[15] = 65536;
    tree->baseCode[14] = (nextCode[15] + lengthCount[15]) << 1;
    tree->baseCode[13] = (nextCode[14] + lengthCount[14]) << 2;

    for (int32_t i = 0; i < symbolCount; i++) {
        uint8_t *valuePtr = &codeLengths[i];
        uint8_t byteValue = *valuePtr;
        int32_t value = byteValue;
        if (value != 0) {
            code = nextCode[value];
            symbolTableIndex = (code - (uint16_t)tree->firstCode[value]) + (uint32_t)(uint16_t)tree->numCodes[value];

            tree->symbols[symbolTableIndex] = byteValue;
            tree->symbolIndex[symbolTableIndex] = (short)(int)valuePtr - (short)(int)codeLengths;

            if (value < 10) {
                int32_t j = ReverseBits(code, byteValue);

                do {
                    tree->fastLookup[j] = (int16_t)symbolTableIndex;
                    j = j + (1 << (byteValue & 0x1f));
                } while (j < 512);
            }

            nextCode[value] = code + 1;
        }
    }

    return 1;
}

static uint8_t LengthDeZigZag[19] = {0x10, 0x11, 0x12, 0x00, 0x08, 0x07, 0x09, 0x06, 0x0a, 0x05,
                                     0x0b, 0x04, 0x0c, 0x03, 0x0d, 0x02, 0x0e, 0x01, 0x0f};

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

#include "deflate/deflate.h"

#include <string.h>

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

int32_t DecodeDeflated(DEFLATECONTEXT *ctx) {
    uint uVar1;
    int iVar2;
    uint uVar3;
    uint8_t bVar4;
    uint uVar5;
    char *buf;

    uVar5 = 0;
    ctx->field2_0x8 = 0;
    ctx->field3_0xc = 0;

    while (true) {
        if ((int)uVar5 < 1) {
            uVar5 = ctx->field2_0x8;
            do {
                buf = ctx->readBuffer;
                uVar3 = 0;
                if (buf < ctx->readBufferEnd) {
                    uVar3 = (uint)*buf;
                    ctx->readBuffer = buf + 1;
                }
                bVar4 = (uint8_t)uVar5;
                uVar5 = uVar5 + 8;
                uVar3 = uVar3 << (bVar4 & 0x1f) | ctx->field3_0xc;
                ctx->field3_0xc = uVar3;
                ctx->field2_0x8 = uVar5;
            } while ((int)uVar5 < 25);
        } else {
            uVar3 = ctx->field3_0xc;
        }

        uVar5 = uVar5 - 1;
        uVar1 = uVar3 >> 1;
        ctx->field3_0xc = uVar1;
        ctx->field2_0x8 = uVar5;

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
                uVar1 = uVar1 << (bVar4 & 0x1f) | ctx->field3_0xc;
                ctx->field3_0xc = uVar1;
                ctx->field2_0x8 = uVar5;
            } while ((int)uVar5 < 0x19);
        }

        ctx->field3_0xc = uVar1 >> 2;
        ctx->field2_0x8 = uVar5 - 2;

        if ((uVar1 & 3) == 2) {
            // iVar2 = DecodeUncompressedBlock(ctx);
            UNIMPLEMENTED("uncompressed block");
        } else {
            if ((uVar1 & 3) == 1) {
                UNIMPLEMENTED("huffman tree building");

                if (DefaultDistances[0x1f] == '\0') {
                    InitHuffmanDefaults();
                }

                // if (BuildHuffmanTree(&ctx->lengthTree, DefaultLengths, 0x120) == 0) {
                // return 0;
                //}
                // iVar2 = BuildHuffmanTree(&ctx->distanceTree, DefaultDistances, 0x20);
            } else {
                // iVar2 = DecompressHuffmanTrees(ctx);
                UNIMPLEMENTED("huffman tree decompression");
            }

            if (iVar2 == 0) {
                return 0;
            }

            // iVar2 = DecodeDeflatedBlock(ctx);
            UNIMPLEMENTED("deflated block decoding");
        }

        if (iVar2 == 0) {
            return 0;
        }

        if (uVar3 & 1 != 0)
            break;

        uVar5 = ctx->field2_0x8;
    }

    return 1;
}
