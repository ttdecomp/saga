// Permanent-data loading: LoadPerm / LoadPermData.
//
// Original addresses: StartPerm 0x470db0, EndPerm 0x470de0, LoadPerm 0x1bf310,
// LoadPermData (file-static) 0x1bebd0. The file-statics legal_tid /
// loadlegal_done / legal_mtl live in this TU in the original as well
// (_ZL9legal_tid / _ZL14loadlegal_done / _ZL9legal_mtl).
#include "legogame/startup.h"

#include <string.h>
#include <time.h>

#include "globals.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels/episode.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"

// C++-linkage declarations (defined in nu3d TUs).
NUMTL *NuMtlCreate3D(i32 count);

// --- Functions without a shared header yet (originals verified to exist).
// Each declaration's language linkage matches its definition TU. ---
extern "C" {
    // nucore_plain.cpp / nurndr_plain.cpp / editor stubs (extern "C" TUs).
    void NuStringFilterLoad(char *, VARIPTR *, VARIPTR *);
    f32 NuIOS_GetAspectRatio(void);
    i32 NuIOS_GetDeviceLanguage(void);
    void NuLanguageSet(i32 language);
    void NuCameraSet(NUCAMERA *cam);
    void NuFrameBegin(void);
    f32 NuFrameEnd(void);
    void NuRndrBeginScene(void);
    void NuRndrEndScene(void);
    void NuRndrGradClear(i32 a, i32 b, i32 c, f32 d);
    void NuRndrClear(u32 flags, u32 colour, f32 alpha);
    void NuMtlDestroy(NUMTL *mtl);
    NUMTL *NuMtlCreate(i32 count);
    void NuMtlUpdate(NUMTL *mtl);
    void edGraEnableTerrainSwap(void);
    void edGraDisableTerrainSwap(void);
    void APICharacterSysInit(VARIPTR *buf, VARIPTR buf_end, i32 char_count, i32 a4, i32 a5, CHARACTERDATA *cdata_list,
                             i32 a7);
    void SetActionInfo(void *action_info, void *extra_action_data);
    void SetProceduralAnimationFn(void *fn);
    void NuAnimBuffProceduralAnimation(void);
    void DrawMenu(i32 menu_id);
}

// C++-linkage definitions in their own TUs:
void InitMemCard(void);                                         // saveload.cpp
void Text_LoadFont(char *path, VARIPTR *buf, VARIPTR *buf_end); // text.cpp
bool Text_IsFontLoaded(void);                                   // text.cpp
void Text_InitStringTable(i32, VARIPTR *, VARIPTR *);
void Text_InitTable(TEXTENTRY *, i32, i32);
void Text_LoadStrings(VARIPTR *, VARIPTR *);
void Text_InitDefaultStrings(void);
void InitStillRender(VARIPTR *buf, VARIPTR buf_end);
void LevelProgress_ReserveBufferSpace(VARIPTR *buf, VARIPTR buf_end);
void LevelObjects_InitForGame(LEVELOBJECT *tab, VARIPTR *buf, VARIPTR *buf_end, i32 a4, i32 a5);
void LevelSplines_InitForGame(LEVELSPLINE *tab);
NUGSCN *NuGScnRead(VARIPTR *buf, VARIPTR buf_end, char *path);
void CreateThingManager(void);
void RegisterGizmoTypes_LSW(VARIPTR *buffer, VARIPTR *buffer_end);
void GameAnimSys_AllocateLevelProgressData(VARIPTR *buf, VARIPTR *buf_end, i32 a3, i32 a4);
void LSW_registerStatusScreen(void);
void initGameHintSys_LSW(void);
void Movies_ConfigureList(char *path, VARIPTR *buf, VARIPTR *buf_end);
CHARACTERDATA *ConfigureCharacterList(char *path, VARIPTR *buf, VARIPTR *buf_end, i32 max, i32 *count, i32 stride,
                                      GAMECHARACTERDATA **gcdata);
void CharScenes_Init(VARIPTR *buf, VARIPTR *buf_end);
void IconScenes_Init(char *prefix, VARIPTR *buf, VARIPTR *buf_end);
void FixUpCharacters(CHARFIXUP *fixup);
void MiniKits_Init(VARIPTR *buf, VARIPTR *buf_end);
void CharCategories_Init(CHARCATEGORY *cat);
void Cheats_Init(CHEAT *cheats);
void CharVariants_Init(CHARVARIANT *variants, i32 count);
LEVELDATA *Levels_ConfigureList(char *path, VARIPTR *buf, VARIPTR *buf_end, i32 max, i32 *count,
                                void (*set_defaults)(LEVELDATA *, i32));
