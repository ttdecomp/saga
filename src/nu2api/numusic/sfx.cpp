#include "nu2api/numusic/sfx.h"

#include "gamelib/crc/crc.h"
#include "globals.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nusound/nusound.h"

#include <cstring>

static float g_audioVersion;

NUSOUNDINFO *g_soundInfo;
NUSOUNDINFO *g_revertSoundInfo;

u16 *g_soundMap;
nusound_filename_info_s *SfxInfo = NULL;

static i32 NumSfx = 0;
static i32 NumSfxInst = 0;
static u32 NumSfxNames = 0;

static char sfx_name[1600][32] = {0};
static char sfx_filename[1600][64];
static i32 sfx_refcount[1600] = {0};

static char cfgfile_name[256] = "Audio/audio.cfg";

void InitSoundInfo(i32 index) {
    NUSOUNDINFO *info = &g_soundInfo[index];
    info->index = -1;
    info->flag_bytes[1] &= 0x7f;
    info->priority = 0;
    info->volume = 0x3fff;
    info->pan = 0.0;
    info->group = -1;
    info->pitch_rnd = 0.0;
    info->rumble_strength = 0;
    info->sfx_name = sfx_name[NumSfxInst];
    u8 flags = info->flag_bytes[2];
    info->category = 0;
    flags &= 0x88;
    info->field29_0x40 = 0;
    info->volume_rnd = 0.0;
    info->flag_bytes[2] = flags;
    info->falloff_near = 0.0;
    info->falloff_far = 0.0;
    info->buzz_timer = 0.0;
    info->rumble_sustain = 0.0;
    info->rumble_release = 0.0;

    g_revertSoundInfo[index] = *info;
}

static void fnAudioAudio(nufpar_s *fpar) {
    g_audioVersion = NuFParGetFloat(fpar);
}

