#include "decomp.h"
#include "legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void PlayAMusic(i32, i32, i32, i32) {
}

void mdct_clear(mdct_lookup *) {
}

void _vi_psy_free(vorbis_info_psy *) {
}

void _vp_psy_init(vorbis_look_psy *, vorbis_info_psy *, vorbis_info_psy_global *, i32, abi_long) {
}

void _vp_tonemask(vorbis_look_psy *, float *, float *, float, float) {
}

void mdct_forward(mdct_lookup *, float *, float *) {
}

void GetMusicIndex(char *, nusound_filename_info_s *, i32) {
}

void MusicClearAll() {
}

void _vi_gpsy_free(vorbis_info_psy_global *) {
}

void _vp_noisemask(vorbis_look_psy *, float *, float *) {
}

void _vp_psy_clear(vorbis_look_psy *) {
}

void drft_backward(drft_lookup *, float *) {
}

void mdct_backward(mdct_lookup *, float *, float *) {
}

void SpaceAudioPoint() {
}

void _vp_global_free(vorbis_look_psy_global *) {
}

void _vp_global_look(vorbis_info *) {
}

void _book_unquantize(static_codebook const *, i32, i32 *) {
}

void _vp_ampmax_decay(float, vorbis_dsp_state *) {
}

void legoSetCutVolume(float) {
}

void GetAudioFadeLevel() {
}

void PS2VolumeToScalar(i32) {
}

void _ve_envelope_init(envelope_lookup *, vorbis_info *) {
}

void _ve_envelope_mark(vorbis_dsp_state *) {
}

void SetBackgroundMusic(i32) {
}

void _ve_envelope_clear(envelope_lookup *) {
}

void _ve_envelope_shift(envelope_lookup *, abi_long) {
}

void _vorbis_window_get(i32) {
}

void _vp_offset_and_mix(vorbis_look_psy *, float *, float *, i32, float *, float *, float *) {
}

void legoSetMusicVolume(float) {
}

void ProcessMusicChanges(LEVELDATA_s *, OPTIONSSAVE_s *) {
}

void _ve_envelope_search(vorbis_dsp_state *) {
}

void _vorbis_block_alloc(vorbis_block *, abi_long) {
}

void SpaceResetAudioPoint() {
}

void _vorbis_apply_window(float *, i32 *, abi_long *, i32, i32, i32) {
}

void CheckMusicSwapInstant() {
}

void UpdateBackgroundMusic() {
}

void _vorbis_block_ripcord(vorbis_block *) {
}

void _book_maptype1_quantvals(static_codebook const *) {
}

void _vp_couple_quantize_normalize(i32, vorbis_info_psy_global *, vorbis_look_psy *, vorbis_info_mapping0 *, float **,
                                   i32 **, i32 *, i32, i32) {
}

void mdct_init(mdct_lookup *, i32) {
}

extern "C" {

        void GetCurPreSeek(void) {
        }

        void GetCurrentMusicId(void) {
        }

        void GetOppMusicId(void) {
        }

        void MusicPreSeek(void) {
        }

        void MusicPreSeekNow(void) {
        }

        void MusicSeekOffset(void) {
        }

        void MusicSeeking(void) {
        }

        void MusicState(void) {
        }

} // extern "C"

