#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/render/fx.h"
#include "legoapi/gizmos/object/lever.h"
#include "legoapi/gizmos/object/technos.h"
#include "legoapi/world/level.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/world.h"
#include "gameapi/edtools/edstubs.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nuspecial.h"

#include <string.h>
#include <stdio.h>

// LoadPerm1 is one of the few game-level entry points which wires together
// otherwise C-linkage engine subsystems.  Keep these declarations local: the
// individual subsystem TUs intentionally expose their original plain names.
extern "C" {
    void rtlInitDynamic(VARIPTR *, VARIPTR, i32);
    void DebrisSetup(VARIPTR *, VARIPTR, char *, i32, i32, i32);
    void DebrisRegisterCutoffCameraVec(void *);
    void edgraSetup(VARIPTR *, VARIPTR, i32, i32, i32);
    void InitParts(i32, VARIPTR *, VARIPTR);
    void ParticleReset(void);
    i32 NuFileExists(char *);
    i32 NuStrCpy(char *, const char *);
    i32 edppLoadPage(char *, i32, usize);
    void NuRndrShadowInit(u8 *);
    void NuTexAnimProgSysInit(void);
    void terrainpickupinit(char *, void **);

    extern i32 Grass_Available;
    extern i32 DEBPAGE_GENERAL;
    extern i32 DEBPAGE_CHARACTER;
    extern void *perm_debrissys;
}

struct AISYS_s;
void AIScriptLoadAll(char *path, VARIPTR *buf, VARIPTR *buf_end, AISYS_s *sys);
void InitTexAnimScripts(char **names);
void BackDrop_Init(char *path, VARIPTR *buf, VARIPTR *buf_end);

i32 PARTPAGE_GENERAL = -1;

// The original table at 0x006281a0.  These are the material animation
// scripts made available before the permanent things scene is loaded.
char *TexAnimList_LSW[32] = {
    (char *)"arrow",          (char *)"blink_01",      (char *)"blink_02",    (char *)"blink_03",
    (char *)"coin",           (char *)"coin_pause",    (char *)"control",     (char *)"ds_esc_intro_1",
    (char *)"ds_esc_intro_2", (char *)"ds_1",          (char *)"helpmeobi",   (char *)"lightening1",
    (char *)"plasma",         (char *)"screen",        (char *)"Dso_screen",  (char *)"anakinbows",
    (char *)"anakinkilling",  (char *)"anakinspod",    (char *)"boost1",      (char *)"boost2",
    (char *)"boost3",         (char *)"boost4",        (char *)"gasganospod", (char *)"helpobi",
    (char *)"hologram1",      (char *)"hologramnoise", (char *)"play8",       (char *)"quidie",
    (char *)"sebulbaspod",    (char *)"sidious",       (char *)"crowd",       NULL,
};

void InitStreaks(VARIPTR *, VARIPTR, char *);
void InitRopeMtl(char *, VARIPTR *, VARIPTR *);
void InitRipples(ripple_set_s **, VARIPTR *, VARIPTR *, i32);
void CreateFadeMaterials();
void CreateUsefulMaterials();
extern ripple_set_s *ripples;
extern NUMTL *ShadowMat;

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void CharConfig_ConfigureAll(i32 permanent, nufpcomjmp_s *game_keywords);
void ExtraCharacterFixUpAfterConfig();
extern i32 CHARPAK;
extern i32 apiloadcharactermodels_nopakfile;
extern "C" i32 apiloadcharactermodels_append;

extern i32 GetMenuID(void);
extern i32 InCollectList_Index(i32 id, COLLECTID *list, i32 count);
extern i32 Collection_Got(i32 id);
extern void IconScenes_Load(APICHARACTERMODELLIST_s *list, i32 permanent, VARIPTR *buf, VARIPTR *buf_end);
extern NUGSCN *IconScene_FindById(i32 character_id);
extern void Customiser_SaveModelTextureIDs(CUSTOMISER *customiser, CHARACTERMODEL_s *model);
extern CUSTOMISER *CharacterCustomiser;
extern VARIPTR characterbuffer_ptr;
extern VARIPTR characterbuffer_end;
extern i32 waiting_for_character;
extern f32 WaitingForCharacterTime;

extern "C" {
    i32 hub_character_ready = -1;
}

APICHARACTERMODELLIST_s PermModelList[] = {{-1, 0}};

