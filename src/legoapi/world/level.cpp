#include "legoapi/world/level.h"

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/numusic.h"

extern "C" char ConfigBuffer[0x10000];

// Defined in legoapi/gameobjects.cpp
void GameAudio_PlaySfxAndSetVolume(i32, nuvec_s *, float);

// These are extern (U) in the original level.cpp.o — defined here as stubs
// until the original defining file is decompiled.
LEVELDATA *levelconfig_ldata = NULL;

extern i16 GetMusicIndex(char *, nusound_filename_info_s *, i32);

#include "legoapi/world/levels/level_functions.h"
static void Credits_Init_Game(WORLDINFO *) {
}
static void Credits_Update_Game(WORLDINFO *) {
}
static void Credits_Draw_Game(WORLDINFO *) {
}

static void Titles_Init(WORLDINFO *) {
}
static void Titles_Update(WORLDINFO *) {
}
static void Titles_Draw(WORLDINFO *) {
}

// Keyword tables for generic level configuration keywords.
// These are combined with game-specific keyword tables by LevelConfig_BeforeLoad
// and LevelConfig_AfterLoad via NuFParPushCom2.
// Both tables exist in the original binary (defined in a separate compilation unit)
// and are terminated by {NULL, NULL}.
// TODO: Populate with actual keyword entries once extracted from the binary.
static NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[] = {
    {NULL, NULL},
};

static NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[] = {
    {NULL, NULL},
};

i32 Text_StripComments(char *text, char *dest, i32 param) {
    char *start = text;
    char *end = text + NuStrLen(text);
    char *out = dest;
    while (text <= end) {
        char c = *text;
        if (c == '#' || c == ';' || (c == '/' && text[1] == '/')) {
            if (c == '/') {
                text++;
            }
            while (c != '\r' && c != '\n' && c != '\0') {
                c = *++text;
            }
            *out++ = c;
            text++;
        } else {
            if (param != 0 && (c == ',' || c == '=')) {
                c = ' ';
            }
            *out++ = c;
            text++;
        }
    }
    *out = '\0';
    return (i32)(out - dest);
}

LEVELDATA *LDataList = NULL;
LEVELDATA *NEWGAME_LDATA = NULL;
LEVELDATA *LOADGAME_LDATA = NULL;

static i32 MAXLDATA = 0x64;

i32 LEVELCOUNT;

