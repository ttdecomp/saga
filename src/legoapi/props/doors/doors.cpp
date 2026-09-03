#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void NewGameMode(void);
void Hub_ActivateDoorMenu(LEVELDATA_s **level);
extern "C" i32 gone_through_door_to_new_level;

namespace {
    i32 door_cutscenesnap;

    const f32 DOOR_BLOCK_VELOCITY = 10.0f;
} // namespace

void Door_Reset() {
    Door_ExitName[0] = '\0';
    Door_Start = 0;
    Door_NextSock = -1;
}

void Doors_Check(WORLDINFO_s *world, GameObject_s *object) {
    if (object->apiobj.field_0x287 != 0 ||
        (LEGOCONTEXT_DOOMED != -1 && LEGOCONTEXT_DOOMED == static_cast<i8>(object->field_0x7a5)) || NewLData != NULL) {
        return;
    }

    DOOR_s *doors = WORLD->doors;
    if (doors == NULL || (object->apiobj.field_0x1f4 & APIOBJECT_STATE_FLAG_IGNORE_DOORS) != 0) {
        return;
    }

    const f32 centre_height = (object->character_bottom + object->character_top) * object->apiobj.field_0xa8 * 0.5f;
    NUVEC previous_position = object->apiobj.start_position;
    previous_position.y += centre_height;
    NUVEC current_position = object->apiobj.position;
    current_position.y += centre_height;

    for (i32 index = 0; index < WORLD->door_count; ++index) {
        DOOR_s *door = &doors[index];
        if ((door->flags & DOOR_FLAG_DO_NOT_USE) != 0 || door->active != 0 ||
            (Mission_Active(NULL) != NULL && door->level != world->level_idx)) {
            continue;
        }

        NUVEC previous_offset;
        NUVEC current_offset;
        NuVecSub(&previous_offset, &previous_position, &door->pos);
        NuVecSub(&current_offset, &current_position, &door->pos);
        const f32 previous_distance = NuVecDot(&door->normal, &previous_offset);
        const f32 current_distance = NuVecDot(&door->normal, &current_offset);

        i32 crossing_direction = 0;
        if (previous_distance < 0.0f && current_distance >= 0.0f) {
            crossing_direction = 1;
        } else if ((door->flags & DOOR_FLAG_ONE_WAY) == 0 && previous_distance >= 0.0f && current_distance < 0.0f) {
            crossing_direction = 2;
        } else {
            continue;
        }

        const f32 interpolation = NuFabs(previous_distance) / (NuFabs(current_distance) + NuFabs(previous_distance));
        NUVEC intersection;
        intersection.x = previous_position.x + (current_position.x - previous_position.x) * interpolation;
        intersection.y = previous_position.y + (current_position.y - previous_position.y) * interpolation;
        intersection.z = previous_position.z + (current_position.z - previous_position.z) * interpolation;

        if (NuPtInPoly(&intersection, &door->point0, &door->point1, &door->point3, &door->plane) == 0 &&
            NuPtInPoly(&intersection, &door->point1, &door->opposite_midpoint, &door->point3, &door->plane) == 0) {
            continue;
        }

        if ((object->apiobj.flags_low & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            Door_GoThrough(world, door, 1);
            return;
        }

        if (NuFabs(door->normal.y) >= NuTrigTable[0xaaa]) {
            return;
        }

        object->apiobj.position = object->apiobj.start_position;
        NUVEC block_normal = door->normal;
        if (crossing_direction == 2) {
            NuVecRotateY(&block_normal, &block_normal, 0x8000);
        }
        object->apiobj.field_0x1fc -= block_normal.x * DOOR_BLOCK_VELOCITY;
        object->apiobj.field_0x204 -= block_normal.z * DOOR_BLOCK_VELOCITY;
        return;
    }
}

void Door_GoThrough(WORLDINFO_s *world, DOOR_s *door, i32 player_triggered) {
    (void)player_triggered;

    if (player != NULL) {
        player->KillTasks();
    }
    if (NewLData != NULL) {
        return;
    }
    if (door->level == -1) {
        ResetBits |= RESETBIT_DOOR_TRANSITION;
        Door_Last = door;
        if (Door_GoThrough_ExtraCodeFn != NULL) {
            Door_GoThrough_ExtraCodeFn(world, door);
        }
        return;
    }

    NuStrCpy(Door_ExitName, door->name);
    if (door->cutscene == NULL || CutScene_HasPlayed(static_cast<CUTINFO *>(door->cutscene))) {
        door_cutscenesnap = 0;
        const i32 destination_level = InStory() != 0 ? door->level : door->freeplay_level;
        if (destination_level == world->level_idx) {
            StartDoorPositions();
            NewGameMode();
            for (i32 index = 0; index < 8; ++index) {
                GameObject_s *current_player = Player[index];
                if (current_player == NULL) {
                    continue;
                }
                PlayerProgress[index].field_0xb = current_player->field_0xe31 == 1;
                PlayerProgress[index].field_0x9 =
                    current_player->apiobj.field_0x287 == 0 ? current_player->field_0xe38 : 4;
            }
            gone_through_door_to_new_mode = 1;
        } else {
            NewLData = &LDataList[destination_level];
            if (FreePlay != 0 && (NewLData->flags & LEVEL_OUTRO) != 0) {
                LEVELDATA_s *status_level = Area_FindStatusLevel(WORLD->area, NULL);
                if (status_level != NULL) {
                    NewLData = status_level;
                    if (ChallengeMode != 0) {
                        ChallengeMode = 4;
                    }
                }
            }
        }
    } else if (door_cutscenesnap == 0) {
        newmode_cutinfo = static_cast<CUTINFO *>(door->cutscene);
        StartDoorPositions();
        NewGameMode();
    } else {
        WORLDINFO_s *active_world = WorldInfo_CurrentlyActive();
        NewCutScene(static_cast<CUTINFO *>(door->cutscene), active_world->cutscene_sys, NULL, 0);
    }

    Door_NextSock = static_cast<i8>(door->next_sock);
    Door_SetCutCam(door);
    ResetBits |= RESETBIT_DOOR_TRANSITION;
    if (NewLData != NULL && NewLData != world->current_level) {
        if (world->area == HUB_ADATA) {
            Hub_ActivateDoorMenu(&NewLData);
        } else if (NewLData->area_index == world->level_sub_id) {
            gone_through_door_to_new_level = 1;
        }
    }

    Door_Last = door;
    if (Door_GoThrough_ExtraCodeFn != NULL) {
        Door_GoThrough_ExtraCodeFn(world, door);
    }
}

void Door_SetCutCam(DOOR_s *door) {
    Door_UseCutCam = 0;
    if (door->camera_spline == NULL) {
        if (door->camera_spline_name[0] == '\0' || door->level == WORLD->level_idx) {
            return;
        }
        Door_UseCutCam = 1;
        NuStrCpy(Door_ExitCameraSplineName, door->camera_spline_name);
    } else {
        Door_UseCutCam = 1;
        Door_CutCamPos0 = door->camera_spline->pts[0];
        Door_CutCamPos1 = door->camera_spline->pts[1];
    }
    Door_CutCamWaitTime = door->camera_wait;
    Door_CutCamWait = door->camera_wait;
    Door_CutCamBlendTime = door->camera_blend_time;
    Door_CutLookAtPlayers = door->flags & DOOR_FLAG_CAMERA_LOOK_AT_PLAYERS;
}

void Doors_SetLastDoor(DOOR_s *door) {
    if (door != NULL && door != setlastdoor_last) {
        NuStrCpy(Door_ExitName, door->name);
        StartDoorPositions();
        setlastdoor_last = door;
        Door_Last = door;
    }
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
        door_config->flags |= DOOR_FLAG_CAMERA_LOOK_AT_PLAYERS;
    }

    void D_one_way(NUFPAR *) {
        door_config->flags |= DOOR_FLAG_ONE_WAY;
    }

    void D_two_player_only(NUFPAR *) {
        door_config->flags |= DOOR_FLAG_TWO_PLAYER_ONLY;
    }

    void D_do_not_use(NUFPAR *) {
        door_config->flags |= DOOR_FLAG_DO_NOT_USE;
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
        door_config->flags |= DOOR_FLAG_USE_AS_START;
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
    setlastdoor_last = NULL;
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

        if (world->start_door == NULL && (door->flags & DOOR_FLAG_USE_AS_START) != 0 && door->spline->length > 5) {
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

void PortalDoors_Update(WORLDINFO_s *world) {
    PORTALDOOR *door = world->portal_doors;
    if (door == NULL || world->portal_door_count <= 0) {
        return;
    }

    i32 index = 0;
    do {
        nuinstanim_s *animation = NuSpecialGetInstAnim(&door->special);
        if (animation != NULL) {
            const f32 end_frame = NuAnimEndFrameOld(door->special.scene->instance_animation_data[animation->anim_ix]);
            const u16 flags = door->flags;
            bool closed = false;
            if ((flags & PORTALDOOR_TRIGGER_AT_END) == 0) {
                closed = animation->ltime <= 1.0f;
            } else {
                closed = animation->ltime < end_frame;
            }

            if (closed) {
                if ((flags & (PORTALDOOR_OPENED | PORTALDOOR_CLOSED)) != PORTALDOOR_CLOSED) {
                    NuPortalSetActive(world->current_gscn, door->portal_id, 0);
                    door->flags = static_cast<u16>((flags & ~PORTALDOOR_OPENED) | PORTALDOOR_CLOSED);
                }
            } else if ((flags & PORTALDOOR_OPENED) == 0) {
                NuPortalSetActive(world->current_gscn, door->portal_id, 1);
                door->flags = static_cast<u16>((flags & ~PORTALDOOR_CLOSED) | PORTALDOOR_OPENED);
            }
        }
        ++index;
        ++door;
    } while (index < world->portal_door_count);
}
