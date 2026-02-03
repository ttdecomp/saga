#include "nu2api.saga/nu3d/nustream.h"

#include <string.h>

#include "nu2api.saga/numemory/numemory.h"

unsigned int rndrstream_nbuffers = 2;
int nurndr_maxstreamsize;
VARIPTR rndrstream_end;
VARIPTR rndrstream_free;
static VARIPTR rndrstream[NURNDR_STREAM_MAX_BUFFERS];

static int rndrstream_buffid;

void NuRndrStreamInit(int stream_buffer_size, VARIPTR *buffer) {
    if (buffer == NULL) {
        for (int i = 0; i < rndrstream_nbuffers; i++) {
            rndrstream[i].void_ptr = NuMemoryGet()->GetThreadMem()->_BlockAlloc(stream_buffer_size, 4, 1, "", 0);
            memset(rndrstream[i].void_ptr, 0, stream_buffer_size);
        }
    } else {
        for (int i = 0; i < rndrstream_nbuffers; i++) {
            buffer->addr = ALIGN(buffer->addr, 128);
            rndrstream[i].void_ptr = buffer->void_ptr;
            memset(buffer->void_ptr, 0, stream_buffer_size);
            buffer->void_ptr = (void *)(buffer->addr + stream_buffer_size);
        }
    }

    rndrstream_buffid = 0;
    nurndr_maxstreamsize = stream_buffer_size;
    rndrstream_end.addr = rndrstream[0].addr + stream_buffer_size;
    rndrstream_free.addr = ALIGN(rndrstream[0].addr, 16);
}
