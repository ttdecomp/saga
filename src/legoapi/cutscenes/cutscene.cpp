#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nugcutscene.h"
#include "legoapi/world/world_shared.h"

#include <stdio.h>
#include <string.h>

#include "legoapi/characters/core/character.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
struct CUTSCENEPLAYERCLIP;
struct instNUGCUTCHAR_s;
struct NUGCUTCHAR_s;
struct NUGCUTRIGID_s;
struct instNUGCUTRIGID_s;

i32 CUTCOUNT = 0;
CUTINFO *CutList = NULL;
i32 ACTIVECUTCOUNT = 0;
i32 CS_area = 0;
CUTSYS *CS_cutsys = NULL;
WORLDINFO *CS_worldinfo = NULL;
f32 CutSceneScale = 1.0f;
i32 CUTCAM = 0;
CHARSCENE_s *CharScene_Area = NULL;

__attribute__((noinline)) static void CutScene_Configure_Load(CUTINFO *cut, char *name, VARIPTR *buf,
                                                              VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    memset(cut, 0, sizeof(*cut));
    cut->flags = 3;
    cut->frames_per_second = 30.0f;

    NUFPAR *fp = NuFParCreate(name);
    if (fp == NULL) {
        return;
    }
    while (NuFParGetLine(fp) != 0) {
        if (NuFParGetWord(fp) == 0) {
            continue;
        }
        if (NuStrICmp(fp->word_buf, "level_intro") == 0) {
            cut->flags |= 0x1000;
        } else if (NuStrICmp(fp->word_buf, "in_game") == 0) {
            cut->flags = (cut->flags & ~3U) | 0x800;
        } else if (NuStrICmp(fp->word_buf, "looping") == 0) {
            cut->flags |= 0x200;
        } else if (NuStrICmp(fp->word_buf, "draw_world") == 0 && NuFParGetWord(fp) != 0 &&
                   NuStrICmp(fp->word_buf, "off") == 0) {
            cut->flags &= ~2U;
        }
    }
    NuFParDestroy(fp);
}

void *CutScenes_Load(char *config, NUGSCN *gscn1, NUGSCN *gscn2, i32 param1, VARIPTR *buf, VARIPTR *buf_end, i32 param2,
                     i32 param3, WORLDINFO *world) {
    NUFPAR *fp;
    CUTSYS *sys;
    void *initial;
    CUTINFO *cut;
    char name[128];
    char full_path[128];
    CUTINFO *entries[32];

    if (CutList != NULL && CUTCOUNT > 0) {
        sys = (CUTSYS *)ALIGN(buf->addr, 4);
        sys->cuts = reinterpret_cast<CUTINFO **>(sys + 1);
        sys->count = CUTCOUNT;
        CS_area = param2;
        CS_worldinfo = world;
        CS_cutsys = sys;
        buf->void_ptr = reinterpret_cast<char *>(sys->cuts) + CUTCOUNT * sizeof(CUTINFO *);
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
    sys = (CUTSYS *)ALIGN((usize)initial, 4);
    sys->cuts = reinterpret_cast<CUTINFO **>(sys + 1);
    sys->count = 0;
    sys->character_bits = reinterpret_cast<u32 *>(sys + 1);
    CS_area = param2;
    CS_worldinfo = world;
    CS_cutsys = sys;
    buf->void_ptr = reinterpret_cast<char *>(sys->character_bits) + ((CHARCOUNT + 0x1f) >> 5) * 4;

    while (NuFParGetLine(fp) != 0) {
        if (NuFParGetWord(fp) == 0 || NuStrICmp(fp->word_buf, "cutscene") != 0 || sys->count > 0x1f ||
            NuFParGetWord(fp) == 0) {
            continue;
        }

        cut = (CUTINFO *)ALIGN(buf->addr, 4);
        entries[sys->count] = cut;
        buf->void_ptr = cut + 1;
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
            NuStrCpy(name, name + 4);
        }
        NuStrCpy(cut->name, name);
        NuStrCpy(full_path, "cut\\");
        NuStrCat(full_path, name);
        NuStrCat(full_path, ".txt");
        CutScene_Configure_Load(cut, full_path, buf, buf_end);

        if ((reinterpret_cast<u8 *>(cut)[0x51] & 8) == 0 && !InStory()) {
            continue;
        }
        buf->void_ptr = (char *)ALIGN(buf->addr, 0x40);
        NuStrCpy(full_path, "cut\\");
        NuStrCat(full_path, name);
        NuStrCat(full_path, ".cu2");
        cut->scene = NuGCutSceneLoad(full_path, buf, buf_end, 0);
        if (cut->scene == NULL) {
            NuStrCpy(full_path, "cut\\");
            NuStrCat(full_path, name);
            NuStrCat(full_path, ".cut");
            cut->scene = NuGCutSceneLoad(full_path, buf, buf_end, 0);
            if (cut->scene == NULL) {
                continue;
            }
        }
        NuGCutSceneFixUp(reinterpret_cast<NUGCUTSCENE_s *>(cut->scene), gscn1, 0, static_cast<i8>(param1));
        NuGCutSceneFixUpExtra(reinterpret_cast<NUGCUTSCENE_s *>(cut->scene), gscn2);
        cut->instance = instNuGCutSceneCreate(reinterpret_cast<NUGCUTSCENE_s *>(cut->scene), NULL, NULL, name, buf, 0);
        if (cut->instance != NULL) {
            reinterpret_cast<instNUGCUTSCENE_s *>(cut->instance)->rate = cut->frames_per_second * DEFAULTFRAMETIME;
            buf->void_ptr = (char *)ALIGN(buf->addr, 0x10);
        }
        sys->count++;
    }
    NuFParDestroy(fp);
    if (sys->count == 0) {
        buf->void_ptr = initial;
        return NULL;
    }
    buf->void_ptr = reinterpret_cast<void *>(ALIGN(buf->addr, 4));
    sys->cuts = reinterpret_cast<CUTINFO **>(buf->void_ptr);
    memmove(sys->cuts, entries, sys->count * sizeof(CUTINFO *));
    buf->void_ptr = reinterpret_cast<char *>(buf->void_ptr) + sys->count * sizeof(CUTINFO *);
    return sys;
}

