#include "game/level.h"

#include "globals.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"

LEVELDATA *LDataList = NULL;
LEVELDATA *NEWGAME_LDATA = NULL;
LEVELDATA *LOADGAME_LDATA = NULL;

i32 MAXLDATA = 64;

LEVELDATA *Levels_ConfigureList(char *file, VARIPTR *param_2, VARIPTR *param_3, i32 maxLevelCount, i32 *levelcount,
                                void *levelSetDefaults) {

    nufpar_s *fp = NuFParCreate(file);
    int i = 0;

    MAXLDATA = maxLevelCount;

    bool bVar2 = false;
    LEVELDATA *pLVar4 = (LEVELDATA *)((usize)param_2->void_ptr + 3U & 0xfffffffc);
    param_2->void_ptr = pLVar4;

    LEVELDATA *level = pLVar4;

LAB_00484810:
    do {
        int iVar3 = NuFParGetLine(fp);
        while (true) {
            if (iVar3 == 0) {
                NuFParDestroy(fp);

                if (i == 0) {
                    pLVar4 = NULL;
                } else {
                    param_2->void_ptr = level;
                    if (levelcount == NULL) {
                        return pLVar4;
                    }
                    *levelcount = i;
                }

                return pLVar4;
            }

            NuFParGetWord(fp);

            char *a = fp->word_buf;

            if (*a == '\0')
                goto LAB_00484810;
            if (!bVar2) {
                if (NuStrICmp(a, "level_start") == 0 && i < MAXLDATA) {

                    level->dir[0] = '\0';
                    level->name[0] = '\0';
                    level->field2_0x60 = 0xffff;
                    level->field3_0x62 = (undefined2)i;
                    level->flags = (LEVELFLAGS)0xe;
                    // iVar3 = g_isLowEndDevice;

                    level->load_fn = (void *)0x0;
                    level->init_fn = (void *)0x0;
                    level->reset_fn = (void *)0x0;
                    level->update_fn = (void *)0x0;
                    level->always_update_fn = (void *)0x0;
                    level->draw_fn = (void *)0x0;
                    level->draw_status_fn = (void *)0x0;
                    (level->field12_0x84).field10_0x14 = 20000;
                    *(i64 *)&level->field12_0x84 = 0x3e19999a3dcccccd;
                    (level->field12_0x84).field8_0x8 = 0x469d0800469c4000;
                    // if (iVar3 == 0) {
                    f32 fVar5 = 1.0;
                    //} else {
                    // fVar5 = 4.0;
                    //}
                    (level->field12_0x84).field9_0x10 = fVar5;
                    (level->field12_0x84).field12_0x17 = 0;
                    (level->field12_0x84).field11_0x16 = 0;
                    (level->field12_0x84).field14_0x19 = 0;
                    (level->field12_0x84).field13_0x18 = 0;
                    (level->field12_0x84).field16_0x1b = 0;
                    (level->field12_0x84).field15_0x1a = 0;
                    level->music_index = -1;
                    level->field94_0x11c = 0.0;
                    level->field95_0x120 = 1.0;
                    level->field14_0xa2 = 0xffff;
                    level->field15_0xa4 = 0x80;
                    level->field16_0xa6 = 0x100;
                    level->field17_0xa8 = 0xffff;
                    level->field18_0xaa = 0xffff;
                    level->field19_0xac = 3;
                    level->field20_0xad = 0x7f;
                    level->field21_0xae = 0xff;
                    level->field22_0xaf = 0xff;
                    level->field25_0xb8 = 0x50;
                    level->field23_0xb0 = 0;
                    level->field24_0xb4 = 0;
                    level->field26_0xb9 = 0x50;
                    level->field27_0xba = 0x50;
                    level->field28_0xbb = 0x32;
                    level->field29_0xbc = 0;
                    level->field30_0xbd = 0;
                    level->field31_0xbe = 0;
                    level->field32_0xbf = 0;
                    level->field33_0xc0 = 0.5;
                    level->field34_0xc4 = 0;
                    level->field35_0xc8 = 0;
                    level->field36_0xcc = 0x49f42400;
                    level->field41_0xd4 = 0xff;
                    level->field42_0xd5 = 5;
                    level->field43_0xd6 = 10;
                    level->field44_0xd7 = 5;
                    level->field45_0xd8 = 5;
                    level->field46_0xd9 = 10;
                    level->field47_0xda = 5;
                    level->field48_0xdb = 5;
                    level->field49_0xdc = 0.0;
                    level->field50_0xe0 = 0.0;
                    level->music_tracks[0] = -1;
                    level->music_tracks[2] = -1;
                    level->music_tracks[4] = -1;
                    level->music_tracks[1] = -1;
                    level->music_tracks[3] = -1;
                    level->music_tracks[5] = -1;
                    bVar2 = true;
                    if (levelSetDefaults != NULL) {
                        // (*levelSetDefaults)(level);
                        void (*func)(LEVELDATA *) = (void (*)(LEVELDATA *))levelSetDefaults;
                        func(level);
                    }
                }
                goto LAB_00484810;
            }

            if (NuStrICmp(a, "level_end") != 0)
                break;

            LEVELFLAGS LVar1;

            bVar2 = false;
            if (((level->dir[0] == '\0') || (level->name[0] == '\0')) ||
                (LVar1 = level->flags, (LVar1 & LEVEL_TEST) != 0))
                goto LAB_00484810;
            if ((LVar1 & LEVEL_NEWGAME) != 0) {
                NEWGAME_LDATA = level;
            }
            if ((LVar1 & LEVEL_LOADGAME) != 0) {
                LOADGAME_LDATA = level;
            }

            i = i + 1;
            bVar2 = false;
            level = level + 1;
            iVar3 = NuFParGetLine(fp);
        }

        if (NuStrICmp(fp->word_buf, "dir") == 0) {
            if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buf) < 0x40) {
                NuStrCpy(level->dir, fp->word_buf);
            }
        } else {
            if (NuStrICmp(fp->word_buf, "file") == 0 && NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buf) < 32) {
                NuStrCpy(level->name, fp->word_buf);
            }
        }

        if (NuStrICmp(fp->word_buf, "test_level") == 0) {
            level->flags = level->flags | LEVEL_TEST;
        } else if (NuStrICmp(fp->word_buf, "intro_level") == 0) {
            level->flags = level->flags | LEVEL_INTRO;
        } else if (NuStrICmp(fp->word_buf, "midtro_level") == 0 ||
                   NuStrICmp(fp->word_buf, "cutscene_level") == 0) {
            level->flags = level->flags | LEVEL_MIDTRO;
        } else if (NuStrICmp(fp->word_buf, "outro_level") == 0) {
            level->flags = level->flags | LEVEL_OUTRO;
        } else if (NuStrICmp(fp->word_buf, "status_level") == 0) {
            level->flags = (LEVELFLAGS)(level->flags & 0xfffffff5 | LEVEL_STATUS);
        } else if (NuStrICmp(fp->word_buf, "newgame_level") == 0) {
            if (NEWGAME_LDATA == NULL) {
                level->flags = level->flags | LEVEL_NEWGAME;
            }
        } else if (NuStrICmp(fp->word_buf, "loadgame_level") == 0 && LOADGAME_LDATA == NULL) {
            level->flags = level->flags | LEVEL_LOADGAME;
        }
    } while (true);
}

void Level_SetDefaults(LEVELDATA *level) {
    level->field51_0xe4 = 0x400100;
    level->field52_0xe8 = 0x1000040;
    level->field53_0xec = 0x800080;
    level->field54_0xf0 = 0x100080;
    level->field55_0xf4 = 10;
    level->field56_0xf5 = 8;
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

// this function chooses the wrong registers for some reason???
LEVELDATA *Level_FindByName(char *name, int *indexDest) {
    for (int i = 0; i < LEVELCOUNT; i++) {
        if (NuStrICmp(LDataList[i].name, name) == 0) {
            if (indexDest != NULL) {
                *indexDest = i;
            }

            return &LDataList[i];
        }
    }

    if (indexDest != NULL) {
        *indexDest = -1;
    }

    return NULL;
}

void Level_Draw(WORLDINFO *world) {
    void (*drawFn)(WORLDINFO *) = (void (*)(WORLDINFO *))world->current_level->draw_fn;
    if (drawFn != NULL) {
        drawFn(world);
    }
}