LEVELDATA *Levels_ConfigureList(char *file, VARIPTR *buf, VARIPTR *buf_end, i32 max_level_count, i32 *level_count_out,
                                LEVELSETDEFAULTSFN *set_defaults_fn) {
    NUFPAR *parser;
    i32 in_level_config;
    i32 n;
    LEVELDATA *level_data;
    LEVELDATA *cur_level;
    i32 i;
    i32 j;

    parser = NuFParCreate(file);

    MAXLDATA = max_level_count;
    n = 0;

    level_data = (LEVELDATA *)ALIGN(buf->addr, 0x4);
    buf->void_ptr = level_data;

    cur_level = level_data;

    in_level_config = 0;

    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (parser->word_buf[0] == '\0') {
            continue;
        }

        if (in_level_config) {
            if (NuStrICmp(parser->word_buf, "level_end") == 0) {
                in_level_config = 0;

                if (cur_level->dir[0] == '\0' || cur_level->name[0] == '\0' || (cur_level->flags & LEVEL_TEST) != 0) {
                    continue;
                }

                if ((cur_level->flags & LEVEL_NEWGAME) != 0) {
                    NEWGAME_LDATA = cur_level;
                }

                if ((cur_level->flags & LEVEL_LOADGAME) != 0) {
                    LOADGAME_LDATA = cur_level;
                }

                n++;
                in_level_config = 0;
                cur_level++;

                continue;
            }

            if (NuStrICmp(parser->word_buf, "dir") == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buf) < 0x40) {
                    NuStrCpy(cur_level->dir, parser->word_buf);
                }
            } else if (NuStrICmp(parser->word_buf, "file") == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buf) < 0x20) {
                    NuStrCpy(cur_level->name, parser->word_buf);
                }
            }

            if (NuStrICmp(parser->word_buf, "test_level") == 0) {
                cur_level->flags |= LEVEL_TEST;
            } else if (NuStrICmp(parser->word_buf, "intro_level") == 0) {
                cur_level->flags |= LEVEL_INTRO;
            } else if (NuStrICmp(parser->word_buf, "midtro_level") == 0 ||
                       NuStrICmp(parser->word_buf, "cutscene_level") == 0) {
                cur_level->flags |= LEVEL_MIDTRO;
            } else if (NuStrICmp(parser->word_buf, "outro_level") == 0) {
                cur_level->flags |= LEVEL_OUTRO;
            } else if (NuStrICmp(parser->word_buf, "status_level") == 0) {
                cur_level->flags &= ~LEVEL_GAMEPLAY;
                cur_level->flags &= ~LEVEL_TERRAIN;
                cur_level->flags |= LEVEL_STATUS;
            } else if (NuStrICmp(parser->word_buf, "newgame_level") == 0) {
                if (NEWGAME_LDATA == NULL) {
                    cur_level->flags |= LEVEL_NEWGAME;
                }
            } else if (NuStrICmp(parser->word_buf, "loadgame_level") == 0) {
                if (LOADGAME_LDATA == NULL) {
                    cur_level->flags |= LEVEL_LOADGAME;
                }
            }

            continue;
        }

        if (NuStrICmp(parser->word_buf, "level_start") == 0 && n < MAXLDATA) {
            cur_level->dir[0] = '\0';
            cur_level->name[0] = '\0';

            cur_level->unknown_060 = -1;

            cur_level->idx = n;

            cur_level->flags = LEVEL_GAMEPLAY | LEVEL_UNKNOWN_FLAG_4 | LEVEL_TERRAIN;

            cur_level->load_fn = NULL;
            cur_level->init_fn = NULL;
            cur_level->reset_fn = NULL;
            cur_level->update_fn = NULL;
            cur_level->always_update_fn = NULL;
            cur_level->draw_fn = NULL;
            cur_level->draw_status_fn = NULL;

            cur_level->data_display.unknown_14 = 20000;
            cur_level->data_display.unknown_00 = 0.1f;
            cur_level->data_display.unknown_04 = 0.15f;
            cur_level->data_display.far_clip = 20000.0f;
            cur_level->data_display.fog_start = 20100.0f;
            cur_level->data_display.particle_thin = g_isLowEndDevice ? 4.0f : 1.0f;
            cur_level->data_display.bg_red_bottom = 0;
            cur_level->data_display.bg_red_top = 0;
            cur_level->data_display.bg_green_bottom = 0;
            cur_level->data_display.bg_green_top = 0;
            cur_level->data_display.bg_blue_bottom = 0;
            cur_level->data_display.bg_blue_top = 0;

            cur_level->music_index = -1;

            cur_level->unknown_11c = 0.0f;
            cur_level->unknown_120 = 1.0f;

            cur_level->unknown_0a2 = -1;
            cur_level->max_ter_platforms = 0x80;
            cur_level->max_ter_groups = 0x100;
            cur_level->unknown_0a8 = -1;
            cur_level->unknown_0aa = -1;

            cur_level->mipmap_mode = 0x03;
            cur_level->blob_shadow_alpha = 0x7f;
            cur_level->unknown_0ae = -1;
            cur_level->area_index = -1;

            cur_level->unknown_0b8 = 0x50;

            cur_level->cam_tilt = 0.0f;

            cur_level->hover_height = 0.0f;

            cur_level->unknown_0b9 = 0x50;
            cur_level->unknown_0ba = 0x50;
            cur_level->unknown_0bb = 0x32;
            cur_level->unknown_0bc = 0x00;
            cur_level->unknown_0bd = 0x00;
            cur_level->unknown_0be = 0x00;
            cur_level->unknown_0bf = 0x00;

            cur_level->unknown_0c0 = 0.5f;
            cur_level->cam_pullback_dist = 0.0f;
            cur_level->cam_lateral_dist = 0.0f;
            cur_level->unknown_0cc = 2e+06f;

            cur_level->area_level_index = -1;
            cur_level->blob_shadow_fade_near = 5;
            cur_level->blob_shadow_fade_far = 10;
            cur_level->cam_pos_seek = 5;
            cur_level->cam_angle_seek = 5;
            cur_level->unknown_0d9 = 10;
            cur_level->unknown_0da = 5;
            cur_level->unknown_0db = 5;

            cur_level->conveyor_x_speed = 0.0f;
            cur_level->conveyor_z_speed = 0.0f;

            for (i = 0; i < 2; i++) {
                for (j = 0; j < 3; j++) {
                    cur_level->music_tracks[j][i] = -1;
                }
            }

            in_level_config = 1;

            if (set_defaults_fn != NULL) {
                (*set_defaults_fn)(cur_level);
            }
        }
    }

    NuFParDestroy(parser);

    if (n == 0) {
        return NULL;
    }

    buf->void_ptr = cur_level;

    if (level_count_out != NULL) {
        *level_count_out = n;
    }

    return level_data;
}

void Level_SetDefaults(LEVELDATA *level) {
    level->max_antinodes = 256;
    level->max_gizmo_blowups = 64;
    level->max_gizmo_blowup_types = 64;
    level->max_pickups = 256;
    level->max_obstacle_objs = 128;
    level->max_buildit_objs = 128;
    level->max_force_objs = 128;
    level->max_bombgen_objs = 16;

    level->max_tightropes = 10;
    level->max_giz_timers = 8;
    level->max_signals = 10;
    level->max_levers = 10;
    level->max_technos = 10;
    level->max_zipups = 10;
    level->max_grapples = 10;
    level->max_obstacles = 32;
    level->max_buildits = 16;
    level->max_shards = 64;
    level->max_spinners = 4;
    level->max_minicuts = 8;
    level->max_minicut_parts = 2;
    level->max_giz_specials = 32;
    level->max_attractos = 5;
    level->max_climb_objs = 10;
    level->max_guidelines = 32;
    level->max_ledges = 96;
    level->max_security_doors = 5;
    level->max_tubes = 5;
    level->max_giz_panels = 8;
    level->max_hat_machines = 2;
    level->max_force = 32;
    level->max_push_blocks = 16;
    level->max_push_block_end_pos = 8;
    level->max_doors = 16;
    level->max_teleports = 4;
    level->max_giz_randoms = 8;
    level->max_torp_machines = 2;
    level->max_spinner_anim_objs = 40;

    level->wind_size = 512.0f;
    level->wind_speed = 3.0f;

    level->max_turrets = 32;
    level->max_bombgens = 8;
    level->max_bridges = 5;
    level->max_plugs = 10;
}

LEVELDATA *Level_FindByName(char *name, i32 *idx_out) {
    for (i32 i = 0; i < LEVELCOUNT; i++) {
        if (NuStrICmp(LDataList[i].name, name) == 0) {
            if (idx_out != NULL) {
                *idx_out = i;
            }

            return &LDataList[i];
        }
    }

    if (idx_out != NULL) {
        *idx_out = -1;
    }

    return NULL;
}

void Level_Draw(WORLDINFO *world) {
    void (*drawFn)(WORLDINFO *) = (void (*)(WORLDINFO *))world->current_level->draw_fn;
    if (drawFn != NULL) {
        drawFn(world);
    }
}

