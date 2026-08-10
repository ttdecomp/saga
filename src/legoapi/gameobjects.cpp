#include "legoapi/gameobject.h"

#include <string.h>

#include "legoapi/touchhacks.h"
#include "nu2api/numath/nufloat.h"

struct CharacterRuntimeData {
    u8 unknown_000[0x4c];
    f32 animated_radius;
    u8 unknown_050[0xbd];
    i8 bounds_joint_index;
    i8 head_joint_index;
    i8 origin_joint_index;
};

struct CharacterInfo {
    u8 unknown_00[8];
    union {
        u32 flags;
        struct {
            u8 flags_0;
            u8 flags_1;
            u8 flags_2;
            u8 flags_3;
        };
    };
    u8 unknown_0c[4];
};

struct GameObjectWorld {
    u8 unknown_0000[0x128];
    void *current_level;
    u8 unknown_012c[0x2860];
    APIOBJECTSYS *object_system;
    u8 unknown_2990[0x158];
    AISYS *ai_system;
};

struct TerrainEntry {
    u32 unknown_0;
    u32 flags;
    u32 unknown_8;
};

extern CharacterInfo *CInfo;
extern u8 *GameCam;
extern u8 grapple_attach_frames;
extern NUVEC plr_lastpos;
extern f32 FRAMETIME;
extern "C" GameObjectWorld *WORLD;
extern i32 VehicleArea;
extern i32 BonusArea;
extern i16 id_SPEEDERBIKE;
extern i16 id_SPEEDERBIKESNOW;
extern i16 id_STAP;
extern i16 id_STAP2;
extern i16 id_TROOPERCANNON;
extern i16 id_CANNON;
extern i16 id_MOSCANNON;
extern i16 id_ATST;
extern i16 id_BASKETCANNON;
extern i16 id_BIGGUN;
extern i16 id_TRAININGREMOTE;
extern GameObject_s *LevGameObject;
extern void *NEWTOWN_LDATA;
extern i32 LEGOCONTEXT_DOOMED;
extern i32 LEGOCONTEXT_BEENTAKENOVER;
extern i32 LEGOCONTEXT_EATEN;
extern i32 LEGOCONTEXT_GETIN;
extern i32 LevSafePlatID[2];
extern TerrainEntry TerLayer[];
extern TerrainEntry TerSurface[];

i32 (*LastSafePosExtraFn)(GameObject_s *object);
void (*GameObjectDimensionsExtraFn)(GameObject_s *object);
i32 HIGHGAMEOBJECT;
GameObject_s BackUpPlayers[2];
GameObject_s *Obj;

extern "C" i32 NuPortalWhichRoom(NUGSCN *scene, NUVEC *position);
extern "C" void AISysGetCharacterPathPos(AISYS *ai_system, GameObject_s *object, AISCRIPTPROCESS **ai_process, i32 mask,
                                         i32 update);

void InitSurfaceInfo(GameObject_s *object);
void SetObjOnSurface(GameObject_s *object, i32 force);
void DestroySnakeBody(GameObject_s *object);
bool NoLayerKill(GameObject_s *object);

MechObjectInterface *GameObject_s::GetMechObjectInterface() {
    if (mech_object_interface == NULL) {
        new CharacterObjectInterface(*this);
    }

    return mech_object_interface;
}

void GameObject_s::KillTasks() {
    MechTouchTask *task = task_list;
    while (task != NULL) {
        MechTouchTask *next = task->next;
        task->Kill();
        delete task;
        task = next;
    }
    task_list = NULL;
}

bool GameObject_s::IsRunningTaskType(const HashedKey &key) {
    MechTouchTask *task = task_list;
    while (task != NULL) {
        MechTouchTask *next = task->next;
        if (task->GetTaskType().value == key.value) {
            return true;
        }
        task = next;
    }
    return false;
}

void GameObject_s::ClearMechObjectInterface() {
    if (mech_object_interface != NULL) {
        delete mech_object_interface;
    }
}

MechAddonCollection *GameObject_s::GetAddons(bool create) {
    if (addons == NULL && create) {
        addons = new MechAddonCollection(GetMechObjectInterface());
    }
    return addons;
}

