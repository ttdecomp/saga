#include "decomp.h"
#include "legoapi/world_shared.h"

#include <stdio.h>
#include <string.h>

#include "legoapi/character.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
struct CUTINFO;
struct CUTSCENEPLAYERCLIP;
struct NUGCUTSCENE_s;
struct instNUGCUTCHAR_s;
struct NUGCUTCHAR_s;
struct NUGCUTRIGID_s;
struct instNUGCUTRIGID_s;

struct CUTINFO_LOAD {
    void *cutscene;
    void *instance;
    char data[0x13c];
};

struct CUTSYS_LOAD {
    void *entries;
    i32 count;
    void *end;
};

i32 CUTCOUNT = 0;
CUTINFO_LOAD *CutList = NULL;
i32 ACTIVECUTCOUNT = 0;
i32 CS_area = 0;
CUTSYS_LOAD *CS_cutsys = NULL;
WORLDINFO *CS_worldinfo = NULL;
f32 CutSceneScale = 1.0f;
i32 CUTCAM = 0;
i32 CharScene_Area = 0;

__attribute__((noinline)) static void CutScene_Configure_Load(CUTINFO_LOAD *cut, char *name, VARIPTR *buf,
                                                              VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    memset(cut, 0, sizeof(*cut));
    NuStrCpy((char *)cut->data, name);
}

void *CutScenes_Load(char *config, NUGSCN *gscn1, NUGSCN *gscn2, i32 param1, VARIPTR *buf, VARIPTR *buf_end, i32 param2,
                     i32 param3, WORLDINFO *world) {
    NUFPAR *fp;
    CUTSYS_LOAD *sys;
    void *initial;
    CUTINFO_LOAD *cut;
    char name[128];
    char path[128];
    char full_path[128];
    CUTINFO_LOAD *entries[32];

    if (CutList != NULL && CUTCOUNT > 0) {
        sys = (CUTSYS_LOAD *)ALIGN(buf->addr, 4);
        sys->entries = sys + 1;
        sys->count = CUTCOUNT;
        CS_area = param2;
        CS_worldinfo = world;
        CS_cutsys = sys;
        buf->void_ptr = (char *)sys->entries + CUTCOUNT * 0x144;
        return sys;
    }
    if (config == NULL) {
        return NULL;
    }

    fp = NuFParCreateMem((char *)"cutscenes", config, 0xffff);
    if (fp == NULL) {
        return NULL;
    }

    initial = buf->void_ptr;
    sys = (CUTSYS_LOAD *)ALIGN((usize)initial, 4);
    sys->entries = sys + 1;
    sys->count = 0;
    sys->end = (char *)sys + 0xc;
    CS_area = param2;
    CS_worldinfo = world;
    CS_cutsys = sys;
    buf->void_ptr = (char *)sys->entries + ((CHARCOUNT + 0x1f) >> 5) * 4;

    while (NuFParGetLine(fp) != 0) {
        if (NuFParGetWord(fp) == 0 || NuStrICmp(fp->word_buf, "cutscene") != 0 || sys->count > 0x1f ||
            NuFParGetWord(fp) == 0) {
            continue;
        }

        cut = (CUTINFO_LOAD *)buf->void_ptr;
        entries[sys->count] = cut;
        buf->void_ptr = (char *)cut + 0x144;
        NuStrCpy(name, fp->word_buf);
        for (char *lower = name; *lower != '\0'; ++lower) {
            *lower = (char)NuToLower((u8)*lower);
        }
        i32 len = NuStrLen(name);
        while (len > 0 && name[len - 1] != '.') {
            --len;
        }
        if (len > 0) {
            name[len - 1] = '\0';
        }

        if (name[0] == 'c' && name[1] == 'u' && name[2] == 't' && name[3] == '\\') {
            NuStrCpy(path, name);
        } else {
            NuStrCpy(path, "cut\\");
            NuStrCat(path, name);
            NuStrCat(path, ".cut");
        }
        NuStrCpy(full_path, "cutscenes");
        NuStrCat(full_path, "\\");
        NuStrCat(full_path, path);
        CutScene_Configure_Load(cut, full_path, buf, buf_end);

        if ((cut->data[0x51] & 8) == 0 && !InStory()) {
            continue;
        }
        buf->void_ptr = (char *)ALIGN(buf->addr, 0x40);
        NuStrCpy(full_path, "cutscenes");
        NuStrCat(full_path, "\\");
        NuStrCat(full_path, path);
        cut->cutscene = NuGCutSceneLoad(full_path, gscn1, gscn2, 0);
        if (cut->cutscene == NULL) {
            continue;
        }
        NuGCutSceneFixUp(cut->cutscene, name, 0, buf_end);
        NuGCutSceneFixUpExtra(cut->cutscene, param1);
        cut->instance = instNuGCutSceneCreate(cut->cutscene, 0, 0, name);
        if (cut->instance != NULL) {
            *(f32 *)((char *)cut->instance + 0x98) = *(f32 *)((char *)cut + 0x5c) * CutSceneScale;
            buf->void_ptr = (char *)ALIGN(buf->addr, 0x10);
        }
        sys->count++;
    }
    NuFParDestroy(fp);
    if (sys->count == 0) {
        buf->void_ptr = initial;
        return NULL;
    }
    buf->void_ptr = initial;
    memmove(initial, entries, sys->count * sizeof(void *));
    buf->void_ptr = (void *)ALIGN((usize)initial + sys->count * 4, 4);
    sys->entries = initial;
    return sys;
}

