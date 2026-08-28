// Startup / permanent-data loading.
//
// This TU owns the permanent arena bootstrap and the loading-screen frame
// loop.  It corresponds to the original `startup` module:
//
//   StartPerm  0x470db0  — reset the permanent bump pointer to the super-buffer base.
//   EndPerm    0x470de0  — no-op tail (kept for link compatibility).
//   LoadPerm   0x1bf310  — public entry: chooses the background vs synchronous path.
//   LoadPermData (file-static) 0x1bebd0 — runs on the bg thread (or inline) and
//               actually carves the permanent arena: fonts, strings, music, SFX,
//               level tables, character tables, fades, gizmos, etc.
//
// File-statics `legal_tid` / `loadlegal_done` / `legal_mtl` live here in the
// original as `_ZL9legal_tid` etc., so they stay file-scoped.  Behaviour is
// faithfully preserved (including the PAL language-select spin and the
// deliberately inverted legal fade-in quirk).

#include "legogame/startup.h"

#include <string.h>
#include <time.h>

#include "globals.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/legoapi_types.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"

// ---------------------------------------------------------------------------
// Engine symbols defined in other TUs (declared here to avoid pulling large
// headers).  Linkage matches the defining TU.
// ---------------------------------------------------------------------------

// C++ linkage — defined in nu3d.
NUMTL *NuMtlCreate3D(i32 count);

