#include "nu2api.saga/numusic/sfx.h"

#include "gamelib/crc/crc.h"
#include "globals.h"
#include "nu2api.saga/nufile/nufpar.h"
#include "nu2api.saga/nusound/nusound.h"

static float g_audioVersion;

static NUSOUNDINFO *g_soundInfo;
static u16 *g_soundMap;

void fnAudioAudio(nufpar_s *fpar) {
    g_audioVersion = NuFParGetFloat(fpar);
}

void fnAudioSample(nufpar_s *fpar) {
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

static char cfgfile_name[256] = "Audio/audio.cfg";

void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file) {
    // bVar15 = 0;
    // g_soundMap = (short *)((int)buffer_start->voidptr + 3U & 0xfffffffc);
    // sfx_info = (nusound_filename_info_s *)(g_soundMap + 0x100);
    // SfxInfo = sfx_info;
    // buffer_start->voidptr = g_soundMap + 0x6500;
    // memset(sfx_info, 0, 0xc800);
    // sound_info = (SoundInfo *)((int)buffer_start->voidptr + 3U & 0xfffffffc);
    // g_soundInfo = sound_info;
    // buffer_start->voidptr = sound_info + 0x640;
    // memset(sound_info, 0, 0x1a900);
    //__s = (void *)((int)buffer_start->voidptr + 3U & 0xfffffffc);
    // g_revertSoundInfo = __s;
    // buffer_start->voidptr = (void *)((int)__s + 0x1a900);
    // memset(__s, 0, 0x1a900);

    CRC_Init(buffer_start);

    // psVar5 = g_soundMap;
    // uVar10 = -(((uint)g_soundMap & 0xf) >> 1) & 7;
    // if (uVar10 == 0) {
    //     iVar12 = 0x100;
    //     uVar6 = 0;
    // } else {
    //     uVar6 = 0;
    //     do {
    //         psVar5[uVar6] = -1;
    //         uVar6 = uVar6 + 1;
    //         iVar12 = 0x100 - uVar6;
    //     } while (uVar6 < uVar10);
    // }
    // uVar8 = 0x100 - uVar10 >> 3;
    // if (uVar8 != 0) {
    //     psVar9 = psVar5 + uVar10;
    //     uVar11 = 0;
    //     do {
    //         psVar9[0] = -1;
    //         psVar9[1] = -1;
    //         psVar9[2] = -1;
    //         psVar9[3] = -1;
    //         psVar9[4] = -1;
    //         psVar9[5] = -1;
    //         psVar9[6] = -1;
    //         psVar9[7] = -1;
    //         uVar11 = uVar11 + 1;
    //         psVar9 = psVar9 + 8;
    //     } while (uVar11 < uVar8);
    //     uVar6 = uVar6 + uVar8 * 8;
    //     iVar12 = iVar12 + uVar8 * -8;
    //     if (0x100 - uVar10 == uVar8 * 8)
    //         goto LAB_00359cdd;
    // }
    // uVar10 = uVar6 + iVar12;
    // do {
    //     psVar5[uVar6] = -1;
    //     uVar6 = uVar6 + 1;
    // } while (uVar6 != uVar10);
    // LAB_00359cdd:
    // NumSfx = 0;
    // NumSfxInst = 0;
    // if (SFX_MUSIC_COUNT < 1) {
    //    iVar12 = 0;
    //    sfx_info = SfxInfo;
    //} else {
    //    sfx_info = SfxInfo;
    //    iVar13 = 0;
    //    do {
    //        pnVar4 = g_music;
    //        finfo = sfx_info + iVar13;
    //        finfo->name = g_music[iVar13].name;
    //        finfo->field1_0x4 = pnVar4[iVar13].field1_0x4;
    //        finfo->index = pnVar4[iVar13].index;
    //        finfo->field3_0xc = pnVar4[iVar13].field3_0xc;
    //        finfo->field4_0x10 = pnVar4[iVar13].field4_0x10;
    //        finfo->field5_0x14 = pnVar4[iVar13].field5_0x14;
    //        finfo->streaming_sample = pnVar4[iVar13].streaming_sample;
    //        finfo->field7_0x1c = pnVar4[iVar13].field7_0x1c;
    //        pcVar3 = finfo->name;
    //        SfxInfo[iVar13].index = iVar13;
    //        iVar12 = iVar13 + 1;
    //        NuStrCpy(sfx_filename + iVar13 * 0x40, pcVar3);
    //        sfx_info = SfxInfo;
    //        NumSfx = NumSfx + 1;
    //        NumSfxNames = NumSfxNames + 1;
    //        SfxInfo[iVar13].name = sfx_filename + iVar13 * 0x40;
    //        iVar13 = iVar12;
    //    } while (iVar12 < SFX_MUSIC_COUNT);
    //    iVar12 = iVar12 * 0x20;
    //}
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
    nusound_filename_info_s *pnVar1;
    nusound_filename_info_s *info;
    nufpar_s *fp;
    int iVar2;
    int *piVar3;
    byte bVar4;

    bVar4 = 0;
    fp = NuFParCreate(const_cast<char *>(file));
    if (fp != NULL) {
        NuFParPushCom(fp, audioCom);
        while (true) {
            iVar2 = NuFParGetLine(fp);
            if (iVar2 == 0)
                break;
            NuFParGetWord(fp);
            if (*fp->word_buf != '\0') {
                NuFParInterpretWord(fp);
            }
        }
        NuFParDestroy(fp);
    }

    // info = SfxInfo;
    // pnVar1 = SfxInfo + NumSfx;
    pnVar1->name = (char *)0x0;
    pnVar1->field1_0x4 = 0;
    pnVar1->index = -1;

    // NuSound3SetSampleTable(info, bufferStart, bufferEnd);

    // piVar3 = SfxBits;
    for (iVar2 = 0x32; iVar2 != 0; iVar2 = iVar2 + -1) {
        *piVar3 = 0;
        // piVar3 = piVar3 + (uint)bVar4 * -2 + 1;
    }

    if (NOSOUND == 0) {
        // NuSound3SetRequestTable(SfxBits, 100);
    }
}

i32 GetSfxId(const char *name) {
    if (name != NULL) {
        u32 hash = CRC_ProcessStringIgnoreCase(name);
        if (g_soundMap != NULL) {
            for (i32 index = g_soundMap[hash & 0xff]; index != -1; index = g_soundInfo[index].next) {
                if (NuStrNICmp(name, g_soundInfo[index].name, 32) == 0) {
                    return index;
                }
            }
        }
    }

    return -1;
}