enum LegoObjectId : i16 {
    LEGO_OBJECT_FLOOR_TARGET = 0x55,
};

void LSW_SetIndy(i32) {
    LedgeTerrain_On = 0;
    Grapples_Available = 0;
    SuperCarry_Bash = 0;
    SuperCarry_Jump = 0;
    PUNCHGAP = 0.3f;
    PUNCHCHARGAP = 0.3f;
    HINTS_ON = 1;

    TechnoSys.interaction_time = 0.2f;
    TechnoSys.idle_offset = v000;
    TechnoSys.active_offset = v000;
    TechnoSys.active_effect_id = 0x2c;
    TechnoSys.success_effect_id = -1;
    TechnoSys.failure_effect_id = -1;
    TechnoSys.complete_offset = v000;
    TechnoSys.floor_target_object_id = LEGO_OBJECT_FLOOR_TARGET;
    TechnoSys.activation_effect_id = 0x2d;
    TechnoSys.completion_effect_id = 0x2e;

    LeverSys.floor_target_object_id = LEGO_OBJECT_FLOOR_TARGET;
    GameAudio_SetActionMusicTimes(1.0f, 6.0f);
}

void HairMovement(GameObject_s *) {
}

void HeadMovement(GameObject_s *) {
}

void TakeOverYoda(GameObject_s *, GameObject_s *, i32, i32) {
}

void fullcodename(i32) {
}

void CharScene_Draw(WORLDINFO_s *, i32, numtx_s *, numtx_s *) {
}

void CharScenes_Init(variptr_u *buf, variptr_u *) {
    buf->addr = ALIGN(buf->addr, 4);
    CharScene_Area = reinterpret_cast<CHARSCENE_s *>(buf->void_ptr);
    buf->addr += static_cast<usize>(CHARCOUNT) * sizeof(*CharScene_Area);
    memset(CharScene_Area, 0, static_cast<usize>(CHARCOUNT) * sizeof(*CharScene_Area));
}

extern i16 tUNKNOWN;
void Move_CHARACTER(GameObject_s *);
void Animate_CHARACTER(GameObject_s *);

void FixUpCharacters(CHARFIXUP *fixup) {
    for (i32 i = 0; i < CHARCOUNT; ++i) {
        GCDataList[i] = GCDATA_DEFAULT;

        CHARACTERDATA &character = CDataList[i];
        if (character.field0_0x0 == -1) {
            character.field0_0x0 = tUNKNOWN;
        }
        if (character.move_fn == NULL) {
            character.move_fn = Move_CHARACTER;
        }
        if (character.animate_fn == NULL) {
            character.animate_fn = Animate_CHARACTER;
        }
    }

    if (fixup != NULL) {
        while (fixup->name != NULL) {
            if (fixup->id != NULL) {
                *fixup->id = (i16)CharIDFromName(fixup->name);
            }
            ++fixup;
        }
    }
}

void PostAnimate_FETT(GameObject_s *) {
}

void ActivateCharacter(char *, nuvec_s *, i32) {
}

void FinishWeirdoNames(i32) {
}

void NewPlayerCharacter(GameObject_s *, i32, i32, i32) {
}

void ResetCharacterIdle(GameObject_s *, i32, i32) {
}

void UpdateCharacterIDs() {
}

void CharScenes_AreaDump() {
    if (CharScene_Area == NULL) {
        return;
    }
    for (i32 i = 0; i < CHARCOUNT; ++i) {
        CHARSCENE_s &entry = CharScene_Area[i];
        if (entry.scene != NULL) {
            NuGScnRemove(entry.scene);
        }
        entry.scene = NULL;
    }
}

void CharScenes_AreaLoad(APICHARACTERMODELLIST_s *list, variptr_u *buf, variptr_u buf_end) {
    if (CharScene_Area == NULL || apicharsys->loaded_model_count == 0) {
        return;
    }

    for (i32 i = 0; i < apicharsys->loaded_model_count; ++i) {
        const i32 model_id = apicharsys->models[i].model_id;
        CHARSCENE_s &entry = CharScene_Area[model_id];
        if (entry.scene != NULL || (CDataList[model_id].flags & 1) == 0) {
            continue;
        }

        i32 list_index;
        if (InModelList(list, model_id, &list_index) == 0 || list[list_index].count == 0) {
            continue;
        }

        char path[128];
        sprintf(path, "chars\\%s\\%s.gsc", CDataList[model_id].dir, CDataList[model_id].file);
        entry.scene = NuGScnRead(buf, buf_end, path);
        if (entry.scene != NULL) {
            NuSpecialFind(entry.scene, &entry.special_scene, CDataList[model_id].file, 1);
        }
    }
}

