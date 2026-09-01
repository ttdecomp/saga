#include "decomp.h"

#include "batman.h"
#include "globals.h"
#include "legoapi/render/fx.h"
#include "nu2api/nusound/nusound_android.hpp"

#include <string.h>

// Local statics owned by this TU (original symbols _ZL8frameout,
// _ZL14frameout_count, _ZL19NuSoundAppTerminatev).
static i32 frameout;
static i32 frameout_count[2];

// Original local static of this TU (calls NuSoundSystem::Shutdown).
namespace {
    void nuSoundAppTerminate(void);
}
// NuSoundSystem::Shutdown() is the original callback target
static void NuSoundAppTerminate(void);

char uberShader2[] = {
#include <uberShader2.array>
};

// Trailer videos played after the intro.
char *Trailer[3] = {"demointro", "", NULL};

SAGA_NOMATCH __attribute__((weak)) i32 main(i32 argc, char **argv) {
    UNIMPLEMENTED();
    return 0;
}

extern "C" i32 NuMain(i32 argc, char **argv) {
    static i32 frameCount = 0;
    static f32 pastFrameTimes[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};

    GAMEPAD_s *gamepad;
    WORLDINFO_s *world;
    GAMECAMERA_s *gameCam;
    AREADATA *afterArea;
    nucamera_s *cam;
    char c;
    GIZAIMESSAGESYS_s *messageSystem;
    GIZAIMESSAGE_s *gizMessage;
    GIZAIMESSAGE_s *missionModeMessage;
    nupad_s *rumblePad0;
    nupad_s *rumblePad1;
    u32 pauseFlag;
    LEVELDATA_s *level;
    i32 currentEpisodeIndex;
    i32 previousEpisodeIndex;
    i32 i;
    i32 averagedFrameCount;
    i32 longestFrameIndex;
    i32 shortestFrameIndex;
    byte panelOpts;
    f32 frameTimeAccumulator;
    f32 shortestFrameTime;
    f32 savedFrametime;
    u32 pausedRender;
    FADETYPE fadeType;
    nupad_s *pads[2];
    ThingProcessData framePacket;
    NUVEC *windObjs[8];

    NuSoundAppTerminateCallback = NuSoundAppTerminate;
    NuCommandLine(&argc, &argv);
    NuIOS_RecordFlurryEvent("app_open");
    WorldInfo_InitOnce();
    InitOnce(argc, argv);
    TriggerExtraDataLoad();
    MenuInitialise(GameMenuInfo, LEGO_MENU_ID_COUNT, LANGUAGECOUNT, DrawSaveSlots, 1, 0);
    MenuReset();
    edGraInitTerrainSwapProtection();
    NuHGobjReversibleCharacters(1);
    NuHGobjForceShadowsOnCharacters(1);
    GameAISysInit();
    APITransparentInit();
    NuSound3SetLoopHoldTime(0.5f);
    InitShadowLights();
    GamePads_Init();
    MechSystems::Get()->EnterLevel((WORLDINFO_s *)0);
    bgProcInit();
    FinishLoop(2);
    Text_InitLanguageList(Game_LanguageList);
    Text_SetLanguage(-1);
    StartPerm();
    LoadPerm();
    EndPerm();

    if (GAMEDEMO != 0) {
        world = WORLD;
        WORLD = NULL;
        PLAYERCOUNT = 0;
        Movie_Play(Trailer[0], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL,
                   (float)Game.options_save.field4_0x4 / 10.0f);
        FinishLoop(2);
        WORLD = world;
    }

    if (openlevels != 0) {
        Areas_OpenAll(0);
    }

    if (Level == -1) {
        Level = 0;
        i = 0;
    } else {
        i = Level * sizeof(LEVELDATA_s);
    }
    last_area = -1;
    Area = reinterpret_cast<LEVELDATA_s *>(reinterpret_cast<char *>(LDataList) + i)->area_index;
    LastAData = NULL;

    if ((Area == -1) || ((ADataList[Area].flags & AREAFLAG_BONUS_AREA) == 0)) {
        if ((1 < GAMEDEMO) || ((GAMEDEMO != 0) && (FreePlay != 0))) {
            FreePlay = 1;
            GAMEDEMO = 2;
        }
    } else {
        NextArea_FreePlay = 1;
        FreePlay = 1;
    }

    makefreeplaymodellist = 0;
    if ((HUB_ADATA == NULL) || (Area != (u32)(byte)HUB_ADATA->index)) {
        if (FreePlay != 0) {
            FreePlay = 0;
            NextArea_FreePlay = 0;
        }
    } else {
        Hub_MakeModelList();
    }

    NuFrameBegin();
    edGraEnableTerrainSwap();
    NuFrameEnd();
    edGraDisableTerrainSwap();

restart_level:
    g_introState = 0;
    LSW_SetIndy(Area);

    if ((((last_area != -1) && (LastLData != NULL)) && ((LastLData->flags & LEVEL_STATUS) != 0)) &&
        ((ADataList[Area].flags != AREAFLAG_ENDING_AREA) && (HUB_ADATA != NULL) &&
         (Area != (u32)(byte)HUB_ADATA->index))) {
        FinishLoop(2);
    }

    GameAISysSetGame();
    GizmoSysSetGame();

    currentEpisodeIndex = -1;
    highallocaddr = 0;
    if (Area != -1) {
        currentEpisodeIndex = static_cast<i8>(ADataList[Area].episode_index);
    }
    previousEpisodeIndex = -1;
    if (last_area != -1) {
        previousEpisodeIndex = static_cast<i8>(ADataList[last_area].episode_index);
    }

    if (Area == -1) {
        SuperStory = 0;
    } else {
        if (currentEpisodeIndex == -1) {
            SuperStory = 0;
        } else if (currentEpisodeIndex != previousEpisodeIndex) {
            if (previousEpisodeIndex != -1) {
                SuperStory = 0;
            }
        }
        if (Area != last_area) {
            messageSystem = NULL;
            if (gizaimessagesys != NULL) {
                ResetGizAIMessageSys(gizaimessagesys);
                messageSystem = gizaimessagesys;
            }
            goto giz_freeplay;
        }
    }

    messageSystem = gizaimessagesys;
    // A reset invalidates the local view even when no message system exists.
    if ((reset_area != 0) && (messageSystem = NULL, gizaimessagesys != NULL)) {
        ClearGizAIMessageSys(gizaimessagesys);
        messageSystem = gizaimessagesys;
    }

giz_freeplay:
    gizMessage = CheckGizAIMessage(messageSystem, "FreePlay", NULL);
    if (gizMessage != NULL) {
        gizMessage->output_values[0] = 1;
        gizMessage->output_values[1] = 0;
        gizMessage->flags |= GIZAIMESSAGE_FLAG_ADD_GIZMO;
        gizMessage->value = (f32)FreePlay;
        gizMessage->output_count = 2;
    }

    missionModeMessage = CheckGizAIMessage(gizaimessagesys, "MissionMode", NULL);
    if (missionModeMessage != NULL) {
        if ((MissionSys != NULL) && (MissionSys->field8_0x1d != 0)) {
            missionModeMessage->value = 1.0f;
        }
        missionModeMessage->flags |= GIZAIMESSAGE_FLAG_ADD_GIZMO;
        missionModeMessage->output_values[0] = 1;
        missionModeMessage->output_values[1] = 0;
        missionModeMessage->output_count = 2;
    }

    gizMessage = CheckGizAIMessage(gizaimessagesys, "ChallengeMode", NULL);
    if (gizMessage != NULL) {
        if (ChallengeMode != 0) {
            gizMessage->value = 1.0f;
        }
        gizMessage->flags |= GIZAIMESSAGE_FLAG_ADD_GIZMO;
        gizMessage->output_values[0] = 1;
        gizMessage->output_values[1] = 0;
        gizMessage->output_count = 2;
    }

    if (((Area == -1) || (static_cast<i8>(ADataList[Area].episode_index) == -1)) ||
        ((ADataList[Area].flags & (AREAFLAG_MINIKIT | AREAFLAG_BONUS_AREA)) != AREAFLAG_MINIKIT)) {
        ChallengeMode = 0;
        Mission_Clear(NULL);
    }

    ResetCharacterBuffer(0);
    memset(Player, 0, sizeof(Player));

    if ((PlayTrailer != -1) && (characterbuffer_ptr.void_ptr == characterbuffer_base.void_ptr)) {
        Movie_Play(Trailer[PlayTrailer * 3], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL,
                   (float)Game.options_save.field4_0x4 / 10.0f);
    }
    PlayTrailer = -1;

    LoadAreaCharacters();
    FixUpLayers();
    if (loadareacharacters_loadedlevel == 0) {
        WorldInfo_ReArrangeBuffers(Area, last_area);
    }

    if ((level_already_loaded == -1) || (level_already_loaded != Level)) {
        next_level = Level;
        abort_load = 0;
        WorldInfo_StreamLevel(NULL);
    }
    level_already_loaded = -1;

    ClearLevData();
    dagobah_training = 0;
    party_cant_be_under_cover = 0;
    WorldInfo_Activate();
    FindGameCutScenes();
    DebrisGlassInit();
    InitCables(WORLD);
    InitSnakes(WORLD);
    ResetOldFStop = 1;
    music_man.SetFader(1.0f, 0.0f);
    new_level_from_menu = 0;
    NuSound3SetReverb(0);

    if (((Area != -1) && (Area != last_area)) || (reset_area != 0)) {
        NewArea();
    }

    GamePlayMusic(WORLD->current_level, 0, &Game.options_save);
    if ((WORLD->current_level == TITLES_LDATA) && (SuperOptions.music_enabled == 0)) {
        legoSetMusicVolume(0.0f);
    }

    reset_area = 0;
    reset_restart = 1;
    no_more_loads = 0;
    ResetTimer(&BonusTimer, 0.0f);
    BonusWinner = -1;
    BonusCoinTotal = 0;
    ResetFrameCounters();
    if (NewMode == 0) {
        reset_load = 1;
    }

    do {
        Particles_Start(WORLD);
        if (WORLD->page_grass != -1) {
            edgraStartPage(WORLD->page_grass);
        }
        Parts_Start(WORLD);

        if (((NOSOUND == 0) && (ClearPause(), NOSOUND == 0)) && (FadeSys.pending_type == FADE_TYPE_NONE)) {
            fadeType.type = FADE_TYPE_STILL_WIPE;
            FadeSys.SetFade(fadeType, 0);
        }
        FadeSys.SetStage(1);

        MainRenderTargetTime = 1.0f;
        MainRenderTime = 1.0f;
        CheckResetBits();
        music_man.SetFader(1.0f, 0.0f);
        GamePlayMusic(WORLD->current_level, 1, &Game.options_save);
        if ((WORLD->current_level == TITLES_LDATA) && (SuperOptions.music_enabled == 0)) {
            legoSetMusicVolume(0.0f);
        }

        if (newlevel_resumecutaudio != 0) {
            RestoreGameCut();
            music_man.ResumeTrack(0x10);
            newlevel_resumecutaudio = 0;
        }

        NuIOS_WaitForRenderThreadCompletion();
        NuIOS_SetRenderComplete();
        NuRndrSwapStreamBuffers();

        frameout = 0;
        frameout_count[0] = 0;
        frameout_count[1] = 0;
        display_list_buffer = reinterpret_cast<VARIPTR *>(&rndrstream_free);
        display_list_buffer_end = reinterpret_cast<VARIPTR *>(rndrstream_end.addr);
        FRAMETIME = DEFAULTFRAMETIME;

        if (reset_load != 0) {
            other_level = -1;
            waiting_for_level = -1;
            gone_through_door_to_new_level = 0;
            waiting_for_new_level = 0;
        }
        reset_load = 1;
        NuVideoRollingFrameRateReset();

        while ((((NewMode == 0) && (NewLData == NULL)) || (FadeSys.fade < 1.0f) || (pFadeInfo->busy != 0)) ||
               ((((waiting_for_level != -1) && ((reset_load != 0) || (abort_load != 0))) ||
                 (waiting_for_character != -1)) &&
                (NewMode == 0))) {
            CutScenes_BGLoadManager();
            NuVideoGetSwapMode();
            NuTimeBarSlotReset(0, 0xf);
            _NuTimeBarSlotBegin(0, 0xf, "frmtmr");
            DebrisSetTimeIncrement(FRAMETIME);

            i = GetMenuID();
            panelOpts = TempOptions.field11_0xb;
            if (i != 4) {
                panelOpts = Game.options_save.field11_0xb;
            }
            InitPanel((u32)panelOpts);

            savedFrametime = FRAMETIME;

            if (((waiting_for_level == -1) || (gone_through_door_to_new_level == 0)) && (screendump == 0)) {
                if ((((ChallengeMode == 1) && (9 < AreaGlobals.values.field_0x20)) &&
                     ((WORLD->current_level != HUB_LDATA) &&
                      ((MiniCutCam == 0) && (i = CutScene_PlayingOrRequested(NULL), i == 0)))) &&
                    (FadeSys.fade == 0.0f)) {
                    EndChallenge(2, 1);
                    world = WORLD;
                    UpdateFrameCounters();
                    if (TimingBarSet == 2) {
                        TBOPENFN("GameCd", 2);
                    }
                } else {
                    world = WORLD;
                    UpdateFrameCounters();
                    if (TimingBarSet == 2) {
                        TBOPENFN("GameCd", 2);
                    }
                }

                PortalDoors_Update(world);
                TERRAINCALLS = 0;
                SHADOWCALLS = 0;
                RAYCASTCALLS = 0;
                DRAWCMODELCALLS = 0;
                NuRndrShadowOnOff(COMPLEXSHADOWS);

                if (Paused == 0) {
                    GameAISysStartFrame(world->ai_sys);
                }

                AddCoinDelay[0] = 0;
                AddCoinDelay[1] = 0;
                other_level_override = -1;
                Shadow_SetMode();
                GizmoPickups_SetOnOff();
                DoInput(WORLD);

                if (((memcard_autosaveenabled != 0) && (memcard_autosavedisabled != 0)) &&
                    (((world == NULL) ||
                      ((world->area == NULL) || ((world->area->flags & AREAFLAG_ENDING_AREA) == 0))) &&
                     (NewMode == 0))) {
                    if (((((NewLData == NULL) && (FadeSys.fade == 0.0f)) && (editor_active == 0)) &&
                         ((GamePads_IgnoreInputFn == NULL) || (i = (*GamePads_IgnoreInputFn)(), i == 0))) &&
                        ((CUTSTOPGAME == 0) || ((i = CutScene_IsSkippable((CUTINFO *)CutStopInfo), i != 0)))) {
                        if ((MiniCutCam == 0) && (CutSceneWaiting == 0)) {
                            if (GameMenu[GameMenuLevel].menu == -1) {
                                if ((Paused == 0) &&
                                    (((Player[0] != NULL) && ((char)Player[0]->apiobj.field_0x1f8 < 0)) ||
                                     ((Player[1] != NULL) && ((char)Player[1]->apiobj.field_0x1f8 < 0)))) {
                                    PauseGame(1);
                                }
                            } else {
                                i = MenuInMemoryCard();
                                if (((i == 0) && (MenuInfo[GameMenu[GameMenuLevel].menu].id != 1)) &&
                                    (MenuInfo[GameMenu[GameMenuLevel].menu].id != 4)) {
                                    NewMenu(0x3f3, 0, -1);
                                } else {
                                    memcard_autosavedisabled = 0;
                                    memcard_autosaveenabled = 0;
                                }
                            }
                        }
                    }
                }

                pads[0] = GamePad[0].pad;
                pads[1] = GamePad[1].pad;
                framePacket.t = FRAMETIME;
                framePacket.paused = Paused;
                framePacket.pads = pads;
                framePacket.flags = 2;
                ((ThingManager *)theGameThings)->ProcessThings(&framePacket);

                if (NOSOUND == 0) {
                    if ((Paused == 0) || (GameMenu[GameMenuLevel].menu != 4)) {
                        NuSound3SetDPL((i32)(byte)Game.options_save.field2_0x2, 0);
                    } else {
                        NuSound3SetDPL(TempOptions.field2_0x2, 0);
                    }
                    UpdateLevelSfx(world, Paused);

                    rumblePad1 = NULL;
                    if (((byte)Game.options_save.field1_0x1 != 0) && (Player[1] != NULL) &&
                        ((char)Player[1]->apiobj.field_0x1f8 < 0)) {
                        rumblePad1 = Player[1]->pad_gamepad->pad;
                    }
                    rumblePad0 = NULL;
                    if (((byte)Game.options_save.field0_0x0 != 0) && (Player[0] != NULL) &&
                        ((char)Player[0]->apiobj.field_0x1f8 < 0)) {
                        rumblePad0 = Player[0]->pad_gamepad->pad;
                    }
                    NuSound3SetRumblePads(rumblePad0, rumblePad1);
                    NuSound3UpdateRumble(FRAMETIME);
                    SoundUpdate(FRAMETIME);
                    ProcessMusicChanges(world->current_level, &Game.options_save);
                }

                windObjs[0] = Player[0] != NULL ? (NUVEC *)&Player[0]->apiobj.field_0x19c : NULL;
                windObjs[1] = Player[1] != NULL ? (NUVEC *)&Player[1]->apiobj.field_0x19c : NULL;
                windObjs[2] = Player[2] != NULL ? (NUVEC *)&Player[2]->apiobj.field_0x19c : NULL;
                windObjs[3] = Player[3] != NULL ? (NUVEC *)&Player[3]->apiobj.field_0x19c : NULL;
                windObjs[4] = Player[4] != NULL ? (NUVEC *)&Player[4]->apiobj.field_0x19c : NULL;
                windObjs[5] = Player[5] != NULL ? (NUVEC *)&Player[5]->apiobj.field_0x19c : NULL;
                windObjs[6] = Player[6] != NULL ? (NUVEC *)&Player[6]->apiobj.field_0x19c : NULL;
                windObjs[7] = Player[7] != NULL ? (NUVEC *)&Player[7]->apiobj.field_0x19c : NULL;

                if (((Paused == 0) || (screendump != 0)) || ((c = IsGrabbingScreen(), c != 0))) {
                    NuRndrGlobalFrameCountPause(0);
                    rtlFrameUpdate(FRAMETIME);
                    DoubleScore = 0;
                    SetTexAnimSignals();
                    NuTexAnimSetMask(0xffff);
                    CutScenes_Start(world);

                    if (Paused == 0) {
                        pauseFlag = 0;
                    } else {
                        pauseFlag = IsGrabbingScreen();
                        pauseFlag ^= 1;
                    }
                    CutScenes_Update(world, pauseFlag);

                    if (TimingBarSet == 2) {
                        TBOPENFN("Scene", 2);
                    }
                    if ((world->current_level->flags & LEVEL_TERRAIN) != 0) {
                        TerrainPlatformOldUpdate();
                    }
                    ProcessGizFlow(world->giz_flow, FRAMETIME);

                    if (world->ai_path_cnx_control_sys != NULL) {
                        AIPathCnxControlSysUpdate(world->ai_path_cnx_control_sys);
                    }

                    if (world->current_gscn != NULL) {
                        NuGScnUpdate(world->current_gscn, FRAMETIME * 60.0f);
                    }
                    GameAnimSys_Update(world->game_anim_sys);
                    edanimUpdateObjects(1.0f);
                    NuBridgeUpdate(&plr_lastpos);

                    if (Grass_Available != 0) {
                        NuWindUpdateArray(&windObjs[0]);
                        NuFadeObjUpdateArray(&windObjs[0]);
                    }

                    rtlProcessLights(world->rtl_set, FRAMETIME);
                    CharPlatforms_Update(world->char_platform_sys);
                    Grabber_Update(world);
                    TrafficAnimSys_Update(world->trafficanim_sys);
                    GizmoSysEarlyUpdate(world->gizmo_sys, world, FRAMETIME);

                    if ((world->current_level->flags & LEVEL_TERRAIN) != 0) {
                        TerrainPlatformNewUpdate();
                    }

                    if (TimingBarSet == 2) {
                        TBCLOSEFN("Scene", 2);
                    }

                    UpdateSpecialSfx(world);
                    Teleports_UpdateBeforeGameObjects(world);

                    if (TimingBarSet == 2) {
                        TBOPENFN("GameObj", 2);
                    }

                    if (CUTSTOPGAME == 0) {
                        ManageGameObjects();
                        UpdateGameObjects(world);
                    } else {
                        if (((Player[0] != NULL) && ((char)Player[0]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[0]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[1] != NULL) && ((char)Player[1]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[1]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[2] != NULL) && ((char)Player[2]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[2]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[3] != NULL) && ((char)Player[3]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[3]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[4] != NULL) && ((char)Player[4]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[4]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[5] != NULL) && ((char)Player[5]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[5]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[6] != NULL) && ((char)Player[6]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[6]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                        if (((Player[7] != NULL) && ((char)Player[7]->apiobj.field_0x1f8 < 0)) &&
                            (((gamepad = Player[7]->pad_gamepad) != NULL) && (gamepad->pad != NULL))) {
                            UpdateRumble(&gamepad->rumble_packet);
                        }
                    }

                    if (TimingBarSet == 2) {
                        TBCLOSEFN("GameObj", 2);
                    }

                    ShoveObjectSysReset();

                    if (CUTSTOPGAME == 0) {
                        if (TimingBarSet == 2) {
                            TBOPENFN("Update", 2);
                        }
                        adaptivedifficulty[1] = 0;
                        Teleports_UpdateAfterGameObjects(world);
                        Pulses_Update(world->pulses_sys);
                        Detonators_Update();
                        Batarangs_Update();
                        UpdateCables();
                        Level_Update(world);
                        Bolts_Update(world);
                        UpdateParts(FRAMETIME);

                        if (0.0f < brickimpactwait) {
                            brickimpactwait -= FRAMETIME;
                        }
                        if (0.0f < coinimpactwait) {
                            coinimpactwait -= FRAMETIME;
                        }
                        if (0.0f < sabrerubwait) {
                            sabrerubwait -= FRAMETIME;
                        }
                        if (0.0f < chattersfxwait) {
                            chattersfxwait -= FRAMETIME;
                        }
                        if (0.0f < tieonsfxwait) {
                            tieonsfxwait -= FRAMETIME;
                        }
                        if (0.0f < tieoffsfxwait) {
                            tieoffsfxwait -= FRAMETIME;
                        }

                        UpdateExplosions();
                        Tag_UpdateTransfers(world->debris_sys->entries[39].effect,
                                            world->debris_sys->entries[40].effect, 0xb4);
                        UpdateRepeatSfx();
                        GizmoSysLateUpdate(world->gizmo_sys, world, FRAMETIME);
                        UpdateRadios();
                        Hint_Process(FRAMETIME);
                        Cheats_Update();

                        if (MechInputTouchMenuController::PackButtonPressed != 0) {
                            MechInputTouchMenuController::PackButtonPressed = 0;
                            Hint_CancelCurrent();
                            i = NuIOS_AreInAppPurchasesAvailable();
                            if ((i == 0) || ((i = NuIOS_CanMakeInAppPurchases(), i == 0))) {
                                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                                GameCam_HitRoll();
                            } else {
                                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                                menu_i_pack = MechInputTouchMenuController::PackButtonID;
                                NewMenu(0x14, -1, -1);
                            }
                        }

                        if (TimingBarSet == 2) {
                            TBCLOSEFN("Update", 2);
                        }
                    }

                    UpdateGameMenu(GamePad, 1);

                    if (TimingBarSet == 2) {
                        TBOPENFN("Deb", 2);
                    }
                    Debris(0);
                    if (CUTSTOPGAME == 0) {
                        DebrisKillPlayers();
                    }
                    UpdateRippleSet(ripples);
                    AddCameraRain(world, 0x7a);

                    if (TimingBarSet == 2) {
                        TBCLOSEFN("Deb", 2);
                    }

                    NuMtlAnimateSetSpeedScale(1.0f);
                    NuMtlAnimateSetMask(0xffff);

                    if (TimingBarSet == 2) {
                        TBOPENFN("cam", 2);
                    }
                    MoveGameCamera(GameCam);
                    GameObjectToCameraDistances();
                    SetCameraMatrices();

                    if (TimingBarSet == 2) {
                        TBCLOSEFN("cam", 2);
                    }

                    if (CUTSTOPGAME == 0) {
                        if (TimingBarSet == 2) {
                            TBOPENFN("Update", 2);
                        }
                        UpdateStreaks(FRAMETIME);
                        if (TimingBarSet == 2) {
                            TBCLOSEFN("Update", 2);
                        }
                    }

                    if (((((world->area != NULL) && (world->area == HUB_ADATA)) ||
                          (world->current_level == TITLES_LDATA)) ||
                         ((world->current_level == STATUS_LDATA) ||
                          ((world->current_level->flags & LEVEL_STATUS) != 0))) ||
                        (world->current_level == CREDITS_LDATA)) {
                        BackDrop_Update(FRAMETIME);
                    }
                } else {
                    NuMtlAnimateSetSpeedScale(0.0f);
                    NuTexAnimSetMask(2);
                    NuRndrGlobalFrameCountPause(1);
                    cam = pNuCam;
                    gameCam = GameCam;
                    cam->mtx = gameCam->render_mtx;
                    NuCameraSet(cam);
                    UpdateGameMenu(GamePad, 1);
                    if (Player[0] != NULL) {
                        UpdateCoinPacket(Player[0]->coinpacket, ((u32)(byte)Player[0]->apiobj.field_0x1f8 >> 7) & 1,
                                         (i32)(char)Player[0]->apiobj.field_0x27c);
                    }
                    if (Player[1] != NULL) {
                        UpdateCoinPacket(Player[1]->coinpacket, ((u32)(byte)Player[1]->apiobj.field_0x1f8 >> 7) & 1,
                                         (i32)(char)Player[1]->apiobj.field_0x27c);
                    }
                    Debris(1);
                }

                if (world->current_level->always_update_fn != NULL) {
                    world->current_level->always_update_fn(world);
                }

                LevelStreaming_Update(world);
                UpdateCutBorders();

                if ((Paused == 0) || (editor_active != 0)) {
                    NuWaterSpeed(FRAMETIME / 0.016666668f);
                } else {
                    NuWaterSpeed(0);
                }
                NuWaterReset();
                GameTiming(world, (f32 *)&Game.field30_0x7c2c);
                UpdateStats();
                GameDisplaySettings(&world->current_level->data_display, (i32 *)&back_rgba);

                if (((NewMode != 0) || (NewLData != NULL)) && (FadeSys.fade == 0.0f)) {
                    if (((world->current_level == NULL) || ((world->current_level->flags & LEVEL_STATUS) == 0)) ||
                        (HUB_LDATA == NULL)) {
                        if (FadeSys.pending_type == FADE_TYPE_NONE) {
                        level_fade_set:
                            level = WORLD->current_level;
                        level_fade_common:
                            if (level == NULL) {
                                if (NewLData != NULL) {
                                    goto level_fade_newl;
                                }
                            level_fade_2:
                                fadeType.type = FADE_TYPE_STILL_WIPE;
                            } else {
                                if (NewLData == NULL) {
                                    goto level_fade_2;
                                }
                                if (NewLData == level) {
                                level_fade_newl:
                                    i = (i32)(char)NewLData->area_index;
                                level_fade_nl:
                                    if ((i == WORLD->level_sub_id) ||
                                        ((SuperStory != 0) && ((WORLD->area->flags & AREAFLAG_ENDING_AREA) == 0))) {
                                        goto level_fade_2;
                                    }
                                } else {
                                    if ((((HUB_LDATA != NewLData) && (HUB_LDATA != level)) &&
                                         (TITLES_LDATA != NewLData)) &&
                                        ((TITLES_LDATA != level) && (CREDITS_LDATA != NewLData))) {
                                        if (SuperStory == 0) {
                                            goto level_fade_newl;
                                        }
                                        i = (i32)(char)NewLData->area_index;
                                        if (((level->flags & LEVEL_STATUS) != 0) &&
                                            ((ADataList[i].flags & AREAFLAG_ENDING_AREA) != 0)) {
                                            goto level_fade_1;
                                        }
                                        goto level_fade_nl;
                                    }
                                }
                            level_fade_1:
                                fadeType.type = FADE_TYPE_WIPE;
                            }
                            FadeSys.SetFade(fadeType, 0);
                            goto level_fade_stage2;
                        }
                        if (NewLData != NULL) {
                            goto level_fade_common;
                        }
                    } else {
                        if (HUB_LDATA == NewLData) {
                        level_fade_hub:
                            if (FadeSys.pending_type == FADE_TYPE_NONE) {
                                goto level_fade_set;
                            }
                        level_fade_common2:
                            level = WORLD->current_level;
                            if ((level == NewLData) || (HUB_LDATA != NewLData)) {
                                goto level_fade_stage2;
                            }
                        } else {
                            if (NewLData != NULL) {
                                const i32 nextAreaIndex = NewLData->area_index;
                                if ((nextAreaIndex != -1) &&
                                    ((ADataList[nextAreaIndex].flags & AREAFLAG_ENDING_AREA) == 0)) {
                                    FadeSys.fade = 1.0f;
                                    FinishLoop_On = 0;
                                    goto level_fade_done;
                                }
                                goto level_fade_hub;
                            }
                            if (FadeSys.pending_type != FADE_TYPE_NONE) {
                                goto level_fade_stage2;
                            }
                            level = WORLD->current_level;
                        }
                        goto level_fade_common;
                    }
                level_fade_stage2:
                    FadeSys.SetStage(2);
                }

            level_fade_done:
                if (TimingBarSet == 2) {
                    TBCLOSEFN("GameCd", 2);
                }
            }

            if ((waiting_for_level != -1) && (screendump == 0)) {
                WaitingForLevelTime += FRAMETIME;
            }
            if ((waiting_for_character != -1) && (screendump == 0)) {
                WaitingForCharacterTime += FRAMETIME;
            }

            FRAMETIME = savedFrametime;
            i = GetMenuID();
            panelOpts = TempOptions.field11_0xb;
            if (i != 4) {
                panelOpts = Game.options_save.field11_0xb;
            }
            WidescreenCode((u32)panelOpts);
            UpdateBackgroundMusic();

            NuFrameBegin();
            if (TimingBarSet == 5) {
                TBOPENFN("DrwCd", 5);
            }

            world = WORLD;
            if ((pause_rndr_on == 0) || (FadeSys.pending_type == FADE_TYPE_WIPE)) {
                if (MainRenderTime <= 0.0f) {
                    NoRender();
                } else {
                    GameFog_Update(WORLD);
                    GameFog_Set();
                    SetLevelLights(world->rtl_set, 1.0f);
                    SetSpotLightMode();

                    if (CUTSTOPGAME == 0) {
                        if (TimingBarSet == 5) {
                            TBOPENFN("Process", 5);
                        }
                        DrawGameObjectsProcess();
                        if (TimingBarSet == 5) {
                            TBCLOSEFN("Process", 5);
                        }
                    }

                    if (NOSOUND == 0) {
                        NuRndrBeginScene(-1);
                    }
                    SpeedBlur_Update();

                    if ((i32)back_rgba[0] == back_rgba[1]) {
                        NuRndrClear(0x1f00, (i32)back_rgba[0], 1.0f);
                    } else {
                        NuRndrGradClear(0xf00, (i32)back_rgba[0], (i32)back_rgba[1], 0x3f800000);
                    }

                    if (CUTSTOPGAME == 0) {
                        EnableShadowMapRendering(0);
                        DrawParts(Paused);
                        ResetShadowMapRendering();
                    }

                    if (TimingBarSet == 5) {
                        TBOPENFN("DrwObjs", 5);
                    }
                    if (CUTSTOPGAME == 0) {
                        DrawGameObjects();
                    }
                    if (TimingBarSet == 5) {
                        TBCLOSEFN("DrwObjs", 5);
                    }

                    Grabber_Draw(world);
                    SetLevelLights(world->rtl_set, 1.0f);

                    if ((editor_active == 0) && (Paused == 0)) {
                        if (CUTSTOPGAME == 0) {
                            CharShadows_Update();
                            GameObjectStuffAfterAnimation();
                            goto shadows_draw;
                        }
                    shadows_screendump:
                        if (screendump == 0) {
                            goto grabrender;
                        }
                    shadows_paused:
                        pausedRender = save_paused;
                    } else {
                    shadows_draw:
                        if (CUTSTOPGAME != 0) {
                            goto shadows_screendump;
                        }
                        CharShadows_Draw();
                        if (screendump != 0) {
                            goto shadows_paused;
                        }
                    grabrender:
                        pausedRender = Paused;
                    }

                    c = IsGrabbingScreen();
                    pauseFlag = 0;
                    if (c == 0) {
                        pauseFlag = pausedRender;
                    }

                    if (world->lev_objs[1].active == 0) {
                    draw_world:
                        if (CUTSTOPGAME == 0) {
                            if (TimingBarSet == 5) {
                                TBOPENFN("DrwWld", 5);
                            }
                            WorldInfo_DrawScene(world);
                            if (TimingBarSet == 5) {
                                TBCLOSEFN("DrwWld", 5);
                            }
                            Level_Draw(world);
                            Faders_Draw(world);
                            DrawStreaks();
                            Bolts_Draw(world);
                            DrawExplosions();
                            ZipUps_DrawLines();
                            DrawCables();
                            Detonators_Draw();
                            Batarangs_Draw();
                            TrafficAnimSys_Draw(world->trafficanim_sys);
                            GizmoSysDraw(world->gizmo_sys, world, FRAMETIME);
                            SpecialMiniKits_Draw(world);
                            NuBridgeDraw(0);
                        }
                    } else if ((CUTSTOPGAME == 0) || (CUTDRAWWORLD != 0)) {
                        DrawParallax(&world->lev_objs[1].special);
                        goto draw_world;
                    }

                    if (Grass_Available != 0) {
                        NuWindDraw();
                        NuFadeObjDraw();
                    }

                    ((ThingManager *)theGameThings)->RenderThings((ThingRenderData *)0);
                    CutScenes_Draw(world);
                    NuRndrShadPolys(ShadowMat);
                    NuLgtLaserDraw(pauseFlag);

                    level = world->current_level;
                    if ((((level == TITLES_LDATA) || ((level->flags & LEVEL_STATUS) != 0)) ||
                         (level == CREDITS_LDATA)) ||
                        ((level == STATUS_LDATA) && ((StatusPacket.status_flags & STATUS_FLAG_DRAW_BACKDROP) != 0))) {
                        BackDrop_Draw(1.0f, 0);
                    }

                    if (MainRenderTime != 1.0f) {
                        BackDrop_Draw(1.0f - MainRenderTime, 0);
                    }

                    if (TimingBarSet == 5) {
                        TBOPENFN("Deb", 5);
                    }
                    NuRndrLine3dDbgFlush();
                    DebrisSetCutSceneMode(CUTCAM);
                    DebrisDraw(pauseFlag ^ editor_active, 1);
                    if ((TimingBarSet == 5) && (TBCLOSEFN("Deb", 5), TimingBarSet == 5)) {
                        TBOPENFN("Ripples", 5);
                    }
                    DrawRippleSet(ripples);
                    if (TimingBarSet == 5) {
                        TBCLOSEFN("Ripples", 5);
                    }

                    SpeedBlur_Apply(world);
                    NuRndrEndScene();
                    RenderShadowLights(Paused);
                    NuRndrBeginScene(-1);
                    SetDepthOfField();

                    if (BURNOUTON != 0) {
                        BurnoutApply(pauseFlag);
                    }

                    UpdateDebrisRenderStackPriority();
                    SortDebrisRenderStack();
                    DebrisDraw(pauseFlag ^ editor_active, 1);
                    DebrisDraw(pauseFlag ^ editor_active, 0);
                    NuRndrEndScene();

                    if (TimingBarSet == 5) {
                        TBOPENFN("RndrFx", 5);
                    }
                    NuRndrFx(pauseFlag, &plr_lastpos);
                    if ((TimingBarSet == 5) && (TBCLOSEFN("RndrFx", 5), TimingBarSet == 5)) {
                        TBOPENFN("Deb", 5);
                    }

                    NuRndrBeginScene(-1);
                    UpdateDebrisRenderStackPriority();
                    SortDebrisRenderStack();
                    DebrisDraw(pauseFlag ^ editor_active, 1);
                    DebrisDraw(pauseFlag ^ editor_active, 0);
                    NuRndrEndSceneEx(0);
                    DebrisDrawGlass();

                    if (TimingBarSet == 5) {
                        TBCLOSEFN("Deb", 5);
                    }
                    if (screendump == 0) {
                        ViewCamDraw();
                    }
                }
            }

            if (PANELOFF == 0) {
                DrawCutBorders((u32)(byte)Game.options_save.field11_0xb);
            }
            if (MainRenderTime != 1.0f) {
                clear_screen_onstill = 0;
            }

            GrabStillScreen();
            HandleStillRender();

            if (HubMainRenderTimeHack != 0) {
                HubMainRenderTimeHack = 0;
                MainRenderTargetTime = 1.0f;
                MainRenderTime = 1.0f;
            }

            PanelRender(WORLD);
            if (TimingBarSet == 5) {
                TBCLOSEFN("DrwCd", 5);
            }

            g_val += 0.002;
            CutScenes_End();

            frameout_count[1] = nuvideo_global_vbcnt >> 0x1f;
            i = (nuvideo_global_vbcnt - 1) - (i32)frameout_count[0];
            frameout = 0;
            if (0 <= i) {
                frameout = i;
            }
            peak_poly_count = peak_poly_count <= nurndr_tritot_this_frame ? nurndr_tritot_this_frame : peak_poly_count;
            poly_count = nurndr_tritot_this_frame;
            frameout_count[0] = nuvideo_global_vbcnt;

            edGraEnableTerrainSwap();
            FRAMETIME = NuFrameEnd();
            edGraDisableTerrainSwap();

            if (DEFAULTFRAMETIME <= FRAMETIME) {
                if (MAXFRAMETIME < FRAMETIME) {
                    FRAMETIME = MAXFRAMETIME;
                }
                savedFrametime = DEFAULTFRAMETIME;
                if (DEFAULTFRAMETIME <= FRAMETIME) {
                    goto frametime_clamp;
                }
            } else {
                FRAMETIME = DEFAULTFRAMETIME;
            frametime_clamp:
                savedFrametime = FRAMETIME;
                if (MAXFRAMETIME < FRAMETIME) {
                    FRAMETIME = MAXFRAMETIME;
                    savedFrametime = FRAMETIME;
                }
            }
            FRAMETIME = savedFrametime;

            if (enable_zero_frametime != 0) {
                FRAMETIME = 0.0f;
            }

            savedFrametime = FRAMETIME;
            pastFrameTimes[frameCount % 5] = FRAMETIME;

            longestFrameIndex = -1;
            shortestFrameIndex = -1;
            frameTimeAccumulator = -1.0f;
            shortestFrameTime = 999.0f;

            if (frameTimeAccumulator < pastFrameTimes[0]) {
                longestFrameIndex = 0;
            }
            if (pastFrameTimes[0] < shortestFrameTime) {
                shortestFrameIndex = 0;
            }
            frameTimeAccumulator = MAX(pastFrameTimes[0], frameTimeAccumulator);
            shortestFrameTime = MIN(pastFrameTimes[0], shortestFrameTime);

            if (frameTimeAccumulator < pastFrameTimes[1]) {
                longestFrameIndex = 1;
            }
            if (pastFrameTimes[1] < shortestFrameTime) {
                shortestFrameIndex = 1;
            }
            frameTimeAccumulator = MAX(pastFrameTimes[1], frameTimeAccumulator);
            shortestFrameTime = MIN(pastFrameTimes[1], shortestFrameTime);

            if (frameTimeAccumulator < pastFrameTimes[2]) {
                longestFrameIndex = 2;
            }
            if (pastFrameTimes[2] < shortestFrameTime) {
                shortestFrameIndex = 2;
            }
            frameTimeAccumulator = MAX(pastFrameTimes[2], frameTimeAccumulator);
            shortestFrameTime = MIN(pastFrameTimes[2], shortestFrameTime);

            if (frameTimeAccumulator < pastFrameTimes[3]) {
                longestFrameIndex = 3;
            }
            if (pastFrameTimes[3] < shortestFrameTime) {
                shortestFrameIndex = 3;
            }
            frameTimeAccumulator = MAX(pastFrameTimes[3], frameTimeAccumulator);
            shortestFrameTime = MIN(pastFrameTimes[3], shortestFrameTime);

            if (frameTimeAccumulator < pastFrameTimes[4]) {
                longestFrameIndex = 4;
            }
            if (pastFrameTimes[4] < shortestFrameTime) {
                shortestFrameIndex = 4;
            }
            frameTimeAccumulator = MAX(pastFrameTimes[4], frameTimeAccumulator);
            shortestFrameTime = MIN(pastFrameTimes[4], shortestFrameTime);

            frameTimeAccumulator = 0.0f;
            averagedFrameCount = 0;
            if (shortestFrameIndex != 0) {
                if (longestFrameIndex != 0) {
                    if (0.0f <= pastFrameTimes[0]) {
                        frameTimeAccumulator = pastFrameTimes[0];
                        averagedFrameCount = 1;
                    }
                }
            }

            if (shortestFrameIndex != 1) {
                if (longestFrameIndex != 1) {
                    if (0.0f <= pastFrameTimes[1]) {
                        frameTimeAccumulator += pastFrameTimes[1];
                        averagedFrameCount += 1;
                    }
                }
            }
            if (shortestFrameIndex != 2) {
                if (longestFrameIndex != 2) {
                    if (0.0f <= pastFrameTimes[2]) {
                        frameTimeAccumulator += pastFrameTimes[2];
                        averagedFrameCount += 1;
                    }
                }
            }
            if (shortestFrameIndex != 3) {
                if (longestFrameIndex != 3) {
                    if (0.0f <= pastFrameTimes[3]) {
                        frameTimeAccumulator += pastFrameTimes[3];
                        averagedFrameCount += 1;
                    }
                }
            }
            if (shortestFrameIndex != 4) {
                if (longestFrameIndex != 4) {
                    if (0.0f <= pastFrameTimes[4]) {
                        frameTimeAccumulator += pastFrameTimes[4];
                        averagedFrameCount += 1;
                    }
                }
            }

            if (averagedFrameCount != 0) {
                savedFrametime = frameTimeAccumulator / (f32)averagedFrameCount;
                FRAMETIME = savedFrametime;
            }

            if (0.1f < savedFrametime) {
                FRAMETIME = 0.1f;
            }

            frameCount++;

            if (g_lowEndLevelBehaviour != 0) {
                drawcharactermodel_nobsa = 1;
            }
            if (2 < frameout) {
                frameout = 2;
            }

            if (screendump != 0) {
                if (screendump == 1) {
                    Paused = save_paused;
                }
                screendump--;
            }

            NuSound3Update();
        }

        NuVideoRollingFrameRateReset();
        ClearPause();
        ViewCamSetActive(0, NULL);

        if (NewMode == 0) {
            FinishLoop(2);
        }
        FinishLoop_On = 1;
        NuSound3StopRumble();
        TerrainTrackFlush();

        if (WORLD->page_grass != -1) {
            edgraStopPage(WORLD->page_grass);
        }
        Parts_Stop(WORLD);
        Particles_Stop(WORLD);
        CutScenes_Stop(WORLD->cutscene_sys);

        if (NOSOUND == 0) {
            NuSound3FlushLoops();
        }

        StoreLevelProgress(WORLD);
        if (netclient == 0) {
            StoreStatusTakeOverObjectSys();
        }
        ReleaseAllTakeOvers();

        if (NewMode == 0) {
            break;
        }
        reset_load = 0;
        LevelChange = 0;
    } while (true);

    reset_load = 1;
    if (new_level_from_menu != 0) {
        ClearAreaProgress(WORLD->level_sub_id, 1);
    }

    LastLData = WORLD->current_level;
    last_area = -1;
    if (PlayTrailer == -1) {
        LastAData = &ADataList[Area];
        last_area = Area;
    }

    if (LastLData == CREDITS_LDATA) {
        savedFrametime = 1.0f;
        if (Game_OptionsSave != NULL) {
            savedFrametime = GameSetMusicVolume(Game_OptionsSave);
        }
        legoSetMusicVolume(savedFrametime);
    }

    Area = (i32)(char)NewLData->area_index;
    i = NewLData->idx;
    Level = i;

    if (Area == last_area) {
        i = 0;
        if (last_area != -1) {
            i = LevelChangesInArea + 1;
        }
        LevelChange = (u32)(NewLData->idx != LastLData->idx);
        LevelChangesInArea = i;
        if (new_level_from_menu == 0) {
            goto after_sound;
        }
    } else {
        LevelChange = (u32)(NewLData->idx != LastLData->idx);
        LevelChangesInArea = 0;
    }

    if (NOSOUND == 0) {
        music_man.StopAll(0);
        MusicClearAll();
        SoundKillAll();
        if (HUB_ADATA != NULL) {
            if (((u32)(byte)HUB_ADATA->index != (u32)last_area) && (Arcade != 0)) {
                if ((u32)(byte)HUB_ADATA->index == (u32)Area) {
                    hub_from_arcade = last_area;
                }
            }
        } else if (Arcade != 0) {
            // falls through to Arcade = 0
        }
    } else {
        if ((HUB_ADATA == NULL) || (Arcade == 0)) {
            goto after_sound;
        }
        if ((u32)(byte)HUB_ADATA->index == (u32)last_area) {
            goto after_sound;
        }
        if ((u32)(byte)HUB_ADATA->index == (u32)Area) {
            hub_from_arcade = last_area;
        }
    }
    Arcade = 0;

after_sound:
    if ((NewLData == STATUS_LDATA) || ((NewLData->flags & LEVEL_STATUS) != 0)) {
        InitStatusScreen(WORLD);
        if ((NewLData != STATUS_LDATA) && (StatusPacket.status_flags != 0)) {
            UpdateAchievements(&StatusPacket);
        }
    }

    if (netclient == 0) {
        if ((Area == -1) || (Area != last_area)) {
            if (0 < PLAYERCOUNT) {
                UsePlayerList = 2;
            }
            MakePlayerList(8);
            Door_UseCutCam = 0;
        } else if (new_level_from_menu == 0) {
            MakePlayerList(8);
            UsePlayerList = 1;
        } else {
            MakePlayerList(8);
            UsePlayerList = 2;
        }
    } else {
        UsePlayerList = 2;
        PlayerProgress[0].active = status_plr_active[0];
        PlayerProgress[1].active = status_plr_active[1];
        PlayerProgress[2].active = status_plr_active[2];
        PlayerProgress[3].active = status_plr_active[3];
        PlayerProgress[4].active = status_plr_active[4];
        PlayerProgress[5].active = status_plr_active[5];
        PlayerProgress[6].active = status_plr_active[6];
        PlayerProgress[7].active = status_plr_active[7];
    }

    afterArea = HUB_ADATA;
    i = Area;

    if (Area == -1) {
        NextArea_FreePlay = 0;
        FreePlay = 0;
    } else {
        if ((ADataList[Area].flags & AREAFLAG_BONUS_AREA) != 0) {
            NextArea_FreePlay = 1;
            FreePlay = 1;
        }

        if ((new_level_from_menu != 0) && (Area != last_area)) {
            if ((HUB_ADATA == NULL) || (Area != (u32)(byte)HUB_ADATA->index)) {
                FreePlay = NextArea_FreePlay;
                if (NextArea_FreePlay != 0) {
                    if (Player[0] != NULL) {
                        Player[0]->id = -1;
                    }
                    if (Player[1] != NULL) {
                        Player[1]->id = -1;
                    }
                    makefreeplaymodellist = 1;
                }
            } else {
                NextArea_FreePlay = 0;
                FreePlay = 0;
            }
        }
    }

    if ((StatusPacket.status_flags == 0) ||
        ((((STATUS_LDATA == NULL) || (STATUS_LDATA != LastLData)) && ((LastLData->flags & LEVEL_STATUS) == 0)) &&
         ((CREDITS_LDATA == NULL) || (CREDITS_LDATA != LastLData)))) {
    status_players:
        if (afterArea == NULL) {
            goto after_status;
        }
    } else {
        PlayerList[0] = StatusPacket.player0_model;
        PlayerProgress[0].active = StatusPacket.player0_active;
        PlayerProgress[0].field_0x7 = 0;
        PlayerProgress[0].coins = 0;
        PlayerList[1] = StatusPacket.player1_model;
        PlayerProgress[1].active = StatusPacket.player1_active;
        PlayerProgress[1].field_0x7 = 0;
        PlayerProgress[1].coins = 0;
        PlayerList[2] = -1;
        PlayerProgress[2].active = 0;
        PlayerProgress[2].field_0x7 = 0;
        PlayerProgress[2].coins = 0;
        PlayerList[3] = -1;
        PlayerProgress[3].active = 0;
        PlayerProgress[3].field_0x7 = 0;
        PlayerProgress[3].coins = 0;
        PlayerList[4] = -1;
        PlayerProgress[4].active = 0;
        PlayerProgress[4].field_0x7 = 0;
        PlayerProgress[4].coins = 0;
        PlayerList[5] = -1;
        PlayerProgress[5].active = 0;
        PlayerProgress[5].field_0x7 = 0;
        PlayerProgress[5].coins = 0;
        PlayerList[6] = -1;
        PlayerProgress[6].active = 0;
        PlayerProgress[6].field_0x7 = 0;
        PlayerProgress[6].coins = 0;
        PlayerList[7] = -1;
        PlayerProgress[7].active = 0;
        PlayerProgress[7].field_0x7 = 0;
        PlayerProgress[7].coins = 0;

        if ((afterArea == NULL) || ((i != (u32)(byte)afterArea->index) || (TITLES_LDATA == LastLData))) {
            UsePlayerList = 2;
            goto status_players;
        }
        UsePlayerList = 1;
    }

    if ((i == (u32)(byte)afterArea->index) && (i != last_area)) {
        if ((CREDITS_LDATA != NULL) && ((CREDITS_LDATA == LastLData) && (StatusPacket.status_flags != 0))) {
            RememberPlayerIDs(1, (i32)(i16)StatusPacket.player0_model, (i32)(i16)StatusPacket.player1_model);
        }
        Hub_MakeModelList();
    }

after_status:
    NuSound3StopRumble();
    FreeGameObjectLights();
    ClearUpAreaData();
    MenuReset();
    WorldInfo_ClearAllIfScreenFaded();

    i = Game_Exit(last_area);
    if (i != 0) {
        return 0;
    }
    goto restart_level;
}

static void NuSoundAppTerminate(void) {
    NuSound.Shutdown();
}
