#include "legoapi/level.h"

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "legoapi/cheat.h"
#include "legoapi/gizmo.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/numusic.h"

extern "C" char *ConfigBuffer;

// Defined in legoapi/gameobjects.cpp
void GameAudio_PlaySfxAndSetVolume(i32, nuvec_s *, float);

// These are extern (U) in the original level.cpp.o — defined here as stubs
// until the original defining file is decompiled.
LEVELDATA *levelconfig_ldata = NULL;

// Keyword tables for generic level configuration keywords.
// These are combined with game-specific keyword tables by LevelConfig_BeforeLoad
// and LevelConfig_AfterLoad via NuFParPushCom2.
// Both tables exist in the original binary (defined in a separate compilation unit)
// and are terminated by {NULL, NULL}.
// TODO: Populate with actual keyword entries once extracted from the binary.
NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[] = {
    {NULL, NULL},
};

NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[] = {
    {NULL, NULL},
};

i32 Text_StripComments(char *text, char *dest, i32) {
    char *start = text;
    i32 length = NuStrLen(text);
    char *out = dest;
    while (text <= start + length) {
        char c = *text;
        if (c == '#' || c == ';' || (c == '/' && text[1] == '/')) {
            if (c == '/') {
                text++;
            }
            do {
                c = *++text;
            } while (c != '\r' && c != '\n' && c != '\0');
        } else {
            if (c == ',' || c == '=') {
                c = ' ';
            }
            *out++ = c;
        }
        text++;
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
    i32 *progress = (i32 *)&gameanimsysprogress;
    if (idx >= progress[0]) {
        return;
    }
    u8 *data = gameanimsysprogress.entries[idx];
    i32 size = progress[1];
    if (size <= 0) {
        return;
    }
    for (i32 i = 0; i < size; i++) {
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
            u32 *src = (u32 *)((u8 *)world + 0x15c);
            u32 *dst = (u32 *)entry;
            for (i32 i = 0xa00; i != 0; i--) {
                *dst++ = *src++;
            }
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
        level->flags |= 1;
    }
}

void Level_LoadConfigFile(WORLDINFO *world) {
    char name[140];

    ConfigBuffer[0] = '\0';
    sprintf(name, "%s.txt", world->config_file);

    world->giz_buffer.void_ptr = (void *)((usize)world->giz_buffer.void_ptr + 3U & ~3U);
    i32 bytesRead = NuFileLoadBuffer(name, world->giz_buffer.void_ptr, 0x10000);
    world->unknown_010c = bytesRead;
    if (bytesRead > 0) {
        ((char *)world->giz_buffer.void_ptr)[bytesRead] = '\0';
        bytesRead = Text_StripComments((char *)world->giz_buffer.void_ptr, ConfigBuffer, 0);
        world->unknown_010c = bytesRead;
    }
}