void GameObject_s::ClearAddons() {
    delete addons;
    addons = NULL;
}

GameObject_s *AddGameObject(i32 character_id) {
    GameObject_s *object = (GameObject_s *)APIObjectCreate(WORLD->object_system);
    if (object == NULL) {
        return NULL;
    }

    u8 mask_index = object->mask_bit_index;
    object->sock_index = -1;
    object->edge_surface_type = -1;
    object->roof_surface_type = -1;
    object->draw_orientation_mode = 2;
    object->state_flags |= 0x1001;
    object->layer_mask = 1;
    if ((mask_index & 0x20) == 0) {
        object->self_mask_low = 1 << (mask_index & 0x1f);
        object->self_mask_high = 0;
    } else {
        object->self_mask_low = 0;
        object->self_mask_high = 1 << (mask_index & 0x1f);
    }
    object->scale = 1.0f;
    object->height_scale = 1.0f;
    object->reflection_plane_y = 2000000.0f;
    object->ground_surface_type = -1;
    object->roof_height = 2000000.0f;
    object->edge_shadow_height = 2000000.0f;
    object->floor_height = 2000000.0f;

    HIGHGAMEOBJECT = 0;
    for (i32 i = 0; i < 64; i++) {
        if ((Obj[i].state_flags & 1) != 0) {
            HIGHGAMEOBJECT = i + 1;
        }
    }

    object->ai_owner_object = object;
    object->in_range_mask_low = 0;
    object->in_range_mask_high = 0;
    object->self = object;
    object->ai_process = &object->ai_script_process;

    MechAddonCollection *collection = object->GetAddons(true);
    if (collection != NULL) {
        MechObjectInterface *mech_interface = object->GetMechObjectInterface();
        collection->Add(*new MechEdgeStopAddon(*mech_interface));

        if (VehicleArea != 0 || character_id == id_SPEEDERBIKE || character_id == id_SPEEDERBIKESNOW ||
            character_id == id_STAP || character_id == id_STAP2 || character_id == id_TROOPERCANNON ||
            character_id == id_CANNON || character_id == id_MOSCANNON || character_id == id_ATST ||
            character_id == id_BASKETCANNON || character_id == id_BIGGUN) {
            collection->Add(*new MechAutofireAddon(*object->GetMechObjectInterface()));
        }
    }

    return object;
}

void RemoveGameObject(GameObject_s *object, i32) {
    if (object == NULL) {
        return;
    }

    object->KillTasks();
    object->ClearAddons();
    object->ClearMechObjectInterface();

    u8 mask_index = object->mask_bit_index;
    u32 low_mask = (mask_index & 0x20) == 0 ? 1 << (mask_index & 0x1f) : 0;
    u32 high_mask = (mask_index & 0x20) != 0 ? 1 << (mask_index & 0x1f) : 0;
    for (i32 i = 0; i < HIGHGAMEOBJECT; i++) {
        Obj[i].collision_mask_low &= ~object->self_mask_low;
        Obj[i].collision_mask_high &= ~object->self_mask_high;
        Obj[i].opponent_mask_low &= ~low_mask;
        Obj[i].opponent_mask_high &= ~high_mask;
        Obj[i].seen_mask_low &= ~low_mask;
        Obj[i].seen_mask_high &= ~high_mask;
        Obj[i].can_see_mask_low &= ~low_mask;
        Obj[i].can_see_mask_high &= ~high_mask;
    }

    if (object->player_packet.game_pad != NULL) {
        ((u8 *)object->player_packet.game_pad)[0x5a] &= ~1;
    }

    DestroySnakeBody(object);
    APIObjectDestroy(WORLD->object_system, object);

    HIGHGAMEOBJECT = 0;
    for (i32 i = 0; i < 64; i++) {
        if ((Obj[i].state_flags & 1) != 0) {
            HIGHGAMEOBJECT = i + 1;
        }
    }

    if (LevGameObject == object) {
        LevGameObject = NULL;
    }
}