void CharScenes_LevelLoad(WORLDINFO *world) {
    if (CHARCOUNT <= 0) {
        return;
    }

    for (i32 i = 0; i < CHARCOUNT; i++) {
        CHARSCENE_s *entry = &world->minikit.character_scenes[i];
        entry->scene = NULL;

        // Check if we should load this character scene
        if ((CharScene_Area == NULL || CharScene_Area[i].scene == NULL) && (CDataList[i].flags & 1) != 0 &&
            world->cutscene_sys != NULL) {
            // Check if this character is in a cutscene
            u32 *cutscene_flags = *(u32 **)((char *)world->cutscene_sys + 8);
            u32 flag = (cutscene_flags[i >> 5] >> (i & 0x1f)) & 1;
            if (flag != 0) {
                // Load the character scene
                char path[136];
                VARIPTR buf_end = world->unknown_0108;
                sprintf(path, "chars\\%s\\%s.gsc", CDataList[i].dir, CDataList[i].file);
                NUGSCN *scene = NuGScnRead(&world->giz_buffer, buf_end, path);
                entry->scene = scene;
                if (scene != NULL) {
                    NuSpecialFind(scene, &entry->special_scene, CDataList[i].file, 1);
                }
            }
        }
    }
}

// --- Extern "C" block: functions with confirmed C linkage in original libTTapp.so ---
extern "C" void *NuAnimData2FixPtrs(void *, isize, isize, i32);
extern "C" StateAnim *StateAnimFixPtrs(StateAnim *, isize);
extern "C" i32 StateAnimEvaluate(StateAnim *, u8 *, u8 *, f32);
extern "C" i32 LookupDebrisEffectPage(char *, i32);
extern "C" i32 LookupDebrisEffectPageOnly(char *, i32);
extern "C" {
    extern i32 NuGCutDebFixUp_SearchAllPages;
    extern NUGCUTLOCATORFNENTRY_s *locatorfns;
    extern i32 (*LookupLocatorVfxFn)(char *);
    extern i32 (*NuCutSceneSFXFixUp)(usize);
    extern void (*NuCutSceneFindCharacters)(NUGCUTSCENE_s *);
}
void NuGCutRigidCalcMtx(NUGCUTRIGID_s *, f32, numtx_s *);

static instNUGCUTSCENE_s *active_cutscene_instances;

