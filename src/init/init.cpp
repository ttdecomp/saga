#include "init/init.hpp"

#include <stdarg.h>

#include "decomp.h"
#include "game/area.h"
#include "game/character.h"
#include "game/cheat.h"
#include "game/collection.h"
#include "game/episode.h"
#include "game/level.h"
#include "game/mission.h"
#include "globals.h"
#include "nu2api.saga/nu3d/nucamera.h"
#include "nu2api.saga/nu3d/nutex.h"
#include "nu2api.saga/nuandroid/ios_graphics.h"
#include "nu2api.saga/nucore/bgproc.h"
#include "nu2api.saga/nucore/nuapi.h"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nucore/nuvideo.h"
#include "nu2api.saga/numusic/numusic.h"
#include "nu2api.saga/nusound/nusound.h"
#include "saveload/saveload.h"

extern "C" {
    void *DVD = NULL;

    struct nupad_s;
    nupad_s *Game_NuPad_Store[2];

    nupad_s **Game_NuPad;
};

u16 MakeSaveHash(void) {
    return Game.completion;
}

i32 drawautosaveicon = 0;

void DrawAutoSaveIcon(void) {
    drawautosaveicon = 1;
    return;
}

char SuperOptions[24] = {0};

void InitGameBeforeConfig() {
    if (PAL == 0) {
        NuStrCpy(prodcode, "BASLUS-21409");
        FRAMETIME = 0.016666668;
    } else {
        NuStrCpy(prodcode, "BESLES-54221");
        FRAMETIME = 0.02;
    }
    DEFAULTFPS = 1.0 / FRAMETIME;
    DEFAULTFRAMETIME = 0.016666668;
    MAXFRAMETIME = 0.1;
    permbuffer_ptr = permbuffer_base;
    permbuffer_end = superbuffer_end;

    saveloadInit(&permbuffer_base, superbuffer_end, 0x7e58, prodcode, iconname, unicodename, 4);
    original_permbuffer_base = permbuffer_base;
    SaveSystemInitialise(3, (void *)MakeSaveHash, &Game, sizeof(GAMESAVE_s), 1, (void *)DrawAutoSaveIcon, SuperOptions,
                         sizeof(SuperOptions));
}

void NuPhoneOSRegisterEventCallback(i32 param_1, void (*param_2)()) {
}

void DummyCallback() {
}

void InitOnce(i32 argc, char **param_2) {
    NuPhoneOSRegisterEventCallback(1, /* TouchCallback */ DummyCallback);
    NuPhoneOSRegisterEventCallback(3, /* SystemPauseCallback */ DummyCallback);
    NuPhoneOSRegisterEventCallback(6, /* SystemDidBecomeActiveCallback */ DummyCallback);

    if (NuIOS_IsLowEndDevice()) {
        SUPERBUFFERSIZE -= 0x38370;
    }

    i32 size = SUPERBUFFERSIZE;

    permbuffer_base.void_ptr = NU_ALLOC(size, 4, 1, "", NUMEMORY_CATEGORY_NONE);
    superbuffer_end.void_ptr = (void *)(SUPERBUFFERSIZE + (usize)permbuffer_base.void_ptr);
    original_permbuffer_base.void_ptr = permbuffer_base.void_ptr;
    InitGameBeforeConfig();

    Game_NuPad = &Game_NuPad_Store[0];

#define SETUP(cmd, ...) cmd, ##__VA_ARGS__

    NuInitHardware(&permbuffer_base, &superbuffer_end, 0,                      //
                   SETUP(NUAPI_SETUP_HOSTFS, 0),                               //
                   SETUP(NUAPI_SETUP_SWAPMODE, NUVIDEO_SWAPMODE_ASYNC),        //
                   SETUP(NUAPI_SETUP_STREAMSIZE, 0x20000),                     //
                   SETUP(NUAPI_SETUP_VIDEOMODE, (PAL == 0) ? 0xdeadbeef : 8),  //
                   SETUP(NUAPI_SETUP_RESOLUTION, 512, (PAL == 0) ? 224 : 256), //
                   SETUP(NUAPI_SETUP_GLASSRPLANE, 1),                          //
                   SETUP(NUAPI_SETUP_CDDVDMODE, &DVD),                         //
                   // (-(uint)(NOSOUND == 0) & 0xffffffee) + 0x1c, 0, 0x640, 0, 0,
                   SETUP(NUAPI_SETUP_PAD0, &Game_NuPad_Store[0]), //
                   SETUP(NUAPI_SETUP_PAD1, &Game_NuPad_Store[1]), //
                   SETUP(NUAPI_SETUP_0x46, 1),                    //
                   SETUP(NUAPI_SETUP_0x47, 1),                    //
                   SETUP(NUAPI_SETUP_0x49, 1),                    //
                   SETUP(NUAPI_SETUP_0x4b, 1),                    //
                   NUAPI_SETUP_END                                //
    );

    pNuCam = NuCameraCreate();
    // Game.optionsSave._11_1_ = NuIOS_IsWidescreen();
    // WidescreenCode((uint)(byte)Game.optionsSave._11_1_);
    // InitPanel((uint)(byte)Game.optionsSave._11_1_);

    // app_tbgameset = NuTimeBarCreateSet(0);
    // app_tbplayerset = NuTimeBarCreateSet(0);
    // app_tbaiset = NuTimeBarCreateSet(0);
    // app_tbdrawset = NuTimeBarCreateSet(0);
}

