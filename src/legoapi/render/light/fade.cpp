#include "legoapi/legoapi_types.h"
#include "legoapi/core/input/qrand.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/nutrig.h"
extern f32 FRAMETIME;
extern i32 pause_rndr_on;
extern i32 wait_till_next_frame;
extern void NeedScreenGrab(i32);
extern void DrawStillScreen(i32);
extern void DrawPauseScreenWipe(void);
extern void SetFramesToWait(u32);
extern void DrawFadeScreenWipe(void);
extern "C" void NuRndrRect2di(i32, i32, i32, i32, i32, numtl_s *);

// The four global fade effect objects handed to FadeSystem::AddFade() during
// LoadPermData (original bss @0x127bf08 / 0x127bf00 / 0x127bef8 / 0x127bef0).
Fade fade;
FadeWipe fadeWipe;
FadeStillWipe fadeStillWipe;
FadeStill fadeStill;

// Material handles used by the original 2-D fade paths.
NUMTL *ScreenFadeMtl;
NUMTL *FadeMtl2;
NUMTL *FadeMtl;

i32 FadeSystem::AddFade(FadeBase *o) {
    if (o == NULL)
        return 0;
    FADETYPE_VALUE type = o->GetFadeType();
    o->Init(this);
    fades[type] = o;
    return 1;
}

void FadeSystem::Draw() {
    if (pending_type != FADE_TYPE_NONE && fades[pending_type] != NULL)
        fades[pending_type]->DrawFade();
}

void FadeSystem::Init() {
    fades[0] = NULL;
    fades[1] = NULL;
    fades[2] = NULL;
    fades[3] = NULL;
    field_28 = 1;
    pending_type = FADE_TYPE_NONE;
}

i32 FadeSystem::SetFade(FADETYPE const &t, u32 frames) {
    FADETYPE_VALUE type = t.type;
    if (type != FADE_TYPE_NONE && fades[type] != NULL) {
        pending_type = type;
        direction = frames;
        return 1;
    }
    pending_type = FADE_TYPE_NONE;
    return 0;
}

void FadeSystem::SetStage(char stage) {
    this->stage = stage;
    busy = 0;
    if (pending_type != FADE_TYPE_NONE && fades[pending_type] != NULL)
        fades[pending_type]->InitFade();
}

void FadeSystem::Update() {
    f32 old_fade = fade;
    FADETYPE_VALUE type = pending_type;
    if (type == FADE_TYPE_NONE)
        return;

    f32 next = fade + rate * FRAMETIME;
    if (next > 1.0f)
        fade = 1.0f;
    else
        fade = next < 0.0f ? 0.0f : next;

    if (old_fade < 1.0f && fade == 1.0f)
        busy = 1;
    else if (busy != 0)
        --busy;

    if (fades[type] != NULL)
        fades[type]->UpdateFade();
    if (fade == 0.0f || fade == 1.0f)
        rate = 0.0f;
    if (fade == 0.0f)
        pending_type = FADE_TYPE_NONE;
}

FADETYPE_VALUE Fade::GetFadeType() const {
    return FADE_TYPE_SCREEN;
}
FADETYPE_VALUE FadeWipe::GetFadeType() const {
    return FADE_TYPE_WIPE;
}
FADETYPE_VALUE FadeStillWipe::GetFadeType() const {
    return FADE_TYPE_STILL_WIPE;
}
FADETYPE_VALUE FadeStill::GetFadeType() const {
    return FADE_TYPE_STILL;
}

void FadeStillWipe::DrawFade() {
    if (wait_till_next_frame != 0)
        return;
    if (info->stage & 2)
        DrawStillScreen(1);
    else
        DrawPauseScreenWipe();
}

void FadeStillWipe::Init(FADEINFO_s *state) {
    info = state;
}

void FadeStillWipe::InitFade() {
    const u32 old_direction = info->direction;
    if ((info->stage & 1) == 0) {
        info->fade = 1.0f;
        info->rate = 2.0f;
        pause_rndr_on = 0;
        if (info->field_28 == 0)
            info->field_28 = 1;
        else
            NeedScreenGrab(1);
        wait_till_next_frame = 1;
    } else {
        info->fade = 1.0f;
        info->rate = -1.3333334f;
    }
    do {
        i32 value = qrand();
        if (value < 0)
            value += 0x3fff;
        info->direction = 1u << (value >> 14);
    } while (info->direction == old_direction);
}

