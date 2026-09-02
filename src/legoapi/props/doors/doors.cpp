#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Door_Reset() {
    Door_ExitName[0] = '\0';
    Door_Start = 0;
    Door_NextSock = -1;
}

void Doors_Check(WORLDINFO_s *, GameObject_s *) {
}

void Door_GoThrough(WORLDINFO_s *, DOOR_s *, i32) {
}

void Door_SetCutCam(DOOR_s *) {
}

void Doors_SetLastDoor(DOOR_s *) {
}

namespace {
    WORLDINFO_s *door_config_world;
    DOOR_s *door_config;

    void D_spline(NUFPAR *parser) {
        if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buf) > 63) {
            return;
        }

        NuStrCpy(door_config->name, parser->word_buf);
        door_config->spline = NuSplineFind(door_config_world->current_gscn, door_config->name);
        if (door_config->spline == NULL || door_config->spline->length < 4) {
            door_config->spline = NULL;
            door_config->name[0] = '\0';
            return;
        }

        for (i32 i = 0; i < door_config_world->door_count; i++) {
            DOOR_s *other = &door_config_world->doors[i];
            if (other->spline == door_config->spline) {
                door_config->spline = NULL;
                door_config->name[0] = '\0';
                return;
            }
        }
    }

    void D_level(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            i32 index = -1;
            Level_FindByName(parser->word_buf, &index);
            if (index != -1) {
                door_config->level = static_cast<i16>(index);
            }
        }
    }

    void D_level_freeplay(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            i32 index = -1;
            LEVELDATA_s *level = Level_FindByName(parser->word_buf, &index);
            if (index != -1 && (level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0) {
                door_config->freeplay_level = static_cast<i16>(index);
            }
        }
    }

    void D_cam_spline(NUFPAR *parser) {
        if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buf) > 31) {
            return;
        }
        NuStrCpy(door_config->camera_spline_name, parser->word_buf);
        door_config->camera_spline = NuSplineFind(door_config_world->current_gscn, door_config->camera_spline_name);
        if (door_config->camera_spline != NULL && door_config->camera_spline->length != 2) {
            door_config->camera_spline_name[0] = '\0';
            door_config->camera_spline = NULL;
        }
    }

    void D_cam_wait(NUFPAR *parser) {
        door_config->camera_wait = NuFParGetFloat(parser);
    }

    void D_cam_blend_time(NUFPAR *parser) {
        door_config->camera_blend_time = NuFParGetFloat(parser);
    }

    void D_cam_lookatplayers(NUFPAR *) {
        door_config->flags |= 2;
    }

    void D_one_way(NUFPAR *) {
        door_config->flags |= 1;
    }

    void D_two_player_only(NUFPAR *) {
        door_config->flags |= 0x10;
    }

    void D_do_not_use(NUFPAR *) {
        door_config->flags |= 4;
    }

    void D_next_sock(NUFPAR *parser) {
        i32 next_sock = NuFParGetInt(parser);
        if (static_cast<u32>(next_sock) < 64) {
            door_config->next_sock = static_cast<u8>(next_sock);
        }
    }

    void D_vehicle(NUFPAR *parser) {
        // Vehicle names are consumed here exactly as a list on the current
        // line. Their name-to-type callback is registered by the game layer.
        while (NuFParGetWord(parser) != 0) {
        }
    }

    void D_use_as_start(NUFPAR *) {
        door_config->flags |= 8;
    }

    void D_cut_scene(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            WORLDINFO_s *world = WorldInfo_CurrentlyLoading();
            door_config->cutscene = CutScene_Find(world->cutscene_sys, parser->word_buf);
        }
    }

    NUFPCOMJMP Door_ConfigKeywords[] = {
        {const_cast<char *>("spline"), D_spline},
        {const_cast<char *>("level"), D_level},
        {const_cast<char *>("level_freeplay"), D_level_freeplay},
        {const_cast<char *>("cam_spline"), D_cam_spline},
        {const_cast<char *>("cam_wait"), D_cam_wait},
        {const_cast<char *>("cam_blend_time"), D_cam_blend_time},
        {const_cast<char *>("cam_lookatplayers"), D_cam_lookatplayers},
        {const_cast<char *>("1_way"), D_one_way},
        {const_cast<char *>("one_way"), D_one_way},
        {const_cast<char *>("2_player_only"), D_two_player_only},
        {const_cast<char *>("2_players_only"), D_two_player_only},
        {const_cast<char *>("two_player_only"), D_two_player_only},
        {const_cast<char *>("two_players_only"), D_two_player_only},
        {const_cast<char *>("do_not_use"), D_do_not_use},
        {const_cast<char *>("next_sock"), D_next_sock},
        {const_cast<char *>("vehicle"), D_vehicle},
        {const_cast<char *>("use_as_start"), D_use_as_start},
        {const_cast<char *>("cut_scene"), D_cut_scene},
        {const_cast<char *>("cutscene"), D_cut_scene},
        {NULL, NULL},
    };
} // namespace

