#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void PopBalloon(GameObject_s *) {
}

void Disorientate(GameObject_s *, nuvec_s *) {
}

void SpecialSfxAdd(i32) {
}

void SpecialSfxLoad(char *, WORLDINFO_s *) {
}

void EngineNoiseCode(GameObject_s *, i32) {
}

void NewSeekHalfLife(i32 &, i32, float) {
}

void DisorientateCode(GameObject_s *, nuvec_s *, float) {
}

void UpdateSpecialSfx(WORLDINFO_s *) {
}

void SetSpecialSfxBits(i32 *, i32 *, WORLDINFO_s *) {
}

extern "C" i32 GetSfxIdN(char *, i32);

template <typename T> static inline void EffectField(debinftype *effect, usize offset, T value) {
    *reinterpret_cast<T *>(reinterpret_cast<u8 *>(effect) + offset) = value;
}

void FileLoadSingleEffectType(debinftype *effect, i32 version, char category) {
    // All shipped Android general/character pages use version 41.  Keep this
    // branch expressed as the original typed reads: the file record is not a
    // byte-for-byte image of debinftype (runtime fields and padding differ).
    if (version != 41) {
        return;
    }

    u8 *bytes = reinterpret_cast<u8 *>(effect);
    EdFileRead(effect->name, sizeof(effect->name));
    effect->category = static_cast<u8>(category);
    effect->frequency = EdFileReadShort();
    effect->max_particles = EdFileReadShort();
    effect->emission_period = EdFileReadFloat();
    effect->emission_period_random = EdFileReadFloat();
    effect->emission_pause = EdFileReadFloat();
    effect->emission_pause_random = EdFileReadFloat();
    effect->start_offset_random = EdFileReadFloat();
    if (effect->frequency != 0) {
        const f32 minimum_period = 1.0f / static_cast<f32>(effect->frequency);
        if (effect->emission_period_random <= minimum_period && effect->emission_period_random != minimum_period) {
            effect->emission_period_random = minimum_period;
        }
    }

    effect->generator_type = static_cast<u8>(EdFileReadChar());
    effect->momentum_adjustment_type = static_cast<u8>(EdFileReadChar());
    effect->cutscene_only = static_cast<u8>(EdFileReadChar());
    effect->disabled = 0;
    effect->particle_type = static_cast<u8>(EdFileReadChar());
    effect->status = 1;
    effect->camera_facing = static_cast<u8>(EdFileReadChar());

    for (usize offset = 0x30; offset <= 0xa4; offset += sizeof(f32)) {
        EffectField<f32>(effect, offset, EdFileReadFloat());
    }
    EffectField<i16>(effect, 0xa8, EdFileReadShort());
    bytes[0xaa] = static_cast<u8>(EdFileReadChar());
    bytes[0xab] = static_cast<u8>(EdFileReadChar());
    for (usize offset = 0xac; offset <= 0xbc; offset += sizeof(f32)) {
        EffectField<f32>(effect, offset, EdFileReadFloat());
    }

    for (usize offset = 0xc0; offset <= 0xf8; offset += 8) {
        EffectField<f32>(effect, offset, EdFileReadFloat());
        bytes[offset + 4] = EdFileReadUnsignedChar();
        bytes[offset + 5] = EdFileReadUnsignedChar();
        bytes[offset + 6] = EdFileReadUnsignedChar();
        bytes[offset + 7] = EdFileReadUnsignedChar();
    }
    for (usize offset = 0x100; offset <= 0x13c; offset += sizeof(f32)) {
        EffectField<f32>(effect, offset, EdFileReadFloat());
    }
    EffectField<f32>(effect, 0x140, EdFileReadFloat());
    EffectField<f32>(effect, 0x144, EdFileReadFloat());
    for (usize offset = 0x148; offset <= 0x2a4; offset += sizeof(f32)) {
        EffectField<f32>(effect, offset, EdFileReadFloat());
    }
    for (usize offset = 0; offset < sizeof(effect->fields_2b0); offset += sizeof(f32)) {
        *reinterpret_cast<f32 *>(effect->fields_2b0 + offset) = EdFileReadFloat();
    }

    effect->process_spheres = static_cast<u8>(EdFileReadChar());
    effect->time_group = static_cast<u8>(EdFileReadChar());
    if (effect->particle_type == 7) {
        effect->time_group = 2;
    }
    effect->field_2f2 = static_cast<u8>(EdFileReadChar());
    effect->use_explicit_clip_box = static_cast<u8>(EdFileReadChar());
    *reinterpret_cast<f32 *>(effect->fields_2f8 + 0x0) = EdFileReadFloat();
    *reinterpret_cast<f32 *>(effect->fields_2f8 + 0x4) = EdFileReadFloat();
    *reinterpret_cast<f32 *>(effect->fields_2f8 + 0x8) = EdFileReadFloat();
    effect->thinning = EdFileReadFloat();
    for (usize offset = 0xc; offset < sizeof(effect->fields_2f8); offset += sizeof(f32)) {
        *reinterpret_cast<f32 *>(effect->fields_2f8 + offset) = EdFileReadFloat();
    }

    for (usize i = 0; i < sizeof(effect->particle_keys) / sizeof(effect->particle_keys[0]); ++i) {
        effect->particle_keys[i] = -1;
    }
    for (usize i = 0; i < sizeof(effect->sound_data) / sizeof(effect->sound_data[0]); i += 3) {
        effect->sound_data[i] = -1;
    }
    const i32 sound_count = EdFileReadInt();
    for (i32 i = 0; i < sound_count; ++i) {
        char sound_name[16];
        EdFileRead(sound_name, sizeof(sound_name));
        const usize index = static_cast<usize>(i) * 3;
        effect->sound_data[index] = GetSfxIdN(sound_name, sizeof(sound_name));
        effect->sound_data[index + 1] = EdFileReadInt();
        effect->sound_data[index + 2] = EdFileReadInt();
    }

    effect->trail_count = static_cast<u8>(EdFileReadChar());
    effect->trail_time = EdFileReadFloat();
    effect->radial_segments = static_cast<u8>(EdFileReadChar());
    effect->radial_floor = EdFileReadFloat();
    effect->scale_in_time = EdFileReadFloat();
    effect->scale = 1.0f;
    effect->unscaled_effect_index = 0;

    if (NuStrCmp(effect->name, (char *)"STARDESTROYER") == 0) {
        effect->frequency = 0;
    }
}