i32 Episode_ContainsArea(i32 areaId, i32 *areaIndex) {
    for (i32 i = 0; i < EPISODECOUNT; i++) {
        EPISODEDATA *episode = &EDataList[i];

        for (i32 j = 0; j < episode->area_count; j++) {
            i16 id = episode->area_ids[j];
            if (id == areaId) {
                if (areaIndex != NULL) {
                    *areaIndex = j;
                }

                return i;
            }
        }
    }

    if (areaIndex != NULL) {
        *areaIndex = -1;
    }

    return -1;
}

MISSIONSYS *MissionSys = NULL;

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
        AREAFLAGS areaFlags;
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
                    Cheat_SetArea((int)(char)area->cheat, areaId);
                }
            }
            areaId = areaId + 1;
            area = area + 1;
        } while (areaId < AREACOUNT);
    }

    // char cVar1;
    // char cVar2;
    // ushort uVar3;
    // int iVar8;

    // int local_28;
    // int local_24;
    // int local_20[3];

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
                // level->field2_0x60 = tab;
            }
            level->unknown_0af = 0xff;
            level->unknown_0d4 = 0xff;
            if (iVar3 < 1) {
                areaIndex = 0;
            } else {
                areaIndex = 0;
                area = pAVar2;
                do {
                    if (area->field28_0x7d != 0) {
                        i32 iVar4 = 0;
                        do {
                            if (area->field2_0x60[iVar4] == i) {
                                level->unknown_0af = (byte)areaIndex;
                                level->unknown_0d4 = (byte)iVar4;
                                episode = (byte)areaIndex;
                            } else {
                                episode = level->unknown_0af;
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
                        } while (iVar4 < (int)(uint)area->field28_0x7d);
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

    //  Level_RegisterGameConfigKeywords((nufpcomjmp_s *)LevelConfigKeywords_BeforeLoad,
    //  &LevelConfigKeywords_AfterLoad);
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
    //  CutScenes_InitSystem((CUTSCENESYS *)&CutSceneSys_LSW);
    //  NuGCutDebFixUp_SearchAllPages = 1;
    //  NarrowSockExceptions_Init((NARROWSOCKEXCEPTION *)NarrowSockException_LSW);
    //  APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_LSW, "chars\\commonanims");
    //  SurfaceMaskOff(&TERRAINMASK_NONWEAPON);
    //  SurfaceMaskOn(&TERRAINMASK_NONDROID);
    //  Hub_UsePlayerList = 1;
    //  BoltSys_Init((BOLTSYS *)BoltSys_LSW);
    //  GameAudio_Init((GAMEAUDIO *)GameAudio_LSW);
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
    //  SetSoundFadeDistCallBackFn = SetSoundFadeDistCallBackFn_LSW;
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
    //  WorldInfo_InitMenuFn = Game_WorldInfo_InitMenu;
    //  WorldInfo_InitLastFn = Game_WorldInfo_InitLast;
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
    //  LEGOCONTEXT_DROPIN = 0x23;
    //  LEGOCONTEXT_DOOMED = 0x2b;
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
    //  LEGOCONTEXT_WEAPONIN = 6;
    //  LEGOCONTEXT_WEAPONOUT = 7;
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
    //  LEGOMENU_NEWGAME = 1;
    //  LEGOMENU_PAUSEMAIN = 2;
    //  LEGOMENU_PAUSECUT = 0x1b;
    //  LEGOMENU_CREDITS = 0x1e;
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
    //  LEGOSPL_SPLIT = 5;
    //  LEGOSPL_START = 0;
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
    //  GamePads_IgnoreInputFn = Game_IgnoreInput;
    //  Door_GoThrough_ExtraCodeFn = GoThroughDoor_ExtraCode;
    //  GizmoBlowup_TransformDrawFn = GizmoBlowup_TransformDraw_Game;
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
    //  CheckMusicOtherFn = CheckMusicOther;
    //  GizBuildIt_CanStartBuildingFn = GizBuildIt_CanStartBuildingFn_Game;
    //  GizBuildIt_FinishFn = GizBuildIt_FinishFn_Game;
    //  POWERUP_TEXTID = (int)tPOWERUP;
    //  BackDrop_AlphaFn = BackDrop_Alpha;
    //  Game_100PercentFn = LEGO_100PercentFn;
    //  Game_AllGoldBricksFn = LEGO_AllGoldBricksFn;
    //  LastSafePosExtraFn = LastSafePosExtra;
    //  GameAudio_ActionMusicFn = ActionMusicFn;
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
    //      CutScenePlayCount = (uint) * (ushort *)(iVar9 + 8);
    //  }
    //  if (g_lowEndLevelBehaviour != 0) {
    //      Reflections_On = 0;
    //      CharClipToBlobShadows = 1;
    //  }
}

struct LEVELFIXUP {};
LEVELFIXUP LevFixUp;

void FixUpLevels(LEVELFIXUP *fixup) {
    // Levels_FixUp(fixup);

    LEVELDATA *level = Level_FindByName("titles", NULL);
    // TITLES_LDATA = level;
    if (level != NULL) {
        // level->init_fn = Titles_Init;
        // level->update_fn = Titles_Update;
        // level->draw_fn = Titles_Draw;
        // level->flags = level->flags & 0xfffffff5;
        // level->music_index = GetMusicIndex("titles", MusicInfo, -1);
        level->music_index = -1;

        i32 handle = music_man.GetTrackHandle(TRACK_CLASS_QUIET, "titles");
        level->music_tracks[0][0] = handle;
        level->music_tracks[0][1] = handle;

        handle = music_man.GetTrackHandle(TRACK_CLASS_ACTION, "titles");
        level->music_tracks[1][0] = handle;
        level->music_tracks[1][1] = handle;

        handle = music_man.GetTrackHandle(TRACK_CLASS_NOMUSIC, "titles");
        level->music_tracks[2][0] = handle;
        level->music_tracks[2][1] = handle;

        LOG_DEBUG("Titles level track handles: %d, %d, %d, %d, %d, %d", level->music_tracks[0], level->music_tracks[1],
                  level->music_tracks[2], level->music_tracks[3], level->music_tracks[4], level->music_tracks[5]);
    }
}

extern "C" {
    void RegisterMusic(nusound_filename_info_s *files) {
        LOG_DEBUG("files=%p", files);

        g_music = files;
        SFX_MUSIC_COUNT = 0;

        if (files != NULL) {
            for (; files->name != NULL; files = files + 1) {
                NuStrLen(files->name);
                LOG_DEBUG("Registered music file: %s", files->name);
                SFX_MUSIC_COUNT++;
            }
        }
    }

    void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file) {
    }
}

static void LoadPermData(BGPROCINFO *proc) {
    VARIPTR legalTex;
    legalTex.addr = superbuffer_end.addr + -0x400000;

    i32 legal_tid = NuTexRead("stuff\\legal\\LEGAL_ENGLISH", &legalTex, &superbuffer_end);

    MusicInfo = ConfigureMusic("audio\\music.txt", &permbuffer_ptr, &permbuffer_end);
    RegisterMusic(MusicInfo);
    InitSfx(&permbuffer_ptr, permbuffer_end, "Audio\\Audio.cfg");

    CDataList =
        ConfigureCharacterList("chars\\chars.txt", &permbuffer_ptr, &permbuffer_end, 340, &CHARCOUNT, 288, &GCDataList);
    for (int i = 0; i < CHARCOUNT; i++) {
        LOG_INFO("Character %d: %s / %s", i, CDataList[i].dir, CDataList[i].file);
    }
    LOG_INFO("Loaded %d characters", CHARCOUNT);

    LDataList = Levels_ConfigureList("levels\\levels.txt", &permbuffer_ptr, &permbuffer_end, 365, &LEVELCOUNT,
                                     Level_SetDefaults);
    FixUpLevels(&LevFixUp);

    for (int i = 0; i < LEVELCOUNT; i++) {
        LOG_INFO("Level %d: %s / %s", i, LDataList[i].dir, LDataList[i].name);
    }
    LOG_INFO("Loaded %d levels", LEVELCOUNT);

    ADataList = Areas_ConfigureList("levels\\areas.txt", &permbuffer_ptr, &permbuffer_end, 72, &AREACOUNT);
    // FixUpAreas();

    for (int i = 0; i < AREACOUNT; i++) {
        LOG_INFO("Area %d: %s / %s", i, ADataList[i].dir, ADataList[i].file);
    }
    LOG_INFO("Loaded %d areas", AREACOUNT);

    EDataList = Episodes_ConfigureList("levels\\episodes.txt", &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);

    for (int i = 0; i < EPISODECOUNT; i++) {
        LOG_INFO("Episode %d: %hu areas", i, EDataList[i].area_count);
    }
    LOG_INFO("Loaded %d episodes", EPISODECOUNT);

    InitGameAfterConfig();
}

void StartPerm() {
    permbuffer_ptr = permbuffer_base;
    permbuffer_end = superbuffer_end;
}
void LoadPerm() {
    LoadPermData(NULL);
}

void EndPerm() {
}