void DeactivateCharacter(char *) {
}

void LoadSingleCharacter(bgprocinfo_s *) {
    APICHARACTERMODELLIST_s list[2] = {
        {static_cast<i16>(waiting_for_character), 1},
        {-1, 0},
    };

    apiloadcharactermodels_append = 1;
    apiloadcharactermodels_nopakfile = CHARPAK == 0;
    APILoadCharacterModels(list, 0, &characterbuffer_ptr, characterbuffer_end, 1);
    IconScenes_Load(list, 0, &characterbuffer_ptr, &characterbuffer_end);

    NUGSCN *icon_scene = IconScene_FindById(list[0].model_id);
    if (icon_scene != NULL && LEVELOBJECTCOUNT > 0) {
        for (i32 i = 0; i < LEVELOBJECTCOUNT; ++i) {
            if (ObjTab[i].kind != 3) {
                continue;
            }

            LEVEL_OBJECT_RUNTIME &object = WORLD->lev_objs[i];
            if (object.active != 0) {
                continue;
            }

            if (NuSpecialFind(icon_scene, &object.special, ObjTab[i].name, 1) != 0) {
                object.active = 1;
            }
        }
    }

    Customiser_SaveModelTextureIDs(CharacterCustomiser, APICharacterLoaded(list[0].model_id));
    hub_character_ready = waiting_for_character;
    waiting_for_character = -1;
    g_loadingCharacterInHub = 0;
}

void UpdateCharacterIdle(GameObject_s *) {
}

void UpdateCharacterLoad() {
    const i32 menu_id = GetMenuID();
    if (menu_id == 13 || (menu_id >= 15 && menu_id <= 19)) {
        return;
    }

    if (LOADEROFF != 0 || BGLOAD == 0 || waiting_for_character != -1 || hub_character_ready != -1 ||
        bgGetProcActive() != NULL || global_dlist_manager.ndisplay_lists > 0xfd) {
        return;
    }

    const i32 required_buffer =
        (static_cast<i32>((static_cast<f32>(CHARACTERBUFFERSIZE) / 7077888.0f) * 1048576.0f) + 0x3ff) & ~0x3ff;
    if (required_buffer > static_cast<i32>(characterbuffer_end.addr - characterbuffer_ptr.addr)) {
        return;
    }

    i32 candidates[0x154] __attribute__((aligned(16)));
    i32 candidate_count = 0;
    i32 candidate = 0;
    i16 *fixed_candidate = NULL;
    i32 fixed_value = -1;

    if (id_BARMAN != -1 && APICharacterLoaded(id_BARMAN) == NULL) {
        fixed_candidate = &id_BARMAN;
        goto queue_fixed_character;
    }
    if (id_CANTINABAND != -1 && APICharacterLoaded(id_CANTINABAND) == NULL) {
        fixed_candidate = &id_CANTINABAND;
        goto queue_fixed_character;
    }
    if (id_WEIRDO1 != -1 && APICharacterLoaded(id_WEIRDO1) == NULL) {
        fixed_candidate = &id_WEIRDO1;
        goto queue_fixed_character;
    }
    if (id_WEIRDO2 != -1 && APICharacterLoaded(id_WEIRDO2) == NULL) {
        fixed_candidate = &id_WEIRDO2;
        goto queue_fixed_character;
    }
    if (id_JABBA != -1 && APICharacterLoaded(id_JABBA) == NULL) {
        fixed_candidate = &id_JABBA;
        goto queue_fixed_character;
    }
    if (id_MOSEISLEYCITIZEN != -1 && APICharacterLoaded(id_MOSEISLEYCITIZEN) == NULL) {
        fixed_candidate = &id_MOSEISLEYCITIZEN;
        goto queue_fixed_character;
    }
    if (id_CANTINAALIEN != -1 && APICharacterLoaded(id_CANTINAALIEN) == NULL) {
        fixed_candidate = &id_CANTINAALIEN;
        goto queue_fixed_character;
    }
    if (id_WOMPRAT != -1 && APICharacterLoaded(id_WOMPRAT) == NULL) {
        fixed_candidate = &id_WOMPRAT;
        goto queue_fixed_character;
    }

    for (i32 pack = 0; pack < 11; ++pack) {
        if (g_lowEndLevelBehaviour != 0 && Hub_LowEnd_IconsInsteadOfModels != 0) {
            continue;
        }
        if (Store_IsPackUnlocked(pack) != 0 || StorePack[pack].id == NULL) {
            continue;
        }

        const i32 id = *StorePack[pack].id;
        if (id != -1 && APICharacterLoaded(id) == NULL) {
            fixed_value = id;
            goto queue_fixed_value;
        }
    }

    for (i16 id = 0; id < CHARCOUNT; ++id) {
        if (id == id_DROIDEKA || (CDataList[id].model_flags & 0x04002000) != 0 || APICharacterLoaded(id) != NULL) {
            continue;
        }
        if (static_cast<i32>(CDataList[id].field5_0x14) + apicharsys->loaded_animation_count >
            apicharsys->animation_capacity) {
            continue;
        }
        if (InCollectList_Index(id, NULL, 0) == -1 || Collection_Got(id) == 0) {
            continue;
        }
        candidates[candidate_count++] = id;
    }

    if (candidate_count == 0) {
        return;
    }

    if (candidate_count != 1) {
        candidate = qrand() / ((0xffff / candidate_count) + 1);
    }
    goto queue_character;

queue_fixed_character:
    candidates[0] = *fixed_candidate;
    candidate = 0;
    goto queue_character;

queue_fixed_value:
    candidates[0] = fixed_value;
    candidate = 0;
queue_character:
    waiting_for_character = candidates[candidate];
    WaitingForCharacterTime = 0.0f;
    bgPostRequest(LoadSingleCharacter, NULL, NULL, 0);
}

