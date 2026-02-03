#pragma once

#include <cstddef>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif
    ssize_t ExplodeBufferNoHeader(char *in_buf, char *out_buf, ssize_t orig_size, ssize_t compressed_size);

    ssize_t ExplodeBufferSize(char *buf);
    ssize_t ExplodeCompressedSize(char *buf);

    extern char *implode_inbuffer;
    extern char *implode_outbuffer;
    extern size_t implode_origsize;
    extern size_t implode_compsize;
    extern uint32_t implode_bitbuf;

    extern uint16_t implode_left[];
    extern uint16_t implode_right[];
#ifdef __cplusplus
}

int32_t ImplodeGetI(void *buf, int32_t size);

uint32_t ImplodeDecodeC();
uint32_t ImplodeDecodeOffset();

void ImplodeMakeTable(int32_t, uint8_t *, int32_t, uint16_t *);

void ImplodeHufDecodeStart();
void ImplodeInitGetBits();

uint32_t ImplodeGetBits(int32_t count);
void ImplodeFillBuf(int32_t count);
uint8_t ImplodeGetByteFromMem();

void ImplodeError(char *msg, ...);

#endif