void GameObjectDimensions(GameObject_s *object) {
    f32 radius;
    f32 half_height;

    if (object->terrain_contact_age != 0 && object->player_packet.damage_scale == 1.0f) {
        CharacterRuntimeData *runtime = (CharacterRuntimeData *)object->character_data->field11_0x24;
        i8 joint_index = runtime->bounds_joint_index;

        if (joint_index != -1 && *(void **)((u8 *)object->game_character_data + 0x14 + joint_index * 4) != NULL &&
            object->drawn_this_frame != 0) {
            radius = object->height_scale * runtime->animated_radius;
            half_height = radius;
        } else {
            radius = object->collision_radius;
            half_height = object->collision_half_height;
        }
    } else {
        radius = object->collision_radius;
        half_height = object->collision_half_height;
    }

    object->bounds_radius = radius;
    object->bounds_half_height = half_height;

    if (GameObjectDimensionsExtraFn != NULL) {
        GameObjectDimensionsExtraFn(object);
    }
}

void ScaleGameObject(GameObject_s *object) {
    CHARACTERDATA *character_data = object->character_data;

    object->model_scale = character_data->field13_0x2c * object->scale;
    object->collision_radius = object->scale * object->base_collision_radius;
    object->collision_half_height = object->collision_radius * object->half_height_ratio;
    object->model_height = (character_data->field16_0x38 - character_data->field15_0x34) * object->height_scale;
}

void GetTopBot(GameObject_s *object) {
    f32 radius = object->base_collision_radius;
    object->model_bottom = object->character_data->field15_0x34;
    object->model_top = object->character_data->field16_0x38;
    if (radius == 0.0f) {
        object->half_height_ratio = 0.0f;
    } else {
        object->half_height_ratio = (object->model_top - object->model_bottom) / (radius + radius);
    }
}

void SetToLastSafePos(GameObject_s *object) {
    f32 y = object->last_safe_position.y;
    f32 z = object->last_safe_position.z;
    f32 x = object->last_safe_position.x;
    object->old_position.x = x;
    object->old_position.y = y;
    object->old_position.z = z;
    object->position = object->old_position;
    object->previous_safe_position = object->old_position;
}

i32 UnderPlayerControl(GameObject_s *object) {
    if ((i8)object->state_flags >= 0 && (object->player_packet.takeover_object == NULL ||
                                         (i8)object->player_packet.takeover_object->state_flags >= 0)) {
        return 0;
    }
    return 1;
}

GameObject_s *FindGameObject(i32 character_id, u32 object_flags, i32 alive_only, i32 vehicle_only, i32 non_level_only) {
    for (i32 i = 0; i < HIGHGAMEOBJECT; i++) {
        GameObject_s *object = &Obj[i];

        if ((object->state_flags & 1) != 0 &&
            (object_flags == 0 || (object->object_flags & object_flags) == object_flags) &&
            (vehicle_only == 0 || (object->state_flags & 0x1000) != 0) &&
            (character_id == -1 || object->character_id == character_id) &&
            (alive_only == 0 ||
             (object->death_state == 0 && (CInfo[object->player_packet.character_state].flags_1 & 0x80) == 0)) &&
            (non_level_only == 0 || object->level_object_index == -1)) {
            return object;
        }
    }

    return NULL;
}