void CharScenes_LevelDump(WORLDINFO_s *) {
}

void CollectAllCharacters(i32) {
}

extern VARIPTR characterbuffer_base;
extern i32 CHARACTERBUFFERSIZE;
extern i32 Area;
extern i32 last_area;

void ResetCharacterBuffer(i32 force_reset) {
    if (force_reset == 0 && Area != -1 && Area == last_area) {
        return;
    }

    characterbuffer_ptr = characterbuffer_base;
    memset(characterbuffer_base.void_ptr, 0, CHARACTERBUFFERSIZE);
    apicharsys->loaded_model_count = apicharsys->permanent_model_count;
    apicharsys->animation_load_attempts = apicharsys->area_animation_count;
    apicharsys->loaded_animation_count = apicharsys->area_animation_count;
}

void CollectCharcters_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void CollectCharcters_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void E1CharacterBonus_Init(WORLDINFO_s *) {
}

void LocalGetRandomLocator(AILOCATOR_s **, i32, float, nuvec_s *, float, i32, float, float) {
}

void PostAnimate_ASTROMECH(GameObject_s *) {
}

void CharScene_FindHSpecial(WORLDINFO_s *, i32) {
}

void LocalGetNearestLocator(AILOCATOR_s **, i32, float, nuvec_s *, float, i32, float, float) {
}

void newCharactersCollected(STATUSPACKET_s *) {
}

void CollectCharcters_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void RegisterGizmoTypes_Indy(variptr_u *, variptr_u *) {
}

void Area_CharIDInCurrentList(i32) {
}

void SetProtocolDroidFallAnim(GameObject_s *) {
}

void CollectCharactersOff_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void CollectCharactersOff_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void ScaleGameObject(GameObject_s *obj);

void SetGameObjectCharacterData(GameObject_s *obj) {
    CHARACTERDATA *data = obj->apiobj.character_data;
    obj->apiobj.field_0xa8 = data->field17_0x3c;
    obj->field_0x1008 = data->field14_0x30;
    obj->field_0xffc = data->field15_0x34;
    obj->field_0x1000 = data->field16_0x38;
    ScaleGameObject(obj);
}

void CollectCharactersOff_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void TakeOverYodaSeekDistanceHack(GameObject_s *, GameObject_s *, nuvec_s *) {
}

void SetProtocolDroidInterfaceAction(GameObject_s *) {
}

void SetProtocolDroidDeactivatedAction(GameObject_s *) {
}

