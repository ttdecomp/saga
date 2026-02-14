#include "game/level.h"

#include "nu2api.saga/nuandroid/ios_graphics.h"
#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"

LEVELDATA *LDataList = NULL;
LEVELDATA *NEWGAME_LDATA = NULL;
LEVELDATA *LOADGAME_LDATA = NULL;

static i32 MAXLDATA = 0x64;

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
            cur_level->data_display.level_width = 20000.0f;
            cur_level->data_display.level_depth = 20100.0f;
            cur_level->data_display.unknown_10 = g_isLowEndDevice ? 4.0f : 1.0f;
            cur_level->data_display.unknown_17 = 0;
            cur_level->data_display.unknown_16 = 0;
            cur_level->data_display.unknown_19 = 0;
            cur_level->data_display.unknown_18 = 0;
            cur_level->data_display.unknown_1b = 0;
            cur_level->data_display.unknown_1a = 0;

            cur_level->music_index = -1;

            cur_level->unknown_11c = 0.0f;
            cur_level->unknown_120 = 1.0f;

            cur_level->unknown_0a2 = -1;
            cur_level->unknown_0a4 = 0x80;
            cur_level->unknown_0a6 = 0x100;
            cur_level->unknown_0a8 = -1;
            cur_level->unknown_0aa = -1;

            cur_level->unknown_0ac = 0x03;
            cur_level->unknown_0ad = 0x7f;
            cur_level->unknown_0ae = -1;
            cur_level->unknown_0af = -1;

            cur_level->unknown_0b8 = 0x50;

            cur_level->unknown_0b0 = 0;
            cur_level->unknown_0b4 = 0;

            cur_level->unknown_0b9 = 0x50;
            cur_level->unknown_0ba = 0x50;
            cur_level->unknown_0bb = 0x32;
            cur_level->unknown_0bc = 0x00;
            cur_level->unknown_0bd = 0x00;
            cur_level->unknown_0be = 0x00;
            cur_level->unknown_0bf = 0x00;

            cur_level->unknown_0c0 = 0.5f;
            cur_level->unknown_0c4 = 0.0f;
            cur_level->unknown_0c8 = 0.0f;
            cur_level->unknown_0cc = 2e+06f;

            cur_level->unknown_0d4 = -1;
            cur_level->unknown_0d5 = 5;
            cur_level->unknown_0d6 = 10;
            cur_level->unknown_0d7 = 5;
            cur_level->unknown_0d8 = 5;
            cur_level->unknown_0d9 = 10;
            cur_level->unknown_0da = 5;
            cur_level->unknown_0db = 5;

            cur_level->unknown_0dc = 0.0f;
            cur_level->unknown_0e0 = 0.0f;

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
    level->max_antinodes = 0x100;
    level->max_gizmo_blowups = 0x40;
    level->max_gizmo_blowup_types = 0x40;

    level->unknown_0ea = 0x100;
    level->unknown_0ec = 0x80;
    level->unknown_0ee = 0x80;
    level->unknown_0f0 = 0x80;
    level->unknown_0f2 = 0x10;

    level->field55_0xf4 = 10;
    level->max_giz_timers = 8;
    level->field57_0xf6 = 10;
    level->field58_0xf7 = 10;
    level->field59_0xf8 = 10;
    level->field60_0xf9 = 10;
    level->field61_0xfa = 10;
    level->max_obstacles = 32;
    level->field63_0xfc = 0x10;
    level->field64_0xfd = 0x40;
    level->field65_0xfe = 4;
    level->field66_0xff = 8;
    level->field67_0x100 = 2;
    level->field68_0x101 = 0x20;
    level->field69_0x102 = 5;
    level->field70_0x103 = 10;
    level->field71_0x104 = 0x20;
    level->field72_0x105 = 0x60;
    level->field73_0x106 = 5;
    level->field74_0x107 = 5;
    level->field75_0x108 = 8;
    level->field76_0x109 = 2;
    level->field77_0x10a = 0x20;
    level->field78_0x10b = 0x10;
    level->field79_0x10c = 8;
    level->field80_0x10d = 0x10;
    level->field81_0x10e = 4;
    level->field82_0x10f = 8;
    level->field83_0x110 = 2;
    level->field84_0x111 = 0x28;
    level->field97_0x128 = 0x44000000;
    level->field96_0x124 = 0x40400000;
    level->field85_0x112 = 0x20;
    level->max_bombgens = 8;
    level->field87_0x114 = 5;
    level->field88_0x115 = 10;
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