GameObject_s *FindNearestGameObject(NUVEC *position, GameObject_s *source, u32 character_flags, f32 maximum_distance,
                                    f32 radius_extra, i32 game_character_index, i32 character_id, i32 player_filter,
                                    f32 *distance_squared, i32 horizontal_only, i32 (*filter)(GameObject_s *),
                                    bool first_match) {
    f32 maximum_distance_squared = maximum_distance * maximum_distance;
    f32 best_distance_squared = 100000000.0f;
    GameObject_s *nearest = NULL;

    for (i32 i = 0; i < HIGHGAMEOBJECT; i++) {
        GameObject_s *candidate = &Obj[i];
        if ((candidate->state_flags & 0x1001) != 0x1001 || candidate->death_state != 0 ||
            (candidate->player_packet.runtime_flags_0 & 0x20) != 0 ||
            (CInfo[candidate->player_packet.character_state].flags_1 & 0x80) != 0 ||
            (character_id != -1 && candidate->character_id != character_id)) {
            continue;
        }

        if (player_filter >= 0) {
            if (player_filter < 2) {
                if (candidate->player_index != player_filter) {
                    continue;
                }
            } else if (player_filter == 99) {
                if (candidate->player_index == -1) {
                    continue;
                }
            } else if (player_filter == 100 && candidate->player_index != -1) {
                continue;
            }
        }

        if (source != NULL && candidate == source) {
            continue;
        }
        if (character_flags != 0 &&
            (((u32)candidate->character_data->field1_0x4 & character_flags) != character_flags)) {
            continue;
        }
        if (game_character_index != -1 &&
            ((void **)*(void **)((u8 *)candidate->game_character_data + 0xc))[game_character_index] == NULL) {
            continue;
        }
        if (filter != NULL && !filter(candidate)) {
            continue;
        }

        f32 x = candidate->origin.x - position->x;
        f32 z = candidate->origin.z - position->z;
        f32 candidate_distance_squared;
        if (horizontal_only == 0) {
            f32 y = candidate->origin.y - position->y;
            candidate_distance_squared = x * x + y * y + z * z;
        } else {
            candidate_distance_squared = x * x + z * z;
        }

        if (maximum_distance_squared > 0.0f && candidate_distance_squared >= maximum_distance_squared) {
            continue;
        }
        if (source != NULL) {
            f32 combined_radius = source->collision_radius + candidate->collision_radius + radius_extra;
            if (candidate_distance_squared >= combined_radius * combined_radius) {
                continue;
            }
        }
        if (candidate_distance_squared < best_distance_squared) {
            nearest = candidate;
            best_distance_squared = candidate_distance_squared;
            if (first_match) {
                break;
            }
        }
    }

    if (nearest != NULL && distance_squared != NULL) {
        *distance_squared = best_distance_squared;
    }
    return nearest;
}

i32 GameObjectNearFloor(GameObject_s *object, f32 distance, f32 *floor_delta) {
    i32 near_floor;

    if (object->floor_height != 2000000.0f) {
        i32 integral_distance = (i32)distance;
        if (integral_distance < 0) {
            integral_distance = 0;
        }

        f32 threshold = integral_distance * 0.025f;
        threshold = MAX(threshold, object->collision_radius / 0.225f * threshold);

        f32 delta = object->bounds_min.y - object->floor_height;
        if (floor_delta != NULL) {
            *floor_delta = delta;
        }
        near_floor = delta < threshold;
    } else {
        if (floor_delta != NULL) {
            *floor_delta = 2000000.0f;
        }
        near_floor = false;
    }

    return near_floor;
}

void ClearGameObjects(APIOBJECTSYS *object_system) {
    for (i32 i = 0; i < 64; i++) {
        Obj[i].KillTasks();
        Obj[i].ClearAddons();
        Obj[i].ClearMechObjectInterface();
    }

    memset(Obj, 0, sizeof(GameObject_s) * 64);
    APIObjectDestroyAll(object_system);
    HIGHGAMEOBJECT = 0;
}

void GameObjectToCameraDistances() {
    f32 camera_x = *(f32 *)(GameCam + 0x11c);
    f32 camera_y = *(f32 *)(GameCam + 0x120);
    f32 camera_z = *(f32 *)(GameCam + 0x124);

    GameObject_s *object = Obj;
    for (i32 i = 0; i < HIGHGAMEOBJECT; i++, object++) {
        if ((object->state_flags_low & 0x1001) == 0x1001) {
            f32 x = camera_x - object->position.x;
            f32 y = camera_y - object->position.y;
            f32 z = camera_z - object->position.z;
            object->camera_distance = NuFsqrt(x * x + y * y + z * z);
        }
    }
}

