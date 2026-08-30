#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include <vorbis/codec.h>
#include <ogg/ogg.h>
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void res1_class(vorbis_block *, void *, i32 **, i32 *, i32) {
}

void res2_class(vorbis_block *, void *, i32 **, i32 *, i32) {
}

void ImplodePutI(void *, u32, i32) {
}

void res0_unpack(vorbis_info *, oggpack_buffer *) {
}

void refpack_init() {
}

void res0_inverse(vorbis_block *, void *, float **, i32 *, i32) {
}

void res1_forward(oggpack_buffer *, vorbis_block *, void *, i32 **, i32 *, i32, abi_long **, i32) {
}

void res1_inverse(vorbis_block *, void *, float **, i32 *, i32) {
}

void res2_forward(oggpack_buffer *, vorbis_block *, void *, i32 **, i32 *, i32, abi_long **, i32) {
}

void res2_inverse(vorbis_block *, void *, float **, i32 *, i32) {
}

void res0_free_info(void *) {
}

void res0_free_look(void *) {
}

void ImplodeFReadMem(unsigned char *, i32) {
}

void ImplodeMakeTree(i32, u16 *, unsigned char *, u16 *) {
}

void ImplodePutByteToMem(unsigned char) {
}

void refpack(unsigned char *, abi_long, unsigned char *) {
}

void res0_look(vorbis_dsp_state *, void *) {
}

void res0_pack(void *, oggpack_buffer *) {
}