void CharScenes_LevelLoad(WORLDINFO *world) {
    if (CHARCOUNT <= 0) {
        return;
    }

    for (i32 i = 0; i < CHARCOUNT; i++) {
        void **entry = (void **)(*(i32 *)&world->minikit.field_0x18 + i * 0x10);
        *entry = NULL;

        // Check if we should load this character scene
        if ((CharScene_Area == 0 || *(i32 *)(CharScene_Area + i * 0x10) == 0) && (CDataList[i].flags & 1) != 0 &&
            world->cutscene_sys != NULL) {
            // Check if this character is in a cutscene
            u32 *cutscene_flags = *(u32 **)((char *)world->cutscene_sys + 8);
            u32 flag = (cutscene_flags[i >> 5] >> (i & 0x1f)) & 1;
            if (flag != 0) {
                // Load the character scene
                char path[136];
                VARIPTR buf_end = world->unknown_0108;
                sprintf(path, "chars\\%s\\%s.gsc", CDataList[i].dir, CDataList[i].file);
                NUGSCN *scene = (NUGSCN *)NuGScnRead(&world->giz_buffer, buf_end, path);
                *entry = scene;
                if (scene != NULL) {
                    NuSpecialFind(scene, (void **)(entry + 1), CDataList[i].file);
                }
            }
        }
    }
}

// --- Extern "C" block: functions with confirmed C linkage in original libTTapp.so ---
extern "C" {

    void *NuGCutSceneLoad(char *name, NUGSCN *gscn1, NUGSCN *gscn2, i32 flags) {
        (void)name;
        (void)gscn1;
        (void)gscn2;
        (void)flags;
        return NULL;
    }
    void NuGCutSceneFixUp(void *cutscene, char *name, i32 flags, VARIPTR *end) {
        (void)cutscene;
        (void)name;
        (void)flags;
        (void)end;
    }
    void NuGCutSceneFixUpExtra(void *cutscene, i32 area) {
        (void)cutscene;
        (void)area;
    }
    void *instNuGCutSceneCreate(void *cutscene, i32 flags, i32 param, char *name) {
        (void)cutscene;
        (void)flags;
        (void)param;
        (void)name;
        return NULL;
    }
} // extern "C"

struct instNUGCUTSCENE_s;
struct NUGCUTLOCATORSYS_s;
struct instNUGCUTLOCATOR_s;
struct NUGCUTLOCATOR_s;
struct numtx_s;
static __used__ void LocatorFunction_Blaster(instNUGCUTSCENE_s *, NUGCUTLOCATORSYS_s *,
                                                          instNUGCUTLOCATOR_s *, NUGCUTLOCATOR_s *,
                                                          float, numtx_s *, int) {
}
static __used__ void instNuGCutRigidSysEnd(instNUGCUTSCENE_s *, float) {
}

static __used__ void instNuGCutSceneUpdate(instNUGCUTSCENE_s *, int, int, float) {
}

static __used__ void instNuGCutCamSysUpdate(instNUGCUTSCENE_s *, float) {
}

static __used__ void instNuGCutSceneClipTest(instNUGCUTSCENE_s *) {
}

static __used__ void instNuGCutRigidSysRender(instNUGCUTSCENE_s *, float, int) {
}

static __used__ void instNuGCutRigidSysUpdate(instNUGCUTSCENE_s *, float, int) {
}

static __used__ void instNuGCutTriggerSysUpdate(instNUGCUTSCENE_s *, float) {
}

static __used__ void CutScene_Configure(CUTINFO*, char*, variptr_u*, variptr_u*) {}

static __used__ void bgAckStreamCutScene(bgprocinfo_s*) {}

static __used__ void bgLoadStreamCutScene(bgprocinfo_s*) {}

static __used__ unsigned int CutScenePlayer_Accept(CUTSCENEPLAYERCLIP*) { return {}; }

static __used__ void CutScene_DrawCharacter(instNUGCUTSCENE_s*, NUGCUTSCENE_s*, instNUGCUTCHAR_s*, NUGCUTCHAR_s*, float, int) {}

static __used__ void CutScene_EvalCharacter(instNUGCUTSCENE_s*, NUGCUTSCENE_s*, instNUGCUTCHAR_s*, NUGCUTCHAR_s*, float) {}

static __used__ unsigned int CutScene_FindCharacters(NUGCUTSCENE_s*) { return {}; }

static __used__ void CutScene_ResetCharacters(instNUGCUTSCENE_s*) {}

static __used__ void CutScene_RigidPostRender(NUGCUTRIGID_s*, instNUGCUTRIGID_s*, numtx_s*) {}

static __used__ void * CutScene_CreateCharacterInstance(NUGCUTCHAR_s*, instNUGCUTCHAR_s*, variptr_u*) { return {}; }

static __used__ void CutScene_OverrideConfigFileName_LSW(char*, int, int) {}

static __used__ void copyAnims(NUGCUTSCENE_s*, NUGCUTSCENE_s*) {}