void FixUpLevels(LEVELFIXUP *fixup);
AREADATA *Areas_ConfigureList(char *path, VARIPTR *buf, VARIPTR *buf_end, i32 max, i32 *count);
void FixUpAreas(void);
EPISODEDATA *Episodes_ConfigureList(char *path, VARIPTR *buf, VARIPTR *buf_end, i32 max, i32 *count);
void NewGame(void);
void InitGameAfterConfig(void);
GIZAIMESSAGESYS_s *CreateGizAIMessageSys(VARIPTR *buf, VARIPTR *buf_end, i32 size);
void BackDrop_Init(char *path, VARIPTR *buf, VARIPTR *buf_end);
void LoadPerm1(void);
void LoadPerm2(void);
void RegisterHelpers(void);
void InitPanel(i32 panel);
void UpdateGameMenu(GAMEPAD_s *pad, i32 a2);
void SetBackgroundMusic(i32 track);
void UpdateTimer(TIMER *timer);
void IntroText_Draw(f32 alpha);
void ReadPads(void);

// Globals without headers.
extern i16 id_DEFAULTCHARACTER[2];
extern i16 id_OBIWANKENOBI;
extern GAMEPAD_s GamePad[64]; // gamepads.cpp, bss @0x127a500
extern i32 readpads_always;

// fade.cpp global effect objects.
extern Fade fade;
extern FadeWipe fadeWipe;
extern FadeStillWipe fadeStillWipe;
extern FadeStill fadeStill;

void *GameBufferAlloc(VARIPTR *buf, VARIPTR *buf_end, i32 size); // gameobjects.cpp
struct CHARFIXUP;
extern CHARFIXUP CharFixUp[222]; // characters.cpp

// float -> IEEE half for the half-UV vertex format.
static u16 loadperm_f32_to_f16(f32 v) {
    u32 bits;
    memcpy(&bits, &v, 4);
    u32 sign = (bits >> 16) & 0x8000;
    i32 exp = (i32)((bits >> 23) & 0xff) - 127 + 15;
    u32 mant = bits & 0x7fffff;
    if (exp >= 0x1f) {
        return (u16)(sign | 0x7c00);
    }
    if (exp <= 0) {
        return (u16)sign;
    }
    return (u16)(sign | (u32)(exp << 10) | (mant >> 13));
}

// LSW gameplay hook implementations wired up by LoadPermData.
void CutScene_StartFn_LSW(CUTINFO *);
void CutScene_PreUpdateFn_LSW(CUTINFO *);
void CutScene_PostUpdateFn_LSW(void);
void CutScene_StoppedFn_LSW(CUTINFO *);
void CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *, NUGCUTCHAR_s *);
void InitBolt_AddMomentumType_LSW(BOLT_s *, GameObject_s *, nuvec_s *);
void Bolt_HitPlatFn_LSW(BOLT_s *);
void Bolt_HitCustomFn_LSW(BOLT_s *, nuvec_s *);
void GameBlowUpBlownUpFn_LSW(GIZMOBLOWUP_s *);
void GizObstacle_SetDefaultSFXFn_LSW(void *, GIZOBSTACLE_s *);

// File-statics of the original startup TU.
static i32 legal_tid;       // texture id of the loaded legal screen
static bool loadlegal_done; // legal texture already attempted
static NUMTL *legal_mtl;    // material wrapping the legal texture