extern "C" {
    // nucore / nurndr plain stubs and editor helpers (extern "C" TUs).
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

// C++ linkage — defined in their own TUs.
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

// buffer globals owned by the batman TU (batman.h declares them extern "C")
extern "C" {
    extern VARIPTR characterbuffer_ptr;
    extern VARIPTR characterbuffer_base;
    extern VARIPTR characterbuffer_end;
}
void BackDrop_Init(char *path, VARIPTR *buf, VARIPTR *buf_end);
void BackDrop_Draw(float alpha, i32 flags);
void BackDrop_Update(float dt);
void BackDrop_ResetColours();
void BackDrop_UpdateColours(i32 instant);
void TextCrawl_Init(TEXTCRAWL_s *crawl, i32 id, i32 unk);
void TextCrawl_Draw(float dt, i32 paragraphs, float alpha, char *text);
void LoadPerm1(void);
void LoadPerm2(void);
void RegisterHelpers(void);
void InitPanel(i32 panel);
void UpdateGameMenu(GAMEPAD_s *pad, i32 a2);
void SetBackgroundMusic(i32 track);
void UpdateTimer(TIMER *timer);
void IntroText_Draw(f32 alpha);
void ReadPads(void);

// Globals / objects declared elsewhere.
extern i16 id_DEFAULTCHARACTER[2];
extern i16 id_OBIWANKENOBI;
extern GAMEPAD_s GamePad[64]; // gamepads.cpp, bss @0x127a500
extern i32 readpads_always;

extern Fade fade;
extern FadeWipe fadeWipe;
extern FadeStillWipe fadeStillWipe;
extern FadeStill fadeStill;

void *GameBufferAlloc(VARIPTR *buf, VARIPTR *buf_end, i32 size); // gameobjects.cpp
struct CHARFIXUP;
extern CHARFIXUP CharFixUp[222]; // characters.cpp

// LSW gameplay hooks wired by LoadPermData.
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

// ---------------------------------------------------------------------------
// File-local constants and helpers.
// ---------------------------------------------------------------------------

namespace {

    constexpr i32 kLevelHackTableBytes = 0x80;
    constexpr i32 kExtraGizAIMessageCount = 0x40;
    constexpr usize kSmallHeapSize = 0x200;
    constexpr usize kLegalTextureReserve = 0x400000; // carved from top of super buffer
    constexpr f32 kLegalFadeInDuration = 0.3f;
    constexpr f32 kLegalHoldEnd = 5.3f;
    constexpr f32 kLegalFadeOutDuration = 0.30000019f;
    constexpr f32 kLegalVisibleEnd = 5.6f;
    constexpr f32 kLegalTimerMax = 5.80000019f;
    constexpr f32 kIntroFadeInStart = 0.3f;
    constexpr f32 kIntroFadeInDuration = 0.29999995f;
    constexpr f32 kIntroHoldEnd = 3.3f;
    constexpr f32 kIntroFadeOutDuration = 0.29999995f;
    constexpr f32 kIntroDuration = 3.8f;
    constexpr f32 kMenuFlashPeriod = 0.2f;
    constexpr f32 kMenuFlashThreshold = 0.1f;

    // Convert IEEE-754 f32 -> IEEE-754 binary16 (half) for the half-UV vertex
    // format used by `LegalVertex` when `g_NuPrim_NeedsHalfUVs` is set.
    u16 F32ToF16(f32 value) {
        u32 bits;
        memcpy(&bits, &value, sizeof(bits));

        const u32 sign = (bits >> 16) & 0x8000u;
        i32 exp = static_cast<i32>((bits >> 23) & 0xffu) - 127 + 15;
        const u32 mant = bits & 0x7fffffu;

        if (exp >= 0x1f) {
            return static_cast<u16>(sign | 0x7c00u); // inf / overflow -> inf
        }
        if (exp <= 0) {
            return static_cast<u16>(sign); // subnormals flushed to zero
        }
        return static_cast<u16>(sign | (static_cast<u32>(exp) << 10) | (mant >> 13));
    }

} // namespace

// ---------------------------------------------------------------------------
// File-statics — original TU statics (_ZL* in the original ELF).
// ---------------------------------------------------------------------------

static i32 legal_tid;       // texture id of the loaded legal screen
static bool loadlegal_done; // legal texture already attempted
static NUMTL *legal_mtl;    // material wrapping the legal texture

// ---------------------------------------------------------------------------
// Permanent-data initialisation (background thread entry).
// ---------------------------------------------------------------------------

static void LoadPermData(BGPROCINFO *proc) {
    InitMemCard();

    // Font path depends on language.  `Text_Language == 0` is Japanese in the
    // original; everything else uses the Latin font.
    {
        char *font_path =
            (Text_Language == 0) ? (char *)"stuff\\text\\starwars_font_j" : (char *)"stuff\\text\\starwars_font";
        Text_LoadFont(font_path, &permbuffer_ptr, &permbuffer_end);
    }

    // PAL builds show the language menu — the loader spins here until the menu
    // writes `LoadPerm_LanguageSelect = 3` ("selection confirmed").
    while (LoadPerm_LanguageSelect != 3) {
    }

    Text_InitStringTable(0x70d, &permbuffer_ptr, &permbuffer_end);
    Text_InitTable(reinterpret_cast<TEXTENTRY *>(LSW_Text), 0, 0x70c);
    Text_LoadStrings(&permbuffer_ptr, &permbuffer_end);
    Text_InitDefaultStrings();
    LoadPerm_StringsLoaded = 1;

    // Original fills IntroText_TextID from tALONGTIMEAGO (1) via Text_LoadStrings.
    // Host fetches it through the faithful IntroText_SetTextID path.
    if (IntroText_TextID == -1) {
        extern void IntroText_SetTextID(i32);
        // tALONGTIMEAGO == 1 (006a1ec8 B). Use faithful setter so TTab validation runs.
        IntroText_SetTextID(1);
        // If TTab not yet resident (host no OBB), fallback to 1 directly for timer to run
        if (IntroText_TextID == -1)
            IntroText_TextID = 1;
    }

    LOG_INFO("LoadPermData: proc=%p langsel=%d", static_cast<void *>(proc), LoadPerm_LanguageSelect);

    // Legal-screen texture — carved from the top of the super buffer so it
    // never collides with the permanent bump allocator.  Only attempted once.
    if (proc != nullptr && legal_tid == 0 && !loadlegal_done) {
        VARIPTR legal_tex_base;
        legal_tex_base.addr = superbuffer_end.addr - kLegalTextureReserve;

        const char *legal_path = (Text_Language == 2) ? "stuff\\legal\\LEGAL_FRENCH" : "stuff\\legal\\LEGAL_ENGLISH";
        legal_tid = NuTexRead(const_cast<char *>(legal_path), &legal_tex_base, &superbuffer_end);
        loadlegal_done = true;
        LOG_INFO("LoadPermData: legal_tid=%d", legal_tid);
    }

    MechSystems::Get()->LoadPerm();

    NuStringFilterLoad((char *)"stuff\\text\\badwords.txt", &permbuffer_ptr, &permbuffer_end);

    // Audio / rendering permanents.
    MusicInfo = ConfigureMusic((char *)"audio\\music.txt", &permbuffer_ptr, &permbuffer_end);
    RegisterMusic(MusicInfo);
    InitSfx(&permbuffer_ptr, permbuffer_end, "Audio\\Audio.cfg");
    InitStillRender(&permbuffer_ptr, permbuffer_end);

    LevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, kLevelHackTableBytes);
    OldLevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, kLevelHackTableBytes);
    LevelHackSize = 0;

    LevelProgress_ReserveBufferSpace(&permbuffer_ptr, permbuffer_end);

    // Fade system — the four global fade objects are registered once.
    FadeSys.Init();
    FadeSys.AddFade(&fade);
    FadeSys.AddFade(&fadeWipe);
    FadeSys.AddFade(&fadeStillWipe);
    FadeSys.AddFade(&fadeStill);
    pFadeInfo = &FadeSys;

    LevelObjects_InitForGame(reinterpret_cast<LEVELOBJECT *>(ObjTab), &permbuffer_ptr, &permbuffer_end, 0x2ee, 0x1f40);
    LevelSplines_InitForGame(reinterpret_cast<LEVELSPLINE *>(SplTab));

    saveicon_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\ps2_bits.gsc");
    button_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\pc_bits.gsc");

    // ------------------------------------------------------------------
    // Tiny bump allocator embedded in `theMemoryManager`.
    //
    // Original inlines this at the call site — a 0x200-byte bump region
    // carved from the *next* 0x200 bytes of the permanent buffer, with a
    // handful of housekeeping words stored in the global `theMemoryManager`
    // blob (0x248 bytes).  Offsets below match the original layout.
    // ------------------------------------------------------------------
    {
        u8 *mm = theMemoryManager;
        *reinterpret_cast<u8 **>(mm + 0x8) = mm; // self pointer
        *reinterpret_cast<u32 *>(mm + 0x14) = 0;

        const u32 aligned_base = (static_cast<u32>(permbuffer_ptr.addr) + 0xf) & ~0xfu;
        *reinterpret_cast<u32 *>(mm + 0x0) = aligned_base;
        *reinterpret_cast<u32 *>(mm + 0x4) = aligned_base + kSmallHeapSize;
        permbuffer_ptr.addr = aligned_base + kSmallHeapSize;

        *reinterpret_cast<u32 *>(mm + 0x18) = static_cast<u32>(kSmallHeapSize);
        // Stored as u32 truncation of a host pointer — explicit via usize.
        *reinterpret_cast<u32 *>(mm + 0xc) = static_cast<u32>(reinterpret_cast<usize>(mm + 4));
        *reinterpret_cast<u32 *>(mm + 0x10) = 0;
        memset(mm + 0x1c, 0, 0x22c);
    }

    CreateThingManager();
    RegisterGizmoTypes_LSW(&permbuffer_ptr, &permbuffer_end);

    GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
    LSW_registerStatusScreen();
    initGameHintSys_LSW();

    // Wire LSW gameplay hooks.
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

    SetProceduralAnimationFn(reinterpret_cast<void *>(NuAnimBuffProceduralAnimation));

    // Characters, levels, areas, episodes — each carves from the perm buffer.
    CDataList = ConfigureCharacterList((char *)"chars\\chars.txt", &permbuffer_ptr, &permbuffer_end, 0x154, &CHARCOUNT,
                                       0x120, &GCDataList);
    CharScenes_Init(&permbuffer_ptr, &permbuffer_end);
    IconScenes_Init((char *)"stuff\\icons\\", &permbuffer_ptr, &permbuffer_end);
    FixUpCharacters(CharFixUp);
    MiniKits_Init(&permbuffer_ptr, &permbuffer_end);

    PlayerID[0] = id_DEFAULTCHARACTER[0];
    PlayerID[1] = id_OBIWANKENOBI;

    CharCategories_Init(reinterpret_cast<CHARCATEGORY *>(LSW_CharCategory));
    Cheats_Init(reinterpret_cast<CHEAT *>(Cheat));
    CharVariants_Init(reinterpret_cast<CHARVARIANT *>(CharVariants_Game), 0x17);

    LDataList = Levels_ConfigureList((char *)"levels\\levels.txt", &permbuffer_ptr, &permbuffer_end, 0x16d, &LEVELCOUNT,
                                     &Level_SetDefaults);
    FixUpLevels(&LevFixUp);

    ADataList = Areas_ConfigureList((char *)"levels\\areas.txt", &permbuffer_ptr, &permbuffer_end, 0x48, &AREACOUNT);
    FixUpAreas();

    EDataList =
        Episodes_ConfigureList((char *)"levels\\episodes.txt", &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);

    NewGame();
    InitGameAfterConfig();

    APICharacterSysInit(&permbuffer_ptr, permbuffer_end, CHARCOUNT, 0x30, 0xe9, CDataList, 0x400);
    SetActionInfo(ActionInfo, ExtraActionData);

    gizaimessagesys = CreateGizAIMessageSys(&permbuffer_ptr, &permbuffer_end, kExtraGizAIMessageCount);

    LOG_INFO("LoadPermData: before backdrop");
    permbuffer_ptr.addr = ALIGN(permbuffer_ptr.addr, 0x10);
    BackDrop_Init((char *)"stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);

    LOG_INFO("LoadPermData: before LoadPerm1");
    LoadPerm1();
    LOG_INFO("LoadPermData: before LoadPerm2");
    LoadPerm2();
    LOG_INFO("LoadPermData: after LoadPerm2");

    if (theGameThings != nullptr) {
        static_cast<GameThingManager *>(theGameThings)->AddOnceOnlyThings();
    }

    LOG_INFO("LoadPermData: before RegisterHelpers");
    RegisterHelpers();

    permbuffer_ptr.addr = ALIGN(permbuffer_ptr.addr, 0x10);
    PermDataLoaded = 1;
}