LEVELFIXUP LevFixUp;

void Levels_FixUp(LEVELFIXUP *fixup) {
    if (fixup != NULL) {
        for (; fixup->name != NULL; fixup++) {
            if (fixup->level != NULL) {
                LEVELDATA *level = Level_FindByName(fixup->name, NULL);
                *fixup->level = level;
                if (level != NULL) {
                    if (fixup->load_fn != NULL) {
                        level->load_fn = fixup->load_fn;
                    }
                    if (fixup->init_fn != NULL) {
                        level->init_fn = fixup->init_fn;
                    }
                    if (fixup->reset_fn != NULL) {
                        level->reset_fn = fixup->reset_fn;
                    }
                    if (fixup->update_fn != NULL) {
                        level->update_fn = fixup->update_fn;
                    }
                    if (fixup->always_update_fn != NULL) {
                        level->always_update_fn = fixup->always_update_fn;
                    }
                    if (fixup->draw_fn != NULL) {
                        level->draw_fn = fixup->draw_fn;
                    }
                    if (fixup->draw_status_fn != NULL) {
                        level->draw_status_fn = fixup->draw_status_fn;
                    }
                }
            }
        }
    }
}

void Level_RegisterGameConfigKeywords(nufpcomjmp_s *beforeLoadKeywords, nufpcomjmp_s *afterLoadKeywords) {
    Level_ConfigBeforeLoad_GameKeywords = beforeLoadKeywords;
    Level_ConfigAfterLoad_GameKeywords = afterLoadKeywords;
}

