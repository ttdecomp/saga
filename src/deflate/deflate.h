#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct DEFHUFFMAN {
        uint16_t fast_lookup[512];
        uint16_t first_code[17];
        int32_t base_code[16];
        uint16_t num_codes[16];
        uint8_t symbols[288];
        uint16_t symbol_index[288];
    };
    typedef struct DEFHUFFMAN DEFHUFFMAN;

    struct DEFLATECONTEXT {
        uint8_t *read_buffer;
        uint8_t *read_buffer_end;
        int32_t num_bits_available;
        uint32_t bit_buffer;
        char *current_pos;
        char *start_pos;
        char *end_pos;
        DEFHUFFMAN length_tree;
        DEFHUFFMAN distance_tree;
        DEFHUFFMAN temp_code_length;
    };
    typedef struct DEFLATECONTEXT DEFLATECONTEXT;

    int32_t ExplodeBufferNoHeader(uint8_t *param_1, uint8_t *param_2, int32_t param_3, int32_t param_4);

    int32_t InflateBuffer(char *buffer, int decodedSize, char *readBuffer, int32_t readBufferSize);

    int32_t ExplodeBufferSize(uint8_t *buf);

    int32_t ExplodeCompressedSize(uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

int32_t ImplodeGetI(uint8_t *buffer, int32_t size);
void InitHuffmanDefaults(void);
int DecodeDeflated(DEFLATECONTEXT *ctx);
int32_t Inflate(DEFLATECONTEXT *ctx, char *buffer, int32_t size);
uint32_t BuildHuffmanTree(DEFHUFFMAN *tree, uint8_t *codeLengths, int32_t symbolCount);
bool DecompressHuffmanTrees(DEFLATECONTEXT *ctx);

#endif
