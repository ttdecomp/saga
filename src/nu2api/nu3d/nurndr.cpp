#include "decomp.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/numtl.h"

#include <string.h>

#include "nu2api/nucore/numemory.h"

struct nugobj_s;
struct PartHeader;
struct uv1debdata;
struct nuvec4_s;

i32 nurndr_pixel_width;
i32 nurndr_pixel_height;

void NuRndrInitEx(i32 stream_buffer_size, VARIPTR *buffer) {
    NuRndrStreamInit(stream_buffer_size, buffer);
}

u32 rndrstream_nbuffers = 2;
i32 nurndr_maxstreamsize;
VARIPTR rndrstream_end;
VARIPTR rndrstream_free;
static VARIPTR rndrstream[NURNDR_STREAM_MAX_BUFFERS];

static i32 rndrstream_buffid;

void NuRndrStreamInit(i32 stream_buffer_size, VARIPTR *buffer) {
    i32 i;

    if (buffer == NULL) {
        for (i = 0; i < rndrstream_nbuffers; i++) {
            rndrstream[i].void_ptr = NU_ALLOC(stream_buffer_size, 4, 1, "", NUMEMORY_CATEGORY_NONE);
            memset(rndrstream[i].void_ptr, 0, stream_buffer_size);
        }
    } else {
        for (i = 0; i < rndrstream_nbuffers; i++) {
            buffer->addr = ALIGN(buffer->addr, 0x80);
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

void NuRndrGobj(nugobj_s *, numtx_s *) {
}

void NuRndrFlush(i32) {
}

void NuRndrRectUV2d(float, float, float, float, float, float, float, float, float, i32, numtl_s *) {
}

void NuRndrFlickerEnd() {
}

void NuRndrRectUV2diZ(i32, i32, i32, i32, float, float, float, float, i32, numtl_s *, i32) {
}

void NuRndrSetScissor(i32, i32, i32, i32) {
}

void NuRndrInitGeneric() {
}

void NuRndrSetXYOffset(i32, i32) {
}

void NuRndrGradRect2diZ(i32, i32, i32, i32, i32 *, numtl_s *, i32) {
}

void NuRndrParticleDraw(variptr_u *, PartHeader *, uv1debdata *, float, numtx_s *, i32 *, float, i32, numtl_s *, float,
                        float) {
}

void NuRndrBurstObjBegin(nugobj_s *, void (*)(rndrstream_s *, numtx_s *, i32)) {
}

void NuRndrRectUV2dNoScale(float, float, float, float, float, float, float, float, i32, numtl_s *) {
}

void NuRndrFlickerBeginScene() {
}

void NuRndrParticleSetRepeat(nuvec_s *) {
}

void NuRndrCalcRandEllipsePos(nuvec4_s *, numtx_s *, nuvec_s *) {
}

void NuRndrCalcRandCylinderPos(nuvec4_s *, numtx_s *, nuvec_s *) {
}

void NuRndrCreateBlendShapeDWAPointers(i32) {
}

struct nuglobalrndrstate_s;

extern "C" {
    static __used__ void NuRndrSetAmbientLight(f32 *) {
    }

    static __used__ void NuRndrSetDirectionalLights(f32 *, f32 *, f32 *, f32 *, f32 *, f32 *) {
    }

    static __used__ void NuRndrSetSpecularLight(nuvec_s *, nucolour4_s *) {
    }

    static __used__ void RndrStateClear(struct nuglobalrndrstate_s *) {
    }
}