static void fnAudioSample(nufpar_s *fpar) {
    InitSoundInfo(NumSfxInst);

    NUSOUND_FILENAME_INFO *finfo = NULL;
    while (true) {
        if (NuFParGetWord(fpar) == 0) {
            NUSOUNDINFO *sinfo = &g_soundInfo[NumSfxInst];
            sinfo->dirty = 0;
            sinfo->revertable = 1;
            memmove(&g_revertSoundInfo[NumSfxInst], sinfo, sizeof(NUSOUNDINFO));
            NumSfxInst++;
            return;
        }

        if (NuStrICmp(fpar->word_buf, "disable") == 0) {
            g_soundInfo[NumSfxInst].disabled = 1;
        } else if (NuStrICmp(fpar->word_buf, "comment") == 0) {
            g_soundInfo[NumSfxInst].comment = 1;
        } else if (NuStrICmp(fpar->word_buf, "maxvoicesbehaviour") == 0) {
            NuFParGetWord(fpar);
            if (NuStrICmp(fpar->word_buf, "noplay") == 0) {
                g_soundInfo[NumSfxInst].field29_0x40 = 0;
            } else if (NuStrICmp(fpar->word_buf, "replace") == 0) {
                g_soundInfo[NumSfxInst].field29_0x40 = 1;
            }
        } else if (NuStrICmp(fpar->word_buf, "name") == 0) {
            NuFParGetWord(fpar);
            NuStrNCpy(sfx_name[NumSfxInst], fpar->word_buf, 0x20);

            const char *str = sfx_name[NumSfxInst];
            g_soundInfo[NumSfxInst].sfx_name = str;
            u32 hash = CRC_ProcessStringIgnoreCase(str);

            u16 *id = &g_soundMap[hash & 0xff];
            if (*id == -1) {
                *id = NumSfxInst;
                g_soundInfo[NumSfxInst].next = -1;
            } else {
                g_soundInfo[NumSfxInst].next = *id;
                *id = NumSfxInst;
            }
        } else if (NuStrICmp(fpar->word_buf, "fname") == 0) {
            NuFParGetWord(fpar);

            if (g_soundInfo[NumSfxInst].disabled == 0 && g_soundInfo[NumSfxInst].comment == 0) {
                i32 i = 0;
                for (; i < NumSfx; i++) {
                    if (NuStrICmp(SfxInfo[i].filename, fpar->word_buf) == 0) {
                        finfo = &SfxInfo[i];
                        g_soundInfo[NumSfxInst].index = i;
                        g_soundInfo[NumSfxInst].filename = finfo->filename;
                        sfx_refcount[i]++;
                        break;
                    }
                }
                if (i == NumSfx) {
                    const i32 filename_index = NumSfxNames;
                    finfo = &SfxInfo[i];
                    finfo->filename = sfx_filename[filename_index];
                    finfo->field4_0x4 = NULL;
                    finfo->index = i + 0x1000 - SFX_MUSIC_COUNT;
                    NumSfxNames = filename_index + 1;
                    NuStrNCpy(const_cast<char *>(finfo->filename), fpar->word_buf, 0x40);
                    const i32 sample_index = NumSfx;
                    sfx_refcount[sample_index] = 1;
                    g_soundInfo[NumSfxInst].filename = finfo->filename;
                    g_soundInfo[NumSfxInst].index = sample_index;
                    NumSfx = sample_index + 1;
                }
            } else {
                g_soundInfo[NumSfxInst].index = -1;
                i32 i = 0;
                for (; i < NumSfx; i++) {
                    if (NuStrICmp(SfxInfo[i].filename, fpar->word_buf) == 0) {
                        sfx_refcount[i]++;
                        g_soundInfo[NumSfxInst].filename = SfxInfo[i].filename;
                        break;
                    }
                }
                if (i == NumSfx) {
                    NuStrNCpy(sfx_filename[NumSfxNames], fpar->word_buf, 0x40);
                    g_soundInfo[NumSfxInst].filename = sfx_filename[NumSfxNames++];
                }
            }
        } else if (NuStrICmp(fpar->word_buf, "pitch") == 0) {
            g_soundInfo[NumSfxInst].pitch = NuFParGetInt(fpar);
        } else if (NuStrICmp(fpar->word_buf, "pan") == 0) {
            f32 pan = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].pan = CLAMP(pan, -1.0f, 1.0f);
        } else if (NuStrICmp(fpar->word_buf, "pri") == 0) {
            i32 priority = NuFParGetInt(fpar);
            g_soundInfo[NumSfxInst].priority = CLAMP(priority, -128, 127);
        } else if (NuStrICmp(fpar->word_buf, "loop") == 0) {
            g_soundInfo[NumSfxInst].loop = 1;
        } else if (NuStrICmp(fpar->word_buf, "pitch_rnd") == 0) {
            f32 pitch_rnd = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].pitch_rnd = CLAMP(pitch_rnd, 0.0f, 1.0f);
        } else if (NuStrICmp(fpar->word_buf, "volume") == 0) {
            i32 volume = NuFParGetInt(fpar);
            g_soundInfo[NumSfxInst].volume = CLAMP(volume, 0, 0x3fff);
        } else if (NuStrICmp(fpar->word_buf, "nofade") == 0) {
            g_soundInfo[NumSfxInst].nofade = 1;
        } else if (NuStrICmp(fpar->word_buf, "volume_rnd") == 0) {
            f32 volume_rnd = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].volume_rnd = CLAMP(volume_rnd, -1.0f, 0.0f);
        } else if (NuStrICmp(fpar->word_buf, "near") == 0) {
            f32 falloff_near = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].falloff_near = CLAMP(falloff_near, 0.0f, 250.0f);
        } else if (NuStrICmp(fpar->word_buf, "far") == 0) {
            f32 falloff_far = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].falloff_far = CLAMP(falloff_far, 0.0f, 250.0f);
        } else if (NuStrICmp(fpar->word_buf, "global") == 0) {
            g_soundInfo[NumSfxInst].global = 1;
        } else if (NuStrICmp(fpar->word_buf, "rumble") == 0) {
            f32 buzz_timer = NuFParGetFloat(fpar);
            i32 rumble_strength = NuFParGetInt(fpar);
            g_soundInfo[NumSfxInst].buzz_timer = CLAMP(buzz_timer, 0.0f, 5.0f);
            g_soundInfo[NumSfxInst].rumble_strength = CLAMP(rumble_strength, 0, 0xff);
            f32 rumble_sustain = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].rumble_sustain = CLAMP(rumble_sustain, 0.0f, 5.0f);
            f32 rumble_release = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].rumble_release = CLAMP(rumble_release, 0.0f, 5.0f);
        } else if (NuStrICmp(fpar->word_buf, "fcat") == 0) {
            i32 category = NuFParGetInt(fpar);
            g_soundInfo[NumSfxInst].category = CLAMP(category, 0, 0xffff);
        }

        if (finfo != NULL) {
            finfo->field7_0x1c = g_soundInfo[NumSfxInst].field29_0x40;
        }
    }
}

