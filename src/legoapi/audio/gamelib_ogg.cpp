#include "decomp.h"
#include "legoapi/legoapi_types.h"
struct nuqthdr_s;
struct nuqtdim_s;

void ov_bitrate(OggVorbis_File *, i32) {
}

void ov_comment(OggVorbis_File *, i32) {
}

void ov_streams(OggVorbis_File *) {
}

void ov_crosslap(OggVorbis_File *, OggVorbis_File *) {
}

void ov_halfrate(OggVorbis_File *, i32) {
}

void ov_pcm_seek(OggVorbis_File *, i64) {
}

void ov_pcm_tell(OggVorbis_File *) {
}

void ov_raw_seek(OggVorbis_File *, i64) {
}

void ov_raw_tell(OggVorbis_File *) {
}

void ov_seekable(OggVorbis_File *) {
}

void ov_pcm_total(OggVorbis_File *, i32) {
}

void ov_raw_total(OggVorbis_File *, i32) {
}

void ov_test_open(OggVorbis_File *) {
}

void ov_time_seek(OggVorbis_File *, double) {
}

void ov_time_tell(OggVorbis_File *) {
}

void ov_halfrate_p(OggVorbis_File *) {
}

void ov_read_float(OggVorbis_File *, float ***, i32, i32 *) {
}

void ov_time_total(OggVorbis_File *, i32) {
}

void vorbis_window(vorbis_dsp_state *, i32) {
}

void ov_read_filter(OggVorbis_File *, char *, i32, i32, i32, i32, i32 *, void (*)(float **, abi_long, abi_long, void *),
                    void *) {
}

void ov_pcm_seek_lap(OggVorbis_File *, i64) {
}

void ov_raw_seek_lap(OggVorbis_File *, i64) {
}

void ov_serialnumber(OggVorbis_File *, i32) {
}

void ov_pcm_seek_page(OggVorbis_File *, i64) {
}

void ov_time_seek_lap(OggVorbis_File *, double) {
}

void vorbis_dsp_clear(vorbis_dsp_state *) {
}

void vorbis_info_init(vorbis_info *) {
}

void vorbis_synthesis(vorbis_block *, ogg_packet *) {
}

void ov_open_callbacks(void *, OggVorbis_File *, char const *, abi_long, ov_callbacks) {
}

void ov_test_callbacks(void *, OggVorbis_File *, char const *, abi_long, ov_callbacks) {
}

void ov_time_seek_page(OggVorbis_File *, double) {
}

void vorbis_block_init(vorbis_dsp_state *, vorbis_block *) {
}

void vorbis_book_clear(codebook *) {
}

void vorbis_info_clear(vorbis_info *) {
}

void vorbis_lpc_to_lsp(float *, float *, i32) {
}

void ov_bitrate_instant(OggVorbis_File *) {
}

void vorbis_block_clear(vorbis_block *) {
}

void vorbis_book_decode(codebook *, oggpack_buffer *) {
}

void vorbis_book_encode(codebook *, i32, oggpack_buffer *) {
}

void vorbis_comment_add(vorbis_comment *, char const *) {
}

void vorbis_lpc_predict(float *, float *, i32, float *, abi_long) {
}

void vorbis_bitrate_init(vorbis_info *, bitrate_manager_state *) {
}

void vorbis_book_codelen(codebook *, i32) {
}

void vorbis_comment_init(vorbis_comment *) {
}

void vorbis_granule_time(vorbis_dsp_state *, i64) {
}

void vorbis_lsp_to_curve(float *, i32 *, i32, i32, float *, i32, float, float) {
}

void ov_pcm_seek_page_lap(OggVorbis_File *, i64) {
}

void vorbis_analysis_init(vorbis_dsp_state *, vorbis_info *) {
}

void vorbis_bitrate_clear(bitrate_manager_state *) {
}

void vorbis_book_codeword(codebook *, i32) {
}

void vorbis_comment_clear(vorbis_comment *) {
}

void vorbis_comment_query(vorbis_comment *, char const *, i32) {
}

void vorbis_lpc_from_data(float *, float *, i32, i32) {
}

void ov_time_seek_page_lap(OggVorbis_File *, double) {
}

void vorbis_analysis_wrote(vorbis_dsp_state *, i32) {
}

void vorbis_info_blocksize(vorbis_info *, i32) {
}

void vorbis_synthesis_init(vorbis_dsp_state *, vorbis_info *) {
}

void vorbis_synthesis_read(vorbis_dsp_state *, i32) {
}

void vorbis_version_string() {
}

void vorbis_analysis_buffer(vorbis_dsp_state *, i32) {
}

void vorbis_bitrate_managed(vorbis_block *) {
}

void vorbis_comment_add_tag(vorbis_comment *, char const *, char const *) {
}

void vorbis_staticbook_pack(static_codebook const *, oggpack_buffer *) {
}

void vorbis_bitrate_addblock(vorbis_block *) {
}