static void NuGCutSceneFixPtrs_Title(NUGCUTSCENE_s *cutscene, isize anim_delta) {
    usize data_delta = (usize)cutscene->string_delta;
    if (cutscene->strings != NULL) {
        cutscene->strings = reinterpret_cast<char *>(reinterpret_cast<usize>(cutscene->strings) + data_delta);
    }
    if (cutscene->rigid_system != NULL) {
        cutscene->rigid_system =
            reinterpret_cast<NUGCUTRIGIDSYS_s *>(reinterpret_cast<usize>(cutscene->rigid_system) + data_delta);
        NUGCUTRIGIDSYS_s *system = cutscene->rigid_system;
        if (system->rigids != NULL) {
            system->rigids = reinterpret_cast<NUGCUTRIGID_s *>(reinterpret_cast<usize>(system->rigids) + data_delta);
            for (u32 i = 0; i < system->count; ++i) {
                NUGCUTRIGID_s *rigid = &system->rigids[i];
                if (rigid->name != NULL) {
                    rigid->name = reinterpret_cast<char *>(reinterpret_cast<usize>(rigid->name) +
                                                           reinterpret_cast<usize>(cutscene->strings) - 1);
                }
                rigid->animation = static_cast<nuanimdata2_s *>(NuAnimData2FixPtrs(rigid->animation, anim_delta, 0, 0));
                rigid->state_animation = StateAnimFixPtrs(rigid->state_animation, anim_delta);
            }
        }
    }
    if (cutscene->locator_system != NULL) {
        cutscene->locator_system =
            reinterpret_cast<NUGCUTLOCATORSYS_s *>(reinterpret_cast<usize>(cutscene->locator_system) + data_delta);
        NUGCUTLOCATORSYS_s *system = cutscene->locator_system;
        if (system->locators != NULL) {
            system->locators =
                reinterpret_cast<NUGCUTLOCATOR_s *>(reinterpret_cast<usize>(system->locators) + data_delta);
            for (u32 i = 0; i < system->locator_count; ++i) {
                NUGCUTLOCATOR_s *locator = &system->locators[i];
                locator->animation =
                    static_cast<nuanimdata2_s *>(NuAnimData2FixPtrs(locator->animation, anim_delta, 0, 0));
            }
        }
        if (system->types != NULL) {
            system->types = reinterpret_cast<NUGCUTLOCATORTYPE_s *>(reinterpret_cast<usize>(system->types) + data_delta);
            for (u32 i = 0; i < system->type_count; ++i) {
                if (system->types[i].name != NULL) {
                    system->types[i].name = reinterpret_cast<char *>(reinterpret_cast<usize>(system->types[i].name) +
                                                                    reinterpret_cast<usize>(cutscene->strings) - 1);
                }
            }
        }
    }
    if (cutscene->bounds != NULL) {
        cutscene->bounds = reinterpret_cast<void *>(reinterpret_cast<usize>(cutscene->bounds) + data_delta);
    }
    if (cutscene->trigger_system != NULL) {
        cutscene->trigger_system =
            reinterpret_cast<void *>(reinterpret_cast<usize>(cutscene->trigger_system) + data_delta);
    }
}