void GameObjectOrigin(GameObject_s *object) {
    u32 original_flags = object->object_flags;
    object->object_flags |= 0x100;

    f32 delta_x = object->frame_offset.x * FRAMETIME;
    f32 delta_y = object->frame_offset.y * FRAMETIME;
    f32 delta_z = object->frame_offset.z * FRAMETIME;
    f32 origin_x;
    f32 origin_y;
    f32 origin_z;
    f32 predicted_x;
    f32 predicted_z;
    bool found_origin = false;

    if (object->terrain_contact_age != 0) {
        CharacterRuntimeData *runtime = (CharacterRuntimeData *)object->character_data->field11_0x24;
        i32 origin_joint = runtime->origin_joint_index;
        if (origin_joint != -1 && object->game_character_data != NULL &&
            *(void **)((u8 *)object->game_character_data + 0x14 + origin_joint * 4) != NULL) {
            if ((object->player_packet.runtime_flags_4 & 8) == 0) {
                object->origin.x = 0.0f;
                object->origin.y = -object->model_bottom;
                object->origin.z = 0.0f;
                NuVecMtxRotate(&object->origin, &object->origin, &object->transform);
                NuVecAdd(&object->origin, &object->origin, &object->position);
            } else {
                NUMTX *joint = &object->player_packet.joint_matrices[origin_joint];
                object->origin.x = joint->m30 + delta_x;
                object->origin.y =
                    joint->m31 + delta_y + (object->model_bottom + object->model_top) * object->scale * 0.5f;
                object->origin.z = joint->m32 + delta_z;
            }

            object->origin.x += delta_x;
            object->origin.y += delta_y;
            object->origin.z += delta_z;
            origin_x = object->origin.x;
            origin_y = object->origin.y;
            origin_z = object->origin.z;
            predicted_x = object->position.x + delta_x;
            predicted_z = object->position.z + delta_z;
            found_origin = true;
        } else {
            i32 bounds_joint = runtime->bounds_joint_index;
            if (object->player_packet.damage_scale == 1.0f && bounds_joint != -1 &&
                *(void **)((u8 *)object->game_character_data + 0x14 + bounds_joint * 4) != NULL &&
                object->drawn_this_frame != 0) {
                NUMTX *joint = &object->player_packet.joint_matrices[bounds_joint];
                object->origin.x = joint->m30;
                object->origin.y = joint->m31;
                object->origin.z = joint->m32;
                origin_x = object->origin.x;
                origin_y = object->origin.y;
                origin_z = object->origin.z;
                predicted_x = object->position.x + delta_x;
                predicted_z = object->position.z + delta_z;
                found_origin = true;
            }
        }
    }

    if (!found_origin) {
        object->object_flags = original_flags;
        origin_x = object->position.x + delta_x;
        origin_y = object->position.y + delta_y + (object->model_bottom + object->model_top) * object->scale * 0.5f;
        origin_z = object->position.z + delta_z;
        object->origin.x = origin_x;
        object->origin.y = origin_y;
        object->origin.z = origin_z;
        predicted_x = origin_x;
        predicted_z = origin_z;
    }

    object->predicted_bottom_center.x = predicted_x;
    object->ground_position.x = predicted_x;
    object->predicted_bottom_center.z = predicted_z;
    object->bounds_top_center.x = predicted_x;
    object->bounds_max.x = origin_x + object->bounds_radius;
    object->bounds_top_center.z = predicted_z;
    object->bounds_min.x = origin_x - object->bounds_radius;
    object->bounds_bottom_center.x = predicted_x;
    object->bounds_bottom_center.z = predicted_z;
    object->bounds_min.z = origin_z - object->bounds_radius;
    object->bounds_max.z = origin_z + object->bounds_radius;

    f32 bottom = origin_y - object->bounds_half_height;
    f32 top = origin_y + object->bounds_half_height;
    object->bounds_min.y = bottom;
    object->bounds_max.y = top;
    object->bounds_top_center.y = top;
    object->bounds_bottom_center.y = bottom;
    object->predicted_bottom_center.y = bottom + delta_y;
    object->ground_position.y = object->predicted_bottom_center.y;
    object->ground_position.z = predicted_z;

    u32 character_flags = CInfo[object->player_packet.character_state].flags;
    if ((character_flags & 0x80000) != 0 && (object->player_packet.combat_flags_0 & 8) != 0) {
        object->ground_position.y = (top - bottom) + object->predicted_bottom_center.y;
    } else if ((character_flags & 0x100000) != 0) {
        object->ground_position = object->position;
    } else {
        f32 ground_y = object->predicted_bottom_center.y - object->player_packet.origin_y_offset;
        object->ground_position.y = ground_y;
        if (ground_y < object->floor_height &&
            object->floor_height < object->predicted_bottom_center.y + object->model_height) {
            object->ground_position.y = object->floor_height;
        }
    }
}