void vorbis_book_decodev_add(codebook *, float *, oggpack_buffer *, i32) {
}

void vorbis_book_decodev_set(codebook *, float *, oggpack_buffer *, i32) {
}

void vorbis_book_init_decode(codebook *, static_codebook const *) {
}

void vorbis_book_init_encode(codebook *, static_codebook const *) {
}

void vorbis_packet_blocksize(vorbis_info *, ogg_packet *) {
}

void vorbis_synthesis_lapout(vorbis_dsp_state *, float ***) {
}

void vorbis_synthesis_pcmout(vorbis_dsp_state *, float ***) {
}

void vorbis_analysis_blockout(vorbis_dsp_state *, vorbis_block *) {
}

void vorbis_book_decodevs_add(codebook *, float *, oggpack_buffer *, i32) {
}

void vorbis_book_decodevv_add(codebook *, float **, abi_long, i32, oggpack_buffer *, i32) {
}

void vorbis_commentheader_out(vorbis_comment *, ogg_packet *) {
}

void vorbis_staticbook_unpack(oggpack_buffer *) {
}

void vorbis_synthesis_blockin(vorbis_dsp_state *, vorbis_block *) {
}

void vorbis_synthesis_restart(vorbis_dsp_state *) {
}

void vorbis_analysis_headerout(vorbis_dsp_state *, vorbis_comment *, ogg_packet *, ogg_packet *, ogg_packet *) {
}

void vorbis_staticbook_destroy(static_codebook *) {
}

void vorbis_synthesis_halfrate(vorbis_info *, i32) {
}

void vorbis_synthesis_headerin(vorbis_info *, vorbis_comment *, ogg_packet *) {
}

void vorbis_synthesis_idheader(ogg_packet *) {
}

void vorbis_bitrate_flushpacket(vorbis_dsp_state *, ogg_packet *) {
}

void vorbis_comment_query_count(vorbis_comment *, char const *) {
}

void vorbis_synthesis_trackonly(vorbis_block *, ogg_packet *) {
}

void vorbis_synthesis_halfrate_p(vorbis_info *) {
}

void ov_info(OggVorbis_File *, i32) {
}

void ov_open(__sFILE *, OggVorbis_File *, char const *, abi_long) {
}

void ov_read(OggVorbis_File *, char *, i32, i32, i32, i32, i32 *) {
}

void ov_test(__sFILE *, OggVorbis_File *, char const *, abi_long) {
}

void ov_clear(OggVorbis_File *) {
}

void ov_fopen(char const *, OggVorbis_File *) {
}

static __used__ void mapping0_free_info(void *) {
}

static __used__ void mapping0_forward(vorbis_block *) {
}

static __used__ void mapping0_inverse(vorbis_block *, void *) {
}

static __used__ void mapping0_pack(vorbis_info *, void *, oggpack_buffer *) {
}

static __used__ void mapping0_unpack(vorbis_info *, oggpack_buffer *) {
}

static __used__ int local_book_besterror(codebook *, int *) {
    return 0;
}

static __used__ void AddElementR(nuqthdr_s*, int, nuqtdim_s*, nuqtdim_s*, void*, int) {}

static __used__ void AddNode(nuqthdr_s*, int) {}

static __used__ void bark_noise_hybridmp(int, long const*, float const*, float*, float, int) {}  // NOLINT(google-runtime-int)
static __used__ void dradb2(int, int, float*, float*, float*) {}

static __used__ void dradb3(int, int, float*, float*, float*, float*) {}

static __used__ void dradb4(int, int, float*, float*, float*, float*, float*) {}

static __used__ void dradbg(int, int, int, int, float*, float*, float*, float*, float*, float*) {}

static __used__ void dradf2(int, int, float*, float*, float*) {}

static __used__ void dradf4(int, int, float*, float*, float*, float*, float*) {}

static __used__ void dradfg(int, int, int, int, float*, float*, float*, float*, float*, float*) {}

static __used__ int comp(void const*, void const*) { return {}; }

static __used__ int apsort(void const*, void const*) { return {}; }

static __used__ void downheap(int) {}

static __used__ int count_len(int) { return {}; }

static __used__ void RemoveData(nuqthdr_s *, char *, i32) {
}

void drft_clear(drft_lookup *) {
}

void floor1_fit(vorbis_block *, vorbis_look_floor1 *, float const *, float const *) {
}

void drft_forward(drft_lookup *, float *) {
}

void _float32_pack(float) {
}

void floor1_encode(oggpack_buffer *, vorbis_block *, vorbis_look_floor1 *, i32 *, i32 *) {
}

void _float32_unpack(abi_long) {
}

void floor1_interpolate_fit(vorbis_block *, vorbis_look_floor1 *, i32 *, i32 *, i32) {
}

void _ilog(u32) {
}

void drft_init(drft_lookup *, i32) {
}

extern "C" {

    void OggAllocMem(void) {
    }

    void OggFreeMem(void) {
    }

    void OggReAllocMem(void) {
    }

} // extern "C"