extern "C" {

    i32 NuGCutDebFixUp_SearchAllPages = 0;
    NUGCUTLOCATORFNENTRY_s *locatorfns = NULL;
    i32 (*LookupLocatorVfxFn)(char *) = NULL;
    i32 (*NuCutSceneSFXFixUp)(usize) = NULL;
    void (*NuCutSceneFindCharacters)(NUGCUTSCENE_s *) = NULL;

    NUGCUTSCENE_s *NuGCutSceneLoad(char *name, VARIPTR *buf, VARIPTR *buf_end, i32 flags) {
        (void)flags;
        buf->addr = ALIGN(buf->addr, 0x10);
        NUGCUTSCENE_s *cutscene = reinterpret_cast<NUGCUTSCENE_s *>(buf->void_ptr);
        i32 bytes = NuFileLoadBuffer(name, cutscene, static_cast<i32>(buf_end->addr - buf->addr));
        if (bytes == 0) {
            return NULL;
        }
        usize anim_delta = reinterpret_cast<usize>(cutscene) - (usize)cutscene->relocation_delta;
        cutscene->string_delta = (isize)(reinterpret_cast<usize>(cutscene) - (usize)cutscene->string_delta);
        cutscene->relocation_delta = (isize)anim_delta;
        cutscene->loaded_size = bytes;
        NuGCutSceneFixPtrs_Title(cutscene, (isize)anim_delta);
        buf->addr += bytes;
        return cutscene;
    }
    void NuGCutSceneFixUp(NUGCUTSCENE_s *cutscene, NUGSCN *scene, i32 flags, i8 area) {
        if (cutscene == NULL) {
            return;
        }
        if (cutscene->version > 1) {
            cutscene->scene = scene;
            cutscene->extra_scene = reinterpret_cast<void *>(static_cast<usize>(flags));
        }
        if (scene != NULL && cutscene->rigid_system != NULL && cutscene->rigid_system->rigids != NULL) {
            for (u32 i = 0; i < cutscene->rigid_system->count; ++i) {
                NUGCUTRIGID_s *rigid = &cutscene->rigid_system->rigids[i];
                nuhspecial_s special;
                if (NuSpecialFind(scene, &special, rigid->name, 1) != 0) {
                    rigid->flags |= 4;
                    rigid->scene = special.scene;
                    rigid->special_object = special.display_special != NULL ? special.display_special : special.special;
                }
                if (rigid->locator_count != 0 && cutscene->locator_system != NULL && rigid->locator < 0xff) {
                    rigid->locator_index = static_cast<u8>(rigid->locator);
                    rigid->locator = reinterpret_cast<usize>(&cutscene->locator_system->locators[rigid->locator_index]);
                } else {
                    rigid->locator_index = 0xff;
                }
            }
        }
        if (cutscene->character_system != NULL && NuCutSceneFindCharacters != NULL) {
            NuCutSceneFindCharacters(cutscene);
        }
        NUGCUTLOCATORSYS_s *system = cutscene->locator_system;
        if (system == NULL || system->types == NULL || system->type_count == 0) {
            return;
        }
        for (u32 i = 0; i < system->type_count; ++i) {
            NUGCUTLOCATORTYPE_s *type = &system->types[i];
            if ((type->flags & 1) != 0) {
                if (NuGCutDebFixUp_SearchAllPages == 0) {
                    type->function_index = static_cast<u16>(LookupDebrisEffectPageOnly(type->name, area));
                } else {
                    type->function_index = static_cast<u16>(LookupDebrisEffectPage(type->name, area));
                }
            } else if ((type->flags & 2) != 0) {
                i32 function_index = -1;
                if (locatorfns != NULL && type->name != NULL && locatorfns[0].name != NULL) {
                    for (i32 j = 0; locatorfns[j].name != NULL; ++j) {
                        if (NuStrICmp(type->name, locatorfns[j].name) == 0) {
                            function_index = j;
                            break;
                        }
                    }
                }
                type->function_index = static_cast<u16>(function_index);
            } else if ((type->flags & 0x10) != 0 && LookupLocatorVfxFn != NULL && type->name != NULL) {
                char *underscore = NuStrRChr(type->name, '_');
                if (underscore != NULL && underscore[1] > '/' && underscore[1] < ':') {
                    *underscore = '\0';
                }
                type->function_index = static_cast<u16>(LookupLocatorVfxFn(type->name));
            } else if ((type->flags & 4) != 0 && NuCutSceneSFXFixUp != NULL && type->name != NULL) {
                type->function_index = static_cast<u16>(NuCutSceneSFXFixUp(reinterpret_cast<usize>(type->name)));
                if (static_cast<i16>(type->function_index) != -1) {
                    cutscene->flags |= 4;
                }
            }
        }
    }
    void NuGCutSceneFixUpExtra(NUGCUTSCENE_s *cutscene, NUGSCN *area) {
        if (cutscene == NULL || cutscene->rigid_system == NULL || area == NULL) {
            return;
        }

        NUGCUTRIGIDSYS_s *system = cutscene->rigid_system;
        for (u32 i = 0; i < system->count; ++i) {
            NUGCUTRIGID_s *rigid = &system->rigids[i];
            if ((rigid->flags & 4) != 0) {
                continue;
            }

            nuhspecial_s special;
            if (NuSpecialFind(area, &special, rigid->name, 1) == 0) {
                continue;
            }
            rigid->flags |= 4;
            rigid->scene = special.scene;
            rigid->special_object = special.display_special != NULL ? special.display_special : special.special;
        }
    }
    instNUGCUTSCENE_s *instNuGCutSceneCreate(NUGCUTSCENE_s *cutscene, NUGSCN *scene, void *extra, char *name,
                                             VARIPTR *buf, i32 flags) {
        (void)scene;
        (void)extra;
        (void)flags;
        if (cutscene == NULL) {
            return NULL;
        }
        buf->addr = ALIGN(buf->addr, 0x10);
        instNUGCUTSCENE_s *instance = reinterpret_cast<instNUGCUTSCENE_s *>(buf->void_ptr);
        buf->void_ptr = instance + 1;
        memset(instance, 0, sizeof(*instance));
        instance->alpha = 1.0f;
        instance->cutscene = cutscene;
        instance->cutscene_copy = cutscene;
        instance->current_frame = 1.0f;
        instance->render_frame = 1.0f;
        instance->rate = 1.0f;
        NuMtxSetIdentity(&instance->matrix);
        if (name != NULL) {
            snprintf(instance->name, sizeof(instance->name), "%s", name);
        }

        if (cutscene->rigid_system != NULL && cutscene->rigid_system->count != 0) {
            instance->rigid_instance = reinterpret_cast<instNUGCUTRIGIDSYS_s *>(ALIGN(buf->addr, 4));
            buf->void_ptr = instance->rigid_instance + 1;
            buf->addr = ALIGN(buf->addr, 0x10);
            instance->rigid_instance->rigids = reinterpret_cast<instNUGCUTRIGID_s *>(buf->void_ptr);
            buf->void_ptr = instance->rigid_instance->rigids + cutscene->rigid_system->count;
            memset(instance->rigid_instance->rigids, 0, cutscene->rigid_system->count * sizeof(instNUGCUTRIGID_s));
            for (u32 i = 0; i < cutscene->rigid_system->count; ++i) {
                NUGCUTRIGID_s *rigid = &cutscene->rigid_system->rigids[i];
                instNUGCUTRIGID_s *inst_rigid = &instance->rigid_instance->rigids[i];
                inst_rigid->scene = rigid->scene;
                inst_rigid->display_special = rigid->special_object;
                inst_rigid->visible = rigid->flags & 1;
            }
        }

        if (cutscene->locator_system != NULL && cutscene->locator_system->locator_count != 0) {
            instance->locator_instance = reinterpret_cast<instNUGCUTLOCATORSYS_s *>(ALIGN(buf->addr, 4));
            buf->void_ptr = instance->locator_instance + 1;
            buf->addr = ALIGN(buf->addr, 0x10);
            instance->locator_instance->locators = reinterpret_cast<instNUGCUTLOCATOR_s *>(buf->void_ptr);
            buf->void_ptr = instance->locator_instance->locators + cutscene->locator_system->locator_count;
            memset(instance->locator_instance->locators, 0,
                   cutscene->locator_system->locator_count * sizeof(instNUGCUTLOCATOR_s));
            for (u32 i = 0; i < cutscene->locator_system->locator_count; ++i) {
                NUGCUTLOCATOR_s *locator = &cutscene->locator_system->locators[i];
                NUGCUTLOCATORTYPE_s *type = &cutscene->locator_system->types[locator->type_index];
                if ((type->flags & 1) != 0 && (locator->flags & 0x20) != 0) {
                    instance->locator_instance->locators[i].effect_handle = -1;
                }
            }
        }

        instance->next = active_cutscene_instances;
        if (active_cutscene_instances != NULL) {
            active_cutscene_instances->previous = instance;
        }
        active_cutscene_instances = instance;
        instance->allocation_size = static_cast<i32>(buf->addr - reinterpret_cast<usize>(instance));
        return instance;
    }

    void instNuGCutSceneReset(instNUGCUTSCENE_s *instance) {
        if (instance == NULL) {
            return;
        }
        instance->current_frame = 1.0f;
        instance->render_frame = 1.0f;
        instance->flags_88 &= 0xf8;
        instance->flags_89 &= 0xef;
        instance->flags_8c &= 0xbf;
        instance->flags_8d &= 0xef;
        if (instance->rate < 0.0f) {
            instance->rate = -instance->rate;
        }
        instance->cutscene = instance->cutscene_copy;
        if (instance->rigid_instance != NULL && instance->cutscene->rigid_system != NULL) {
            for (u32 i = 0; i < instance->cutscene->rigid_system->count; ++i) {
                NUGCUTRIGID_s *rigid = &instance->cutscene->rigid_system->rigids[i];
                instNUGCUTRIGID_s *inst_rigid = &instance->rigid_instance->rigids[i];
                inst_rigid->state_index = 0;
                if ((rigid->flags & 6) == 6) {
                    inst_rigid->visible = rigid->flags & 1;
                }
            }
        }
    }

    void instNuGCutSceneStart(instNUGCUTSCENE_s *instance) {
        instance->current_frame = 1.0f;
        instance->render_frame = 1.0f;
        instance->flags_89 &= 0xef;
        instance->flags_8d &= 0xef;
        instance->flags_88 = (instance->flags_88 & 0xfe) | 2;
        if (instance->rate < 0.0f) {
            instance->rate = -instance->rate;
        }
        if (instance->rigid_instance != NULL && instance->cutscene->rigid_system != NULL) {
            for (u32 i = 0; i < instance->cutscene->rigid_system->count; ++i) {
                NUGCUTRIGID_s *rigid = &instance->cutscene->rigid_system->rigids[i];
                instNUGCUTRIGID_s *inst_rigid = &instance->rigid_instance->rigids[i];
                inst_rigid->state_index = 0;
                if ((rigid->flags & 6) == 6) {
                    inst_rigid->visible = rigid->flags & 1;
                }
            }
        }
    }

    void instNuGCutScenePause(instNUGCUTSCENE_s *instance, u8 paused) {
        instance->flags_88 = (instance->flags_88 & 0xfb) | ((paused & 1) << 2);
    }
} // extern "C"

