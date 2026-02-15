#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct DEFHUFFMAN {
        u16 fast_lookup[512];
        u16 first_code[17];
        i32 base_code[16];
        u16 num_codes[16];
        u8 symbols[288];
        u16 symbol_index[288];
    } DEFHUFFMAN;

    typedef struct DEFLATECONTEXT {
        u8 *read_buffer;
        u8 *read_buffer_end;
        i32 num_bits_available;
        u32 bit_buffer;
        char *current_pos;
        char *start_pos;
        char *end_pos;
        DEFHUFFMAN length_tree;
        DEFHUFFMAN distance_tree;
        DEFHUFFMAN temp_code_length;
    } DEFLATECONTEXT;

    i32 InflateBuffer(char *buffer, int decodedSize, char *readBuffer, i32 readBufferSize);
#ifdef __cplusplus
}

void InitHuffmanDefaults();
int DecodeDeflated(DEFLATECONTEXT *ctx);
i32 Inflate(DEFLATECONTEXT *ctx, char *buffer, i32 size);
u32 BuildHuffmanTree(DEFHUFFMAN *tree, u8 *codeLengths, i32 symbolCount);
int DecompressHuffmanTrees(DEFLATECONTEXT *ctx);

#endif
