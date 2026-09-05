#include "legogame/game.h"

#include <string.h>

#include "MechInputTouch/MechInputTouch_types.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/area.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/episode.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/nufile/nufpar.h"

extern "C" i32 NuIOS_IsSmallScreen(void);
extern "C" void PlaySfxById(i32 sfx_id, nuvec_s *position);
extern "C" void SetAPIObjPlaySfxByIdFn(void (*play_sfx)(i32, nuvec_s *));
extern "C" void SetAnimBlendMode(i32 mode);
f32 GameSetSoundVolume(OPTIONSSAVE_s *);
f32 GameSetMusicVolume(OPTIONSSAVE_s *);
void GameAudio_Reset(void);
void GameRegisterGizActions(void);
void InitPartTable(char **names);
void ResetSeeds(void);
void GizSpinners_InitTerrain(WORLDINFO_s *world);
void Signals_InitTerrain(WORLDINFO_s *world);
void Attractos_InitTerrain(WORLDINFO_s *world);
void SecurityDoors_InitTerrain(WORLDINFO_s *world);
void Levers_InitTerrain(WORLDINFO_s *world);
void GizPanel_InitTerrain(WORLDINFO_s *world);
void HatMachines_InitTerrain(WORLDINFO_s *world);
void GizmoBlowupsFinalSetup(WORLDINFO_s *world);
void GizmoBlowup_TransformDraw_Game(GIZMOBLOWUP_s *blowup);
void InitClimbObjectSys(WORLDINFO_s *world);
void GizmoPushBlockInitAndReset(WORLDINFO_s *world, void *progress);
extern NUFPCOMJMP LevelConfigKeywords_BeforeLoad[];
extern NUFPCOMJMP LevelConfigKeywords_AfterLoad;
extern i32 (*GamePads_IgnoreInputFn)(void);
extern AREADATA *GUNSHIP_ADATA;
extern AREADATA *BONUS_GUNSHIP_ADATA;
extern AREADATA *DOGFIGHT_ADATA;
extern AREADATA *DEATHSTARESCAPE_ADATA;
extern AREADATA *ASTEROIDCHASE_ADATA;
extern AREADATA *JABBASPALACE_ADATA;
extern AREADATA *SPEEDERCHASE_ADATA;
extern AREADATA *DEATHSTARBATTLE2_ADATA;
extern AREADATA *E1CHARACTER_ADATA;
extern AREADATA *E2CHARACTER_ADATA;
extern AREADATA *E3CHARACTER_ADATA;

static i32 SetSoundFadeDistCallBackFn_LSW(WORLDINFO_s *) {
    LEVELDATA *level = WORLD->current_level;
    AREADATA *area = WORLD->area;

    if (level == KAMINOE_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 22.5f;
    } else if (area != NULL && (area == GUNSHIP_ADATA || area == BONUS_GUNSHIP_ADATA)) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 125.0f;
    } else if (area != NULL && area == DOGFIGHT_ADATA) {
        nusound_fade_start = 100.0f;
        nusound_fade_end = 250.0f;
    } else if (level == DEATHSTARRESCUEE_LDATA) {
        nusound_fade_start = 5.0f;
        nusound_fade_end = 50.0f;
    } else if (area != NULL && area == DEATHSTARESCAPE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 15.0f;
    } else if (area != NULL && area == ASTEROIDCHASE_ADATA) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 150.0f;
    } else if (area != NULL && area == JABBASPALACE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 20.0f;
    } else if (level == SARLACCPITA_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 20.0f;
    } else if (level == SARLACCPITC_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 55.0f;
    } else if (area != NULL && area == SPEEDERCHASE_ADATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 50.0f;
    } else if (level == ENDORBATTLEA_LDATA) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 12.0f;
    } else if (level == ENDORBATTLEB_LDATA || level == ENDORBATTLEC_LDATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 45.0f;
    } else if (level == ENDORBATTLED_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 12.0f;
    } else if (area != NULL && area == DEATHSTARBATTLE2_ADATA) {
        nusound_fade_start = 50.0f;
        nusound_fade_end = 200.0f;
    } else if (area != NULL && (area == E1CHARACTER_ADATA || area == E2CHARACTER_ADATA || area == E3CHARACTER_ADATA ||
                                area == BONUSKAMINO_ADATA || area == BONUSDAGOBAH_ADATA)) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 40.0f;
    } else {
        return 0;
    }
    return 1;
}

u16 MakeSaveHash(void) {
    return Game.completion;
}

i32 drawautosaveicon = 0;

void DrawAutoSaveIcon(void) {
    drawautosaveicon = 1;
    return;
}

SUPEROPTIONS_s SuperOptions = {};
static CUTSCENESYS CutSceneSys_LSW = {0x5b, 0x5c, 0xe7, 2};
void CutScenes_InitSystem(CUTSCENESYS *);
void GameAudio_Init(GAMEAUDIO *);

