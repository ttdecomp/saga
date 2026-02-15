#include "nu2api.saga/numusic/sfx.h"

#include "gamelib/crc/crc.h"
#include "globals.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"
#include "nu2api.saga/nusound/nusound.h"

#include <cstring>

static float g_audioVersion;

NUSOUNDINFO *g_soundInfo;
NUSOUNDINFO *g_revertSoundInfo;

static u16 *g_soundMap;
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
    info->priority = 0;
    info->volume = 0x3fff;
    info->pan = 0.0;
    info->group = -1;
    info->pitch_rnd = 0.0;
    info->rumble_strength = 0;
    info->sfx_name = sfx_name[NumSfxInst];
    info->category = 0;
    info->field29_0x40 = 0;
    info->volume_rnd = 0.0;

    // info->field_0x6 = info->field_0x6 & 0x88;
    info->dirty = 1;
    info->disabled = 1;

    info->falloff_near = 0.0;
    info->falloff_far = 0.0;
    info->buzz_timer = 0.0;
    info->rumble_sustain = 0.0;
    info->rumble_release = 0.0;

    memcpy(&g_revertSoundInfo[index], info, sizeof(NUSOUNDINFO));
}

void fnAudioAudio(nufpar_s *fpar) {
    g_audioVersion = NuFParGetFloat(fpar);
}

void fnAudioSample(nufpar_s *fpar) {
    InitSoundInfo(NumSfxInst);

    NUSOUND_FILENAME_INFO *finfo = NULL;
    NUSOUNDINFO *sinfo = NULL;

    i32 i = 0;
    while (true) {
        if (NuFParGetWord(fpar) == 0) {
            sinfo = &g_soundInfo[NumSfxInst];
            // *(byte *)((int)&sinfo->flags + 2) = *(byte *)((int)&sinfo->flags + 2) & 0xef | 0x20;
            sinfo->dirty = 0;

            LOG_DEBUG("fpar->line_buf=%s", fpar->line_buf);
            LOG_DEBUG("Loaded sound %d: name=%s, filename=%s, pitch=%d, volume=%hd, pri=%d, loop=%d, global=%d, "
                      "pitch_rnd=%.2f, volume_rnd=%.2f",
                      NumSfxInst, sinfo->sfx_name, sinfo->filename, sinfo->pitch, sinfo->volume, sinfo->priority,
                      sinfo->loop, sinfo->global, sinfo->pitch_rnd, sinfo->volume_rnd);

            memmove(&g_revertSoundInfo[NumSfxInst], sinfo, 0x44);
            NumSfxInst++;
            return;
        }

        if (NuStrICmp(fpar->word_buf, "name") == 0) {
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

            if (finfo == NULL) {
                continue;
            }
            sinfo = &g_soundInfo[NumSfxInst];

        } else if (NuStrICmp(fpar->word_buf, "fname") == 0) {
            NuFParGetWord(fpar);

            i32 i = 0;
            for (; i < NumSfx; i++) {
                sinfo = g_soundInfo;
                if (NuStrICmp(SfxInfo[i].filename, fpar->word_buf) == 0) {
                    sfx_refcount[i] = sfx_refcount[i] + 1;
                    sinfo[NumSfxInst].filename = SfxInfo[i].filename;
                    break;
                }
            }
            if (i == NumSfx) {
                NuStrNCpy(sfx_filename[NumSfxNames++], fpar->word_buf, 0x40);
                g_soundInfo[NumSfxInst].filename = sfx_filename[NumSfxNames - 1];
            }

            if (finfo == NULL) {
                continue;
            }

            sinfo = &g_soundInfo[NumSfxInst];
        } else if (NuStrICmp(fpar->word_buf, "pitch") == 0) {
            g_soundInfo[NumSfxInst].pitch = NuFParGetInt(fpar);
        } else if (NuStrICmp(fpar->word_buf, "global") == 0) {
            g_soundInfo[NumSfxInst].global = 1;
        } else if (NuStrICmp(fpar->word_buf, "pitch_rnd") == 0) {
            f32 pitch_rnd = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].pitch_rnd = CLAMP(pitch_rnd, -1.0f, 1.0f);
        } else if (NuStrICmp(fpar->word_buf, "volume") == 0) {
            i32 volume = NuFParGetInt(fpar);
            g_soundInfo[NumSfxInst].volume = CLAMP(volume, -0x3fff, 0x3fff);
        } else if (NuStrICmp(fpar->word_buf, "pri") == 0) {
            i32 priority = NuFParGetInt(fpar);
            g_soundInfo[NumSfxInst].priority = CLAMP(priority, -128, 127);
        } else if (NuStrICmp(fpar->word_buf, "volume_rnd") == 0) {
            f32 volume_rnd = NuFParGetFloat(fpar);
            g_soundInfo[NumSfxInst].volume_rnd = CLAMP(volume_rnd, -1.0f, 1.0f);
        } else if (NuStrICmp(fpar->word_buf, "loop") == 0) {
            g_soundInfo[NumSfxInst].loop = 1;
        } else {
            LOG_WARN("Unknown AudioSample command '%s'", fpar->word_buf);
        }
    }
}