struct instNUGCUTSCENE_s;
struct NUGCUTLOCATORSYS_s;
struct instNUGCUTLOCATOR_s;
struct NUGCUTLOCATOR_s;
struct numtx_s;
extern "C" i32 NuGCutLocatorCalcMtx(NUGCUTLOCATOR_s *, f32, NUMTX *, nuanimtime_s *);
extern "C" i32 NuGCutLocatorIsVisble(NUGCUTLOCATOR_s *, f32, nuanimtime_s *, f32 *, f32 *);
extern "C" void NuAnimData2CalcTime(nuanimdata2_s *, f32, nuanimtime_s *);
extern "C" void instNuGCutLocatorUpdate(instNUGCUTSCENE_s *, NUGCUTLOCATORSYS_s *, instNUGCUTLOCATOR_s *,
                                        NUGCUTLOCATOR_s *, f32, NUMTX *, i32);
void Draw3DObjectMtx(WORLDINFO_s *, i32, numtx_s *);
extern CUTSCENESYS *CutSceneSys;

static __used__ void LocatorFunction_Blaster(instNUGCUTSCENE_s *, NUGCUTLOCATORSYS_s *, instNUGCUTLOCATOR_s *,
                                             NUGCUTLOCATOR_s *locator, float frame, numtx_s *parent_mtx, int) {
    nuanimtime_s time;
    NuAnimData2CalcTime(locator->animation, frame, &time);
    if (NuGCutLocatorIsVisble(locator, frame, &time, NULL, NULL) == 0) {
        return;
    }
    NUMTX matrix;
    NuGCutLocatorCalcMtx(locator, frame, &matrix, &time);
    if ((locator->flags & 4) != 0) {
        NuMtxPreTranslate(&matrix, &locator->pivot);
    }
    if (parent_mtx != NULL) {
        NuMtxMul(&matrix, &matrix, parent_mtx);
    }
    if (CutSceneSys != NULL) {
        Draw3DObjectMtx(NULL, CutSceneSys->blaster_object_0, &matrix);
        Draw3DObjectMtx(NULL, CutSceneSys->blaster_object_1, &matrix);
    }
}