static void fnAudioGroup(nufpar_s *fpar) {
    bool bVar1;
    i32 iVar2;
    i32 iVar3;

    iVar3 = -1;
    bVar1 = true;
    iVar2 = NuFParGetWord(fpar);
    do {
        if (iVar2 == 0) {
            return;
        }
        iVar2 = GetSfxId(fpar->word_buf);
        if (bVar1) {
            if (iVar2 == -1) {
                return;
            }
            // iVar3 = GroupBuffer_MakeGroup(iVar2);
        } else {
            iVar2 = GetSfxId(fpar->word_buf);
            if (iVar2 != -1) {
                // GroupBuffer_AddToGroup(iVar3, iVar2);
            }
        }
        bVar1 = false;
        iVar2 = NuFParGetWord(fpar);
    } while (true);
}

static NUFPCOMJMP audioCom[] = {
    {"Audio", fnAudioAudio},
    {"Sample", fnAudioSample},
    {"Group", fnAudioGroup},
    {NULL, NULL},
};

void LoadSfx(const char *file, variptr_u *buffer_start, variptr_u buffer_end);

#if defined(__i386__)
__attribute__((force_align_arg_pointer))
#endif
void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file) {
    // bVar15 = 0;
    // g_soundMap = (short *)((i32)buffer_start->voidptr + 3U & 0xfffffffc);
    usize allocation = ALIGN(buffer_start->addr, 4);
    g_soundMap = reinterpret_cast<u16 *>(allocation);

    // sfx_info = (nusound_filename_info_s *)(g_soundMap + 0x100);
    // SfxInfo = sfx_info;
    // buffer_start->voidptr = g_soundMap + 0x6500;
    // memset(sfx_info, 0, 0xc800);
    SfxInfo = reinterpret_cast<nusound_filename_info_s *>(allocation + 0x100 * sizeof(u16));
    buffer_start->addr = reinterpret_cast<usize>(SfxInfo + 1600);
    memset(SfxInfo, 0, 1600 * sizeof(nusound_filename_info_s));

    // sound_info = (SoundInfo *)((i32)buffer_start->voidptr + 3U & 0xfffffffc);
    // g_soundInfo = sound_info;
    // buffer_start->voidptr = sound_info + 0x640;
    // memset(sound_info, 0, 0x1a900);
    g_soundInfo = static_cast<NUSOUNDINFO *>(BUFFER_ALLOC(buffer_start, 1600 * sizeof(NUSOUNDINFO), 4));
    memset(g_soundInfo, 0, 1600 * sizeof(NUSOUNDINFO));

    //__s = (void *)((i32)buffer_start->voidptr + 3U & 0xfffffffc);
    // g_revertSoundInfo = __s;
    // buffer_start->voidptr = (void *)((i32)__s + 0x1a900);
    // memset(__s, 0, 0x1a900);
    g_revertSoundInfo = static_cast<NUSOUNDINFO *>(BUFFER_ALLOC(buffer_start, 1600 * sizeof(NUSOUNDINFO), 4));
    memset(g_revertSoundInfo, 0, 1600 * sizeof(NUSOUNDINFO));

    CRC_Init(buffer_start);

    // psVar5 = g_soundMap;
    // uVar10 = -(((u32)g_soundMap & 0xf) >> 1) & 7;
    memset(g_soundMap, -1, 0x100 * sizeof(u16));

    NumSfx = 0;
    // NumSfxInst = 0;

    for (i32 i = 0; i < SFX_MUSIC_COUNT; i++) {
        SfxInfo[i].index = i;
        SfxInfo[i].filename = g_music[i].filename;
        SfxInfo[i].index = g_music[i].index;
        SfxInfo[i].sample = g_music[i].sample;
        SfxInfo[i].field1_0x4 = g_music[i].field1_0x4;
        SfxInfo[i].field3_0xc = g_music[i].field3_0xc;
        // SfxInfo[i].field4_0x10 = g_music[i].field4_0x10;
        // SfxInfo[i].field5_0x14 = g_music[i].field5_0x14;
        // SfxInfo[i].field7_0x1c = g_music[i].field7_0x1c;

        NuStrCpy(sfx_filename[i], SfxInfo[i].filename);
        SfxInfo[i].filename = sfx_filename[i];

        NumSfx = NumSfx + 1;
        NumSfxNames = NumSfxNames + 1;

        LOG_DEBUG("SfxInfo[%d]: name=%s", i, SfxInfo[i].filename);
    }

    // puVar1 = (undefined4 *)((i32)&sfx_info->name + iVar12);
    //*puVar1 = 0;
    // puVar1[1] = 0;
    // puVar1[2] = 0xffffffff;

    NuStrCpy(cfgfile_name, file);

    LoadSfx(file, buffer_start, buffer_end);

    memset(GlobalSfxBits, 0, sizeof(GlobalSfxBits));

    for (i32 i = 0; i < NumSfxInst; i++) {
        NUSOUNDINFO *sound_info = &g_soundInfo[i];
        if (sound_info->global != 0) {
            i32 index = sound_info->index;
            GlobalSfxBits[index >> 4] |= static_cast<u16>(1 << (index & 0xf));
        }
    }

    ResetSounds();
}