// ---------------------------------------------------------------------------
// Public permutations — thin wrappers that set up the arena.
// ---------------------------------------------------------------------------

void StartPerm(void) {
    permbuffer_ptr.addr = permbuffer_base.addr;
    permbuffer_end.addr = superbuffer_end.addr;
}

void LoadPerm(void) {
    SetBackgroundMusic(1);
    PermDataLoaded = 0;

    WORLDINFO_s *saved_world = WORLD;
    WORLD = nullptr;

    // With background loading enabled and not forced off, run the
    // loading-screen path: `LoadPermData` executes on the bg thread while
    // this thread drives the legal-screen / intro-text frame loop.  Otherwise
    // everything is done synchronously.
    // Original branch at 0x1bf351.
    if (BGLOAD != 0 && LOADEROFF == 0) {
        // ---- Loading-screen path ----

        pNuCam->mtx = numtx_identity;
        pNuCam->fov = 20.0f;
        pNuCam->aspect = 0.609375f;
        NuCameraSet(pNuCam);

        // Panel setup driven by the options byte at `Game + 0xf`.
        InitPanel(*reinterpret_cast<u8 *>(&Game + 0xf));

        NuQFntSetCoordinateSystem(static_cast<NUQFNT_CSMODE>(3));

        NUMTX scale;
        NUVEC scl = {0.125f, 0.125f, 0.125f};
        NuMtxSetScale(&scale, &scl);

        if (!PAL) {
            LoadPerm_LanguageSelect = 3; // NTSC: skip the language menu
        } else {
            for (i32 i = 0; i < LANGUAGECOUNT; i++) {
                Text_LanguageList[i].language = Text_LanguageList_Default[i].language;
                Text_LanguageList[i].unknown_4 = Text_LanguageList_Default[i].unknown_4;
            }
        }

        // Amazon devices skip the locale probe — their locale service returns
        // garbage in the original release.
        if (NuStrICmp(g_deviceManufacturer, (char *)"Amazon") != 0) {
            const i32 device_lang = NuIOS_GetDeviceLanguage();
            for (i32 i = 0; i < LANGUAGECOUNT; i++) {
                if (Text_LanguageList[i].language == device_lang) {
                    NuLanguageSet(device_lang);
                    Text_Language = static_cast<u32>(device_lang);
                    break;
                }
            }
        }

        bgPostRequest(LoadPermData, nullptr, nullptr, 0);

        // Timers driving the loading-screen presentation.  Names match the
        // original stack layout for objdiff readability.
        f32 menu_flash_timer = 0.2f; // DrawMenu trigger — deliberately 0.2 so t84 == 0.2 fires once
        f32 intro_gate_timer = 0.0f; // t84 — 1 s lead before legal fade starts
        f32 intro_text_timer = 0.0f; // t88 — intro text alpha & exit
        f32 legal_timer = 0.0f;      // t8c — legal fade in / hold / fade out
        bool crawl_started = false;
        f32 crawl_timer = 0.0f;
        static TEXTCRAWL_s crawlObj{};
        bool crawl_init_done = false;

        while (true) {
            NuFrameBegin();
            NuCameraSet(pNuCam);

            readpads_always = 1;
            ReadPads();
            UpdateGameMenu(&GamePad[0], 0);
            UpdateTimer(&GlobalTimer);

            menu_flash = (kMenuFlashThreshold > NuFmod(GlobalTimer.time_elapsed_mod_seconds, kMenuFlashPeriod));

            // Legal fade timer: only advances once fonts & strings are ready
            // and the language is settled, after a 1 s gate.
            const bool intro_ready = Text_IsFontLoaded() && LoadPerm_StringsLoaded != 0 && IntroText_TextID != -1;

            if (intro_ready && LoadPerm_LanguageSelect == 3) {
                intro_gate_timer += FRAMETIME;
                if (intro_gate_timer >= 1.0f) {
                    legal_timer += FRAMETIME;
                    if (legal_timer > kLegalTimerMax) {
                        legal_timer = kLegalTimerMax;
                    }
                }
            }

            // Intro text — alpha ramps in over 0.3 s, holds, then fades out.
            // Compute alpha outside scene, draw inside scene for proper host prim batching.
            f32 intro_alpha = 0.0f;
            bool intro_will_draw = false;
            if (PermDataLoaded != 0 && intro_ready) {
                if (intro_text_timer > kIntroFadeInStart) {
                    if (intro_text_timer < 0.6f) {
                        intro_alpha = (intro_text_timer - kIntroFadeInStart) / kIntroFadeInDuration;
                    } else if (kIntroHoldEnd > intro_text_timer) {
                        intro_alpha = 1.0f;
                    } else {
                        intro_alpha = 1.0f - (intro_text_timer - kIntroHoldEnd) / kIntroFadeOutDuration;
                    }
                    if (intro_alpha > 0.0f)
                        intro_will_draw = true;
                    // Clamp like original (no explicit clamp but color calc clamps)
                    if (intro_alpha < 0.0f)
                        intro_alpha = 0.0f;
                    if (intro_alpha > 1.0f)
                        intro_alpha = 1.0f;
                }
                intro_text_timer += FRAMETIME;
                if (intro_text_timer >= kIntroDuration && legal_timer >= kLegalTimerMax && !crawl_started) {
                    crawl_started = true;
                    crawl_timer = 0.0f;
                    BackDrop_ResetColours();
                    if (!crawl_init_done) {
                        i32 crawlId = 0x1f0;
                        TextCrawl_Init(&crawlObj, crawlId, 1);
                        crawl_init_done = true;
                    }
                }
            }

            if (crawl_started) {
                crawl_timer += FRAMETIME;
                BackDrop_Update(FRAMETIME);
                BackDrop_UpdateColours(0);
                TextCrawl_Draw(FRAMETIME, 2, 1.0f, nullptr);
                BackDrop_Draw(1.0f, 0);
                const f32 crawl_end = 25.0f;
                if (crawl_timer >= crawl_end) {
                    SetBackgroundMusic(-1);
                    NuRndrGradClear(0xf00, 0x80000000, 0x80000000, 1.0f);
                    break;
                }
            }

            // One-shot legal material creation from the bg-loaded texture.
            if (legal_tid != 0 && legal_mtl == nullptr) {
                NUMTL *mtl = NuMtlCreate3D(1);
                if (mtl != nullptr) {
                    mtl->diffuse_color = {1.0f, 1.0f, 1.0f};
                    mtl->opacity = 1.0f;
                    mtl->shader_desc.flags = 0x1000;
                    mtl->tex_id = static_cast<i16>(legal_tid);

                    // Material attribute flag bytes at mtl+0x40..0x42
                    // (original 0x127c0a..).  Kept as byte pokes until the
                    // attrib struct is fully typed; each preserves the
                    // original mask/or pair.
                    u8 *flags = reinterpret_cast<u8 *>(mtl) + 0x40;
                    flags[1] = (flags[1] & 0x0F) | 0x60;
                    flags[0] = (flags[0] & 0xF0) | 0x01;
                    flags[2] = (flags[2] & 0x8C) | 0x12;

                    NuMtlUpdate(mtl);
                    legal_mtl = mtl;
                }
            }

            if (menu_flash_timer == 0.2f) {
                DrawMenu(0);
            }

            NuRndrBeginScene();
            NuRndrGradClear(0xf00, 0x80000000, 0x80000000, 1.0f);

            // ---- Intro text (blue screen) — faithful: inside scene, after grad clear ----
            if (intro_will_draw) {
                IntroText_Draw(intro_alpha);
            }

            // ---- Legal screen quad ----
            do {
                if (legal_timer <= 0.0f || legal_timer >= 5.8f || legal_mtl == nullptr) {
                    break;
                }

                f32 alpha;
                if (kLegalFadeInDuration > legal_timer) {
                    alpha = legal_timer / kLegalFadeInDuration;
                } else if (kLegalHoldEnd > legal_timer) {
                    alpha = 1.0f;
                } else if (kLegalVisibleEnd <= legal_timer) {
                    break;
                } else {
                    alpha = 1.0f - (legal_timer - kLegalHoldEnd) / kLegalFadeOutDuration;
                }

                if (alpha <= 0.0f) {
                    break;
                }

                if (legal_timer < kLegalFadeInDuration) {
                    // Original quirk: alpha is multiplied by a *negative* ramp
                    // here, producing an inverted fade-in.  Replicated
                    // literally for matching.
                    alpha *= (legal_timer - kLegalFadeInDuration) / kLegalFadeInDuration;
                }

                const u32 colour = (static_cast<u32>(alpha * 255.0f) << 24) | 0x808080u;

                const f32 aspect = NuIOS_GetAspectRatio();
                f32 half_w;
                f32 half_h;
                if (aspect > (16.0f / 9.0f)) {
                    half_h = 0.5f;
                    half_w = aspect * 0.5f / (16.0f / 9.0f);
                } else {
                    half_w = 0.5f;
                    half_h = aspect * 0.5f / (16.0f / 9.0f);
                }

                NuRndrClear(0xb00, 0, 1.0f);

                NuPrimCSPos++;
                NuPrimSetCoordinateSystem(NUPRIM_SCALEMODE_ABSOLUTE);
                NuPrim2DBegin(4, 7, legal_mtl);

                struct LegalVertex {
                    f32 x, y, z;
                    u32 color;
                    union {
                        struct {
                            f32 u, v;
                        } full;
                        struct {
                            u16 u, v;
                            u32 pad; // NOLINT(readability-identifier-naming)
                        } half;
                    };
                };
                static_assert(sizeof(LegalVertex) == 0x18, "LegalVertex is 24 bytes (pos+colour+uv)");

                for (i32 vtx = 0; vtx < 2; vtx++) {
                    const f32 u = (vtx == 0) ? 0.0f : 1.0f;
                    const f32 v = (vtx == 0) ? 0.0f : 1.0f;

                    auto *vert = reinterpret_cast<LegalVertex *>((*g_NuPrim_StreamBufferPtr)->addr);
                    vert->color = g_NuPrim_NeedsOverbrightening ? colour : (colour & 0xff000000u) | 0x404040u;

                    if (g_NuPrim_NeedsHalfUVs) {
                        vert->half.u = F32ToF16(u);
                        vert->half.v = F32ToF16(v);
                    } else {
                        vert->full.u = u;
                        vert->full.v = v;
                    }

                    if (vtx == 0) {
                        NuPrim2DAddXYZ(0.5f - half_w, 0.5f - half_h, 0.0f);
                    } else {
                        NuPrim2DAddXYZ(0.5f + half_w, 0.5f + half_h, 0.0f);
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
        // Synchronous path — no loading screen.
        LoadPerm_LanguageSelect = 3;
        LoadPermData(nullptr);

        if (legal_mtl != nullptr) {
            NuMtlDestroy(legal_mtl);
            legal_mtl = nullptr;
        }
    }

    WORLD = saved_world;
}

// libTTapp.so 0x470de0: close the permbuffer, carve out the character
// buffer, and split the remaining superbuffer in half between the two world
// bump buffers. This is what gives each WORLDINFO its buffer_start /
// giz_buffer / buffer_end cursors that WorldInfo_Reset restores.
void EndPerm(void) {
    u32 aligned = (u32)((permbuffer_ptr.addr + 3) & ~3);
    permbuffer_end.addr = aligned;
    permbuffer_size = (i32)(aligned - permbuffer_base.addr);

    characterbuffer_ptr.addr = aligned;
    characterbuffer_base.addr = aligned;
    u32 world_start = aligned + (u32)CHARACTERBUFFERSIZE;
    characterbuffer_end.addr = world_start;
    superbuffer_ptr.addr = world_start;
    superbuffer_base.addr = world_start;

    u32 half = (u32)(superbuffer_end.addr - world_start) >> 1;
    WorldInfo[0].giz_buffer.addr = world_start;
    WorldInfo[0].buffer_start = (void *)world_start;
    WorldInfo[0].unknown_0108.addr = world_start + half;
    WorldInfo[1].giz_buffer.addr = world_start + half;
    WorldInfo[1].buffer_start = (void *)(world_start + half);
    WorldInfo[1].unknown_0108.addr = superbuffer_end.addr;
    editbuffer_end.addr = superbuffer_end.addr - (u32)EDITBUFFERENDSIZE;
}