void Doors_Init(WORLDINFO_s *world) {
    Door_Last = NULL;
    world->start_door = NULL;

    DOOR_s *door = world->doors;
    for (i32 i = 0; door != NULL && i < world->door_count; i++, door++) {
        NUVEC *points = door->spline->pts;

        NuVecAdd(&door->pos, &points[0], &points[1]);
        NuVecAdd(&door->pos, &door->pos, &points[2]);
        NuVecAdd(&door->pos, &door->pos, &points[3]);
        NuVecScale(&door->pos, &door->pos, 0.25f);

        door->radius = NuVecDist(&door->pos, &points[0], NULL);
        for (i32 point = 1; point < 4; point++) {
            f32 distance = NuVecDist(&door->pos, &points[point], NULL);
            if (distance > door->radius) {
                door->radius = distance;
            }
        }
        door->radius *= 1.01f;

        NuVecSurfaceNormal(&door->normal, &points[0], &points[1], &points[3]);
        door->point0 = points[0];
        door->point1 = points[1];
        door->point3 = points[3];
        NuVecAdd(&door->opposite_midpoint, &door->point1, &door->point3);
        NuVecScale(&door->opposite_midpoint, &door->opposite_midpoint, 0.5f);
        NuVecSub(&door->opposite_midpoint, &door->opposite_midpoint, &door->point0);
        NuVecScale(&door->opposite_midpoint, &door->opposite_midpoint, 2.0f);
        NuVecAdd(&door->opposite_midpoint, &door->opposite_midpoint, &door->point0);
        NuPlnEqn(&door->plane, &door->point0, &door->point1, &door->point3);

        if (world->start_door == NULL && (door->flags & 8) != 0 && door->spline->length > 5) {
            world->start_door = door;
        }
    }
}

void Doors_Configure(WORLDINFO_s *world, char *config) {
    world->doors = NULL;
    if (world->current_gscn == NULL) {
        return;
    }

    NUFPAR *parser = NuFParCreateMem(const_cast<char *>("doors"), config, 0xffff);
    if (parser == NULL) {
        return;
    }

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
    DOOR_s *doors = reinterpret_cast<DOOR_s *>(world->giz_buffer.void_ptr);
    world->doors = doors;
    door_config_world = world;
    door_config = doors;
    NuFParPushCom(parser, Door_ConfigKeywords);

    bool in_door = false;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0) {
            continue;
        }

        if (!in_door) {
            if (NuStrICmp(parser->word_buf, const_cast<char *>("door_start")) != 0) {
                continue;
            }

            door_config->name[0] = '\0';
            door_config->camera_spline_name[0] = '\0';
            door_config->spline = NULL;
            door_config->pos = v000;
            door_config->radius = 1.0f;
            door_config->normal = v010;
            door_config->level = -1;
            door_config->freeplay_level = -1;
            door_config->next_sock = 0xff;
            door_config->flags = 0;
            door_config->vehicle = 0xff;
            door_config->active = 0;
            door_config->camera_spline = NULL;
            door_config->camera_wait = 0.0f;
            door_config->camera_blend_time = 1.0f;
            door_config->vehicle_mask = 0;
            door_config->vehicle_mode = 0;
            door_config->cutscene = NULL;
            in_door = true;
            continue;
        }

        if (NuStrICmp(parser->word_buf, const_cast<char *>("door_end")) != 0) {
            NuFParInterpretWord(parser);
            continue;
        }

        if (door_config->spline != NULL && door_config->level != -1) {
            if (door_config->freeplay_level == -1) {
                door_config->freeplay_level = door_config->level;
            }
            world->door_count++;
            door_config++;
        }
        in_door = false;
    }

    NuFParDestroy(parser);
    if (world->door_count > 0) {
        world->giz_buffer.addr = ALIGN(reinterpret_cast<usize>(door_config), 16);
    } else {
        world->doors = NULL;
    }
}

void PortalDoors_Update(WORLDINFO_s *) {
}