static GAMEAUDIO GameAudio_LSW = {
    NULL,
    NULL,
    {
        "Jp_Ami_Jump",
        NULL,
        "Jp_OQ_Jump",
        "Jp_OQ_DJump",
        "JumpLunge",
        "JumpSlam",
        "Jp_OQ_BackF",
        "FS_JLandS",
        "FS_JLandM",
        "FS_DLandS",
        "FS_DLandM",
        "JediLunge",
        "JediSlam",
        "Grv_GrievAngryJump",
        NULL,
        "FS_JWalkS",
        "FS_JWalkM",
        "FS_DWalkS",
        "FS_DWalkM",
        "FS_WaterWade",
        "Leia_grunt",
        "Rebel_grunt",
        "Leia_Hurt",
        "Luke_Hurt",
        "Explode1",
        "exp_thermalDet",
        "DroidDie01",
        "Leia_Death",
        "Luke_Death",
        "CountdownTimerTick",
        "CountdownTimerTock",
        "SwChar",
        NULL,
        "SwChar",
        "SwChar",
        "ToggleChar",
        "ToggleChar",
        "MK-Loop",
        "MK-Panel",
        "PickupHeart",
        "BlastHit",
        "BlastRic",
        "SabDefBlas",
        "StatusAward",
        "TrueJedi_100pc",
        "WipeScreen",
        "NegC_MagnetCoil",
        "MenuMove",
        "MenuSelect",
        "MenuBack",
        "MenuNoEntry",
        "Explode1",
        "imp_punch",
        "ui_DoubleScoreText",
        "MenuSelect",
        "MenuBack",
        "fly_paddle_rotate_lp",
        "fly_paddle_stuck",
        "MK-Pickup",
        "LegoForm",
        "LegoSingle",
        "SaberOff",
        "SaberOn",
        "SaberSaber01",
        "SaberMove01",
        "SaberSmack01",
        "BlasterHolster",
        "BlasterDraw",
        NULL,
        NULL,
        "FS_WaterOut",
        "Block_Shove",
        "Block_Push_Lp",
        "Blaster",
        "wpn_punch",
        "Honk",
        "WhipSwish",
        "WhipHit",
        "WhipNowt",
        "FallApart01",
        "PowerUp_On",
        "PowerUp_Loop",
        "PowerUp_Off",
        "env_padLight_on",
        "Shop_BuyCheat",
    },
    {},
};

void InitGameBeforeConfig(void) {
    f32 frame_time;
    if (PAL != 0) {
        NuStrCpy(prodcode, "BESLES-54221");
        frame_time = 0.02f;
    } else {
        NuStrCpy(prodcode, "BASLUS-21409");
        frame_time = 0.016666668f;
    }
    FRAMETIME = frame_time;
    DEFAULTFPS = 1.0f / frame_time;
    DEFAULTFRAMETIME = 0.016666668f;
    MAXFRAMETIME = 0.1f;
    permbuffer_ptr = permbuffer_base;
    permbuffer_end = superbuffer_end;

    saveloadInit(&permbuffer_base, superbuffer_end, 0x7e58, prodcode, iconname, unicodename, 4);
    original_permbuffer_base = permbuffer_base;
    SaveSystemInitialise(3, (void *)MakeSaveHash, &Game, sizeof(GAMESAVE_s), 1, DrawAutoSaveIcon, &SuperOptions,
                         sizeof(SuperOptions));

    Game_OptionsSave = &Game.options_save;
    Game_LevelSave = Game.level_save;
    Game_AreaSave = Game.area_save;
    Game_EpisodeSave = Game.episode_save;
    Game_CharacterSave = Game.character_save;
    Game_CompletionSave = &Game.completion;
    Game_MissionSave = &Game.mission_save;
    StatusCollectList.ptr = reinterpret_cast<STATUSCOLLECT_s *>(&Game.completion);

    ResetSeeds();
    ResetTimer(&GlobalTimer, 0.0f);
    ResetTimer(&OverallGamePlayTimer, 0.0f);
    SetAnimBlendMode(2);
    SetAPIObjPlaySfxByIdFn(PlaySfxById);
    NuSetPadDemoEndButtons(GAMEPAD_SKIP);

    // Original option defaults.  OPTIONSSAVE occupies Game[0x4..0x10]; the
    // final brightness byte at 0xc was previously misplaced outside the type.
    const i32 aspect = NuVideoGetAspect();
    Game.options_save.field0_0x0 = 1;
    Game.options_save.field1_0x1 = 1;
    Game.options_save.field2_0x2 = 0;
    Game.options_save.field4_0x4 = 10;
    Game.options_save.field3_0x3 = 10;
    Game.options_save.field7_0x7 = 0;
    Game.options_save.field8_0x8 = 0;
    Game.options_save.field5_0x5 = 10;
    Game.options_save.field6_0x6 = 1;
    const bool standard_aspect = aspect == 0 || aspect == 3;
    Game.options_save.field11_0xb = !standard_aspect;
    Game.options_save.field12_0xc = 10;

    // Persistent touch/control options are initialized before any save data is
    // loaded.  In particular, byte 0x14 is the original music-enable gate.
    SuperOptions.field0_0x0 = -1;
    SuperOptions.field8_0x15 = -1;
    SuperOptions.touch_controls = 1;
    SuperOptions.field2_0x3 = 1;
    if (NuIOS_IsSmallScreen() == 0) {
        SuperOptions.left_control_x = MechInputTouchVirtualConsoleController::s_defaultDPadPosX;
        SuperOptions.left_control_y = MechInputTouchVirtualConsoleController::s_defaultDPadPosY;
        SuperOptions.right_control_x = MechInputTouchVirtualConsoleController::s_defaultButtonsPosX;
        SuperOptions.right_control_y = MechInputTouchVirtualConsoleController::s_defaultButtonsPosY;
    } else {
        SuperOptions.left_control_x = MechInputTouchVirtualConsoleController::s_defaultDPadPosX_SmallScreen;
        SuperOptions.left_control_y = MechInputTouchVirtualConsoleController::s_defaultDPadPosY_SmallScreen;
        SuperOptions.right_control_x = MechInputTouchVirtualConsoleController::s_defaultButtonsPosX_SmallScreen;
        SuperOptions.right_control_y = MechInputTouchVirtualConsoleController::s_defaultButtonsPosY_SmallScreen;
    }
    SuperOptions.music_enabled = 1;

    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    Text_GameSetLanguageFn = Text_SetLanguage_Game;

    if (Text_Language != 13) {
        if (Text_Language != 0 && PAL != 0) {
            Text_SetLanguage(NuLanguageGet());
        }
    }

    GameRegisterGizActions();
    WorldInfo_Reset(&WorldInfo[0], -1);
    WorldInfo_Reset(&WorldInfo[1], -1);
    memset(ZeroRTL, 0, sizeof(ZeroRTL));
    memset(GameCam, 0, sizeof(*GameCam));
    InitPartTable(partdebris_name);
    GameAudio_Reset();
    MENUTEXTSCALE *= 1.2f;
    MENUDY *= 1.5f;
}