void PortalGameObject(GameObject_s *object, i32 enable, i32 secondary, i16 room_id, NUGSCN *scene) {
    if (enable != 0) {
        object->portal_flags |= 0x40;
        object->portal_flags &= 0x7f;
        object->portal_flags |= (u8)secondary << 7;

        if (room_id != -1) {
            object->room_id = room_id;
        } else {
            i16 object_room = -1;
            if (scene != NULL) {
                object_room = NuPortalWhichRoom(scene, &object->origin);
            }
            object->room_id = object_room;
        }
    } else {
        object->portal_flags &= ~0x40;
    }
}

void SnapCreaturePos(GameObject_s *object, NUVEC *position, i32 heading, AIPATHINFO *path_info, i32 set_on_surface) {
    object->position = *position;
    object->yaw = heading;
    object->heading = heading;
    object->desired_heading = heading;
    object->spawn_position = object->position;
    object->origin = object->position;
    plr_lastpos = object->position;
    object->old_position = object->position;
    object->position_history_1 = object->position;
    object->last_safe_position = object->position;
    object->previous_safe_position = object->position;
    object->player_packet.cached_velocity = v000;
    object->velocity = v000;

    InitSurfaceInfo(object);
    if (set_on_surface != 0) {
        SetObjOnSurface(object, 0);
    }

    if (path_info == NULL) {
        AISysGetCharacterPathPos(WORLD->ai_system, object, &object->ai_process, 0xff, 1);
    } else {
        object->path_info = *path_info;
    }
}