extern "C" {
    __attribute__((weak)) NUGCUTLOCATORFNENTRY_s cutscene_locatorfns[] = {
        {"blaster", 0, 0, 0, LocatorFunction_Blaster},
        {NULL, 0, 0, 0, NULL},
    };
}

static __used__ void instNuGCutRigidSysEnd(instNUGCUTSCENE_s *, float) {
}

static void instNuGCutRigidSysUpdate(instNUGCUTSCENE_s *, float, int);

static __used__ void instNuGCutSceneUpdate(instNUGCUTSCENE_s *instance, int paused, int, float elapsed) {
    instance->flags_89 |= 4;
    if ((instance->flags_88 & 2) == 0) {
        return;
    }
    if ((instance->flags_88 & 1) == 0) {
        if (paused == 0) {
            instance->flags_88 |= 1;
        }
    } else if (paused == 0) {
        f32 frame = instance->current_frame + instance->rate * elapsed;
        f32 end_frame = instance->cutscene->duration - 1.0f;
        if (frame >= end_frame) {
            if ((instance->flags_88 & 8) != 0) {
                frame -= end_frame;
                instNuGCutSceneStart(instance);
                instance->flags_88 |= 1;
                instance->current_frame += frame;
            } else {
                instance->current_frame = end_frame;
                instance->flags_88 &= ~2U;
            }
        } else {
            instance->current_frame = frame;
        }
    }
    instance->render_frame = instance->current_frame;
    if (instance->rigid_instance != NULL) {
        instNuGCutRigidSysUpdate(instance, static_cast<float>(paused), 0);
    }
}

