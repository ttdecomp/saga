#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct DEFHUFFMAN {
        uint16_t fast_lookup[512];
        uint16_t first_code[17];
        int32_t base_code[16];
        uint16_t num_codes[16];
        uint8_t symbols[288];
        uint16_t symbol_index[288];
    } DEFHUFFMAN;

    typedef struct DEFLATECONTEXT {
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
    } DEFLATECONTEXT;

    int32_t InflateBuffer(char *buffer, int decodedSize, char *readBuffer, int32_t readBufferSize);
#ifdef __cplusplus
}

void InitHuffmanDefaults();
int DecodeDeflated(DEFLATECONTEXT *ctx);
int32_t Inflate(DEFLATECONTEXT *ctx, char *buffer, int32_t size);
uint32_t BuildHuffmanTree(DEFHUFFMAN *tree, uint8_t *codeLengths, int32_t symbolCount);
int DecompressHuffmanTrees(DEFLATECONTEXT *ctx);

#endif