void fnAudioGroup(nufpar_s *fpar) {
    bool bVar1;
    int iVar2;
    int iVar3;

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

void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file) {
    // bVar15 = 0;
    // g_soundMap = (short *)((int)buffer_start->voidptr + 3U & 0xfffffffc);
    g_soundMap = BUFFER_ALLOC_ARRAY(buffer_start, 0x100, u16);

    // sfx_info = (nusound_filename_info_s *)(g_soundMap + 0x100);
    // SfxInfo = sfx_info;
    // buffer_start->voidptr = g_soundMap + 0x6500;
    // memset(sfx_info, 0, 0xc800);
    SfxInfo = BUFFER_ALLOC_ARRAY(buffer_start, 1600, nusound_filename_info_s);

    // sound_info = (SoundInfo *)((int)buffer_start->voidptr + 3U & 0xfffffffc);
    // g_soundInfo = sound_info;
    // buffer_start->voidptr = sound_info + 0x640;
    // memset(sound_info, 0, 0x1a900);
    g_soundInfo = BUFFER_ALLOC_ARRAY(buffer_start, 1600, NUSOUNDINFO);

    //__s = (void *)((int)buffer_start->voidptr + 3U & 0xfffffffc);
    // g_revertSoundInfo = __s;
    // buffer_start->voidptr = (void *)((int)__s + 0x1a900);
    // memset(__s, 0, 0x1a900);
    g_revertSoundInfo = BUFFER_ALLOC_ARRAY(buffer_start, 1600, NUSOUNDINFO);

    CRC_Init(buffer_start);

    // psVar5 = g_soundMap;
    // uVar10 = -(((uint)g_soundMap & 0xf) >> 1) & 7;
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

    // puVar1 = (undefined4 *)((int)&sfx_info->name + iVar12);
    //*puVar1 = 0;
    // puVar1[1] = 0;
    // puVar1[2] = 0xffffffff;

    NuStrCpy(cfgfile_name, file);

    LoadSfx(file, buffer_start, buffer_end);

    // piVar14 = GlobalSfxBits;
    // for (iVar12 = 0x32; iVar12 != 0; iVar12 = iVar12 + -1) {
    //     *piVar14 = 0;
    //     piVar14 = piVar14 + (uint)bVar15 * -2 + 1;
    // }

    // if (0 < NumSfxInst) {
    //     end = g_soundInfo + NumSfxInst;
    //     sound_info = g_soundInfo;
    //     do {
    //         if ((sound_info->flags & 2) != 0) {
    //             sVar7 = (short)(*(short *)&sound_info->field_0x4 * 2) >> 1;
    //             local_14 = (byte)sVar7 & 0xf;
    //             puVar2 = (ushort *)((int)GlobalSfxBits + ((int)sVar7 >> 4) * 2);
    //             *puVar2 = *puVar2 | (ushort)(1 << local_14);
    //         }
    //         sound_info = sound_info + 1;
    //     } while (sound_info != end);
    // }

    // ResetSounds();
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

    // piVar3 = SfxBits;
    // for (iVar2 = 0x32; iVar2 != 0; iVar2 = iVar2 + -1) {
    // *piVar3 = 0;
    // piVar3 = piVar3 + (uint)bVar4 * -2 + 1;
    // }

    if (NOSOUND == 0) {
        // NuSound3SetRequestTable(SfxBits, 100);
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