void LoadSfx(const char *file, variptr_u *buffer_start, variptr_u buffer_end) {
    nufpar_s *fp = NuFParCreate(const_cast<char *>(file));
    if (fp != NULL) {
        NuFParPushCom(fp, audioCom);
        while (true) {
            if (NuFParGetLine(fp) == 0) {
                break;
            }
            NuFParGetWord(fp);
            if (*fp->word_buf != '\0') {
                NuFParInterpretWord(fp);
            }
        }
        NuFParDestroy(fp);
    }

    nusound_filename_info_s *last = &SfxInfo[NumSfx];
    last->filename = NULL;
    last->field1_0x4 = 0;
    last->index = -1;

    NuSound3SetSampleTable(SfxInfo, buffer_start, buffer_end);

    memset(SfxBits, 0, sizeof(SfxBits));

    if (NOSOUND == 0) {
        NuSound3SetRequestTable(SfxBits, 100);
    }
}

i32 GetSfxId(const char *name) {
    if (name != NULL) {
        u32 hash = CRC_ProcessStringIgnoreCase(name);
        if (g_soundMap != NULL) {
            for (i32 index = g_soundMap[hash & 0xff]; index != -1; index = g_soundInfo[index].next) {
                if (NuStrNICmp(name, g_soundInfo[index].sfx_name, 32) == 0) {
                    return index;
                }
            }
        }
    }

    return -1;
}