void InitGameAfterConfig(void) {
    AREADATA *pAVar5 = ADataList;

    POINTS_PER_CHARACTER = 1;
    POINTS_PER_SUPERBONUSCOMPLETE = 3;
    POINTS_PER_TIMETRIAL = 2;
    POINTS_PER_STORY = 6;
    POINTS_PER_CHALLENGE = 2;
    POINTS_PER_MINIKIT = 2;
    // TopShelf[6].field0_0x0[0] = '\0';
    // TopShelf[6].field0_0x0[1] = '\0';
    // TopShelf[6].field0_0x0[2] = '\0';
    // TopShelf[6].field0_0x0[3] = '\0';

    POINTS_PER_TRUEJEDI = 2;
    POINTS_PER_REDBRICK = 1;
    POINTS_PER_MISSION = 2;
    POINTS_PER_CHEAT = 1;
    POINTS_PER_GOLDBRICK = 1;
    BOTHTRUEJEDIGOLDBRICKS = 0;
    SHOPGOLDBRICKS = 8;
    GOLDBRICKFORSUPERBONUS = 1;
    GOLDBRICKFORSUPERSTORY = 0;
    GOLDBRICKFORCHALLENGE = 1;

    if (EPISODECOUNT > 0) {
        i32 p = (EPISODECOUNT - 1) * POINTS_PER_SUPERSTORY;
        CompletionPointInfo[0] = POINTS_PER_SUPERSTORY + CompletionPointInfo[0] + p;
        COMPLETIONPOINTS = p + POINTS_PER_SUPERSTORY + COMPLETIONPOINTS;
    }

    AREADATA *pAVar10 = ADataList;

    if (0 < AREACOUNT) {
        i32 areaIndex;
        i32 areaId = 0;
        AREADATA *area = ADataList;
        u16 areaFlags;
        do {
            i32 episode = Episode_ContainsArea(areaId, &areaIndex);
            area->episode_index = episode;
            area->area_index = (byte)areaIndex;
            if ((area != HUB_ADATA) && (areaFlags = area->flags, (areaFlags & 0x2022) == 0)) {
                if ((areaFlags & 0x100) == 0) {
                    if ((areaFlags & 4) != 0) {
                        COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_TIMETRIAL;
                        CompletionPointInfo[1] = CompletionPointInfo[1] + POINTS_PER_TIMETRIAL;
                        goto LAB_0012ca5f;
                    }
                    COMPLETIONPOINTS = POINTS_PER_STORY + COMPLETIONPOINTS;
                    CompletionPointInfo[1] = POINTS_PER_STORY + CompletionPointInfo[1];
                    if ((areaFlags & AREAFLAG_NO_GOLDBRICK) == 0) {
                        GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                    }

                    if ((areaFlags & 0x10) == 0) {
                        if ((areaFlags & 0x4000) != 0) {
                            COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_TRUEJEDI;
                            CompletionPointInfo[1] = POINTS_PER_TRUEJEDI + CompletionPointInfo[1];
                            GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                        }
                    } else {
                        i32 local_24 = COMPLETIONPOINTS + POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI;
                        i32 local_28 = POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI + CompletionPointInfo[1];
                        i32 iVar4 = GOLDBRICKPOINTS + 2;
                        if (BOTHTRUEJEDIGOLDBRICKS != 0) {
                            local_24 = local_24 + POINTS_PER_TRUEJEDI;
                            local_28 = local_28 + POINTS_PER_TRUEJEDI;
                            iVar4 = GOLDBRICKPOINTS + 3;
                        }
                        GOLDBRICKPOINTS = iVar4;
                        COMPLETIONPOINTS = POINTS_PER_REDBRICK + local_24 + POINTS_PER_CHALLENGE;
                        CompletionPointInfo[1] = POINTS_PER_REDBRICK + POINTS_PER_CHALLENGE + local_28;
                        if (GOLDBRICKFORCHALLENGE != 0) {
                            GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                        }
                    }
                } else {
                    COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_SUPERBONUSCOMPLETE;
                    CompletionPointInfo[1] = CompletionPointInfo[1] + POINTS_PER_SUPERBONUSCOMPLETE;
                    if (GOLDBRICKFORSUPERBONUS == 0)
                        goto LAB_0012ca68;
                LAB_0012ca5f:
                    GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                }
            LAB_0012ca68:
                if (area->cheat != 0xff) {
                    Cheat_SetArea((i32)(char)area->cheat, areaId);
                }
            }
            areaId = areaId + 1;
            area = area + 1;
        } while (areaId < AREACOUNT);
    }

    // char cVar1;
    // char cVar2;
    // ushort uVar3;
    // i32 iVar8;

    // i32 local_28;
    // i32 local_24;
    // i32 local_20[3];

    // i16 uVar4 = tUNKNOWN;

    AREADATA *pAVar2 = ADataList;
    i32 iVar3 = AREACOUNT;
    i32 areaId = LEVELCOUNT;

    // i16 tab = tUNKNOWN;
    AREADATA *area;
    u8 episode;
    u8 bVar1;

    i32 areaIndex;

    if (0 < LEVELCOUNT) {
        i32 i = 0;
        LEVELDATA *level = LDataList;
        do {
            if (799 < level->unknown_060) {
                // level->levels = tab;
            }
            level->area_index = 0xff;
            level->area_level_index = 0xff;
            if (iVar3 < 1) {
                areaIndex = 0;
            } else {
                areaIndex = 0;
                area = pAVar2;
                do {
                    if (area->level_count != 0) {
                        i32 iVar4 = 0;
                        do {
                            if (area->levels[iVar4] == i) {
                                level->area_index = (byte)areaIndex;
                                level->area_level_index = (byte)iVar4;
                                episode = (byte)areaIndex;
                            } else {
                                episode = level->area_index;
                            }
                            if (episode != 0xff) {
                                areaIndex = areaIndex + 1;
                                bVar1 = pAVar2[(char)episode].episode_index;
                                level->unknown_0ae = bVar1;
                                if ((pAVar2[(char)episode].flags & 1) != 0) {
                                    level->unknown_0d9 = 0x32;
                                    level->unknown_0da = 0x14;
                                }
                                if (((bVar1 != 0xff) && ((pAVar2[(char)episode].flags & 0x106) == 0)) &&
                                    ((level->flags & (LEVEL_STATUS | LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0)) {
                                    level->flags |= 0x2000;
                                }
                                goto LAB_0012cb40;
                            }
                            iVar4 = iVar4 + 1;
                        } while (iVar4 < (i32)(u32)area->level_count);
                    }
                    areaIndex = areaIndex + 1;
                    area = area + 1;
                } while (areaIndex != iVar3);
            }
        LAB_0012cb40:
            i = i + 1;
            level = level + 1;
        } while (i < areaId);
    }

    Level_RegisterGameConfigKeywords(LevelConfigKeywords_BeforeLoad, &LevelConfigKeywords_AfterLoad);
    //  Suits_Init();

    Collection_Configure("chars\\collection.txt", &permbuffer_ptr, &permbuffer_end);
    LOG_INFO("Loaded %d collection items", COLLECTION_COMPLETIONCOUNT);

    CompletionPointInfo[2] = CompletionPointInfo[2] + POINTS_PER_CHARACTER * COLLECTION_COMPLETIONCOUNT;
    COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_CHARACTER * COLLECTION_COMPLETIONCOUNT;

    MissionSys = Missions_Configure("levels\\missions.txt", &permbuffer_ptr, &permbuffer_end, &Game.mission_save);
    if (MissionSys != NULL) {
        areaId = (u32)MissionSys->count * POINTS_PER_MISSION;
        COMPLETIONPOINTS = COMPLETIONPOINTS + areaId;
        CompletionPointInfo[3] = CompletionPointInfo[3] + areaId;
        GOLDBRICKPOINTS = GOLDBRICKPOINTS + (u32)MissionSys->count;
    }

    // Tag_DrawIconFn = Tag_DrawIcon_LSW;
    //_DAT_006312e8 = 0x5d;
    //_DAT_006312ea = 0x5e;
    //_DAT_0063138c = 0x5f;
    //_DAT_0063138e = 0x60;
    //_DAT_00631430 = 0x61;
    //_DAT_00631432 = 0x62;
    //  DEFAULT_PLAYERHITPOINTS = 8;
    //  PLAYERHITPOINTS_2HEARTSIN1 = 1;
    CutScenes_InitSystem(&CutSceneSys_LSW);
    //  NuGCutDebFixUp_SearchAllPages = 1;
    //  NarrowSockExceptions_Init((NARROWSOCKEXCEPTION *)NarrowSockException_LSW);
    //  APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_LSW, "chars\\commonanims");
    //  SurfaceMaskOff(&TERRAINMASK_NONWEAPON);
    //  SurfaceMaskOn(&TERRAINMASK_NONDROID);
    Hub_UsePlayerList = 1;
    //  BoltSys_Init((BOLTSYS *)BoltSys_LSW);
    GameAudio_Init(&GameAudio_LSW);
    //  KITPOSX = 0;
    //  KITPOS2X = 0;
    //  CONVERTOLDPICKUPS = 1;
    //  GrabScreenWhenFading = 1;
    //  troopers_gdeb._0_4_ = 0x83;
    //  troopers_gdeb._4_4_ = 0x49;
    //  troopers_gdeb._8_4_ = 0x4a;
    //  troopers_gdeb._12_4_ = 0x4b;
    //  LEGOHINT_SHOOTCAMERAS = 0x266;
    //  LEGOHINT_PUSHBLOCKS = 0x267;
    //  LEGOHINT_BUILD = 0x25c;
    //  LEGOHINT_FREEPLAYTOGGLE = 600;
    //  PUNCHGAP = 0.3;
    //  PUNCHCHARGAP = 0.3;
    //  f64Jump_AlwaysReachJump2Height = 1;
    //  f64Jump_JediSlam = 1;
    //  CanPunchGirls = 0;
    //  ExtraHurtSfxFn = ExtraHurtSfx_LSW;
    //  ExtraDieSfxFn = ExtraDieSfx_LSW;
    //  BuckStartExtraFn = BuckStartExtra_LSW;
    //  BoltInitSfxFn = BoltInitSfx_LSW;
    //  REDBRICKPOSX = 0.0;
    //  REDBRICKPOSY = -0.5;
    //  REDBRICKPOS2X = 1.25;
    //  REDBRICKPOS2Y = 0.0;
    //  GameObjectDimensionsExtraFn = GameObjectDimensionsExtra_LSW;
    //  Punch_GetDamageFn = Punch_GetDamage_LSW;
    //  Punch_HitHoldFn = Punch_HitHold;
    //  Punch_HitExtraCodeFn = Punch_HitExtraCode_LSW;
    SetSoundFadeDistCallBackFn = SetSoundFadeDistCallBackFn_LSW;
    //  PlayerItemTypes_Init((PLAYERITEMTYPE_s *)PlayerItemType_INDY);
    //  DisguiseAdjustFn = DisguiseAdjust_LSW;
    //  SUPERCARRY_THROWSPEED_XZ = 0x40000000;
    //  SUPERCARRY_THROWSPEED_Y = 0x3fc00000;
    //  SUPERCARRY_RELEASESPEED_XZ = 0x3f800000;
    //  SuperCarry_UseActionButton = 1;
    //  CutScene_OverrideConfigFileNameFn = CutScene_OverrideConfigFileName_LSW;
    //  CharPivot_Init((CHARPIVOT *)CharPivot_LSW);
    //  DIEAIRSPEED = 0x40600000;
    //  DIEAIRJUMPSPEED = 0x40200000;
    //  ForcePush_Waft = 1;
    //  ForcePush_SuperPush = 1;
    //  ForcePush_SuperMindTrick = 1;
    //  Lighting_HighlightFlash = 1;
    //  Lighting_BlueFlickerFn = ObjZappedBlue;
    //  AddGameMsg_Default._56_4_ = GameMsg_EndDelay_Game;
    WorldInfo_InitMenuFn = Game_WorldInfo_InitMenu;
    WorldInfo_InitLastFn = Game_WorldInfo_InitLast;
    //  TerSurface._152_2_ = 0x15;
    //  TerSurface._164_2_ = 0x16;
    //  LEGOACT_IDLE = 1;
    //  LEGOACT_WALK = 0;
    //  LEGOACT_JUMP = 6;
    //  LEGOACT_LAND = 7;
    //  LEGOACT_JUMP2 = 9;
    //  LEGOACT_JUMP3 = 0xe;
    //  LEGOACT_LAND2 = 10;
    //  LEGOACT_LAND3 = 0x15;
    //  LEGOACT_COMBOJUMP = 0x12;
    //  LEGOACT_COMBOLAND = 0x13;
    //  LEGOACT_FALL = 5;
    //  LEGOACT_FALLLAND = 0x59;
    //  LEGOACT_BACKPACKFALLLAND = 0xb3;
    //  LEGOACT_DEACTIVATED = 0x41;
    //  LEGOACT_PUNCH_BEHIND = 0x94;
    //  LEGOACT_SHOOTRIGHT = 0x5b;
    //  LEGOACT_SHOOTLEFT = 0x5a;
    //  LEGOACT_SHOOTBACK = 0x5c;
    //  LEGOACT_LEDGE_GRAB = 0xda;
    //  LEGOACT_LEDGE_IDLE = 0x9d;
    //  LEGOACT_LEDGE_LEFT = 0x9e;
    //  LEGOACT_LEDGE_RIGHT = 0x9f;
    //  LEGOACT_HANG_IDLE = 0x91;
    //  LEGOACT_HANG_MOVE = 0x92;
    //  LEGOACT_CLIMB_IDLE = 0x87;
    //  LEGOACT_CLIMB_UP = 0xcd;
    //  LEGOACT_CLIMB_DOWN = 0xce;
    //  LEGOACT_CLIMB_LEFT = 0xcf;
    //  LEGOACT_CLIMB_RIGHT = 0xd0;
    //  LEGOACT_WALLSHUFFLE_IDLE = 0x8a;
    //  LEGOACT_WALLSHUFFLE_LEFT = 0x8b;
    //  LEGOACT_WALLSHUFFLE_RIGHT = 0x8c;
    //  LEGOACT_PUSH = 0x52;
    //  LEGOACT_SUPERPUSH_IDLE = 0xae;
    //  LEGOACT_SUPERPUSH_PUSH = 0xaf;
    //  LEGOACT_SUPERPUSH_PULL = 0xb0;
    //  LEGOACT_BUILD = 0x5f;
    //  LEGOACT_FLIP = 0xc;
    //  LEGOACT_FLIPLAND = 0xd;
    //  LEGOACT_BACKFLIP = 0x77;
    //  LEGOACT_WALLJUMP_WAIT = 0xa1;
    //  LEGOACT_GRAPPLE_IDLE = 0xba;
    //  LEGOACT_GRAPPLE_UP = 0xbb;
    //  LEGOACT_GRAPPLE_DOWN = 0xbc;
    //  LEGOACT_GRAPPLE_HANG = 0xbd;
    //  LEGOACT_GLIDE = 0x93;
    //  LEGOACT_SUPERCARRY_PICKUP = 0xa3;
    //  LEGOACT_SUPERCARRY_IDLE = 0xa4;
    //  LEGOACT_SUPERCARRY_WALK = 0xa5;
    //  LEGOACT_SUPERCARRY_THROW = 0xa6;
    //  LEGOACT_SUPERCARRY_PUTDOWN = 0xd5;
    //  LEGOACT_SUPERCARRY_BASH = 0xd6;
    //  LEGOACT_SUPERCARRY_JUMP = 0xd7;
    //  LEGOACT_SUPERCARRY_LAND = 0xd8;
    //  LEGOACT_SUPERCARRY_FALLLAND = 0xd9;
    //  LEGOACT_MAGNET_WALK_METAL = 0xc5;
    //  LEGOACT_MAGNET_TIPTOE = 0xc6;
    //  LEGOACT_MAGNET_WALK = 199;
    //  LEGOACT_MAGNET_RUN = 200;
    //  LEGOACT_MAGNET_JUMP = 0xc9;
    //  LEGOACT_MAGNET_LAND = 0xca;
    //  LEGOACT_LUNGE = 0x1f;
    //  LEGOACT_LUNGELAND = 0x20;
    //  LEGOACT_SLAM = 0x21;
    //  LEGOACT_SLAMLAND = 0x22;
    //  LEGOACT_COMBATROLL_JUMP = 0xb4;
    //  LEGOACT_COMBATROLL_FALL = 0xb5;
    //  LEGOACT_COMBATROLL_LAND = 0xb6;
    //  LEGOACT_COMBATROLL_FIRE = 0x57;
    //  LEGOACT_EXTRA_JUMP = 0x78;
    //  LEGOACT_EXTRA_JUMP2 = 0x79;
    //  LEGOACT_EXTRA_LAND = 0x7a;
    //  LEGOACT_EXTRA_LAND2 = 0x7b;
    //  LEGOACT_BUCK = 0x2a;
    //  LEGOACT_TEETER = 0xdb;
    //  LEGOACT_WHIP_START = 0xdc;
    //  LEGOACT_WHIP_CRACK = 0xdd;
    //  LEGOACT_WHIP_GRAB = 0xdd;
    //  LEGOACT_WHIP_BREAK = 0xdd;
    //  LEGOACT_WHIP_SWING_START = 0xe0;
    //  LEGOACT_WHIP_SWING_SWING = 0xe1;
    //  LEGOACT_WHIP_SWING_JUMP = 0xe2;
    //  LEGOCONTEXT_TUBE = 0x11;
    //  LEGOCONTEXT_GLIDE = 0x4f;
    //  LEGOCONTEXT_BLOCK = 0xc;
    //  LEGOCONTEXT_HOLD = 0x18;
    LEGOCONTEXT_DROPIN = 0x23;
    LEGOCONTEXT_DOOMED = 0x2b;
    //  LEGOCONTEXT_LEDGETERRAIN = 0x55;
    //  LEGOCONTEXT_CLIMB = 0x43;
    //  LEGOCONTEXT_HANG = 0x4e;
    //  LEGOCONTEXT_JUMP = 0;
    //  LEGOCONTEXT_BIGJUMP = 0x1f;
    //  LEGOCONTEXT_WALLSHUFFLE = 0x45;
    //  LEGOCONTEXT_COMBO = 5;
    //  LEGOCONTEXT_PUNCH = 0x26;
    //  LEGOCONTEXT_PUSH = 0x27;
    //  LEGOCONTEXT_PUSHSPINNER = 0x28;
    //  LEGOCONTEXT_PUSHOBSTACLE = 0x59;
    //  LEGOCONTEXT_BEENTAKENOVER = 0x3b;
    //  LEGOCONTEXT_GETIN = 0x3c;
    //  LEGOCONTEXT_BUILDIT = 0x2d;
    LEGOCONTEXT_WEAPONIN = 6;
    LEGOCONTEXT_WEAPONOUT = 7;
    //  LEGOCONTEXT_BACKFLIP = 0x20;
    //  LEGOCONTEXT_WALLJUMPWAIT = 0x57;
    //  LEGOCONTEXT_GRAPPLE = 0x46;
    //  LEGOCONTEXT_SUPERCARRY = 0x58;
    //  LEGOCONTEXT_LAND_JUMP = 1;
    //  LEGOCONTEXT_LAND_JUMP2 = 2;
    //  LEGOCONTEXT_LAND_FLIP = 3;
    //  LEGOCONTEXT_LAND_COMBOJUMP = 4;
    //  LEGOCONTEXT_LAND_LUNGE = 0xd;
    //  LEGOCONTEXT_LAND_SLAM = 0xe;
    //  LEGOCONTEXT_LAND_SPECIAL = 0x19;
    //  LEGOCONTEXT_LAND_COMBATROLL = 0x29;
    //  LEGOCONTEXT_EATEN = 0x39;
    //  LEGOCONTEXT_SPECIALMOVE_ATTACKER = 0x26;
    //  LEGOCONTEXT_SPECIALMOVE_VICTIM = 0x30;
    //  LEGOCONTEXT_BUCK = 0x3e;
    //  LEGOCONTEXT_WHIP = 0x62;
    //  LEGOCONTEXT_NETWAIT = 99;
    //  LEGOMENU_TITLES = 0;
    LEGOMENU_NEWGAME = LEGO_MENU_NEW_GAME;
    LEGOMENU_PAUSEMAIN = LEGO_MENU_PAUSE_MAIN;
    LEGOMENU_PAUSECUT = LEGO_MENU_PAUSE_CUTSCENE;
    LEGOMENU_CREDITS = LEGO_MENU_CREDITS;
    //  LEGOMENU_STORE_RESTORING = 0x16;
    //  LEGOMENU_STORE_PURCHASE = 0x17;
    //  LEGOMENU_RESTORE_NEWGAME = 0x1d;
    //  LEGOCAMMODE_DOORCUT = 4;
    //  LEGOCAMMODE_OBSTACLE = 2;
    //  LEGOOBJ_ICON_FRAME_NEUTRAL = 0xa7;
    //  LEGOOBJ_ICON_FRAME_GREEN = 0xa5;
    //  LEGOOBJ_ICON_FRAME_BLUE = 0xa6;
    //  LEGOOBJ_ICON_QUESTION = 0xa1;
    //  LEGOOBJ_ICON_WEIRDO = 0xa3;
    //  LEGOOBJ_CHARKIT = 0xcf;
    //  LEGOOBJ_MINIKIT = 0xce;
    LEGOSPL_SPLIT = 5;
    LEGOSPL_START = 0;
    //  LEGOGDEB_SPLASH = 0xd;
    //  GizBuilditGDeb._0_2_ = 0x4c;
    //  GizBuilditGDeb._2_2_ = 0x4d;
    //  GizBuilditGDeb._4_2_ = 0x4e;
    //  GizBuilditGDeb._6_2_ = 0x4f;
    //  GizBuilditGDeb._8_2_ = 0x50;
    //  GizBuilditGDeb._10_2_ = 0x51;
    //  GizSpinnerGDeb_Fail._0_2_ = 0x57;
    //  GizSpinnerGDeb_Fail._2_2_ = 1;
    //  GizSpinnerGDeb_Fail._4_2_ = 0x58;
    GamePads_IgnoreInputFn = Game_IgnoreInput;
    //  Door_GoThrough_ExtraCodeFn = GoThroughDoor_ExtraCode;
    GizmoBlowup_TransformDrawFn = GizmoBlowup_TransformDraw_Game;
    //  LEGOASCII_UP = ASCII_UP;
    //  LEGOASCII_DOWN = ASCII_DOWN;
    //  LEGOASCII_LEFT = ASCII_LEFT;
    //  LEGOASCII_RIGHT = ASCII_RIGHT;
    //  LEGOASCII_BIGARROW = ASCII_BIGARROW;
    //  KEEPONSCREEN_SIDESONLY = 1;
    //  CutScenePlayer_AcceptFn = CutScenePlayer_Accept;
    //  CutScenePlayer_Configure("cut\\clips.txt", &permbuffer_ptr, &permbuffer_end, &tCLIPi, &tINTRO, &tMIDTRO,
    //  &tOUTRO,
    //                           &tENDING);
    //  CanMagnetClimbFn = CanMagnetClimb_Game;
    //  CanPushObstaclesFn = CanPushObstacles_Game;
    //  CanSuperCarryFn = CanSuperCarry_Game;
    //  CanPushBlocksFn = CanPushBlocks_Game;
    //  CanGlideFn = CanGlide_Game;
    //  UsingExtraActionsFn = UsingExtraActions_Game;
    //  CanStartHoldFn = CanStartHold_Game;
    //  Player_ClearContextFn = Player_ClearContext_Game;
    //  LEGOTHINGSSCENE_TER_SPINBASE = 0;
    //  LEGOTHINGSSCENE_TER_SPINARM = 1;
    //  GizBuildit_AutoBuildPosFn = GizBuildit_AutoBuildPos_Game;
    //  Fighting_WeaponInActionFn = Fighting_WeaponInAction_Game;
    //  Fighting_WeaponOutActionFn = Fighting_WeaponOutAction_Game;
    //  LEGOOBJ_FLOORTARGET = 0x55;
    //  LEGOOBJ_GRAPPLE_HOOK = 0x3d;
    //  Slam_GetDebrisFn = Slam_GetDebris_Game;
    //  IsWearingBackPackFn = IsWearingBackPack_Game;
    //  Grass_Available = 1;
    //  PauseGame_ExtraCodeFn = PauseGame_ExtraCode;
    //  Hub_PanelBusyFn = Hub_PanelBusy;
    CheckMusicOtherFn = CheckMusicOther;
    //  GizBuildIt_CanStartBuildingFn = GizBuildIt_CanStartBuildingFn_Game;
    //  GizBuildIt_FinishFn = GizBuildIt_FinishFn_Game;
    //  POWERUP_TEXTID = (i32)tPOWERUP;
    //  BackDrop_AlphaFn = BackDrop_Alpha;
    //  Game_100PercentFn = LEGO_100PercentFn;
    //  Game_AllGoldBricksFn = LEGO_AllGoldBricksFn;
    //  LastSafePosExtraFn = LastSafePosExtra;
    GameAudio_ActionMusicFn = ActionMusicFn;
    //  PauseRenderOffFn = PauseRenderOff;
    //  FindSlamOrigin_UseCPosFn = FindSlamOrigin_UseCPos;
    //  GizmoBlowUp_NoTargetFn = GizmoBlowUp_NoTarget;
    //  MatrixReflection_CanOverrideFn = MatrixReflection_CanOverride;
    //  Jump_PreventJumpFn = Jump_PreventJump;
    //  SurfaceInfo_ExtraReflectFn = SurfaceInfo_ExtraReflect;
    //  Hint_AlphaTargetFn = Hint_AlphaTarget;
    //  Arcade_TextCrawlID = 0x1f1;
    //  Arcade_TextCrawlParagraphs = 2;
    //  GizmoPickups_Collide2DFn = GizmoPickups_Collide2D;
    //  LEGOOBJ_DEFAULTLASTCOIN = 0xb7;
    //  Tag_NoHiddenIconFn = Tag_NoHiddenIcon;
    //  Collection_GetSelectingPlayerIDsFn = Collection_GetSelectingPlayerIDs;
    //  GizmoBlowUp_SfxFn = GizmoBlowUp_Sfx;
    //  APIObjResetShadowMapRenderingFn = ResetShadowMapRenderingFn;
    //  APIObjEnableShadowMapRenderingFn = EnableShadowMapRenderingFn;
    //  GameMsg_GetExtraObjFn = GameMsg_GetExtraObj;
    //  Jump_EndOfLandContextFn = Jump_EndOfLandContext;
    //  BigJump_EndOfLandFn = BigJump_EndOfLand;

    COMPLETIONPOINTS = POINTS_PER_CHEAT * 0x2c + COMPLETIONPOINTS;
    CompletionPointInfo[4] = POINTS_PER_CHEAT * 0x2c + CompletionPointInfo[4];
    if (0 < SHOPGOLDBRICKS) {
        areaId = (SHOPGOLDBRICKS + -1) * POINTS_PER_GOLDBRICK;
        CompletionPointInfo[6] = CompletionPointInfo[6] + POINTS_PER_GOLDBRICK + areaId;
        COMPLETIONPOINTS = POINTS_PER_GOLDBRICK + COMPLETIONPOINTS + areaId;
        GOLDBRICKPOINTS = SHOPGOLDBRICKS + -1 + GOLDBRICKPOINTS + 1;
    }

    //  iVar9 = CutScenePlayer_Available();
    //  CutScenePlayCount = 0;
    //  if (iVar9 != 0) {
    //      CutScenePlayCount = (u32) * (ushort *)(iVar9 + 8);
    //  }
    //  if (g_lowEndLevelBehaviour != 0) {
    //      Reflections_On = 0;
    //      CharClipToBlobShadows = 1;
    //  }
}

void CompleteLevel(WORLDINFO *world) {
    grab_screen_image = 1;

    if (NewLData == NULL || ((NewLData->flags & 0xe0) != 0 && FreePlay != 0)) {
        LEVELDATA *statusLevel = Area_FindStatusLevel(world->area, NULL);
        NewLData = statusLevel;
        if (statusLevel == NULL) {
            NewLData = HUB_LDATA;
        }
    }

    if (waiting_for_level != -1) {
        waiting_for_new_level = 1;
    }
}

void StoreLevelProgress(WORLDINFO *world) {
    if (world != NULL) {
        // StoreLevelProgressFn(world, world->field277_0x130, 0);
    }
}

void NewAreaMusicChanges(void) {
    BeenAttacked = 0;
}

void Game_WorldInfo_InitLast(WORLDINFO *world) {
    GizSpinners_InitTerrain(world);
    Signals_InitTerrain(world);
    Attractos_InitTerrain(world);
    SecurityDoors_InitTerrain(world);
    Levers_InitTerrain(world);
    GizPanel_InitTerrain(world);
    HatMachines_InitTerrain(world);
    GizmoBlowupsFinalSetup(world);
    InitClimbObjectSys(world);
    GizmoPushBlockInitAndReset(world, NULL);
}
