#pragma once

#include <stdint.h>

#include "decomp.h"

C_API_START

struct DEFHUFFMAN {
    int16_t fastLookup[512];
    uint16_t firstCode[17];
    int16_t field2_0x422;
    uint32_t baseCode[16];
    uint16_t numCodes[16];
    uint8_t symbols[288];
    int16_t symbolIndex[288];
};
typedef struct DEFHUFFMAN DEFHUFFMAN;

struct DEFLATECONTEXT {
    char *readBuffer;
    char *readBufferEnd;
    uint bitBuffer;
    uint numBitsAvailable;
    void *currentPos;
    void *startPos;
    void *endPos;
    DEFHUFFMAN lengthTree;
    DEFHUFFMAN distanceTree;
    DEFHUFFMAN tempCodeLength;
};
typedef struct DEFLATECONTEXT DEFLATECONTEXT;

int32_t ExplodeBufferNoHeader(char *param_1, void *param_2, int32_t param_3, int32_t param_4);

int32_t InflateBuffer(char *buffer, int decodedSize, char *readBuffer, int32_t readBufferSize);

int32_t ExplodeBufferSize(char *buf);

int32_t ExplodeCompressedSize(char *buffer);

C_API_END

#ifdef __cplusplus

int32_t ImplodeGetI(void *buffer, int32_t size);
void InitHuffmanDefaults(void);
int32_t DecodeDeflated(DEFLATECONTEXT *ctx);
int32_t Inflate(DEFLATECONTEXT *ctx, char *buffer, int32_t size);
uint32_t BuildHuffmanTree(DEFHUFFMAN *tree, uint8_t *codeLengths, int32_t symbolCount);
bool DecompressHuffmanTrees(DEFLATECONTEXT *ctx);

#endif