static void LoadPermData(BGPROCINFO *proc) {
    InitMemCard();

    Text_LoadFont(Text_Language == 0 ? (char *)"stuff\\text\\starwars_font_j" : (char *)"stuff\\text\\starwars_font",
                  &permbuffer_ptr, &permbuffer_end);

    // The language selection is settled before the background request runs.
    while (LoadPerm_LanguageSelect != 3) {
    }

    Text_InitStringTable(0x70d, &permbuffer_ptr, &permbuffer_end);
    Text_InitTable((TEXTENTRY *)LSW_Text /* original: &LSW_Text */, 0, 0x70c);
    Text_LoadStrings(&permbuffer_ptr, &permbuffer_end);
    Text_InitDefaultStrings();
    LoadPerm_StringsLoaded = 1;

    // HOST-ONLY: the original fills IntroText_TextID from the loaded string
    // table during permanent-data processing; until that lookup is
    // decompiled, give it a valid id so the loading-screen timer state
    // machine (t84/t8c/t88) runs exactly as it does on device.
    if (IntroText_TextID == -1) {
        IntroText_TextID = 0;
    }

    LOG_INFO("LoadPermData: proc=%p langsel=%d", (void *)proc, LoadPerm_LanguageSelect);
    if (proc != NULL && legal_tid == 0 && !loadlegal_done) {
        // Legal-screen texture, carved from the top of the super buffer.
        VARIPTR legalTex;
        legalTex.addr = superbuffer_end.addr - 0x400000;

        legal_tid =
            NuTexRead((char *)(Text_Language == 2 ? "stuff\\legal\\LEGAL_FRENCH" : "stuff\\legal\\LEGAL_ENGLISH"),
                      &legalTex, &superbuffer_end);
        loadlegal_done = true;
        LOG_INFO("LoadPermData: legal_tid=%d", legal_tid);
    }

    MechSystems::Get()->LoadPerm();

    NuStringFilterLoad((char *)"stuff\\text\\badwords.txt", &permbuffer_ptr, &permbuffer_end);

    MusicInfo = ConfigureMusic((char *)"audio\\music.txt", &permbuffer_ptr, &permbuffer_end);
    RegisterMusic(MusicInfo);
    InitSfx(&permbuffer_ptr, permbuffer_end, "Audio\\Audio.cfg");
    InitStillRender(&permbuffer_ptr, permbuffer_end);

    LevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, 0x80);
    OldLevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, 0x80);
    LevelHackSize = 0;

    LevelProgress_ReserveBufferSpace(&permbuffer_ptr, permbuffer_end);

    FadeSys.Init();
    FadeSys.AddFade(&fade);
    FadeSys.AddFade(&fadeWipe);
    FadeSys.AddFade(&fadeStillWipe);
    FadeSys.AddFade(&fadeStill);
    pFadeInfo = &FadeSys;

    LevelObjects_InitForGame((LEVELOBJECT *)ObjTab, &permbuffer_ptr, &permbuffer_end, 0x2ee, 0x1f40);
    LevelSplines_InitForGame((LEVELSPLINE *)SplTab);

    saveicon_scene = NuGScnRead(&permbuffer_ptr, *(VARIPTR *)&permbuffer_end, (char *)"stuff\\ps2_bits.gsc");
    button_scene = NuGScnRead(&permbuffer_ptr, *(VARIPTR *)&permbuffer_end, (char *)"stuff\\pc_bits.gsc");

    {
        // Inline theMemoryManager block initialisation (original does this at
        // the site, no call): a tiny bump allocator over the next 0x200 bytes
        // of the permanent buffer.
        u8 *mm = theMemoryManager;
        *(u8 **)(mm + 0x8) = mm; // self pointer
        *(u32 *)(mm + 0x14) = 0;
        u32 base = ((u32)permbuffer_ptr.addr + 0xf) & ~0xfu;
        *(u32 *)(mm + 0x0) = base;
        *(u32 *)(mm + 0x4) = base + 0x200;
        permbuffer_ptr.addr = base + 0x200;
        *(u32 *)(mm + 0x18) = 0x200;
        // The original stores a self-referential pointer as u32; go through
        // usize so the truncation is explicit.
        *(u32 *)(mm + 0xc) = (u32)(usize)(mm + 4);
        *(u32 *)(mm + 0x10) = 0;
        memset(mm + 0x1c, 0, 0x22c);
    }

    CreateThingManager();
    RegisterGizmoTypes_LSW(&permbuffer_ptr, &permbuffer_end);

    GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
    LSW_registerStatusScreen();
    initGameHintSys_LSW();

    CutScene_StartFn = CutScene_StartFn_LSW;
    CutScene_PreUpdateFn = CutScene_PreUpdateFn_LSW;
    CutScene_PostUpdateFn = CutScene_PostUpdateFn_LSW;
    CutScene_StoppedFn = CutScene_StoppedFn_LSW;
    CutScene_ReplaceCharacterModelFn = CutScene_ReplaceCharacterModelFn_LSW;

    InitBolt_AddMomentumType = InitBolt_AddMomentumType_LSW;
    Bolt_HitPlatFn = Bolt_HitPlatFn_LSW;
    Bolt_HitCustomFn = Bolt_HitCustomFn_LSW;
    GameBlowUpBlownUpFn = GameBlowUpBlownUpFn_LSW;
    GizObstacle_SetDefaultSFXFn = GizObstacle_SetDefaultSFXFn_LSW;

    Movies_ConfigureList((char *)"movies\\movies.txt", &permbuffer_ptr, &permbuffer_end);

    SetProceduralAnimationFn((void *)NuAnimBuffProceduralAnimation);

    CDataList = ConfigureCharacterList((char *)"chars\\chars.txt", &permbuffer_ptr, &permbuffer_end, 0x154, &CHARCOUNT,
                                       0x120, &GCDataList);
    CharScenes_Init(&permbuffer_ptr, &permbuffer_end);
    IconScenes_Init((char *)"stuff\\icons\\", &permbuffer_ptr, &permbuffer_end);
    FixUpCharacters(CharFixUp);
    MiniKits_Init(&permbuffer_ptr, &permbuffer_end);

    PlayerID[0] = id_DEFAULTCHARACTER[0];
    PlayerID[1] = id_OBIWANKENOBI;

    CharCategories_Init((CHARCATEGORY *)LSW_CharCategory);
    Cheats_Init((CHEAT *)Cheat);
    CharVariants_Init((CHARVARIANT *)CharVariants_Game, 0x17);

    LDataList = Levels_ConfigureList((char *)"levels\\levels.txt", &permbuffer_ptr, &permbuffer_end, 0x16d, &LEVELCOUNT,
                                     &Level_SetDefaults);
    FixUpLevels(&LevFixUp);

    ADataList = Areas_ConfigureList((char *)"levels\\areas.txt", &permbuffer_ptr, &permbuffer_end, 0x48, &AREACOUNT);
    FixUpAreas();

    EDataList =
        Episodes_ConfigureList((char *)"levels\\episodes.txt", &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);

    NewGame();
    InitGameAfterConfig();

    APICharacterSysInit(&permbuffer_ptr, *(VARIPTR *)&permbuffer_end, CHARCOUNT, 0x30, 0xe9, CDataList, 0x400);
    SetActionInfo(ActionInfo, ExtraActionData);

    gizaimessagesys = CreateGizAIMessageSys(&permbuffer_ptr, &permbuffer_end, 0x40);

    LOG_INFO("LoadPermData: before backdrop");
    permbuffer_ptr.addr = ALIGN(permbuffer_ptr.addr, 0x10);
    BackDrop_Init((char *)"stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);

    LOG_INFO("LoadPermData: before LoadPerm1");
    LoadPerm1();
    LOG_INFO("LoadPermData: before LoadPerm2");
    LoadPerm2();
    LOG_INFO("LoadPermData: after LoadPerm2");
    if (theGameThings != NULL) {
        // Original passes the global manager unconditionally; ours is created
        // by CreateThingManager() which is still a stub on this path.
        ((GameThingManager *)theGameThings)->AddOnceOnlyThings();
    }
    LOG_INFO("LoadPermData: before RegisterHelpers");
    RegisterHelpers();

    permbuffer_ptr.addr = ALIGN(permbuffer_ptr.addr, 0x10);
    PermDataLoaded = 1;
}