void FadeStillWipe::UpdateFade() {
    if (wait_till_next_frame > 0)
        --wait_till_next_frame;
    if ((info->stage & 2) && wait_till_next_frame == 0)
        pause_rndr_on = 1;
}

void Fade::DrawFade() {
    if (info->fade > 0.0f && FadeMtl != NULL) {
        const i32 index = (static_cast<i32>(info->fade * 16384.0f + 65536.0f) >> 1) & 0x7fff;
        NuRndrRect2di(0, 0, 0x2800, 0xe00, static_cast<i32>(NuTrigTable[index] * 128.0f) << 24, ScreenFadeMtl);
    }
}

void Fade::Init(FADEINFO_s *state) {
    info = state;
}

void Fade::InitFade() {
    if ((info->stage & 1) == 0) {
        info->fade = 0.0f;
        info->rate = 2.0f;
    } else {
        info->fade = 1.0f;
        info->rate = -1.3333334f;
    }
}

void Fade::UpdateFade() {
}

void FadeWipe::DrawFade() {
    DrawFadeScreenWipe();
}

void FadeWipe::Init(FADEINFO_s *state) {
    info = state;
}

void FadeWipe::InitFade() {
    const u32 old = info->direction;
    if ((info->stage & 1) == 0) {
        info->fade = 0.0f;
        info->rate = 2.0f;
    } else {
        info->fade = 1.0f;
        info->rate = -1.3333334f;
    }
    do {
        i32 value = qrand();
        if (value < 0)
            value += 0x3fff;
        info->direction = 1u << (value >> 14);
    } while (info->direction == old);
}

void FadeWipe::UpdateFade() {
}

void FadeStill::DrawFade() {
    if (wait_till_next_frame != 0)
        return;
    if (info->stage & 2)
        DrawStillScreen(1);
    else
        DrawPauseScreenWipe();
}

void FadeStill::Init(FADEINFO_s *state) {
    info = state;
}

void FadeStill::InitFade() {
    const u32 old_direction = info->direction;
    if ((info->stage & 1) == 0) {
        info->fade = 1.0f;
        info->rate = 2.0f;
        NeedScreenGrab(1);
        wait_till_next_frame = 1;
    } else {
        info->fade = 0.0f;
        info->rate = 0.0f;
    }
    do {
        i32 value = qrand();
        if (value < 0)
            value += 0x3fff;
        info->direction = 1u << (value >> 14);
    } while (info->direction == old_direction);
}

void FadeStill::UpdateFade() {
    if (info->stage & 2)
        pause_rndr_on = 1;
}

void FadeLoop_SetObj(nugscn_s *, char *) {
}

void FadeLoop_DrawObj(float) {
}

void FadeLoop_UsingObj() {
}

void CreateFadeMaterials() {
    ScreenFadeMtl = NuMtlCreate(1);
    u8 *attrib = reinterpret_cast<u8 *>(&ScreenFadeMtl->attribs);
    attrib[1] = static_cast<u8>((attrib[1] & 0x30) | 0xc5);
    attrib[0] = static_cast<u8>((attrib[0] & 0xc0) | 0x13);
    attrib[2] = static_cast<u8>((attrib[2] & 0xfc) | 0x06);
    NuMtlUpdate(ScreenFadeMtl);

    FadeMtl2 = NuMtlCreate(1);
    attrib = reinterpret_cast<u8 *>(&FadeMtl2->attribs);
    attrib[1] = static_cast<u8>((attrib[1] & 0x30) | 0xc5);
    attrib[0] = static_cast<u8>((attrib[0] & 0xc0) | 0x11);
    attrib[2] = static_cast<u8>((attrib[2] & 0xfc) | 0x06);
    NuMtlUpdate(FadeMtl2);

    FadeMtl = NuMtlCreate(1);
    FadeMtl->opacity = 1.0f;
    FadeMtl->tex_id = 0;
    FadeMtl->sort_pri = 0x7ffe;
    attrib = reinterpret_cast<u8 *>(&FadeMtl->attribs);
    attrib[1] = static_cast<u8>((attrib[1] & 0x30) | 0xc5);
    attrib[2] = static_cast<u8>((attrib[2] & 0xfc) | 0x06);
    attrib[0] = static_cast<u8>((attrib[0] & 0xc0) | 0x11);
    NuMtlUpdate(FadeMtl);
}

void FadeLoop(char *, i32, float, void (*)(float)) {
}
