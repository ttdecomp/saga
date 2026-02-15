#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    isize ExplodeBufferNoHeader(char *in_buf, char *out_buf, isize orig_size, isize compressed_size);

    isize ExplodeBufferSize(char *buf);
    isize ExplodeCompressedSize(char *buf);

    extern char *implode_inbuffer;
    extern char *implode_outbuffer;
    extern usize implode_origsize;
    extern usize implode_compsize;
    extern u32 implode_bitbuf;

    extern u16 implode_left[];
    extern u16 implode_right[];
#ifdef __cplusplus
}

i32 ImplodeGetI(void *buf, i32 size);

u32 ImplodeDecodeC();
u32 ImplodeDecodeOffset();

void ImplodeMakeTable(i32, u8 *, i32, u16 *);

void ImplodeHufDecodeStart();
void ImplodeInitGetBits();

u32 ImplodeGetBits(i32 count);
void ImplodeFillBuf(i32 count);
u8 ImplodeGetByteFromMem();

void ImplodeError(char *msg, ...);

#endif