void StartPerm(void) {
    permbuffer_ptr.addr = permbuffer_base.addr;
    permbuffer_end.addr = superbuffer_end.addr;
}

void LoadPerm(void) {
    SetBackgroundMusic(1);
    PermDataLoaded = 0;

    WORLDINFO_s *saved_world = WORLD;
    WORLD = NULL;

    // Original branch at 0x1bf351: with background loading enabled and the
    // loader not disabled, run the loading-screen frame loop; otherwise load
    // synchronously.
    if (BGLOAD != 0 && LOADEROFF == 0) {
        // Loading-screen path: kick LoadPermData off on the background thread
        // and run the legal-texture / intro-text frame loop until it finishes.

        pNuCam->mtx = numtx_identity;
        pNuCam->fov = 20.0f;
        pNuCam->aspect = 0.609375f;
        NuCameraSet(pNuCam);

        InitPanel(*((u8 *)&Game + 0xf)); // options byte driving panel setup

        NuQFntSetCoordinateSystem((NUQFNT_CSMODE)3);

        NUMTX scale;
        NUVEC scl = {0.125f, 0.125f, 0.125f};
        NuMtxSetScale(&scale, &scl);

        if (!PAL) {
            LoadPerm_LanguageSelect = 3;
        } else {
            // PAL build shows the language menu first.
            for (i32 i = 0; i < LANGUAGECOUNT; i++) {
                Text_LanguageList[i].language = Text_LanguageList_Default[i].language;
                Text_LanguageList[i].unknown_4 = Text_LanguageList_Default[i].unknown_4;
            }
        }

        // Amazon devices skip the device-language probe (broken locale service).
        if (NuStrICmp(g_deviceManufacturer, (char *)"Amazon") != 0) {
            i32 lang = NuIOS_GetDeviceLanguage();
            for (i32 i = 0; i < LANGUAGECOUNT; i++) {
                if (Text_LanguageList[i].language == lang) {
                    NuLanguageSet(lang);
                    Text_Language = lang;
                    break;
                }
            }
        }

        bgPostRequest(LoadPermData, NULL, NULL, 0);

        f32 t80 = 0.2f; // menu flash timer start
        f32 t84 = 0.0f; // DrawMenu trigger timer
        f32 t88 = 0.0f; // intro text timer
        f32 t8c = 0.0f; // legal texture timer

        while (true) {
            NuFrameBegin();
            NuCameraSet(pNuCam);

            readpads_always = 1;
            ReadPads();
            UpdateGameMenu(&GamePad[0], 0);
            UpdateTimer(&GlobalTimer);
            LOG_WARN("[startup] FRAMETIME=%f t84=%f t8c=%f GlobalTimer=%f", FRAMETIME, t84, t8c,
                     GlobalTimer.time_elapsed);

            menu_flash = (0.1f > NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.2f));

            // Original timer semantics (LoadPerm @0x1bf98f..0x1bfe98):
            //  - before the permanent data arrives, the legal-screen fade-in
            //    timer t8c counts up to 5.80000019 s, but only after a one-second
            //    lead time (t84) and only once fonts/strings are ready;
            //  - once PermDataLoaded is set, the intro text plays over the screen
            //    and its timer t88 drives the exit at 3.8 s.
            const bool intro_ready = Text_IsFontLoaded() && LoadPerm_StringsLoaded != 0 && IntroText_TextID != -1;

            // The legal-screen fade timer runs in both states once fonts/strings
            // are ready; the exit below additionally waits for it to expire.
            if (intro_ready && LoadPerm_LanguageSelect == 3) {
                t84 += FRAMETIME;
                if (t84 >= 1.0f) {
                    t8c += FRAMETIME;
                    if (t8c > 5.80000019f) {
                        t8c = 5.80000019f;
                    }
                }
            }

            if (PermDataLoaded != 0 && intro_ready) {
                if (t88 > 0.3f) {
                    f32 alpha;
                    if (t88 < 0.6f) {
                        alpha = (t88 - 0.3f) / 0.29999995f;
                    } else if (3.3f > t88) {
                        alpha = 1.0f;
                    } else {
                        alpha = 1.0f - (t88 - 3.3f) / 0.29999995f;
                    }

                    if (alpha > 0.0f) {
                        IntroText_Draw(alpha);
                    }
                }
                t88 += FRAMETIME;
                if (t88 >= 3.8f && t8c >= 5.80000019f) {
                    // Original 0x1bfe52: SetBackgroundMusic(-1), final gradient
                    // clear, then leave the loop.
                    SetBackgroundMusic(-1);
                    NuRndrGradClear(0xf00, 0x80000000, 0x80000000, 1.0f);
                    break;
                }
            }

            // One-shot legal material setup from the background-loaded texture.
            LOG_WARN("[startup] legal_tid=%d legal_mtl=%p t8c=%f", legal_tid, legal_mtl, t8c);
            if (legal_tid != 0 && legal_mtl == NULL) {
                LOG_WARN("[startup] creating legal mtl for tid %d", legal_tid);
                NUMTL *mtl = NuMtlCreate3D(1);
                if (mtl != NULL) {
                    mtl->diffuse_color.r = 1.0f;
                    mtl->diffuse_color.g = 1.0f;
                    mtl->diffuse_color.b = 1.0f;
                    mtl->opacity = 1.0f;
                    mtl->shader_desc.flags = 0x1000;
                    mtl->tex_id = (i16)legal_tid;
                    // original 0x127c0a..0x127c3b — attribute byte pokes
                    u8 b1 = *((u8 *)mtl + 0x41);
                    *((u8 *)mtl + 0x41) = (u8)((b1 & 0x0f) | 0x60);
                    u8 b0 = *((u8 *)mtl + 0x40);
                    *((u8 *)mtl + 0x40) = (u8)((b0 & 0xf0) | 0x01);
                    u8 b2 = *((u8 *)mtl + 0x42);
                    *((u8 *)mtl + 0x42) = (u8)((b2 & 0x8c) | 0x12);
                    NuMtlUpdate(mtl);
                    legal_mtl = mtl;
                }
            }

            if (t84 == 0.2f) {
                DrawMenu(0);
            }

            NuRndrBeginScene();
            NuRndrGradClear(0xf00, 0x80000000, 0x80000000, 1.0f);

            // --- Legal screen quad ---
            do {
                if (t8c <= 0.0f || t8c >= 5.8f || legal_mtl == NULL) {
                    break;
                }

                f32 alpha;
                if (0.3f > t8c) {
                    alpha = t8c / 0.3f;
                } else if (5.3f > t8c) {
                    alpha = 1.0f;
                } else if (5.6f <= t8c) {
                    break;
                } else {
                    alpha = 1.0f - (t8c - 5.3f) / 0.30000019f;
                }

                if (alpha <= 0.0f) {
                    break;
                }

                if (t8c < 0.3f) {
                    // Original quirk: multiplied by a negative ramp here,
                    // producing an inverted fade-in. Replicated literally.
                    alpha *= (t8c - 0.3f) / 0.3f;
                }

                u32 colour = ((i32)(alpha * 255.0f)) << 24 | 0x808080;

                f32 aspect = NuIOS_GetAspectRatio();
                f32 hw;
                f32 hh;
                if (aspect > (16.0f / 9.0f)) {
                    hh = 0.5f;
                    hw = aspect * 0.5f / (16.0f / 9.0f);
                } else {
                    hw = 0.5f;
                    hh = aspect * 0.5f / (16.0f / 9.0f);
                }

                NuRndrClear(0xb00, 0, 1.0f);

                NuPrimCSPos++;
                NuPrimSetCoordinateSystem(NUPRIM_SCALEMODE_ABSOLUTE);
                NuPrim2DBegin(4, 7, legal_mtl);

                for (i32 vtx = 0; vtx < 2; vtx++) {
                    f32 u = (vtx == 0) ? 0.0f : 1.0f;
                    f32 v = (vtx == 0) ? 0.0f : 1.0f;

                    u8 *cur = (u8 *)(*g_NuPrim_StreamBufferPtr)->addr;
                    if (!g_NuPrim_NeedsOverbrightening) {
                        *(u32 *)(cur + 0xc) = (colour & 0xff000000) | 0x404040;
                    } else {
                        *(u32 *)(cur + 0xc) = colour;
                    }
                    if (g_NuPrim_NeedsHalfUVs) {
                        *(u16 *)(cur + 0x10) = loadperm_f32_to_f16(u);
                        *(u16 *)(cur + 0x12) = loadperm_f32_to_f16(v);
                    } else {
                        *(f32 *)(cur + 0x10) = u;
                        *(f32 *)(cur + 0x14) = v;
                    }

                    if (vtx == 0) {
                        NuPrim2DAddXYZ(0.5f - hw, 0.5f - hh, 0.0f);
                    } else {
                        NuPrim2DAddXYZ(0.5f + hw, 0.5f + hh, 0.0f);
                    }
                }

                NuPrim2DEnd();

                NuPrimCSPos--;
                NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
            } while (false);

            NuRndrEndScene();
            edGraEnableTerrainSwap();
            FRAMETIME = NuFrameEnd();
            edGraDisableTerrainSwap();

            if (FRAMETIME < DEFAULTFRAMETIME || FRAMETIME > DEFAULTFRAMETIME * 3.0f) {
                FRAMETIME = DEFAULTFRAMETIME;
            }
        }

    } else {
        // No loading screen: load everything synchronously and bail out.
        LoadPerm_LanguageSelect = 3;
        LoadPermData(NULL);

        if (legal_mtl != NULL) {
            NuMtlDestroy(legal_mtl);
            legal_mtl = NULL;
        }
    }

    WORLD = saved_world;
}

void EndPerm(void) {
}
