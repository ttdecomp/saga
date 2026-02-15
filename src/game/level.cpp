#include "game/level.h"

#include "nu2api.saga/nuandroid/ios_graphics.h"
#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"
#include "nu2api.saga/numusic/numusic.h"

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
                cur_level->flags &= ~LEVEL_UNKNOWN_FLAG_2;
                cur_level->flags &= ~LEVEL_UNKNOWN_FLAG_8;
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

            cur_level->flags = 0xe;

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
            cur_level->unknown_0af = -1;

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

            cur_level->unknown_0d4 = -1;
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

LEVELDATA *Level_FindByName(char *name, int *idx_out) {
    for (int i = 0; i < LEVELCOUNT; i++) {
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