static __used__ void instNuGCutCamSysUpdate(instNUGCUTSCENE_s *, float) {
}

static __used__ void instNuGCutSceneClipTest(instNUGCUTSCENE_s *instance) {
    instance->flags_89 |= 4;
}

static __used__ void instNuGCutRigidSysRender(instNUGCUTSCENE_s *instance, float paused, int) {
    NUGCUTRIGIDSYS_s *system = instance->cutscene->rigid_system;
    if (system == NULL || instance->rigid_instance == NULL) {
        return;
    }
    for (u32 i = 0; i < system->count; ++i) {
        NUGCUTRIGID_s *rigid = &system->rigids[i];
        if ((rigid->flags & 6) != 6) {
            continue;
        }
        instNUGCUTRIGID_s *inst_rigid = &instance->rigid_instance->rigids[i];
        u8 visible = inst_rigid->visible;
        if (rigid->state_animation != NULL && StateAnimEvaluate(rigid->state_animation, &inst_rigid->state_index,
                                                                &visible, instance->render_frame) != 0) {
            inst_rigid->visible = visible != 0;
        }
        if (inst_rigid->visible == 0) {
            continue;
        }
        NUMTX matrix;
        NuGCutRigidCalcMtx(rigid, instance->render_frame, &matrix);
        if ((instance->flags_88 & 0x80) != 0) {
            NuMtxMul(&matrix, &matrix, &instance->matrix);
        }
        if (instance->alpha == 1.0f) {
            NuSpecialDrawAt(inst_rigid, &matrix);
        } else {
            NuSpecialDrawAtAlpha(inst_rigid, &matrix, instance->alpha);
        }
        if (rigid->locator_index != 0xff && rigid->locator_count != 0 && instance->cutscene->locator_system != NULL &&
            instance->locator_instance != NULL) {
            NUGCUTLOCATORSYS_s *locator_system = instance->cutscene->locator_system;
            for (u32 locator_offset = 0; locator_offset < rigid->locator_count; ++locator_offset) {
                u32 locator_index = rigid->locator_index + locator_offset;
                instNuGCutLocatorUpdate(instance, locator_system, &instance->locator_instance->locators[locator_index],
                                        &locator_system->locators[locator_index], instance->render_frame, &matrix,
                                        static_cast<i32>(paused));
            }
        }
    }
}

static __used__ void instNuGCutRigidSysUpdate(instNUGCUTSCENE_s *instance, float paused, int) {
    NUGCUTRIGIDSYS_s *system = instance->cutscene->rigid_system;
    if (system == NULL || instance->rigid_instance == NULL) {
        return;
    }

    for (u32 i = 0; i < system->count; ++i) {
        NUGCUTRIGID_s *rigid = &system->rigids[i];
        if ((rigid->flags & 4) == 0 || (rigid->flags & 2) != 0) {
            continue;
        }

        instNUGCUTRIGID_s *inst_rigid = &instance->rigid_instance->rigids[i];
        u8 visible;
        if (rigid->state_animation != NULL && StateAnimEvaluate(rigid->state_animation, &inst_rigid->state_index,
                                                                &visible, instance->render_frame) != 0) {
            NuSpecialSetVisibility(inst_rigid, visible != 0);
        }

        if (NuSpecialGetVisibilityFn(inst_rigid) == 0) {
            continue;
        }

        NUMTX matrix;
        NuGCutRigidCalcMtx(rigid, instance->render_frame, &matrix);
        if ((instance->flags_88 & 0x80) != 0) {
            NuMtxMul(&matrix, &matrix, &instance->matrix);
        }
        NuSpecialSetDrawMtx(inst_rigid, &matrix);

        if (rigid->locator_index != 0xff && rigid->locator_count != 0 && instance->cutscene->locator_system != NULL &&
            instance->locator_instance != NULL) {
            NUGCUTLOCATORSYS_s *locator_system = instance->cutscene->locator_system;
            for (u32 locator_offset = 0; locator_offset < rigid->locator_count; ++locator_offset) {
                u32 locator_index = rigid->locator_index + locator_offset;
                instNuGCutLocatorUpdate(instance, locator_system, &instance->locator_instance->locators[locator_index],
                                        &locator_system->locators[locator_index], instance->render_frame, &matrix,
                                        static_cast<i32>(paused));
            }
        }
    }
}

