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
#include "legoapi/world/levels/level_functions.h"

extern "C" char ConfigBuffer[0x10000];

// Defined in legoapi/gameobjects.cpp
void GameAudio_PlaySfxAndSetVolume(i32, nuvec_s *, float);

// These are extern (U) in the original level.cpp.o — defined here as stubs
// until the original defining file is decompiled.
LEVELDATA *levelconfig_ldata = NULL;

extern i16 GetMusicIndex(char *, nusound_filename_info_s *, i32);

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
    if (world->current_level->draw_fn != NULL) {
        world->current_level->draw_fn(world);
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

// Look up a level by name, cache it in a global, and install its handler functions.
// The if(fn) guards are constant-folded by -O3: a function name is always non-NULL
// (emits just the store), while NULL emits nothing - matching the original's code,
// which only stores the present (non-NULL) handlers. Args are init/reset/update/always/draw/status.
#define LEVEL_FIXUP_LDATA(name, ldata, init, reset, update, always, draw, status)                                      \
    {                                                                                                                  \
        LEVELDATA *level = Level_FindByName(name, NULL);                                                               \
        ldata = level;                                                                                                 \
        if (level != NULL) {                                                                                           \
            if (init)                                                                                                  \
                level->init_fn = init;                                                                                 \
            if (reset)                                                                                                 \
                level->reset_fn = reset;                                                                               \
            if (update)                                                                                                \
                level->update_fn = update;                                                                             \
            if (always)                                                                                                \
                level->always_update_fn = always;                                                                      \
            if (draw)                                                                                                  \
                level->draw_fn = draw;                                                                                 \
            if (status)                                                                                                \
                level->draw_status_fn = status;                                                                        \
        }                                                                                                              \
    }
#define LEVEL_FIXUP(name, init, reset, update, always, draw, status)                                                   \
    {                                                                                                                  \
        LEVELDATA *level = Level_FindByName(name, NULL);                                                               \
        if (level != NULL) {                                                                                           \
            if (init)                                                                                                  \
                level->init_fn = init;                                                                                 \
            if (reset)                                                                                                 \
                level->reset_fn = reset;                                                                               \
            if (update)                                                                                                \
                level->update_fn = update;                                                                             \
            if (always)                                                                                                \
                level->always_update_fn = always;                                                                      \
            if (draw)                                                                                                  \
                level->draw_fn = draw;                                                                                 \
            if (status)                                                                                                \
                level->draw_status_fn = status;                                                                        \
        }                                                                                                              \
    }

void FixUpLevels(LEVELFIXUP *fixup) {
    Levels_FixUp(fixup);

    LEVELDATA *level = Level_FindByName("titles", NULL);
    TITLES_LDATA = level;
    if (level != NULL) {
        level->init_fn = Titles_Init;
        level->update_fn = Titles_Update;
        level->draw_fn = Titles_Draw;
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
        level->update_fn = UpdateStatusScreen;
        level->draw_status_fn = DrawStatusScreen;
        level->flags = (level->flags & ~(LEVEL_GAMEPLAY | LEVEL_TERRAIN)) | LEVEL_STATUS;
    }

    {
        LEVELDATA *level = Level_FindByName("credits", NULL);
        CREDITS_LDATA = level;
        if (level != NULL) {
            level->init_fn = Credits_Init_Game;
            level->update_fn = Credits_Update_Game;
            level->draw_fn = Credits_Draw_Game;
            level->draw_status_fn = Credits_DrawPanel;
            level->flags = 0;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("map", NULL);
        if (level != NULL) {
            level->load_fn = Hub_Load;
            level->init_fn = Hub_Init;
            level->reset_fn = Hub_Reset;
            level->update_fn = Hub_Update;
            level->draw_fn = Hub_Draw3D;
            level->draw_status_fn = Hub_DrawPanel;
        }
    }

    LEVEL_FIXUP_LDATA("speederchase_a", SPEEDERCHASEA_LDATA, SpeederChaseA_Init, SpeederChaseA_Reset,
                      SpeederChaseA_Update, NULL, NULL, SpeederChaseA_Panel)

    LEVEL_FIXUP_LDATA("negotiations_a", NEGOTIATIONSA_LDATA, NegotiationsA_Init, NULL, NULL, NULL, NULL, NULL);

    LEVEL_FIXUP_LDATA("negotiations_b", NEGOTIATIONSB_LDATA, NegotiationsB_Init, NULL, NULL, NULL, NULL, NULL)

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
            level->init_fn = GunganA_Init;
            level->update_fn = GunganA_Update;
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
            level->init_fn = RescueA_Init;
        }
    }

    LEVEL_FIXUP_LDATA("rescue_b", RESCUEB_LDATA, RescueB_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("rescue_c", RESCUEC_LDATA, RescueC_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("rescue_e", RESCUEE_LDATA, RescueE_Init, NULL, NULL, NULL, NULL, NULL)

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

    LEVEL_FIXUP("ANewHope_A", ANewHopeA_Init, NULL, NULL, NULL, NULL, NULL);

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

    LEVEL_FIXUP_LDATA("podrace_b", PODRACEB_LDATA, PodRaceBInit, PodRaceBReset, PodRaceBUpdate, PodRaceAlwasyUpdate,
                      NULL, PodRacePanel);

    LEVEL_FIXUP_LDATA("podrace_a", PODRACEA_LDATA, PodRaceAInit, PodRaceAReset, PodRaceAUpdate, PodRaceA_AlwaysUpdate,
                      PodRaceADraw, PodRacePanel)

    LEVEL_FIXUP_LDATA("podrace_c", PODRACEC_LDATA, PodRaceCInit, PodRaceCReset, PodRaceCUpdate, PodRaceAlwasyUpdate,
                      NULL, PodRacePanel)

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

    LEVEL_FIXUP_LDATA("PodSprint_A", PODSPRINTA_LDATA, PodSprintA_Init, PodSprintA_Reset, PodSprintA_Update, NULL, NULL,
                      PodSprintA_Panel)

    LEVEL_FIXUP_LDATA("AnakinsFlight_B", ANAKINSFLIGHTB_LDATA, AnakinsFlightB_Init, NULL, AnakinsFlightB_Update, NULL,
                      AnakinsFlightB_Draw, NULL)

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

    LEVEL_FIXUP_LDATA("retake_d", RETAKED_LDATA, RetakeD_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("retake_e", RETAKEE_LDATA, RetakeE_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("retake_g", RETAKEG_LDATA, RetakeG_Init, RetakeG_Reset, RetakeG_Update, NULL, NULL, RetakeG_Panel)

    LEVEL_FIXUP_LDATA("Maul_A", MAULA_LDATA, MaulA_Init, MaulA_Reset, MaulA_Update, NULL, NULL, MaulA_Panel)

    LEVEL_FIXUP_LDATA("Maul_B", MAULB_LDATA, MaulB_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Maul_D", MAULD_LDATA, MaulD_Init, NULL, MaulD_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Maul_E", MAULE_LDATA, MaulE_Init, NULL, MaulE_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Maul_F", MAULF_LDATA, MaulF_Init, MaulF_Reset, MaulF_Update, NULL, NULL, MaulF_Panel)

    LEVEL_FIXUP_LDATA("Jedi_B", JEDI_B_LDATA, JediB_Init, JediB_Reset, JediB_Update, NULL, NULL, JediB_DrawPanel)

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
            level->init_fn = GunshipA_Init;
            level->update_fn = GunshipA_Update;
            level->draw_fn = GunshipA_Draw;
        }
    }

    LEVEL_FIXUP_LDATA("gunship_b", GUNSHIPB_LDATA, NULL, GunshipB_Reset, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("bonus_gunship_a", BONUS_GUNSHIPA_LDATA, NULL, BonusGunshipA_Reset, BonusGunshipA_Update, NULL,
                      NULL, NULL)

    LEVEL_FIXUP_LDATA("bonus_gunship_b", BONUS_GUNSHIPB_LDATA, BonusGunshipB_Init, BonusGunshipB_Reset,
                      BonusGunshipB_Update, NULL, NULL, BonusGunshipB_Panel)

    LEVEL_FIXUP_LDATA("pursuit_a", BOUNTYHUNTERPURSUITA_LDATA, BountyHunterPursuitA_Init, BountyHunterPursuitA_Reset,
                      BountyHunterPursuitA_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("pursuit_b", BOUNTYHUNTERPURSUITB_LDATA, BountyHunterPursuitB_Init, BountyHunterPursuitB_Reset,
                      BountyHunterPursuitB_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("pursuit_c", BOUNTYHUNTERPURSUITC_LDATA, BountyHunterPursuitC_Init, BountyHunterPursuitC_Reset,
                      BountyHunterPursuitC_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("pursuit_d", BOUNTYHUNTERPURSUITD_LDATA, BountyHunterPursuitD_Init, BountyHunterPursuitD_Reset,
                      BountyHunterPursuitD_Update, NULL, NULL, NULL)

    {
        LEVELDATA *level = Level_FindByName("pursuit_e", NULL);
        BOUNTYHUNTERPURSUITE_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("dogfight_a", NULL);
        if (level != NULL) {
            level->init_fn = ChrisDogFightAInit;
            level->reset_fn = ChrisDogFightAReset;
            level->update_fn = ChrisDogFightAUpdate;
            level->draw_fn = ChrisDogFightADraw;
        }
    }

    LEVEL_FIXUP_LDATA("Factory_B", FACTORYB_LDATA, FactoryB_Init, FactoryB_Reset, FactoryB_Update, NULL, FactoryB_Draw,
                      NULL)

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

    LEVEL_FIXUP_LDATA("Factory_G", FACTORYG_LDATA, FactoryG_Init, NULL, FactoryG_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Dooku_C", DOOKUC_LDATA, DookuC_Init, DookuC_Reset, DookuC_Update, NULL, NULL, DookuC_DrawPanel)

    {
        LEVELDATA *level = Level_FindByName("Dooku_Outro", NULL);
        DOOKUOUTRO_LDATA = level;
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("Dooku_Outro", NULL);
        if (level != NULL) {
            level->always_update_fn = KaminoA_AlwaysUpdate;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("?", NULL);
        if (level != NULL) {
            level->reset_fn = KaminoC_Reset;
            level->init_fn = KaminoC_Init;
            level->update_fn = KaminoC_Update;
        }
    }

    LEVEL_FIXUP_LDATA("Kamino_D", KAMINOD_LDATA, KaminoD_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Kamino_Outro1", KAMINOOUTRO_LDATA, KaminoOutro_Init, NULL, NULL, NULL, NULL, NULL)

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
            level->reset_fn = KaminoE_Reset;
            level->init_fn = KaminoE_Init;
            level->always_update_fn = KaminoE_AlwaysUpdate;
            level->update_fn = KaminoE_Update;
            level->draw_fn = KaminoE_Draw;
        }
    }

    LEVEL_FIXUP_LDATA("Kamino_F", KAMINOF_LDATA, KaminoF_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("NB_Kamino_a", NB_KAMINOALDATA_LDATA, NbKaminoA_Init, NULL, NULL, KaminoA_AlwaysUpdate, NULL,
                      NULL)

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
            level->init_fn = CruiserAInit;
            level->update_fn = CruiserAUpdate;
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
            level->reset_fn = CruiserCReset;
            level->update_fn = CruiserCUpdate;
            level->draw_status_fn = CruiserCPanel;
        }
    }

    LEVEL_FIXUP_LDATA("Cruiser_D", CRUISERD_LDATA, CruiserDInit, CruiserDReset, CruiserDUpdate, NULL, NULL, NULL)

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

    LEVEL_FIXUP_LDATA("Grievous_A", GRIEVOUSA_LDATA, GrievousA_Init, GrievousA_Reset, GrievousA_Update, NULL, NULL,
                      NULL)

    LEVEL_FIXUP_LDATA("temple_A", TEMPLEA_LDATA, TempleA_Init, NULL, NULL, NULL, NULL, NULL)

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
            level->init_fn = TempleC_Init;
            level->always_update_fn = TempleC_AlwaysUpdate;
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
            level->init_fn = KashyyykA_Init;
            level->reset_fn = KashyyykA_Reset;
            level->update_fn = KashyyykA_Update;
            level->draw_status_fn = KashyyykA_Panel;
        }
    }

    LEVEL_FIXUP_LDATA("Kashyyyk_B", KASHYYYKB_LDATA, KashyyykB_Init, KashyyykB_Reset, KashyyykB_Update, NULL, NULL,
                      NULL)

    LEVEL_FIXUP_LDATA("Kashyyyk_C", KASHYYYKC_LDATA, KashyyykC_Init, NULL, KashyyykC_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Kashyyyk_D", KASHYYYKD_LDATA, KashyyykD_Init, KashyyykD_Reset, KashyyykD_Update, NULL, NULL,
                      NULL)

    LEVEL_FIXUP_LDATA("vader_a", VADERA_LDATA, VaderA_Init, VaderA_Reset, VaderA_Update, NULL, NULL, VaderA_DrawPanel)

    LEVEL_FIXUP_LDATA("vader_b", VADERB_LDATA, VaderB_Init, VaderB_Reset, VaderB_Update, NULL, NULL, VaderB_DrawPanel)

    LEVEL_FIXUP_LDATA("vader_c", VADERC_LDATA, VaderC_Init, VaderC_Reset, VaderC_Update, NULL, NULL, VaderC_DrawPanel)

    LEVEL_FIXUP_LDATA("BlockadeRunner_B", BLOCKADERUNNERB_LDATA, BlockadeRunnerB_Init, NULL, BlockadeRunnerB_Update,
                      NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("BlockadeRunner_C", BLOCKADERUNNERC_LDATA, BlockadeRunnerC_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("BlockadeRunner_D", BLOCKADERUNNERD_LDATA, NULL, BlockadeRunnerD_Reset, BlockadeRunnerD_Update,
                      NULL, NULL, NULL)

    LEVEL_FIXUP("BlockadeRunner_D", TatooineB_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("tatooine_c", TATOOINEC_LDATA, TatooineC_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("moseisley_a", MOSEISLEYA_LDATA, MosEisleyA_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("moseisley_b", MOSEISLEYB_LDATA, MosEisleyB_Init, NULL, MosEisleyB_Update, NULL, NULL, NULL)

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
            level->init_fn = MosEisleyD_Init;
            level->always_update_fn = MosEisleyD_AlwaysUpdate;
        }
    }

    LEVEL_FIXUP_LDATA("moseisley_e", MOSEISLEYE_LDATA, MosEisleyE_Init, MosEisleyE_Reset, MosEisleyE_Update, NULL, NULL,
                      NULL)

    LEVEL_FIXUP_LDATA("tatooine_a", TATOOINEA_LDATA, TatooineA_Init, NULL, TatooineA_Update, NULL, NULL, NULL)

    {
        LEVELDATA *level = Level_FindByName("tatooine_a", NULL);
        if (level != NULL) {
        }
    }

    {
        LEVELDATA *level = Level_FindByName("tatooine_d", NULL);
        TATOOINED_LDATA = level;
        if (level != NULL) {
            level->init_fn = TatooineD_Init;
            level->update_fn = TatooineD_Update;
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

    LEVEL_FIXUP_LDATA("deathstarrescue_b", DEATHSTARRESCUEB_LDATA, DeathStarRescueB_Init, NULL, DeathStarRescueB_Update,
                      DeathStarRescueB_AlwaysUpdate, NULL, NULL)

    LEVEL_FIXUP_LDATA("deathstarrescue_c", DEATHSTARRESCUEC_LDATA, DeathStarRescueC_Init, NULL, NULL,
                      DeathStarRescueC_AlwaysUpdate, NULL, NULL)

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

    LEVEL_FIXUP_LDATA("deathstarescape_a", DEATHSTARESCAPEA_LDATA, DeathStarEscapeA_Init, NULL, DeathStarEscapeA_Update,
                      NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("deathstarescape_b", DEATHSTARESCAPEB_LDATA, DeathStarEscapeB_Init, NULL, DeathStarEscapeB_Update,
                      DeathStarEscapeB_AlwaysUpdate, DeathStarEscapeB_Draw, NULL)

    {
        LEVELDATA *level = Level_FindByName("deathstarescape_c", NULL);
        DEATHSTARESCAPEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = DeathStarEscapeC_Init;
            level->update_fn = DeathStarEscapeC_Update;
            level->reset_fn = DeathStarEscapeC_Reset;
        }
    }

    LEVEL_FIXUP_LDATA("deathstarescape_d", DEATHSTARESCAPED_LDATA, NULL, NULL, DeathStarEscapeD_Update, NULL, NULL,
                      NULL)

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

    LEVEL_FIXUP_LDATA("deathstarbattle_c", DEATHSTARBATTLEC_LDATA, NULL, NULL, NULL, DeathStarBattleC_AlwaysUpdate,
                      NULL, NULL)

    {
        LEVELDATA *level = Level_FindByName("deathstarbattle_d", NULL);
        DEATHSTARBATTLED_LDATA = level;
        if (level != NULL) {
            level->init_fn = DeathStarBattleDInit;
            level->update_fn = DeathStarBattleDUpdate;
            level->draw_fn = DeathStarBattleDDraw;
            level->reset_fn = DeathStarBattleDReset;
        }
    }

    LEVEL_FIXUP_LDATA("hothbattle_a", HOTHBATTLEA_LDATA, HothBattleA_Init, HothBattleA_Reset, HothBattleA_Update, NULL,
                      HothBattleA_Draw, NULL)

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
            level->init_fn = HothBattleC_Init;
            level->reset_fn = HothBattleC_Reset;
            level->update_fn = HothBattleC_Update;
            level->draw_fn = HothBattleC_Draw;
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
            level->init_fn = HothBattleE_Init;
            level->update_fn = HothBattleE_Update;
            level->draw_fn = HothBattleE_Draw;
            level->draw_status_fn = HothBattleE_Panel;
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
            level->init_fn = HothEscapeA_Init;
            level->update_fn = HothEscapeA_Update;
            level->reset_fn = HothEscapeA_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_b", NULL);
        HOTHESCAPEB_LDATA = level;
        if (level != NULL) {
            level->init_fn = HothEscapeB_Init;
            level->update_fn = HothEscapeB_Update;
            level->reset_fn = HothEscapeB_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_c", NULL);
        HOTHESCAPEC_LDATA = level;
        if (level != NULL) {
            level->init_fn = HothEscapeC_Init;
            level->update_fn = HothEscapeC_Update;
            level->always_update_fn = HothEscapeC_AlwaysUpdate;
            level->reset_fn = HothEscapeC_Reset;
        }
    }

    {
        LEVELDATA *level = Level_FindByName("hothescape_d", NULL);
        HOTHESCAPED_LDATA = level;
        if (level != NULL) {
            level->init_fn = HothEscapeD_Init;
            level->update_fn = HothEscapeD_Update;
            level->reset_fn = HothEscapeD_Reset;
        }
    }

    LEVEL_FIXUP_LDATA("AsteroidChase_A", ASTEROIDCHASEA_LDATA, AsteroidChaseA_Init, AsteroidChaseA_Reset,
                      AsteroidChaseA_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("AsteroidChase_B", ASTEROIDCHASEB_LDATA, AsteroidChaseB_Init, AsteroidChaseB_Reset,
                      AsteroidChaseB_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("AsteroidChase_C", ASTEROIDCHASEC_LDATA, AsteroidChaseC_Init, AsteroidChaseC_Reset,
                      AsteroidChaseC_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("AsteroidChase_D", ASTEROIDCHASED_LDATA, AsteroidChaseD_Init, NULL, AsteroidChaseD_Update, NULL,
                      AsteroidChaseD_Panel, NULL)

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
            level->init_fn = DagobahA_Init;
            level->update_fn = DagobahA_Update;
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

    LEVEL_FIXUP("dagobah_d", DagobahE_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("dagobah_b", DAGOBAHB_LDATA, DagobahB_Init, DagobahB_Reset, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("dagobah_c", DAGOBAHC_LDATA, DagobahC_Init, NULL, NULL, NULL, NULL, DagobahC_Panel)

    {
        LEVELDATA *level = Level_FindByName("CloudCityTrap_A", NULL);
        CLOUDCITYTRAPA_LDATA = level;
        if (level != NULL) {
            level->init_fn = CloudCityTrapA_Init;
            level->update_fn = CloudCityTrapA_Update;
            level->reset_fn = CloudCityTrapA_Reset;
        }
    }

    LEVEL_FIXUP_LDATA("CloudCityTrap_B", CLOUDCITYTRAPB_LDATA, CloudCityTrapB_Init, NULL, CloudCityTrapB_Update, NULL,
                      NULL, NULL)

    LEVEL_FIXUP_LDATA("CloudCityTrap_C", CLOUDCITYTRAPC_LDATA, NULL, CloudCityTrapC_Reset, CloudCityTrapC_Update, NULL,
                      NULL, CloudCityTrapC_Panel)

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
            level->init_fn = CloudCityEscapeA_Init;
            level->reset_fn = CloudCityEscapeA_Reset;
            level->update_fn = CloudCityEscapeA_Update;
            level->draw_status_fn = CloudCityEscapeA_Panel;
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
            level->init_fn = CloudCityEscapeC_Init;
            level->update_fn = CloudCityEscapeC_Update;
        }
    }

    LEVEL_FIXUP_LDATA("jabbaspalace_a", JABBASPALACEA_LDATA, JabbasPalaceA_Init, JabbasPalaceA_Reset,
                      JabbasPalaceA_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("jabbaspalace_b", JABBASPALACEB_LDATA, JabbasPalaceB_Init, JabbasPalaceB_Reset, NULL, NULL, NULL,
                      NULL)

    LEVEL_FIXUP_LDATA("jabbaspalace_d", JABBASPALACED_LDATA, NULL, JabbasPalaceD_Reset, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("jabbaspalace_e", JABBASPALACEE_LDATA, JabbasPalaceE_Init, JabbasPalaceE_Reset,
                      JabbasPalaceE_Update, NULL, NULL, JabbasPalaceE_Panel)

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
            level->init_fn = EndorBattleA_Init;
            level->update_fn = EndorBattleA_Update;
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
            level->init_fn = EndorBattleC_Init;
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
            level->always_update_fn = DeathStar2BattleA_AlwaysUpdate;
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
            level->init_fn = DeathStar2BattleD_Init;
            level->update_fn = DeathStar2BattleD_Update;
        }
    }

    LEVEL_FIXUP_LDATA("DeathStar2Battle_E", DEATHSTAR2BATTLEE_LDATA, DeathStar2BattleFire_Init, NULL,
                      DeathStar2BattleFire_Update, NULL, DeathStar2BattleFire_Draw, NULL)

    LEVEL_FIXUP_LDATA("DeathStar2Battle_F", DEATHSTAR2BATTLEF_LDATA, DeathStar2BattleFire_Init, NULL,
                      DeathStar2BattleFire_Update, NULL, DeathStar2BattleFire_Draw, NULL)

    LEVEL_FIXUP_LDATA("DeathStar2Battle_G", DEATHSTAR2BATTLEG_LDATA, DeathStar2BattleFire_Init, NULL,
                      DeathStar2BattleFire_Update, NULL, DeathStar2BattleFire_Draw, NULL)

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
            level->init_fn = EmperorFightA_Init;
            level->reset_fn = EmperorFightA_Reset;
            level->update_fn = EmperorFightA_Update;
            level->draw_status_fn = EmperorFightA_Panel;
        }
    }

    LEVEL_FIXUP_LDATA("SarlaccPit_A", SARLACCPITA_LDATA, NULL, SarlaccPitA_Reset, NULL, NULL, SarlaccPitA_Draw, NULL)

    LEVEL_FIXUP_LDATA("SarlaccPit_B", SARLACCPITB_LDATA, SarlaccPitB_Init, SarlaccPitB_Reset, SarlaccPitB_Update, NULL,
                      NULL, NULL)

    LEVEL_FIXUP_LDATA("SarlaccPit_C", SARLACCPITC_LDATA, SarlaccPitC_Init, SarlaccPitC_Reset, SarlaccPitC_Update, NULL,
                      NULL, NULL)

    LEVEL_FIXUP_LDATA("lego_city", LEGOCITY_LDATA, LegoCity_Init, LegoCity_Reset, LegoCity_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("Senate_a", SENATEA_LDATA, SenateA_Init, NULL, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("new_town", NEWTOWN_LDATA, NewTown_Init, NewTown_Reset, NewTown_Update, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("platform", PLATFORM_LDATA, Platform_Init, Platform_Reset, NULL, NULL, NULL, NULL)

    LEVEL_FIXUP_LDATA("E1CharacterBonus_A", E1CHARACTERBONUSA_LDATA, E1CharacterBonus_Init, NULL, NULL, NULL, NULL,
                      NULL)

    {
        LEVELDATA *level = Level_FindByName("E2VehicleBonus_A", NULL);
        E2VEHICLEBONUSA_LDATA = level;
        if (level != NULL) {
        }
    }
}

void Level_Update(WORLDINFO *world) {
    void (*updateFn)(WORLDINFO *) = world->current_level->update_fn;
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