void FixUpLevels(LEVELFIXUP *fixup) {
    Levels_FixUp(fixup);

    LEVELDATA *level = Level_FindByName("titles", NULL);
    TITLES_LDATA = level;
    if (level != NULL) {
        level->init_fn = (void *)Titles_Init;
        level->update_fn = (void *)Titles_Update;
        level->draw_fn = (void *)Titles_Draw;
        level->flags &= ~(LEVEL_GAMEPLAY | LEVEL_TERRAIN);
        level->music_index = (i16)GetMusicIndex("titles", MusicInfo, -1);

        i32 handle = music_man.GetTrackHandle(TRACK_CLASS_QUIET, "titles");
        level->music_tracks[0][1] = handle;
        level->music_tracks[0][0] = handle;

        handle = music_man.GetTrackHandle(TRACK_CLASS_ACTION, "titles");
        level->music_tracks[1][1] = handle;
        level->music_tracks[1][0] = handle;

        handle = music_man.GetTrackHandle(TRACK_CLASS_NOMUSIC, "titles");
        level->music_tracks[2][1] = handle;
        level->music_tracks[2][0] = handle;
    }

    level = Level_FindByName("status", NULL);
    STATUS_LDATA = level;
    if (level != NULL) {
        level->update_fn = (void *)UpdateStatusScreen;
        level->draw_status_fn = (void *)DrawStatusScreen;
        level->flags = (level->flags & ~(LEVEL_GAMEPLAY | LEVEL_TERRAIN)) | LEVEL_STATUS;
    }

    {
        LEVELDATA *level = Level_FindByName("credits", NULL);
        CREDITS_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)Credits_Init_Game;
            level->update_fn = (void *)Credits_Update_Game;
            level->draw_fn = (void *)Credits_Draw_Game;
            level->draw_status_fn = (void *)Credits_DrawPanel;
            level->flags = 0;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("map", NULL);
        if (level != NULL) {
            level->load_fn = (void *)Hub_Load;
            level->init_fn = (void *)Hub_Init;
            level->reset_fn = (void *)Hub_Reset;
            level->update_fn = (void *)Hub_Update;
            level->draw_fn = (void *)Hub_Draw3D;
            level->draw_status_fn = (void *)Hub_DrawPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("speederchase_a", NULL);
        SPEEDERCHASEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)SpeederChaseA_Init;
            level->reset_fn = (void *)SpeederChaseA_Reset;
            level->update_fn = (void *)SpeederChaseA_Update;
            level->draw_status_fn = (void *)SpeederChaseA_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("negotiations_a", NULL);
        NEGOTIATIONSA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)NegotiationsA_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("negotiations_b", NULL);
        NEGOTIATIONSB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)NegotiationsB_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("negotiations_c", NULL);
        NEGOTIATIONSC_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("gungan_a", NULL);
        GUNGAN_A_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)GunganA_Init;
            level->update_fn = (void *)GunganA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("gungan_b", NULL);
        GUNGAN_B_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("rescue_a", NULL);
        RESCUEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RescueA_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("rescue_b", NULL);
        RESCUEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RescueB_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("rescue_c", NULL);
        RESCUEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RescueC_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("rescue_e", NULL);
        RESCUEE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RescueE_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Arrival1", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Arrival2", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Arrival3", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Arrival4", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Intro", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_B", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_C", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_A", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_A", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Outro2", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodRace_Status", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("ANewHope_Intro", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("ANewHope_A", NULL);
        if (level != NULL) {
            level->init_fn = (void *)ANewHopeA_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("ANewHope_B", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("ANewHope_Status", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("podrace_b", NULL);
        PODRACEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)PodRaceBInit;
            level->reset_fn = (void *)PodRaceBReset;
            level->update_fn = (void *)PodRaceBUpdate;
            level->always_update_fn = (void *)PodRaceAlwasyUpdate;
            level->draw_status_fn = (void *)PodRacePanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("podrace_a", NULL);
        PODRACEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)PodRaceAInit;
            level->reset_fn = (void *)PodRaceAReset;
            level->update_fn = (void *)PodRaceAUpdate;
            level->always_update_fn = (void *)PodRaceA_AlwaysUpdate;
            level->draw_fn = (void *)PodRaceADraw;
            level->draw_status_fn = (void *)PodRacePanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("podrace_c", NULL);
        PODRACEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)PodRaceCInit;
            level->reset_fn = (void *)PodRaceCReset;
            level->update_fn = (void *)PodRaceCUpdate;
            level->always_update_fn = (void *)PodRaceAlwasyUpdate;
            level->draw_status_fn = (void *)PodRacePanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("?", NULL);
        PODRACEOUTRO1_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("podrace_status", NULL);
        PODRACESTATUS_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("PodSprint_A", NULL);
        PODSPRINTA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)PodSprintA_Init;
            level->reset_fn = (void *)PodSprintA_Reset;
            level->update_fn = (void *)PodSprintA_Update;
            level->draw_status_fn = (void *)PodSprintA_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("AnakinsFlight_B", NULL);
        ANAKINSFLIGHTB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)AnakinsFlightB_Init;
            level->update_fn = (void *)AnakinsFlightB_Update;
            level->draw_fn = (void *)AnakinsFlightB_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_intro1", NULL);
        RETAKEINTRO1_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_intro2", NULL);
        RETAKEINTRO2_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_intro3", NULL);
        RETAKEINTRO3_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_b", NULL);
        RETAKEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_d", NULL);
        RETAKED_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RetakeD_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_e", NULL);
        RETAKEE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RetakeE_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("retake_g", NULL);
        RETAKEG_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)RetakeG_Init;
            level->reset_fn = (void *)RetakeG_Reset;
            level->update_fn = (void *)RetakeG_Update;
            level->draw_status_fn = (void *)RetakeG_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Maul_A", NULL);
        MAULA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MaulA_Init;
            level->reset_fn = (void *)MaulA_Reset;
            level->update_fn = (void *)MaulA_Update;
            level->draw_status_fn = (void *)MaulA_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Maul_B", NULL);
        MAULB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MaulB_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Maul_D", NULL);
        MAULD_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MaulD_Init;
            level->update_fn = (void *)MaulD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Maul_E", NULL);
        MAULE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MaulE_Init;
            level->update_fn = (void *)MaulE_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Maul_F", NULL);
        MAULF_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MaulF_Init;
            level->reset_fn = (void *)MaulF_Reset;
            level->update_fn = (void *)MaulF_Update;
            level->draw_status_fn = (void *)MaulF_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Jedi_B", NULL);
        JEDI_B_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)JediB_Init;
            level->reset_fn = (void *)JediB_Reset;
            level->update_fn = (void *)JediB_Update;
            level->draw_status_fn = (void *)JediB_DrawPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Jedi_Outro", NULL);
        JEDI_OUTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("gunship_a", NULL);
        GUNSHIPA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)GunshipA_Init;
            level->update_fn = (void *)GunshipA_Update;
            level->draw_fn = (void *)GunshipA_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("gunship_b", NULL);
        GUNSHIPB_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)GunshipB_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("bonus_gunship_a", NULL);
        BONUS_GUNSHIPA_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)BonusGunshipA_Reset;
            level->update_fn = (void *)BonusGunshipA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("bonus_gunship_b", NULL);
        BONUS_GUNSHIPB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BonusGunshipB_Init;
            level->reset_fn = (void *)BonusGunshipB_Reset;
            level->update_fn = (void *)BonusGunshipB_Update;
            level->draw_status_fn = (void *)BonusGunshipB_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("pursuit_a", NULL);
        BOUNTYHUNTERPURSUITA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BountyHunterPursuitA_Init;
            level->reset_fn = (void *)BountyHunterPursuitA_Reset;
            level->update_fn = (void *)BountyHunterPursuitA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("pursuit_b", NULL);
        BOUNTYHUNTERPURSUITB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BountyHunterPursuitB_Init;
            level->reset_fn = (void *)BountyHunterPursuitB_Reset;
            level->update_fn = (void *)BountyHunterPursuitB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("pursuit_c", NULL);
        BOUNTYHUNTERPURSUITC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BountyHunterPursuitC_Init;
            level->reset_fn = (void *)BountyHunterPursuitC_Reset;
            level->update_fn = (void *)BountyHunterPursuitC_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("pursuit_d", NULL);
        BOUNTYHUNTERPURSUITD_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BountyHunterPursuitD_Init;
            level->reset_fn = (void *)BountyHunterPursuitD_Reset;
            level->update_fn = (void *)BountyHunterPursuitD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("pursuit_e", NULL);
        BOUNTYHUNTERPURSUITE_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dogfight_a", NULL);
        if (level != NULL) {
            level->init_fn = (void *)ChrisDogFightAInit;
            level->reset_fn = (void *)ChrisDogFightAReset;
            level->update_fn = (void *)ChrisDogFightAUpdate;
            level->draw_fn = (void *)ChrisDogFightADraw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Factory_B", NULL);
        FACTORYB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)FactoryB_Init;
            level->reset_fn = (void *)FactoryB_Reset;
            level->update_fn = (void *)FactoryB_Update;
            level->draw_fn = (void *)FactoryB_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Factory_D", NULL);
        FACTORYD_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Factory_F", NULL);
        FACTORYF_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Factory_G", NULL);
        FACTORYG_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)FactoryG_Init;
            level->update_fn = (void *)FactoryG_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Dooku_C", NULL);
        DOOKUC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DookuC_Init;
            level->reset_fn = (void *)DookuC_Reset;
            level->update_fn = (void *)DookuC_Update;
            level->draw_status_fn = (void *)DookuC_DrawPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Dooku_Outro", NULL);
        DOOKUOUTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Dooku_Outro", NULL);
        if (level != NULL) {
            level->always_update_fn = (void *)KaminoA_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("?", NULL);
        if (level != NULL) {
            level->reset_fn = (void *)KaminoC_Reset;
            level->init_fn = (void *)KaminoC_Init;
            level->update_fn = (void *)KaminoC_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kamino_D", NULL);
        KAMINOD_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KaminoD_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kamino_Outro1", NULL);
        KAMINOOUTRO_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KaminoOutro_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kamino_Outro1", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("?", NULL);
        KAMINOC_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kamino_E", NULL);
        KAMINOE_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)KaminoE_Reset;
            level->init_fn = (void *)KaminoE_Init;
            level->always_update_fn = (void *)KaminoE_AlwaysUpdate;
            level->update_fn = (void *)KaminoE_Update;
            level->draw_fn = (void *)KaminoE_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kamino_F", NULL);
        KAMINOF_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KaminoF_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("NB_Kamino_a", NULL);
        NB_KAMINOALDATA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)NbKaminoA_Init;
            level->always_update_fn = (void *)KaminoA_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Dogfight_A", NULL);
        DOGFIGHTA_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Cruiser_A", NULL);
        CRUISERA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)CruiserAInit;
            level->update_fn = (void *)CruiserAUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Cruiser_B", NULL);
        CRUISERB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Cruiser_C", NULL);
        CRUISERC_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)CruiserCReset;
            level->update_fn = (void *)CruiserCUpdate;
            level->draw_status_fn = (void *)CruiserCPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Cruiser_D", NULL);
        CRUISERD_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)CruiserDInit;
            level->reset_fn = (void *)CruiserDReset;
            level->update_fn = (void *)CruiserDUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Cruiser_E", NULL);
        CRUISERE_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("cruiser_g", NULL);
        CRUISERG_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Grievous_A", NULL);
        GRIEVOUSA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)GrievousA_Init;
            level->reset_fn = (void *)GrievousA_Reset;
            level->update_fn = (void *)GrievousA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("temple_A", NULL);
        TEMPLEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)TempleA_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("temple_b", NULL);
        TEMPLEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("temple_c", NULL);
        TEMPLEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)TempleC_Init;
            level->always_update_fn = (void *)TempleC_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("temple_status", NULL);
        TEMPLESTATUS_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kashyyyk_A", NULL);
        KASHYYYKA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KashyyykA_Init;
            level->reset_fn = (void *)KashyyykA_Reset;
            level->update_fn = (void *)KashyyykA_Update;
            level->draw_status_fn = (void *)KashyyykA_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kashyyyk_B", NULL);
        KASHYYYKB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KashyyykB_Init;
            level->reset_fn = (void *)KashyyykB_Reset;
            level->update_fn = (void *)KashyyykB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kashyyyk_C", NULL);
        KASHYYYKC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KashyyykC_Init;
            level->update_fn = (void *)KashyyykC_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Kashyyyk_D", NULL);
        KASHYYYKD_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)KashyyykD_Init;
            level->reset_fn = (void *)KashyyykD_Reset;
            level->update_fn = (void *)KashyyykD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("vader_a", NULL);
        VADERA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)VaderA_Init;
            level->reset_fn = (void *)VaderA_Reset;
            level->update_fn = (void *)VaderA_Update;
            level->draw_status_fn = (void *)VaderA_DrawPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("vader_b", NULL);
        VADERB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)VaderB_Init;
            level->reset_fn = (void *)VaderB_Reset;
            level->update_fn = (void *)VaderB_Update;
            level->draw_status_fn = (void *)VaderB_DrawPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("vader_c", NULL);
        VADERC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)VaderC_Init;
            level->reset_fn = (void *)VaderC_Reset;
            level->update_fn = (void *)VaderC_Update;
            level->draw_status_fn = (void *)VaderC_DrawPanel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("BlockadeRunner_B", NULL);
        BLOCKADERUNNERB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BlockadeRunnerB_Init;
            level->update_fn = (void *)BlockadeRunnerB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("BlockadeRunner_C", NULL);
        BLOCKADERUNNERC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)BlockadeRunnerC_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("BlockadeRunner_D", NULL);
        BLOCKADERUNNERD_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)BlockadeRunnerD_Reset;
            level->update_fn = (void *)BlockadeRunnerD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("BlockadeRunner_D", NULL);
        if (level != NULL) {
            level->init_fn = (void *)TatooineB_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("tatooine_c", NULL);
        TATOOINEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)TatooineC_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("moseisley_a", NULL);
        MOSEISLEYA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MosEisleyA_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("moseisley_b", NULL);
        MOSEISLEYB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MosEisleyB_Init;
            level->update_fn = (void *)MosEisleyB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("moseisley_c", NULL);
        MOSEISLEYC_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("moseisley_d", NULL);
        MOSEISLEYD_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MosEisleyD_Init;
            level->always_update_fn = (void *)MosEisleyD_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("moseisley_e", NULL);
        MOSEISLEYE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)MosEisleyE_Init;
            level->reset_fn = (void *)MosEisleyE_Reset;
            level->update_fn = (void *)MosEisleyE_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("tatooine_a", NULL);
        TATOOINEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)TatooineA_Init;
            level->update_fn = (void *)TatooineA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("tatooine_a", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("tatooine_d", NULL);
        TATOOINED_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)TatooineD_Init;
            level->update_fn = (void *)TatooineD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("tatooine_e", NULL);
        TATOOINEE_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarrescue_a", NULL);
        DEATHSTARRESCUEA_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarrescue_b", NULL);
        DEATHSTARRESCUEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStarRescueB_Init;
            level->update_fn = (void *)DeathStarRescueB_Update;
            level->always_update_fn = (void *)DeathStarRescueB_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarrescue_c", NULL);
        DEATHSTARRESCUEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStarRescueC_Init;
            level->always_update_fn = (void *)DeathStarRescueC_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarrescue_d", NULL);
        DEATHSTARRESCUED_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarrescue_e", NULL);
        DEATHSTARRESCUEE_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarescape_a", NULL);
        DEATHSTARESCAPEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStarEscapeA_Init;
            level->update_fn = (void *)DeathStarEscapeA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarescape_b", NULL);
        DEATHSTARESCAPEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStarEscapeB_Init;
            level->update_fn = (void *)DeathStarEscapeB_Update;
            level->always_update_fn = (void *)DeathStarEscapeB_AlwaysUpdate;
            level->draw_fn = (void *)DeathStarEscapeB_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarescape_c", NULL);
        DEATHSTARESCAPEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStarEscapeC_Init;
            level->update_fn = (void *)DeathStarEscapeC_Update;
            level->reset_fn = (void *)DeathStarEscapeC_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarescape_d", NULL);
        DEATHSTARESCAPED_LDATA = level;
        if (level != NULL) {
            level->update_fn = (void *)DeathStarEscapeD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarbattle_a", NULL);
        DEATHSTARBATTLEA_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarbattle_b", NULL);
        DEATHSTARBATTLEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarbattle_c", NULL);
        DEATHSTARBATTLEC_LDATA = level;
        if (level != NULL) {
            level->always_update_fn = (void *)DeathStarBattleC_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("deathstarbattle_d", NULL);
        DEATHSTARBATTLED_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStarBattleDInit;
            level->update_fn = (void *)DeathStarBattleDUpdate;
            level->draw_fn = (void *)DeathStarBattleDDraw;
            level->reset_fn = (void *)DeathStarBattleDReset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothbattle_a", NULL);
        HOTHBATTLEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothBattleA_Init;
            level->reset_fn = (void *)HothBattleA_Reset;
            level->update_fn = (void *)HothBattleA_Update;
            level->draw_fn = (void *)HothBattleA_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothbattle_b", NULL);
        HOTHBATTLEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothbattle_c", NULL);
        HOTHBATTLEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothBattleC_Init;
            level->reset_fn = (void *)HothBattleC_Reset;
            level->update_fn = (void *)HothBattleC_Update;
            level->draw_fn = (void *)HothBattleC_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothbattle_d", NULL);
        HOTHBATTLED_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothbattle_e", NULL);
        HOTHBATTLEE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothBattleE_Init;
            level->update_fn = (void *)HothBattleE_Update;
            level->draw_fn = (void *)HothBattleE_Draw;
            level->draw_status_fn = (void *)HothBattleE_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("HothBattle_Outro", NULL);
        HOTHBATTLEOUTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_a", NULL);
        HOTHESCAPEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothEscapeA_Init;
            level->update_fn = (void *)HothEscapeA_Update;
            level->reset_fn = (void *)HothEscapeA_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_b", NULL);
        HOTHESCAPEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothEscapeB_Init;
            level->update_fn = (void *)HothEscapeB_Update;
            level->reset_fn = (void *)HothEscapeB_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_c", NULL);
        HOTHESCAPEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothEscapeC_Init;
            level->update_fn = (void *)HothEscapeC_Update;
            level->always_update_fn = (void *)HothEscapeC_AlwaysUpdate;
            level->reset_fn = (void *)HothEscapeC_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_d", NULL);
        HOTHESCAPED_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)HothEscapeD_Init;
            level->update_fn = (void *)HothEscapeD_Update;
            level->reset_fn = (void *)HothEscapeD_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("AsteroidChase_A", NULL);
        ASTEROIDCHASEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)AsteroidChaseA_Init;
            level->reset_fn = (void *)AsteroidChaseA_Reset;
            level->update_fn = (void *)AsteroidChaseA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("AsteroidChase_B", NULL);
        ASTEROIDCHASEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)AsteroidChaseB_Init;
            level->reset_fn = (void *)AsteroidChaseB_Reset;
            level->update_fn = (void *)AsteroidChaseB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("AsteroidChase_C", NULL);
        ASTEROIDCHASEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)AsteroidChaseC_Init;
            level->reset_fn = (void *)AsteroidChaseC_Reset;
            level->update_fn = (void *)AsteroidChaseC_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("AsteroidChase_D", NULL);
        ASTEROIDCHASED_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)AsteroidChaseD_Init;
            level->update_fn = (void *)AsteroidChaseD_Update;
            level->draw_fn = (void *)AsteroidChaseD_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("AsteroidChase_Midtro", NULL);
        ASTEROIDCHASEMITRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dagobah_a", NULL);
        DAGOBAHA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DagobahA_Init;
            level->update_fn = (void *)DagobahA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dagobah_d", NULL);
        DAGOBAHD_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dagobah_d", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dagobah_d", NULL);
        if (level != NULL) {
            level->init_fn = (void *)DagobahE_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dagobah_b", NULL);
        DAGOBAHB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DagobahB_Init;
            level->reset_fn = (void *)DagobahB_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dagobah_c", NULL);
        DAGOBAHC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DagobahC_Init;
            level->draw_status_fn = (void *)DagobahC_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("CloudCityTrap_A", NULL);
        CLOUDCITYTRAPA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)CloudCityTrapA_Init;
            level->update_fn = (void *)CloudCityTrapA_Update;
            level->reset_fn = (void *)CloudCityTrapA_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("CloudCityTrap_B", NULL);
        CLOUDCITYTRAPB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)CloudCityTrapB_Init;
            level->update_fn = (void *)CloudCityTrapB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("CloudCityTrap_C", NULL);
        CLOUDCITYTRAPC_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)CloudCityTrapC_Reset;
            level->update_fn = (void *)CloudCityTrapC_Update;
            level->draw_status_fn = (void *)CloudCityTrapC_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("CloudCityTrap_Outro", NULL);
        CLOUDCITYTRAPOUTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("cloudcityescape_a", NULL);
        CLOUDCITYESCAPEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)CloudCityEscapeA_Init;
            level->reset_fn = (void *)CloudCityEscapeA_Reset;
            level->update_fn = (void *)CloudCityEscapeA_Update;
            level->draw_status_fn = (void *)CloudCityEscapeA_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("cloudcityescape_b", NULL);
        CLOUDCITYESCAPEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("cloudcityescape_c", NULL);
        CLOUDCITYESCAPEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)CloudCityEscapeC_Init;
            level->update_fn = (void *)CloudCityEscapeC_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("jabbaspalace_a", NULL);
        JABBASPALACEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)JabbasPalaceA_Init;
            level->reset_fn = (void *)JabbasPalaceA_Reset;
            level->update_fn = (void *)JabbasPalaceA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("jabbaspalace_b", NULL);
        JABBASPALACEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)JabbasPalaceB_Init;
            level->reset_fn = (void *)JabbasPalaceB_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("jabbaspalace_d", NULL);
        JABBASPALACED_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)JabbasPalaceD_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("jabbaspalace_e", NULL);
        JABBASPALACEE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)JabbasPalaceE_Init;
            level->reset_fn = (void *)JabbasPalaceE_Reset;
            level->update_fn = (void *)JabbasPalaceE_Update;
            level->draw_status_fn = (void *)JabbasPalaceE_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("jabbaspalace_Outro", NULL);
        JABBASPALACE_OUTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("endorbattle_a", NULL);
        ENDORBATTLEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)EndorBattleA_Init;
            level->update_fn = (void *)EndorBattleA_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("endorbattle_b", NULL);
        ENDORBATTLEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("endorbattle_c", NULL);
        ENDORBATTLEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)EndorBattleC_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("endorbattle_d", NULL);
        ENDORBATTLED_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_A", NULL);
        DEATHSTAR2BATTLEA_LDATA = level;
        if (level != NULL) {
            level->always_update_fn = (void *)DeathStar2BattleA_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_B", NULL);
        DEATHSTAR2BATTLEB_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_D", NULL);
        DEATHSTAR2BATTLED_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStar2BattleD_Init;
            level->update_fn = (void *)DeathStar2BattleD_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_E", NULL);
        DEATHSTAR2BATTLEE_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStar2BattleFire_Init;
            level->update_fn = (void *)DeathStar2BattleFire_Update;
            level->draw_fn = (void *)DeathStar2BattleFire_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_F", NULL);
        DEATHSTAR2BATTLEF_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStar2BattleFire_Init;
            level->update_fn = (void *)DeathStar2BattleFire_Update;
            level->draw_fn = (void *)DeathStar2BattleFire_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_G", NULL);
        DEATHSTAR2BATTLEG_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)DeathStar2BattleFire_Init;
            level->update_fn = (void *)DeathStar2BattleFire_Update;
            level->draw_fn = (void *)DeathStar2BattleFire_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("DeathStar2Battle_Midtro", NULL);
        DEATHSTAR2BATTLEMIDTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("EmperorFight_A", NULL);
        EMPERORFIGHTA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)EmperorFightA_Init;
            level->reset_fn = (void *)EmperorFightA_Reset;
            level->update_fn = (void *)EmperorFightA_Update;
            level->draw_status_fn = (void *)EmperorFightA_Panel;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("SarlaccPit_A", NULL);
        SARLACCPITA_LDATA = level;
        if (level != NULL) {
            level->reset_fn = (void *)SarlaccPitA_Reset;
            level->draw_fn = (void *)SarlaccPitA_Draw;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("SarlaccPit_B", NULL);
        SARLACCPITB_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)SarlaccPitB_Init;
            level->reset_fn = (void *)SarlaccPitB_Reset;
            level->update_fn = (void *)SarlaccPitB_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("SarlaccPit_C", NULL);
        SARLACCPITC_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)SarlaccPitC_Init;
            level->reset_fn = (void *)SarlaccPitC_Reset;
            level->update_fn = (void *)SarlaccPitC_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("lego_city", NULL);
        LEGOCITY_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)LegoCity_Init;
            level->reset_fn = (void *)LegoCity_Reset;
            level->update_fn = (void *)LegoCity_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Senate_a", NULL);
        SENATEA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)SenateA_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("new_town", NULL);
        NEWTOWN_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)NewTown_Init;
            level->reset_fn = (void *)NewTown_Reset;
            level->update_fn = (void *)NewTown_Update;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("platform", NULL);
        PLATFORM_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)Platform_Init;
            level->reset_fn = (void *)Platform_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("E1CharacterBonus_A", NULL);
        E1CHARACTERBONUSA_LDATA = level;
        if (level != NULL) {
            level->init_fn = (void *)E1CharacterBonus_Init;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("E2VehicleBonus_A", NULL);
        E2VEHICLEBONUSA_LDATA = level;
        if (level != NULL) {
        }
    }
}

