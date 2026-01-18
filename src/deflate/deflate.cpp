#include "deflate.h"

#include <stdint.h>
#include <string.h>

#include "decomp.h"

static inline uint32_t ReverseBits(uint32_t in) {
    uint32_t out = 0;
    for (int32_t i = 0; i < 16; i++) {
        out = (out << 1) | (in & 1);
        in >>= 1;
    }
    return out;
}

uint32_t BuildHuffmanTree(DEFHUFFMAN *tree, uint8_t *codeLengths, int32_t symbolCount) {
    int16_t shortValue;
    uint8_t *codeLengthPtr;
    uint32_t uVar1;
    int32_t i;
    int16_t sVar2;
    uint8_t *fillPtr;
    int32_t isUnaligned;
    uint32_t nextCode[16];
    int32_t lengthCount[17];
    int32_t symbolTableIndex;
    uint8_t bvalue;
    uint32_t code;
    uint8_t *nextCodeLengthPtr;

    for (int32_t i = 0; i < 17; i++) {
        lengthCount[i] = 0;
    }

    // uint32_t value = 1024;
    // isUnaligned = ((uint32_t)tree & 1) != 0;
    // if (isUnaligned) {
    //     fillPtr = (uint8_t *)((int32_t)tree->fastLookup + 1);
    //     *(uint8_t *)tree->fastLookup = 0xff;
    //     code = (uint32_t)fillPtr & 2;
    //     value = 0x3ff;
    // } else {
    //     code = (uint32_t)tree & 2;
    //     fillPtr = (uint8_t *)tree;
    // }
    uint32_t value = 1024;
    isUnaligned = ((uint32_t)tree & 1) != 0;
    if (isUnaligned) {
        fillPtr = (uint8_t *)((size_t)tree + 1);
        *(uint8_t *)tree = 0xff;
        code = (uint32_t)fillPtr & 2;
        value = 0x3ff;
    } else {
        code = (uint32_t)tree & 2;
        fillPtr = (uint8_t *)tree;
    }

    if (code != 0) {
        //((DEFHUFFMAN *)fillPtr)->fastLookup[0] = -1;
        fillPtr[0] = 0xff;

        value = value - 2;
        // fillPtr = (uint8_t *)(((DEFHUFFMAN *)fillPtr)->fastLookup + 1);
        fillPtr++;
    }

    for (code = value >> 2; code != 0; code--) {
        //((DEFHUFFMAN *)fillPtr)->fastLookup[0] = -1;
        //((DEFHUFFMAN *)fillPtr)->fastLookup[1] = -1;
        fillPtr[0] = 0xff;
        fillPtr[1] = 0xff;
        // fillPtr = (uint8_t *)(((DEFHUFFMAN *)fillPtr)->fastLookup + 2);
        fillPtr += 2;
    }

    if ((value & 2) != 0) {
        //((DEFHUFFMAN *)fillPtr)->fastLookup[0] = -1;
        fillPtr[0] = 0xff;
        // fillPtr = (uint8_t *)(((DEFHUFFMAN *)fillPtr)->fastLookup + 1);
        fillPtr++;
    }

    if (isUnaligned) {
        *(uint8_t *)fillPtr = 0xff;
    }

    for (int32_t i = 0; i < symbolCount; i++) {
        lengthCount[codeLengths[i]] += 1;
    }

    /*
    tree->numCodes[2] = (int16_t)lengthCount[1];
    lengthCount[0] = 0;
    tree->baseCode[0] = lengthCount[1] << 15;
    nextCode[1] = 0;
    nextCode[2] = lengthCount[1] * 2;
    tree->firstCode[2] = (int16_t)nextCode[2];
    shortValue = (int16_t)lengthCount[1] + (int16_t)lengthCount[2];
    nextCode[3] = (nextCode[2] + lengthCount[2]) * 2;
    tree->baseCode[1] = (nextCode[2] + lengthCount[2]) * 16384;
    tree->firstCode[3] = (int16_t)nextCode[3];
    tree->numCodes[3] = shortValue;
    nextCode[4] = (nextCode[3] + lengthCount[3]) * 2;
    tree->baseCode[2] = (nextCode[3] + lengthCount[3]) * 8192;
    shortValue = shortValue + (int16_t)lengthCount[3];
    tree->firstCode[4] = (int16_t)nextCode[4];
    nextCode[5] = (nextCode[4] + lengthCount[4]) * 2;
    sVar2 = (int16_t)lengthCount[4] + shortValue;
    tree->baseCode[3] = (nextCode[4] + lengthCount[4]) * 4096;
    tree->firstCode[5] = (int16_t)nextCode[5];
    tree->numCodes[4] = shortValue;
    tree->numCodes[5] = sVar2;
    sVar2 = sVar2 + (int16_t)lengthCount[5];
    nextCode[6] = (nextCode[5] + lengthCount[5]) * 2;
    tree->firstCode[6] = (int16_t)nextCode[6];
    tree->baseCode[4] = (nextCode[5] + lengthCount[5]) * 2048;
    tree->numCodes[6] = sVar2;
    nextCode[7] = (nextCode[6] + lengthCount[6]) * 2;
    sVar2 = sVar2 + (int16_t)lengthCount[6];
    tree->baseCode[5] = (nextCode[6] + lengthCount[6]) * 1024;
    tree->firstCode[7] = (int16_t)nextCode[7];
    tree->firstCode[1] = 0;
    tree->numCodes[1] = 0;
    tree->numCodes[7] = sVar2;
    sVar2 = sVar2 + (int16_t)lengthCount[7];
    nextCode[8] = (nextCode[7] + lengthCount[7]) * 2;
    tree->numCodes[8] = sVar2;
    tree->firstCode[8] = (int16_t)nextCode[8];
    sVar2 = sVar2 + (int16_t)lengthCount[8];
    tree->baseCode[6] = (nextCode[7] + lengthCount[7]) * 512;
    nextCode[9] = (nextCode[8] + lengthCount[8]) * 2;
    tree->numCodes[9] = sVar2;
    tree->firstCode[9] = (int16_t)nextCode[9];
    sVar2 = sVar2 + (int16_t)lengthCount[9];
    tree->baseCode[7] = (nextCode[8] + lengthCount[8]) * 256;
    nextCode[10] = (nextCode[9] + lengthCount[9]) * 2;
    tree->numCodes[10] = sVar2;
    tree->firstCode[10] = (int16_t)nextCode[10];
    tree->baseCode[8] = (nextCode[9] + lengthCount[9]) * 128;
    nextCode[0xb] = (nextCode[10] + lengthCount[10]) * 2;
    sVar2 = sVar2 + (int16_t)lengthCount[10];
    tree->firstCode[0xb] = (int16_t)nextCode[0xb];
    tree->baseCode[9] = (nextCode[10] + lengthCount[10]) * 64;
    tree->numCodes[0xb] = sVar2;
    sVar2 = sVar2 + (int16_t)lengthCount[0xb];
    nextCode[0xc] = (nextCode[0xb] + lengthCount[0xb]) * 2;
    tree->firstCode[0xc] = (int16_t)nextCode[0xc];
    tree->baseCode[10] = (nextCode[0xb] + lengthCount[0xb]) * 32;
    tree->numCodes[0xc] = sVar2;
    sVar2 = sVar2 + (int16_t)lengthCount[0xc];
    nextCode[0xd] = (nextCode[0xc] + lengthCount[0xc]) * 2;
    tree->firstCode[0xd] = (int16_t)nextCode[0xd];
    tree->baseCode[0xb] = (nextCode[0xc] + lengthCount[0xc]) * 16;
    nextCode[0xe] = (nextCode[0xd] + lengthCount[0xd]) * 2;
    tree->numCodes[0xd] = sVar2;
    tree->baseCode[0xc] = (nextCode[0xd] + lengthCount[0xd]) * 8;
    sVar2 = sVar2 + (int16_t)lengthCount[0xd];
    tree->firstCode[0xe] = (int16_t)nextCode[0xe];
    tree->numCodes[0xe] = sVar2;
    nextCode[0xf] = (nextCode[0xe] + lengthCount[0xe]) * 2;
    tree->firstCode[0xf] = (int16_t)nextCode[0xf];
    tree->numCodes[0xf] = (int16_t)lengthCount[0xe] + sVar2;
    tree->baseCode[0xf] = 65536;
    tree->baseCode[0xe] = (nextCode[0xf] + lengthCount[0xf]) * 2;
    tree->baseCode[0xd] = (nextCode[0xe] + lengthCount[0xe]) * 4;
    */
    tree->numCodes[1] = (int16_t)lengthCount[1];
    lengthCount[0] = 0;
    tree->baseCode[0] = lengthCount[1] << 15;
    nextCode[1] = 0;
    for (int32_t len = 2; len <= 16; len++) {
        nextCode[len] = (nextCode[len - 1] + lengthCount[len - 1]) << 1;
        tree->firstCode[len] = (int16_t)nextCode[len];
        tree->baseCode[len - 1] = (nextCode[len - 1] + lengthCount[len - 1]) << (16 - len);
        tree->numCodes[len] = tree->numCodes[len - 1] + (int16_t)lengthCount[len - 1];
    }
    tree->baseCode[15] = 65536;
    tree->baseCode[16] = (nextCode[16] + lengthCount[16]) << 0;
    tree->baseCode[15] = (nextCode[15] + lengthCount[15]) << 1;
    tree->baseCode[14] = (nextCode[16] + lengthCount[16]) << 2;

    for (int32_t i = 0; i < symbolCount; i++) {
        uint8_t *valuePtr = &codeLengths[i];

        uint32_t value = (uint32_t)*valuePtr;
        if (value != 0) {
            code = nextCode[value];
            symbolTableIndex = (code - (uint16_t)tree->firstCode[value]) + (uint32_t)(uint16_t)tree->numCodes[value];

            tree->symbols[symbolTableIndex] = bvalue;
            tree->symbolIndex[symbolTableIndex] = (uint16_t)i;

            if (value < 10) {
                // uVar1 = (code & 0x5555) * 2 | (int32_t)(code & 0xaaaa) >> 1;
                // uVar1 = (uVar1 & 0x3333) << 2 | (int32_t)(uVar1 & 0xcccc) >> 2;
                // uVar1 = (uVar1 & 0xf0f) << 4 | (int32_t)(uVar1 & 0xf0f0) >> 4;
                uVar1 = ReverseBits(code);

                // int32_t j = (int32_t)((uVar1 & 0xff) << 8 | uVar1 >> 8) >> (0x10 - bvalue & 0x1f);
                int32_t j = (int32_t)(uVar1 >> (16 - (bvalue & 0x1f)));

                do {
                    tree->fastLookup[i] = (int16_t)symbolTableIndex;
                    j = j + (1 << (bvalue & 0x1f));
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