void UpdateLastSafePosition(GameObject_s *object) {
    u8 *raw = (u8 *)object;
    raw[0xf02] &= ~0x40;

    i32 context = object->player_packet.character_state;
    bool eligible = object->death_state == 0 && (LEGOCONTEXT_DOOMED == -1 || context != LEGOCONTEXT_DOOMED) &&
                    (object->state_flags & 4) == 0;
    bool accepted_safe_position = false;

    if (eligible) {
        object->position_history_0 = object->position;

        GameObject_s *surface_object = object;
        if (object->player_packet.takeover_object != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
            context == LEGOCONTEXT_BEENTAKENOVER) {
            surface_object = object->player_packet.takeover_object;
        }

        bool bad_surface = false;
        if (surface_object->ground_surface_type != -1 &&
            (TerSurface[surface_object->ground_surface_type].flags & 0xc041) != 0) {
            bad_surface = true;
        } else if (surface_object->edge_surface_type != -1 &&
                   ((TerLayer[surface_object->edge_surface_type].flags & 1) != 0 ||
                    surface_object->edge_surface_type == 9)) {
            bad_surface = true;
        }
        if (bad_surface && VehicleArea != 0 && BonusArea != 0) {
            bad_surface = *(f32 *)((u8 *)surface_object->character_data->field11_0x24 + 0x28) <= 0.0f;
        }
        if (WORLD->current_level == NEWTOWN_LDATA && ((u8 *)surface_object)[0xe31] == 1 &&
            surface_object->position.y > 0.7f) {
            bad_surface = true;
        }

        bool may_update = (raw[0xf00] & 4) == 0 && (((u8 *)&CInfo[context])[0xc] & 8) == 0;
        if (may_update && object->ground_contact_flags == 0 &&
            (context == -1 ||
             (context != LEGOCONTEXT_BEENTAKENOVER && context != LEGOCONTEXT_EATEN && context != LEGOCONTEXT_GETIN)) &&
            VehicleArea == 0 && object->character_id != id_TRAININGREMOTE) {
            u32 character_flags = (u32)object->character_data->field1_0x4;
            bool vehicle_exception =
                (character_flags & 0x2000) != 0 && *(f32 *)((u8 *)object->character_data->field11_0x24 + 0x28) > 0.0f;
            bool player_exception = (character_flags & 0x8000) != 0 && raw[0xe31] == 1;
            if (!vehicle_exception && !player_exception) {
                may_update = false;
            }
        }

        if (may_update && *(f32 *)(raw + 0xf34) > 0.9f && object->floor_height != 2000000.0f) {
            bool safe_platforms = (object->ground_platform_id == -1 || object->ground_platform_id == LevSafePlatID[0] ||
                                   object->ground_platform_id == LevSafePlatID[1]) &&
                                  (object->shadow_platform_id == -1 || object->shadow_platform_id == LevSafePlatID[0] ||
                                   object->shadow_platform_id == LevSafePlatID[1]);
            if (!safe_platforms && LastSafePosExtraFn != NULL) {
                safe_platforms = LastSafePosExtraFn(object) != 0;
            }

            if (safe_platforms && object->bounds_bottom_center.y > object->floor_height - 0.1f && !bad_surface &&
                (VehicleArea == 0 || !TouchHacks::CheckForAboutToRunIntoKillTerrain(*object, 1.0f))) {
                raw[0xf02] |= 0x40;
                object->last_safe_position = object->position;
                *(f32 *)(raw + 0xd9c) = 0.0f;

                bool can_clear_takeover = (LEGOCONTEXT_GETIN == -1 || context != LEGOCONTEXT_GETIN) &&
                                          (LEGOCONTEXT_BEENTAKENOVER == -1 || context != LEGOCONTEXT_BEENTAKENOVER);
                if ((((u8 *)&object->path_info)[0xe] & 1) != 0 && ((u32 *)object->path_info.connection)[0] == 0 &&
                    ((u32 *)object->path_info.connection)[1] == 0) {
                    if (*(GameObject_s **)(raw + 0xcc4) != NULL && can_clear_takeover) {
                        *(GameObject_s **)(raw + 0xcc4) = NULL;
                    }
                    object->position_history_1 = object->position;
                    object->state_flags |= 0x2000;
                }

                if ((u8)object->player_index < 2 && *(f32 *)(raw + 0xda8) <= 0.0f &&
                    *(f32 *)(GameCam + 0x1c0) <= *(f32 *)(GameCam + 0x1bc) && object->sock_index != -1) {
                    if (*(GameObject_s **)(raw + 0xcc4) != NULL && can_clear_takeover) {
                        *(GameObject_s **)(raw + 0xcc4) = NULL;
                    }
                    *(NUVEC *)(raw + 0xc64) = object->last_safe_position;
                }
                accepted_safe_position = true;
            }
        }

        if (!accepted_safe_position) {
            if ((i8)object->state_flags < 0 && object->edge_surface_type != -1 &&
                (TerLayer[object->edge_surface_type].flags & 1) != 0 && !NoLayerKill(object)) {
                if (*(f32 *)(raw + 0xd9c) <= 0.0f) {
                    *(f32 *)(raw + 0xdb4) = object->position.y;
                }
                *(f32 *)(raw + 0xd9c) += FRAMETIME;
            } else {
                *(f32 *)(raw + 0xd9c) = 0.0f;
            }

            if ((((u8 *)WORLD->current_level)[0x66] & 4) != 0) {
                object->last_safe_position = object->position;
            }
        }
    }

    context = object->player_packet.character_state;
    if ((raw[0xf03] & 1) == 0 && (i8)object->state_flags >= 0 &&
        (LEGOCONTEXT_EATEN == -1 || context != LEGOCONTEXT_EATEN) && (object->state_flags & 0x2000) != 0) {
        bool can_clear_takeover = (LEGOCONTEXT_GETIN == -1 || context != LEGOCONTEXT_GETIN) &&
                                  (LEGOCONTEXT_BEENTAKENOVER == -1 || context != LEGOCONTEXT_BEENTAKENOVER);
        if ((((u8 *)&object->path_info)[0xe] & 1) != 0 && *(GameObject_s **)(raw + 0xcc4) != NULL &&
            can_clear_takeover) {
            *(GameObject_s **)(raw + 0xcc4) = NULL;
        }
        object->last_safe_position = object->position_history_1;
    }

    raw[0xf00] &= ~4;
}

void GameObjectSetCanUse(GameObject_s *object, void *usable_object, u8 action, u8, f32 distance) {
    object->usable_object = usable_object;
    object->use_action = action;
    object->use_frame = grapple_attach_frames;
    object->use_distance = distance;
}