void LoadPerm1() {
    char buf[0x100];

    rtlInitDynamic(&permbuffer_ptr, superbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    DebrisSetup(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\particle", 0x100, 0x200, 0x200);
    DebrisRegisterCutoffCameraVec(reinterpret_cast<NUVEC *>(&global_camera.mtx.m30));
    if (Grass_Available != 0) {
        edgraSetup(&permbuffer_ptr, permbuffer_end, 0x200, 0x20, 0x40);
    }
    InitParts(0x40, &permbuffer_ptr, permbuffer_end);
    ParticleReset();

    NuStrCpy(buf, (char *)"stuff\\general.ptl");
    if (NuFileExists(buf) != 0) {
        DEBPAGE_GENERAL = edppLoadPage(buf, 0, 0);
    }
    NuStrCpy(buf, (char *)"stuff\\char.ptl");
    if (NuFileExists(buf) != 0) {
        DEBPAGE_CHARACTER = edppLoadPage(buf, 5, 0);
    }
    perm_debrissys = InitGameDebris(&permbuffer_ptr, permbuffer_end, 0x190, 0x93, debris_name, 0);

    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitStreaks(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\streak.pnt");
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitRopeMtl((char *)"rope", &permbuffer_ptr, &permbuffer_end);
    InitRipples(&ripples, &permbuffer_ptr, &permbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xff) & ~0xffu;

    ShadowMat = NuMtlCreate3D(1);
    ShadowMat->diffuse_color.r = 1.0f;
    ShadowMat->diffuse_color.g = 1.0f;
    ShadowMat->diffuse_color.b = 1.0f;
    ShadowMat->sort_pri = 0xff;
    ShadowMat->opacity = 0.999f;
    u8 *attributes = reinterpret_cast<u8 *>(&ShadowMat->attribs);
    attributes[1] = (attributes[1] & 0x0f) | 0x60;
    attributes[0] = (attributes[0] & 0xf0) | 1;
    attributes[2] = (attributes[2] & 0x8c) | 0x12;
    ShadowMat->tex_id = static_cast<i16>(NuTexRead((char *)"stuff\\gradient", &permbuffer_ptr, &permbuffer_end));
    NuMtlUpdate(ShadowMat);

    u8 shadow_random[0x800];
    for (i32 i = 0; i < 0x800; ++i) {
        shadow_random[i] = static_cast<u8>(qrand() >> 8);
    }
    NuRndrShadowInit(shadow_random);
    CreateFadeMaterials();
    CreateUsefulMaterials();

    edgraClumpsReset();
    edanimParamReset();
    NuTexAnimProgSysInit();
    InitTexAnimScripts(TexAnimList_LSW);

    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    things_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\things.gsc");
    NUGSCN *terrain_scene = NULL;
    if (things_scene != NULL) {
        edbitsRegisterThingsScene(things_scene);
        terrain_scene = things_scene;
        if (things_scene->display_list != NULL) {
            things_scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;
        }
    }

    things_scene_terrain = TerrainInitEx(-1, &permbuffer_ptr, permbuffer_end.void_ptr, 0, (char *)"stuff\\things",
                                         terrain_scene, 0, 0x14, 0x14, 0x14);
    terrainpickupinit((char *)"stuff\\things", &things_scene_terrain);
    BackDrop_Init((char *)"stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);

    NuMtlSetCurrentRenderPlane(0xf);
    edpartSetParticlePage(DEBPAGE_GENERAL);
    NuStrCpy(buf, (char *)"stuff\\general.par");
    if (NuFileExists(buf) != 0) {
        PARTPAGE_GENERAL = edpartLoadPage(buf, 0, things_scene);
    }
    NuMtlSetCurrentRenderPlane(0);
    AIScriptLoadAll((char *)"scripts", &permbuffer_ptr, &permbuffer_end, NULL);
}

void LoadPerm2() {
    CharConfig_ConfigureAll(1, NULL);
    ExtraCharacterFixUpAfterConfig();
    apiloadcharactermodels_nopakfile = CHARPAK == 0;
    APILoadCharacterModels(PermModelList, 1, &permbuffer_ptr, permbuffer_end, 1);
    Areas_ConfigureResidents(&permbuffer_ptr, &permbuffer_end);
}

void MapToGrid(nuvec_s *, nuvec_s *, i32 *, i32 *, nuvec_s *, nutexmanager_s *) {
}