void Level_Update(WORLDINFO *world) {
    void (*updateFn)(WORLDINFO *) = (void (*)(WORLDINFO *))world->current_level->update_fn;
    if (updateFn != NULL) {
        updateFn(world);
    }

    if (DoubleScoreTime > 0.0f) {
        if ((i32)(GameTimer[0] + GameTimer[0]) != (i32)(GameTimer[1] + GameTimer[1])) {
            GameAudio_PlaySfxAndSetVolume(0x35, NULL, DoubleScoreTime);
        }
    }

    if (AreaGlobals == 0) {
        if (Cheats_CheckFlags(0x2000) != 0) {
            AreaGlobals = 1;
        }
    }
}

i32 LevelObject_GetReflection(i32 objId) {
    if (objId != -1 && LevObjRef_FirstObj <= objId && objId <= LevObjRef_LastObj) {
        objId = (objId - LevObjRef_FirstObj) + LevObjRef_FirstRefObj;
    }
    return objId;
}

char *LevelObject_FindNameFromIndex(i32 index) {
    char *name = (char *)"";
    if (ObjTabList != NULL && index >= 0 && index < LEVELOBJECTCOUNT) {
        name = ObjTabList[index].name;
    }
    return name;
}

i32 LevelObject_FindIndexFromName(char *name) {
    if (ObjTabList != NULL && LEVELOBJECTCOUNT > 0) {
        for (i32 i = 0; i < LEVELOBJECTCOUNT; i++) {
            if (NuStrICmp(ObjTabList[i].name, name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

i32 LevelObject_FindIndexFromName_RefOnly(char *name) {
    if (ObjTabList != NULL && LevObjRef_FirstObj != -1 && LevObjRef_FirstObj <= LevObjRef_LastObj) {
        for (i32 i = LevObjRef_FirstObj; i <= LevObjRef_LastObj; ++i) {
            if (NuStrICmp(ObjTabList[i].name, name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

i32 LevelObject_AddExtra(char *name, i32 kind) {
    if (LEVELOBJECTCOUNT < LEVELOBJECTMAX && ExtraLevelObject_NameTable != NULL) {
        i32 nameLen = NuStrLen(name);
        char *nameDest = ExtraLevelObject_NameTable + ExtraLevelObject_NameTableIndex;
        LEVELOBJECT *obj = &ObjTabList[LEVELOBJECTCOUNT];
        if (nameLen + 1 + ExtraLevelObject_NameTableIndex < ExtraLevelObject_NameTableSize) {
            obj->kind = (u8)kind;
            obj->name = nameDest;
            LEVELOBJECTCOUNT++;
            EXTRALEVELOBJECTCOUNT++;
            NuStrCpy(nameDest, name);
            ExtraLevelObject_NameTableIndex += nameLen + 1;
            return 1;
        }
    }
    return 0;
}

void GameAnimSys_ClearProgress(i32 idx) {
    if (idx < 0) {
        return;
    }
    if (idx >= gameanimsysprogress.count) {
        return;
    }
    u8 *data = gameanimsysprogress.entries[idx];
    if (gameanimsysprogress.entry_size <= 0) {
        return;
    }
    for (i32 i = 0; i < gameanimsysprogress.entry_size; i++) {
        data[i] = 0;
    }
}

void ClearLevelProgress(i32 index, WORLDINFO *world) {
    if (index >= 0) {
        u8 *entry = (u8 *)LevelProgressData + index * 0x2e24;
        memset(entry, 0, 0x2e24);
        *(u32 *)(entry + 0x281c) = 0;
        *(u32 *)(entry + 0x2810) = 0x49f42400;
        if (world != NULL) {
            memcpy(entry, (u8 *)world + 0x15c, 0x2800);
        }
    }
    GizmoSysClearLevelProgress(NULL, index);
    GameAnimSys_ClearProgress(index);
}

void SetLevelExBlowupFlags(u32 flags) {
    EXBLOWUPFLAGS = flags;
}

u32 GetLevelExBlowupFlags(void) {
    return EXBLOWUPFLAGS;
}

void GoToNewLevel(i32 levelIdx) {
    NewLData = &LDataList[levelIdx];
    if (waiting_for_level != -1) {
        waiting_for_new_level = 1;
    }
}

void LevelConfig_BeforeLoad(LEVELDATA *level, char *buffer, nufpcomjmp_s *keywords) {
    NUFPAR *fp = NuFParCreateMem("levelbeforeload", buffer, 0xffff);
    if (fp != NULL) {
        levelconfig_ldata = level;
        NuFParPushCom2(fp, LevelConfig_BeforeLoad_GenericKeywords, keywords);
        while (NuFParGetLine(fp) != 0) {
            if (NuFParGetWord(fp) != 0) {
                NuFParInterpretWord(fp);
            }
        }
        NuFParDestroy(fp);
    }
}

void LevelConfig_AfterLoad(LEVELDATA *level, char *buffer, nufpcomjmp_s *keywords) {
    NUFPAR *fp = NuFParCreateMem("levelafterload", buffer, 0xffff);
    if (fp != NULL) {
        levelconfig_ldata = level;
        NuFParPushCom2(fp, LevelConfig_AfterLoad_GenericKeywords, keywords);
        while (NuFParGetLine(fp) != 0) {
            if (NuFParGetWord(fp) != 0) {
                NuFParInterpretWord(fp);
            }
        }
        NuFParDestroy(fp);
        if (level->blob_shadow_fade_far < level->blob_shadow_fade_near) {
            level->blob_shadow_fade_near = level->blob_shadow_fade_far;
        }
        level->field91_0x118 = level->data_display.unknown_14;
        level->flags |= LEVEL_CONFIG_LOADED;
    }
}

void Level_LoadConfigFile(WORLDINFO *world) {
    char name[140];

    ConfigBuffer[0] = '\0';
    sprintf(name, "%s.txt", world->config_file);

    world->giz_buffer.void_ptr = (void *)ALIGN((usize)world->giz_buffer.void_ptr, 4);
    i32 bytesRead = NuFileLoadBuffer(name, world->giz_buffer.void_ptr, 0x10000);
    world->config_count = bytesRead;
    if (bytesRead > 0) {
        ((char *)world->giz_buffer.void_ptr)[bytesRead] = '\0';
        bytesRead = Text_StripComments((char *)world->giz_buffer.void_ptr, ConfigBuffer, 1);
        world->config_count = bytesRead;
    }
}