static __used__ void instNuGCutTriggerSysUpdate(instNUGCUTSCENE_s *, float) {
}

extern "C" void NuGCutSceneSysUpdate(i32 paused, i32 skip, f32 elapsed) {
    for (instNUGCUTSCENE_s *instance = active_cutscene_instances; instance != NULL; instance = instance->next) {
        instNuGCutSceneUpdate(instance, paused != 0, skip, elapsed);
    }
}

extern "C" void NuGCutSceneSysRender(f32 paused) {
    for (instNUGCUTSCENE_s *instance = active_cutscene_instances; instance != NULL; instance = instance->next) {
        if ((instance->flags_89 & 8) == 0 && (instance->flags_88 & 2) != 0 && (instance->flags_89 & 4) != 0 &&
            instance->rigid_instance != NULL) {
            instNuGCutRigidSysRender(instance, paused, 0);
        }

        if ((instance->flags_89 & 8) != 0 || (instance->flags_88 & 2) == 0 || (instance->flags_89 & 4) == 0 ||
            instance->locator_instance == NULL || instance->cutscene->locator_system == NULL) {
            continue;
        }
        if ((instance->flags_8c & 0x40) != 0 && instance->current_frame == instance->cutscene->duration - 1.0f) {
            continue;
        }
        if ((instance->flags_8b & 0x40) != 0) {
            continue;
        }

        NUGCUTLOCATORSYS_s *system = instance->cutscene->locator_system;
        f32 frame = (instance->flags_8a & 4) == 0 ? instance->render_frame
                                                  : instance->cutscene->duration - instance->render_frame;
        NUMTX *parent_matrix = static_cast<i8>(instance->flags_88) < 0 ? &instance->matrix : NULL;
        for (u32 i = 0; i < system->locator_count; ++i) {
            NUGCUTLOCATOR_s *locator = &system->locators[i];
            if ((locator->flags & 3) == 0) {
                instNuGCutLocatorUpdate(instance, system, &instance->locator_instance->locators[i], locator, frame,
                                        parent_matrix, static_cast<i32>(paused));
            }
        }
    }
}

static __used__ void CutScene_Configure(CUTINFO *, char *, variptr_u *, variptr_u *) {
}

static __used__ void bgAckStreamCutScene(bgprocinfo_s *) {
}

static __used__ void bgLoadStreamCutScene(bgprocinfo_s *) {
}

static __used__ unsigned int CutScenePlayer_Accept(CUTSCENEPLAYERCLIP *) {
    return {};
}

static __used__ void CutScene_DrawCharacter(instNUGCUTSCENE_s *, NUGCUTSCENE_s *, instNUGCUTCHAR_s *, NUGCUTCHAR_s *,
                                            float, int) {
}

static __used__ void CutScene_EvalCharacter(instNUGCUTSCENE_s *, NUGCUTSCENE_s *, instNUGCUTCHAR_s *, NUGCUTCHAR_s *,
                                            float) {
}

static __used__ unsigned int CutScene_FindCharacters(NUGCUTSCENE_s *) {
    return {};
}

static __used__ void CutScene_ResetCharacters(instNUGCUTSCENE_s *instance) {
    NUGCUTSCENE_s *cutscene = instance->cutscene;
    instNUGCUTCHARSYS_s *character_instance = instance->character_instance;
    NUGCUTCHARSYS_s *character_system = cutscene->character_system;

    i32 i = 0;
    while (i < character_system->character_count) {
        instNUGCUTCHAR_s *character = &character_instance->characters[i];
        character->field_04 = 0;
        character->field_14 = 0;
        character->field_15 = 0xff;
        ++i;
    }
}

static __used__ void CutScene_RigidPostRender(NUGCUTRIGID_s *, instNUGCUTRIGID_s *, numtx_s *) {
}

static __used__ void *CutScene_CreateCharacterInstance(NUGCUTCHAR_s *, instNUGCUTCHAR_s *, variptr_u *) {
    return {};
}

static __used__ void CutScene_OverrideConfigFileName_LSW(char *, int, int) {
}

static __used__ void copyAnims(NUGCUTSCENE_s *, NUGCUTSCENE_s *) {
}
