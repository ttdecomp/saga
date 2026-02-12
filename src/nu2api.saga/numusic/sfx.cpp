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
