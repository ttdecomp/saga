#include "legoapi/world/world_shared.h"
#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "gameapi/edtools/edfile.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufilepak.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nutrig.h"

#include <stdio.h>
#include <string.h>

extern "C" {
    char *AiLevelPathName = "";
    SCRIPTPROCESSFIRSTTIMEACTION *ScriptProcessFirstTimeActionFn;
}

void AIPathCalcExtents(AIPATH *path);
i32 AIPathCheckExtents(AIPATH *path, NUVEC *position);
void AIMoveToDestination(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);
void AIRetreatFromDestination(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);
void AICircle(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);
void AIWander(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);
void AIMoveToDestinationAvoidingCamera(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);
void AIMoveDirectlyToDestination(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);

typedef i32 MIDSPECIALMOVE(AISYS *system, AIPACKET *packet, APIOBJECT *object);
extern MIDSPECIALMOVE *MidSpecialMoveFn;

extern "C" void *AISysBufferAlloc(VARIPTR *cursor, VARIPTR *buf_end, u32 size);
extern "C" i32 AISysSetLevelPath(AISYS *system, char *path_name);
extern "C" void AISysFindRoute(AIPACKET *packet);
extern "C" void AISysCharacterSetPath(AIPACKET *packet, AIPATH *path);
extern "C" void AISysCharacterSetPathCnx(AIPACKET *packet, NUVEC *position, AIPATHCNX *connection, i32 direction);

static void *AISysLoadAlloc(AISYS *system, u32 size) {
    return AISysBufferAlloc(&system->storage_cursor, &system->storage_end, size);
}

static char *AISysLoadString(AISYS *system, i32 length) {
    if (length == 0) {
        return NULL;
    }

    char *text = static_cast<char *>(AISysLoadAlloc(system, length + 1));
    EdFileRead(text, length);
    return text;
}

static void AISysLoadPathRoutes(AISYS *system, AIPATH *path) {
    if (path->node_count != 0) {
        path->route_matrix = static_cast<u8 **>(AISysLoadAlloc(system, path->node_count * sizeof(u8 *)));
        for (i32 i = 0; i < path->node_count; ++i) {
            path->route_matrix[i] = static_cast<u8 *>(AISysLoadAlloc(system, path->node_count));
            EdFileRead(path->route_matrix[i], path->node_count);
        }
    }

    path->route_count = static_cast<u8>(EdFileReadChar());
    if (path->route_count != 0) {
        path->routes = static_cast<AIPATHROUTE *>(AISysLoadAlloc(system, path->route_count * sizeof(AIPATHROUTE)));
        for (i32 i = 0; i < path->route_count; ++i) {
            AIPATHROUTE *route = &path->routes[i];
            i32 name_length = EdFileReadChar();
            route->name = AISysLoadString(system, name_length);
            if (name_length != 0) {
                route->route_count = static_cast<u8>(EdFileReadChar());
                route->character_count = static_cast<u8>(EdFileReadChar());
                EdFileReadChar();
                EdFileReadChar();

                if (path->node_count != 0 && route->route_count != 0) {
                    route->node_routes = static_cast<u8 *>(AISysLoadAlloc(system, path->node_count));
                    EdFileRead(route->node_routes, path->node_count);
                    route->node_directions = static_cast<u8 *>(AISysLoadAlloc(system, path->node_count));
                    EdFileRead(route->node_directions, path->node_count);
                    route->route_nodes = static_cast<u8 **>(AISysLoadAlloc(system, route->route_count * sizeof(u8 *)));
                    for (i32 route_index = 0; route_index < route->route_count; ++route_index) {
                        route->route_nodes[route_index] = static_cast<u8 *>(AISysLoadAlloc(system, path->node_count));
                        EdFileRead(route->route_nodes[route_index], path->node_count);
                    }
                    if (route->character_count != 0) {
                        route->characters = static_cast<u8 *>(AISysLoadAlloc(system, route->character_count));
                        EdFileRead(route->characters, route->character_count);
                    }
                }
            }

            i32 character_count = EdFileReadChar();
            for (i32 character_index = 0; character_index < character_count; ++character_index) {
                char character_name[256] = {};
                i32 character_name_length = EdFileReadChar();
                EdFileRead(character_name, character_name_length);
                if (SpecialRouteCharacterTypeIDFn != NULL) {
                    u8 type = SpecialRouteCharacterTypeIDFn(character_name);
                    if (type < 32) {
                        route->character_mask[0] |= 1u << type;
                    } else if (type < 64) {
                        u32 bit = 1u << (type & 31);
                        route->character_mask[1] |= bit;
                        route->character_mask[2] |= bit;
                        route->character_mask[3] |= bit;
                    } else {
                        route->character_mask[0] = 0xffffffff;
                        route->character_mask[1] = 0xffffffff;
                        route->character_mask[3] |= 0x80000000;
                    }
                }
            }
        }
    }

    path->special_route_count = static_cast<u8>(EdFileReadChar());
    if (path->special_route_count != 0) {
        path->special_routes =
            static_cast<AIPATHNODELINK *>(AISysLoadAlloc(system, path->special_route_count * sizeof(AIPATHNODELINK)));
        for (i32 i = 0; i < path->special_route_count; ++i) {
            path->special_routes[i].type = EdFileReadUnsignedChar();
            path->special_routes[i].node = EdFileReadShort();
        }
    }
}

static AIPATHSYS *AISysLoadPaths(AISYS *system, i32 version, NUGSCN *scene) {
    i32 path_count = EdFileReadInt();
    if (path_count == 0) {
        return NULL;
    }

    AIPATHSYS *path_system = static_cast<AIPATHSYS *>(AISysLoadAlloc(system, sizeof(AIPATHSYS)));
    path_system->path_count = static_cast<u8>(path_count);
    path_system->paths = static_cast<AIPATH **>(AISysLoadAlloc(system, path_system->path_count * sizeof(AIPATH *)));

    for (i32 path_index = 0; path_index < path_system->path_count; ++path_index) {
        AIPATH *path = static_cast<AIPATH *>(AISysLoadAlloc(system, sizeof(AIPATH)));
        path_system->paths[path_index] = path;
        EdFileRead(path->name, sizeof(path->name));
        path->node_count = static_cast<u8>(EdFileReadChar());
        path->flags = static_cast<u8>(EdFileReadChar());
        path->index = static_cast<u8>(path_index);
        path->connection_count = EdFileReadShort();

        if (path->connection_count != 0) {
            path->connections =
                static_cast<AIPATHCNX *>(AISysLoadAlloc(system, path->connection_count * sizeof(AIPATHCNX)));
            for (i32 connection_index = 0; connection_index < path->connection_count; ++connection_index) {
                AIPATHCNX *connection = &path->connections[connection_index];
                connection->direction_a = static_cast<u8>(EdFileReadChar());
                connection->direction_b = static_cast<u8>(EdFileReadChar());
                connection->node_a = EdFileReadInt();
                connection->node_b = EdFileReadInt();
                connection->previous_node_a = connection->node_a;
                connection->previous_node_b = connection->node_b;
                connection->flags = EdFileReadShort();
                connection->game_flags = EdFileReadShort();
                connection->width = EdFileReadFloat();
                connection->cost = EdFileReadFloat();
            }
        }

        if (path->node_count != 0) {
            path->nodes = static_cast<AIPATHNODE *>(AISysLoadAlloc(system, path->node_count * sizeof(AIPATHNODE)));
            for (i32 node_index = 0; node_index < path->node_count; ++node_index) {
                AIPATHNODE *node = &path->nodes[node_index];
                node->name = AISysLoadString(system, EdFileReadInt());
                EdFileReadNuVec(&node->position);
                node->radius = EdFileReadFloat();
                node->radius_squared = node->radius * node->radius;
                node->min_height = EdFileReadFloat();
                node->max_height = EdFileReadFloat();
                node->min_height_offset = node->min_height - node->position.y;
                node->max_height_offset = node->max_height - node->position.y;
                node->connection_count = static_cast<u8>(EdFileReadChar());
                node->flags = static_cast<u8>(EdFileReadChar());
                EdFileReadChar();
                node->runtime_flags = static_cast<u8>(EdFileReadChar()) & ~6u;
                node->path_flags = EdFileReadShort();
                node->special_type = static_cast<u8>(EdFileReadChar());

                char special_name[256] = {};
                i32 special_name_length = EdFileReadChar();
                if (special_name_length != 0) {
                    EdFileRead(special_name, special_name_length);
                    node->has_special = static_cast<u8>(
                        NuSpecialFind(scene, reinterpret_cast<void **>(node->special_handle), special_name, 1) != 0);
                    EdFileReadNuVec(&node->special_position);
                }

                if (path->connections != NULL && node->connection_count != 0) {
                    node->connections =
                        static_cast<AIPATHCNX **>(AISysLoadAlloc(system, node->connection_count * sizeof(AIPATHCNX *)));
                    for (i32 i = 0; i < node->connection_count; ++i) {
                        u16 connection_index = EdFileReadShort();
                        node->connections[i] = &path->connections[connection_index];
                    }
                    if ((node->connection_count & 1) != 0) {
                        EdFileReadShort();
                    }
                }

                node->value_0x58 = EdFileReadShort();
                node->value_0x5a = EdFileReadShort();
            }
            AIPathCalcExtents(path);
        }

        AISysLoadPathRoutes(system, path);
    }

    path_system->special_route_count = EdFileReadShort();
    if (path_system->special_route_count != 0) {
        path_system->special_routes = static_cast<AIPATHSPECIALROUTE *>(
            AISysLoadAlloc(system, path_system->special_route_count * sizeof(AIPATHSPECIALROUTE)));
        for (i32 i = 0; i < path_system->special_route_count; ++i) {
            AIPATHSPECIALROUTE *route = &path_system->special_routes[i];
            route->path_count = static_cast<u8>(EdFileReadChar());
            route->paths = static_cast<AIPATH **>(AISysLoadAlloc(system, route->path_count * sizeof(AIPATH *)));
            for (i32 path_index = 0; path_index < route->path_count; ++path_index) {
                route->paths[path_index] = path_system->paths[EdFileReadChar()];
            }
        }
    }
    return path_system;
}

static AIAREA *AISysLoadFindArea(AISYS *system, char *name) {
    for (i32 index = 0; index < system->area_count; ++index) {
        if (NuStrICmp(system->areas[index].name, name) == 0) {
            return &system->areas[index];
        }
    }
    return NULL;
}

// Test the geometric core of a path connection. The original routine also
// handles platform-specific connection rules; this keeps the shared path
// projection and node-volume test explicit while those rules are recovered.
static i32 AISysCharacterTestPathCnx(AISYS *system, APIOBJECT *object, AIPACKET *packet, AIPATHCNX *connection,
                                     i32 requested_direction, f32 *nearest_distance_squared) {
    (void)system;
    AIPATH *path = packet->path_info.path;
    if (object == NULL || path == NULL || path->nodes == NULL || connection == NULL) {
        return 0;
    }

    if (connection->last_search_checksum == path->search_checksum) {
        return 0;
    }
    connection->last_search_checksum = path->search_checksum;

    const i32 first_direction = requested_direction < 0 ? 0 : requested_direction;
    const i32 last_direction = requested_direction < 0 ? 1 : requested_direction;
    for (i32 direction = first_direction; direction <= last_direction; ++direction) {
        const u8 from_index = connection->node_indices[direction];
        const u8 to_index = connection->node_indices[direction ^ 1];
        if (from_index >= path->node_count || to_index >= path->node_count) {
            continue;
        }

        const AIPATHNODE &from = path->nodes[from_index];
        const AIPATHNODE &to = path->nodes[to_index];
        const f32 segment_x = to.position.x - from.position.x;
        const f32 segment_z = to.position.z - from.position.z;
        const f32 segment_length_squared = segment_x * segment_x + segment_z * segment_z;
        if (segment_length_squared <= 0.0f) {
            continue;
        }

        const f32 object_x = object->position.x - from.position.x;
        const f32 object_z = object->position.z - from.position.z;
        f32 along = (object_x * segment_x + object_z * segment_z) / segment_length_squared;
        if (along < 0.0f) {
            along = 0.0f;
        } else if (along > 1.0f) {
            along = 1.0f;
        }

        const f32 nearest_x = from.position.x + segment_x * along;
        const f32 nearest_z = from.position.z + segment_z * along;
        const f32 distance_x = object->position.x - nearest_x;
        const f32 distance_z = object->position.z - nearest_z;
        const f32 distance_squared = distance_x * distance_x + distance_z * distance_z;
        const f32 path_radius = from.radius + (to.radius - from.radius) * along + object->collision_radius;
        if (distance_squared > path_radius * path_radius || distance_squared >= *nearest_distance_squared) {
            continue;
        }

        const f32 min_height = from.min_height + (to.min_height - from.min_height) * along;
        const f32 max_height = from.max_height + (to.max_height - from.max_height) * along;
        if (object->position.y + object->collision_height < min_height || object->position.y > max_height) {
            continue;
        }

        *nearest_distance_squared = distance_squared;
        packet->path_info.flags |= AIPATHINFO_FLAG_ON_PATH;
        packet->path_info.next_check = from_index;
        AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
        return 1;
    }
    return 0;
}

static void AISysResetPathSearchConnectionChecks(AIPATH *path) {
    if (path == NULL || path->nodes == NULL || path->node_count == 0) {
        return;
    }

    ++path->search_checksum;
    if (path->search_checksum == 0) {
        path->search_checksum = 1;
    }

    ++path->search_reset_node;
    if (path->search_reset_node >= path->node_count) {
        path->search_reset_node = 0;
    }

    AIPATHNODE &node = path->nodes[path->search_reset_node];
    for (i32 connection_index = 0; connection_index < node.connection_count; ++connection_index) {
        node.connections[connection_index]->last_search_checksum = 0;
    }
}

static AILOCATOR *AISysLoadFindLocator(AISYS *system, char *name) {
    for (i32 index = 0; index < system->locator_count; ++index) {
        if (NuStrICmp(system->locators[index].name, name) == 0) {
            return &system->locators[index];
        }
    }
    return NULL;
}

static AIAREA *AISysLoadAreaReference(AISYS *system) {
    char name[16];
    if (EdFileReadInt() == 0) {
        return NULL;
    }
    EdFileRead(name, sizeof(name));
    return AISysLoadFindArea(system, name);
}

static AILOCATOR *AISysLoadLocatorReference(AISYS *system) {
    char name[16];
    if (EdFileReadInt() == 0) {
        return NULL;
    }
    EdFileRead(name, sizeof(name));
    return AISysLoadFindLocator(system, name);
}

static void AISysLoadAreas(AISYS *system, i32 version) {
    system->area_count = EdFileReadInt();
    if (system->area_count == 0) {
        return;
    }

    system->areas = static_cast<AIAREA *>(AISysLoadAlloc(system, system->area_count * sizeof(AIAREA)));
    for (i32 index = 0; index < system->area_count; ++index) {
        AIAREA *area = &system->areas[index];
        EdFileRead(area->name, sizeof(area->name));
        area->min_x = EdFileReadFloat();
        area->min_y = EdFileReadFloat();
        area->min_z = EdFileReadFloat();
        area->max_x = EdFileReadFloat();
        area->max_y = EdFileReadFloat();
        area->max_z = EdFileReadFloat();
        area->flags = EdFileReadShort();
        area->system = system;
        if (version > 19) {
            area->game_flags = static_cast<u8>(EdFileReadChar());
        } else {
            EdFileReadChar();
        }
        EdFileReadChar();
    }
}

static void AISysLoadLocators(AISYS *system, i32 version) {
    system->locator_count = EdFileReadInt();
    if (system->locator_count == 0) {
        return;
    }

    system->locators = static_cast<AILOCATOR *>(AISysLoadAlloc(system, system->locator_count * sizeof(AILOCATOR)));
    for (i32 index = 0; index < system->locator_count; ++index) {
        AILOCATOR *locator = &system->locators[index];
        EdFileRead(locator->name, sizeof(locator->name));
        EdFileReadNuVec(&locator->position);
        locator->flags = EdFileReadShort();
        u8 path_index = static_cast<u8>(EdFileReadChar());
        locator->path = system->path_sys->paths[path_index];
        locator->game_flags = static_cast<u8>(EdFileReadChar());
        u16 connection_index = static_cast<u16>(EdFileReadShort());
        locator->connection = &locator->path->connections[connection_index];
        locator->min_distance = EdFileReadFloat();
        locator->max_distance = EdFileReadFloat();
        if (version > 14) {
            locator->locator_flags = EdFileReadInt();
        }
    }
}

static void AISysLoadLocatorSets(AISYS *system, i32 version) {
    if (version <= 17) {
        return;
    }

    system->locator_set_count = EdFileReadInt();
    if (system->locator_set_count == 0) {
        return;
    }

    system->locator_sets =
        static_cast<AILOCATORSET *>(AISysLoadAlloc(system, system->locator_set_count * sizeof(AILOCATORSET)));
    for (i32 index = 0; index < system->locator_set_count; ++index) {
        AILOCATORSET *locator_set = &system->locator_sets[index];
        EdFileRead(locator_set->name, sizeof(locator_set->name));
        locator_set->locator_count = static_cast<u8>(EdFileReadInt());
        locator_set->locator_entries = AISysLoadAlloc(system, locator_set->locator_count * 0x1c);
        u8 *entry = static_cast<u8 *>(locator_set->locator_entries);
        for (i32 locator_index = 0; locator_index < locator_set->locator_count; ++locator_index) {
            *entry = static_cast<u8>(EdFileReadChar());
            entry += 0x1c;
        }
        locator_set->assigned = static_cast<u8 *>(AISysLoadAlloc(system, locator_set->locator_count));
        memset(locator_set->assigned, 0, locator_set->locator_count);
    }
}

static void AISysLoadCreatures(AISYS *system, i32 version) {
    system->creature_count = EdFileReadInt();
    if (system->creature_count == 0) {
        return;
    }

    system->creatures = static_cast<AICREATURE *>(AISysLoadAlloc(system, system->creature_count * sizeof(AICREATURE)));
    for (i32 index = 0; index < system->creature_count; ++index) {
        AICREATURE *creature = &system->creatures[index];
        EdFileRead(creature->name, sizeof(creature->name));
        EdFileRead(creature->script_name, sizeof(creature->script_name));

        if (creature->script_name[0] == '\0') {
            strcpy(creature->script_name, "default");
        } else if (version > 13) {
            char character_name[32];
            EdFileRead(character_name, sizeof(character_name));
            if (GlobalCharacterTypeIDFn != NULL) {
                creature->type = static_cast<i16>(GlobalCharacterTypeIDFn(character_name));
            }
        }

        EdFileReadNuVec(&creature->pos);
        creature->y_rot = static_cast<NUANG>(EdFileReadShort());

        if (version > 15) {
            creature->set = static_cast<u8>(EdFileReadChar());
            creature->count = static_cast<u8>(EdFileReadChar());
            creature->count_across = static_cast<u8>(EdFileReadChar());
            creature->active_mask = EdFileReadUnsignedInt();
            creature->x_spacing = EdFileReadFloat();
            creature->z_spacing = EdFileReadFloat();
            creature->flags = EdFileReadInt();
            u8 path_index = static_cast<u8>(EdFileReadChar());
            creature->path_info.path = system->path_sys->paths[path_index];
            creature->path_info.direction = static_cast<u8>(EdFileReadChar());
            u16 connection_index = static_cast<u16>(EdFileReadShort());
            creature->path_info.connection = &creature->path_info.path->connections[connection_index];
        }

        if (version > 2) {
            for (i32 param = 0; param < 4; ++param) {
                creature->script_params[param] = EdFileReadFloat();
            }
        }

        if (version != 3) {
            creature->area = AISysLoadAreaReference(system);
        }
        if (version > 5) {
            creature->locator = AISysLoadLocatorReference(system);
        }
        if (version > 16) {
            creature->respawn_locator = AISysLoadLocatorReference(system);
        }

        if (version > 7) {
            creature->activation_difficulty = static_cast<u8>(EdFileReadChar());
            creature->min_respawn_count = EdFileReadChar();
            creature->max_respawn_count = EdFileReadChar();
            creature->activate_type = static_cast<u8>(EdFileReadChar());
            creature->min_respawn_time = EdFileReadFloat();
            creature->max_respawn_time = EdFileReadFloat();
        }
        if (version > 9) {
            creature->start_stagger = EdFileReadFloat();
        }
        if (creature->activate_type == 1) {
            char area_name[16];
            EdFileRead(area_name, sizeof(area_name));
            creature->activate_area = AISysLoadFindArea(system, area_name);
        }
        if (version > 10) {
            creature->view_distance = EdFileReadFloat();
            creature->hear_distance = EdFileReadFloat();
            creature->max_view_height = EdFileReadFloat();
            creature->min_view_height = EdFileReadFloat();
            EdFileReadInt();
        }

        if (creature->view_distance == 0.0f && GetViewRangeFn != NULL) {
            creature->view_distance = GetViewRangeFn(creature->type);
        }
        if (creature->hear_distance == 0.0f && GetHearDistanceFn != NULL) {
            creature->hear_distance = GetHearDistanceFn(creature->type);
        }
        if (creature->max_view_height == 0.0f && GetMaxViewHeightFn != NULL) {
            creature->max_view_height = GetMaxViewHeightFn(creature->type);
        }
        if (creature->min_view_height == 0.0f && GetMinViewHeightFn != NULL) {
            creature->min_view_height = GetMinViewHeightFn(creature->type);
        }
    }
}

static void AISysLoadAntinodes(AISYS *system, i32 version, NUGSCN *scene) {
    system->antinode_count = EdFileReadInt();
    if (system->antinode_count == 0) {
        return;
    }

    system->antinodes = static_cast<AIANTINODE *>(AISysLoadAlloc(system, system->antinode_count * sizeof(AIANTINODE)));
    for (i32 index = 0; index < system->antinode_count; ++index) {
        AIANTINODE *antinode = &system->antinodes[index];
        antinode->enabled = 1;
        EdFileReadNuVec(&antinode->position);
        antinode->radius = EdFileReadFloat();
        antinode->base_radius = antinode->radius;
        antinode->base_height = antinode->radius;
        antinode->height = EdFileReadFloat();
        antinode->max_height = EdFileReadFloat();
        antinode->width = antinode->height - antinode->position.y;
        antinode->min_height = antinode->max_height - antinode->position.y;

        if (version > 14) {
            antinode->flags = EdFileReadInt();
            antinode->base_radius = EdFileReadFloat();
            antinode->base_height = EdFileReadFloat();
            antinode->type = static_cast<u8>(EdFileReadChar());
            EdFileReadChar();
            EdFileReadChar();
            antinode->game_flags = static_cast<u8>(EdFileReadChar());

            char special_name[256] = {};
            i32 special_name_length = EdFileReadChar();
            if (special_name_length != 0) {
                EdFileRead(special_name, special_name_length);
                antinode->has_special = static_cast<u8>(
                    NuSpecialFind(scene, reinterpret_cast<void **>(antinode->special_handle), special_name, 1) != 0);
                EdFileReadNuVec(&antinode->special_position);
                antinode->special_type = static_cast<u8>(EdFileReadInt());
            }
        }
    }
}

extern "C" {

    void AIAntinodeCreate(void) {
    }

    void AIAntinodeCreateSingleFrame(void) {
    }

    void AIAntinodeCullSingleFrame(void) {
    }

    void AIAntinodeDestroy(void) {
    }

    void AIAntinodeMove(void) {
    }

    void AIFormationFollow(AIPACKET *) {
    }

    void AILocatorSet_AssignFurthestLocator(void) {
    }

    void AILocatorSet_AssignNearestLocator(void) {
    }

    void AILocatorSet_AssignRandomLocator(void) {
    }

    void AILocatorSet_CheckLocatorsStillAssigned(void) {
    }

    void AIMoveInstruction(AIPACKET *packet, NUVEC *destination, f32 stopping_distance, AIPATHINFO *path_info, i32 mode,
                           f32 movement_parameter) {
        AIGROUP *group = packet->group;
        if (group != NULL && group->is_in_formation) {
            // Formation leaders update their rows through FormationMove; the
            // remaining members derive their instruction from the group.
            // Mode five is the direct single-character form used by scripts.
            if (mode != 5) {
                AIFormationFollow(packet);
                return;
            }
            mode = AIPACKET_MOVEMENT_TO_DESTINATION;
        }

        if (destination != NULL) {
            packet->fallback_destination = *destination;
        }
        if (path_info != NULL) {
            packet->fallback_path_info = *path_info;
        }
        packet->fallback_stopping_distance = stopping_distance;
        packet->movement_flags = (packet->movement_flags & static_cast<u8>(~AIPACKET_MOVEMENT_MODE_MASK)) |
                                 (mode & AIPACKET_MOVEMENT_MODE_MASK);
        packet->movement_parameter = movement_parameter;
    }

    void AIPAthFindPathCnx(void) {
    }

    AILOCATOR *AIPathFindLocator(AISYS *sys, char *name) {
        if (sys != NULL) {
            for (i32 i = 0; i < sys->locator_count; ++i) {
                if (NuStrICmp(sys->locators[i].name, name) == 0) {
                    return &sys->locators[i];
                }
            }
        }
        return NULL;
    }

    void AIPathFindLocatorSet(void) {
    }

    void AIPathFindNode(void) {
    }

    void AIPathFindPathCnxFromIX(void) {
    }

    void AIPathNodeDistanceToPathNode(void) {
    }

    void AIPathNodeUpdatePos(AISYS *system, AIPATH *path, AIPATHNODE *node) {
        if (node->has_special == 0) {
            return;
        }

        const i32 node_index = static_cast<i32>(node - path->nodes);
        const u8 node_bit = static_cast<u8>(1u << (node_index & 7));
        u8 &updated_nodes = path->updated_node_bits[node_index >> 3];
        if ((updated_nodes & node_bit) != 0) {
            return;
        }
        updated_nodes |= node_bit;

        nuhspecial_s *special = reinterpret_cast<nuhspecial_s *>(node->special_handle);
        if ((node->runtime_flags & AIPATHNODE_RUNTIME_SPECIAL_UNAVAILABLE) == 0) {
            if (NuSpecialGetVisibilityFn(special) == 0 &&
                (FindAlternativeSpecialObjectFn == NULL || FindAlternativeSpecialObjectFn(system, special) == 0)) {
                node->runtime_flags |= AIPATHNODE_RUNTIME_SPECIAL_UNAVAILABLE;
                for (i32 index = 0; index < node->connection_count; ++index) {
                    AIPATHCNX *connection = node->connections[index];
                    connection->traversal_flags[0] |= AIPATH_CONNECTION_FLAG_SPECIAL_UNAVAILABLE;
                    connection->traversal_flags[1] |= AIPATH_CONNECTION_FLAG_SPECIAL_UNAVAILABLE;
                }
                return;
            }
        } else {
            if (NuSpecialGetVisibilityFn(special) == 0) {
                return;
            }

            node->runtime_flags &= static_cast<u8>(~AIPATHNODE_RUNTIME_SPECIAL_UNAVAILABLE);
            for (i32 index = 0; index < node->connection_count; ++index) {
                AIPATHCNX *connection = node->connections[index];
                const u8 other_index =
                    connection->direction_a == node_index ? connection->direction_b : connection->direction_a;
                if ((path->nodes[other_index].runtime_flags & AIPATHNODE_RUNTIME_SPECIAL_UNAVAILABLE) == 0) {
                    connection->traversal_flags[0] &= ~AIPATH_CONNECTION_FLAG_SPECIAL_UNAVAILABLE;
                    connection->traversal_flags[1] &= ~AIPATH_CONNECTION_FLAG_SPECIAL_UNAVAILABLE;
                }
            }
        }

        const NUVEC previous_position = node->position;
        NuVecMtxTransform(&node->position, &node->special_position, NuSpecialGetDrawMtx(special));
        node->min_height = node->position.y + node->min_height_offset;
        node->max_height = node->position.y + node->max_height_offset;

        for (i32 index = 0; index < node->connection_count; ++index) {
            AIPATHCNX *connection = node->connections[index];
            AIPATHNODE *other_node;
            NUVEC *from;
            NUVEC *to;
            if (connection->direction_a == node_index) {
                other_node = &path->nodes[connection->direction_b];
                from = &other_node->position;
                to = &node->position;
            } else {
                other_node = &path->nodes[connection->direction_a];
                from = &node->position;
                to = &other_node->position;
            }

            NUVEC direction;
            connection->horizontal_distance = NuVecXZDist(from, to, &direction);
            connection->rotation = static_cast<i16>(NuAtan2(direction.x, direction.z) * 10430.378f);

            if (connection->max_horizontal_distance != 0.0f) {
                if (connection->max_horizontal_distance + node->radius + other_node->radius <
                    connection->horizontal_distance) {
                    connection->traversal_flags[0] |= AIPATH_CONNECTION_FLAG_DYNAMIC_TOO_LONG;
                    connection->traversal_flags[1] |= AIPATH_CONNECTION_FLAG_DYNAMIC_TOO_LONG;
                } else {
                    connection->traversal_flags[0] &= ~AIPATH_CONNECTION_FLAG_DYNAMIC_TOO_LONG;
                    connection->traversal_flags[1] &= ~AIPATH_CONNECTION_FLAG_DYNAMIC_TOO_LONG;
                }
            }
        }

        nuinstanim_s *animation = NuSpecialGetInstAnim(special);
        if (previous_position.x != node->position.x || previous_position.y != node->position.y ||
            previous_position.z != node->position.z ||
            (animation != NULL && (animation->flags & NUINSTANIM_FLAG_PLAYING) != 0 && animation->tfactor != 0.0f)) {
            node->runtime_flags |= AIPATHNODE_RUNTIME_POSITION_CHANGED;
        } else {
            node->runtime_flags &= static_cast<u8>(~AIPATHNODE_RUNTIME_POSITION_CHANGED);
        }
    }

    void AISYSRebuildFromEditorData(void) {
    }

    void AIScriptForceParamReEval(void) {
    }

    void AIScriptNameFromIx(void) {
    }

} // extern "C"

static AICONDITION *AIScriptProcessStateConditions(AISYS *system, NULISTHDR *conditions, APIOBJECT *, AIPACKET *packet,
                                                   AISCRIPTPROCESS *processor, f32, i32 stack_index) {
    if (processor == NULL || conditions == NULL) {
        return NULL;
    }

    AISCRIPTPROCESSSTACK *stack = &processor->param_stack[stack_index];
    AICONDITION *condition = reinterpret_cast<AICONDITION *>(NuLinkedListGetHead(conditions));
    AICONDITION *matched_condition = NULL;
    f32 condition_value = 0.0f;

    while (condition != NULL) {
        if (condition->def != NULL && condition->def->eval_fn != NULL) {
            condition_value = condition->def->eval_fn(system, processor, packet, condition->arg, condition->void_arg);
        }

        f32 comparison_value;
        if (condition->param_cond != NULL && condition->param_cond->def != NULL) {
            AICONDITIONFN *comparison_fn = condition->param_cond->def->eval_fn;
            comparison_value = comparison_fn != NULL
                                   ? comparison_fn(system, processor, packet, condition->arg, condition->void_arg)
                                   : condition_value;
        } else if (condition->is_complex) {
            if (stack->is_first_time_state) {
                stack->complex_params[condition->param_idx] =
                    AIParamToFloatEx(packet, processor, condition->complex_arg);
            }
            comparison_value = stack->complex_params[condition->param_idx];
        } else if (condition->is_param_idx_valid) {
            comparison_value = processor->params[condition->param_idx];
        } else {
            comparison_value = condition->param_val;
        }

        bool comparison_passed = false;
        switch (condition->type) {
            case AICONDITION_EQUAL:
                comparison_passed = condition_value == comparison_value;
                break;
            case AICONDITION_LESS_THAN:
                comparison_passed = condition_value < comparison_value;
                break;
            case AICONDITION_GREATER_THAN:
                comparison_passed = condition_value > comparison_value;
                break;
            case AICONDITION_LESS_THAN_OR_EQUAL:
                comparison_passed = condition_value <= comparison_value;
                break;
            case AICONDITION_GREATER_THAN_OR_EQUAL:
                comparison_passed = condition_value >= comparison_value;
                break;
            case AICONDITION_NOT_EQUAL:
                comparison_passed = condition_value != comparison_value;
                break;
        }

        if (comparison_passed) {
            if (!condition->bool_and) {
                matched_condition = condition;
                break;
            }
        } else {
            // A failed member invalidates the rest of its AND group.  The
            // first-state pass still snapshots complex operands so later
            // frames compare against the same values as the target.
            while (condition->bool_and) {
                condition = reinterpret_cast<AICONDITION *>(NuLinkedListGetNext(conditions, &condition->list_node));
                if (condition == NULL) {
                    break;
                }
                if (stack->is_first_time_state && condition->is_complex) {
                    stack->complex_params[condition->param_idx] =
                        AIParamToFloatEx(packet, processor, condition->complex_arg);
                    condition->param_val = stack->complex_params[condition->param_idx];
                }
            }
        }

        if (condition != NULL) {
            condition = reinterpret_cast<AICONDITION *>(NuLinkedListGetNext(conditions, &condition->list_node));
        }
    }

    if (stack->force_complex_eval) {
        if (stack->is_first_time_state) {
            AICONDITION *remaining = reinterpret_cast<AICONDITION *>(
                NuLinkedListGetNext(conditions, condition != NULL ? &condition->list_node : NULL));
            while (remaining != NULL) {
                if (remaining->is_complex) {
                    stack->complex_params[remaining->param_idx] =
                        AIParamToFloatEx(packet, processor, remaining->complex_arg);
                }
                remaining = reinterpret_cast<AICONDITION *>(NuLinkedListGetNext(conditions, &remaining->list_node));
            }
        }
        stack->force_complex_eval = 0;
    }
    stack->is_first_time_state = 0;
    return matched_condition;
}

extern "C" {

    void AIScriptProcess(AISYS *system, APIOBJECT *object, AIPACKET *packet, AISCRIPTPROCESS *processor, f32 elapsed) {
        if (processor == NULL || processor->script == NULL) {
            return;
        }

        processor->script_timer += elapsed;

        for (i32 index = 0; index < processor->active_ref_count; ++index) {
            AIREFSCRIPT *reference = processor->active_refs[index];
            if (NuLinkedListGetHead(&reference->conditions) != NULL &&
                AIScriptProcessStateConditions(system, &reference->conditions, object, packet, processor, elapsed, 1) ==
                    NULL) {
                processor->active_ref_count = index;
                if (index == 0) {
                    AICREATURE *creature = packet->field_0x134 != 0xff ? &system->creatures[packet->field_0x134] : NULL;
                    AIScriptProcessorInit(system, packet, processor, creature, NULL, NULL, 0, processor->base_script,
                                          reference->return_state);
                } else {
                    AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0,
                                          processor->active_refs[index - 1]->script, reference->return_state);
                }
                break;
            }
        }

        for (;;) {
            if (processor->next_state != NULL) {
                AIScriptSetState(processor, processor->next_state);
                processor->next_state = NULL;
            }

            if (processor->active_ref_count > 0 && processor->state == processor->script->base_state) {
                const i32 completed_index = processor->active_ref_count - 1;
                AIREFSCRIPT *completed_reference = processor->active_refs[completed_index];
                processor->active_ref_count = completed_index;
                if (completed_index == 0) {
                    AICREATURE *creature = packet->field_0x134 != 0xff ? &system->creatures[packet->field_0x134] : NULL;
                    AIScriptProcessorInit(system, packet, processor, creature, NULL, NULL, 0, processor->base_script,
                                          completed_reference->return_state);
                } else {
                    AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0,
                                          processor->active_refs[completed_index - 1]->script,
                                          completed_reference->return_state);
                }
                continue;
            }
            break;
        }

        if (processor->state == NULL) {
            AIScriptSetState(processor, reinterpret_cast<AISTATE *>(NuLinkedListGetHead(&processor->script->states)));
        }

        while (processor->action_node != NULL) {
            AIACTION *action = reinterpret_cast<AIACTION *>(processor->action_node);
            bool action_completed = true;
            if (action->def != NULL && action->def->eval_fn != NULL) {
                i32 first_time = 0;
                if (processor->is_first_time_action) {
                    processor->action_data_1 = 0;
                    processor->action_data_2 = 0;
                    processor->action_data_6 = 0;
                    processor->action_data_3 = NULL;
                    processor->action_data_4 = 0.0f;
                    processor->action_data_5 = 0.0f;
                    memset(&processor->action_pos, 0, sizeof(processor->action_pos));
                    memset(&processor->path_info, 0, sizeof(processor->path_info));
                    processor->action_timer = 0.0f;
                    processor->action_data_7 = 0;

                    if (packet != NULL) {
                        packet->goal_speed_mode = 0;
                        packet->goal_path_node = NULL;
                        memset(packet->pad_17c, 0, sizeof(packet->pad_17c));
                        packet->frame_flags = 0;
                        if (ScriptProcessFirstTimeActionFn != NULL) {
                            ScriptProcessFirstTimeActionFn(system, packet, processor);
                        }
                    }

                    processor->is_first_time_action = 0;
                    first_time = 1;
                }

                action_completed = action->def->eval_fn(system, processor, packet, action->params, action->param_count,
                                                        first_time, elapsed) != 0;
                if (!action_completed) {
                    break;
                }
            } else {
                processor->is_first_time_action = 0;
            }

            NULISTLNK *action_node = processor->action_node;
            if (processor->is_first_time_action) {
                continue;
            }
            if (action_node != NULL) {
                action_node = NuLinkedListGetNext(&processor->state->actions, action_node);
            } else {
                action_node = NuLinkedListGetHead(&processor->state->actions);
            }
            if (action_node != NULL) {
                processor->is_first_time_action = 1;
            }
            processor->action_node = action_node;
        }

        if (system != NULL && processor->script != NULL && processor->active_ref_count < 4) {
            NULISTHDR *references = &processor->script->ref_scripts;
            for (AIREFSCRIPT *reference = reinterpret_cast<AIREFSCRIPT *>(NuLinkedListGetHead(references));
                 reference != NULL;
                 reference = reinterpret_cast<AIREFSCRIPT *>(NuLinkedListGetNext(references, &reference->list_node))) {
                processor->param_stack[1].is_first_time_state = 1;
                processor->param_stack[1].force_complex_eval = 1;

                if (NuLinkedListGetHead(&reference->conditions) != NULL &&
                    AIScriptProcessStateConditions(system, &reference->conditions, object, packet, processor, elapsed,
                                                   1) == NULL) {
                    continue;
                }
                if (reference->script == NULL) {
                    continue;
                }

                processor->param_stack[0].is_first_time_state = 1;
                processor->param_stack[0].force_complex_eval = 1;
                AICONDITION *entry_condition = AIScriptProcessStateConditions(
                    system, &reference->script->base_state->conditions, object, packet, processor, elapsed, 0);
                if (entry_condition != NULL && entry_condition->next_state != reference->script->base_state) {
                    const i32 reference_index = processor->active_ref_count;
                    processor->active_refs[reference_index] = reference;
                    processor->active_ref_count = reference_index + 1;
                    AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0, reference->script,
                                          entry_condition->next_state);
                    return;
                }
            }

            if (processor->state != NULL) {
                references = &processor->state->ref_scripts;
                for (AIREFSCRIPT *reference = reinterpret_cast<AIREFSCRIPT *>(NuLinkedListGetHead(references));
                     reference != NULL; reference = reinterpret_cast<AIREFSCRIPT *>(
                                            NuLinkedListGetNext(references, &reference->list_node))) {
                    processor->param_stack[1].is_first_time_state = 1;
                    processor->param_stack[1].force_complex_eval = 1;

                    if (NuLinkedListGetHead(&reference->conditions) != NULL &&
                        AIScriptProcessStateConditions(system, &reference->conditions, object, packet, processor,
                                                       elapsed, 1) == NULL) {
                        continue;
                    }
                    if (reference->script == NULL) {
                        continue;
                    }

                    processor->param_stack[0].is_first_time_state = 1;
                    processor->param_stack[0].force_complex_eval = 1;
                    AICONDITION *entry_condition = AIScriptProcessStateConditions(
                        system, &reference->script->base_state->conditions, object, packet, processor, elapsed, 0);
                    if (entry_condition != NULL && entry_condition->next_state != reference->script->base_state) {
                        const i32 reference_index = processor->active_ref_count;
                        processor->active_refs[reference_index] = reference;
                        processor->active_ref_count = reference_index + 1;
                        AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0, reference->script,
                                              entry_condition->next_state);
                        return;
                    }
                }
            }
        }

        if (processor->interrupt_timer > 0.0f) {
            processor->interrupt_timer -= elapsed;
            if (processor->interrupt_timer <= 0.0f && processor->interrupt_state != NULL) {
                AIScriptSetState(processor, processor->interrupt_state);
                processor->interrupt_priority = 0;
                processor->interrupt_state = NULL;
                return;
            }
        }

        if (processor->state != NULL) {
            AICONDITION *transition = AIScriptProcessStateConditions(system, &processor->state->conditions, object,
                                                                     packet, processor, elapsed, 0);
            if (transition != NULL) {
                processor->unknown_flag_4 = transition->keep_blocked;
                AIScriptSetState(processor, transition->next_state);
            }
        }
    }

    void AIScriptSetLevelPath(void) {
    }

    void AISetPathHeightTol(void) {
    }

    // libTTapp.so 0x3e5fd0: carve a zeroed, 16-byte-aligned block from the
    // permbuffer cursor. Returns NULL when the cursor or the end pointer is
    // missing or the buffer has less room than requested.
    void *AISysBufferAlloc(VARIPTR *cursor, VARIPTR *buf_end, u32 size) {
        void *block = NULL;
        if (cursor != NULL && buf_end != NULL) {
            const u32 offset = (u32)cursor->addr;
            if (buf_end->addr > (usize)(offset + size)) {
                block = (void *)(usize)((offset + 0xf) & ~0xfu);
                cursor->addr = (usize)block + size;
                memset(block, 0, size);
            }
        }
        return block;
    }

    void AISysCharacterMovement(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks) {
        packet->field_0x1e7 &= static_cast<u8>(~AIPACKET_MOVEMENT_FRAME_RESET);
        if (MidSpecialMoveFn != NULL && MidSpecialMoveFn(system, packet, object) != 0) {
            packet->field_0x1e6 |= AIPACKET_RUNTIME_SPECIAL_MOVE;
            packet->field_0x1e7 |= AIPACKET_MOVEMENT_SPECIAL_HANDLED;
            return;
        }

        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        u8 movement_flags = packet->movement_flags;
        const u8 mode = movement_flags & AIPACKET_MOVEMENT_MODE_MASK;

        if (packet->path_info.path != NULL && packet->path_info.connection != NULL) {
            switch (mode) {
                case AIPACKET_MOVEMENT_TO_DESTINATION:
                    AIMoveToDestination(system, packet, object, checks);
                    break;
                case AIPACKET_MOVEMENT_RETREAT:
                    AIRetreatFromDestination(system, packet, object, checks);
                    break;
                case AIPACKET_MOVEMENT_CIRCLE:
                    AICircle(system, packet, object, checks);
                    break;
                case AIPACKET_MOVEMENT_WANDER:
                    AIWander(system, packet, object, checks);
                    break;
                case AIPACKET_MOVEMENT_AVOIDING_CAMERA:
                    AIMoveToDestinationAvoidingCamera(system, packet, object, checks);
                    break;
                case AIPACKET_MOVEMENT_DIRECT:
                    AIMoveDirectlyToDestination(system, packet, object, checks);
                    break;
            }
            movement_flags = packet->movement_flags;
        } else if (mode == AIPACKET_MOVEMENT_TO_DESTINATION || mode == AIPACKET_MOVEMENT_AVOIDING_CAMERA ||
                   mode == AIPACKET_MOVEMENT_DIRECT) {
            packet->movement_destination = packet->fallback_destination;
            packet->movement_stopping_distance = packet->fallback_stopping_distance;
        }

        packet->movement_flags = movement_flags & static_cast<u8>(~AIPACKET_MOVEMENT_MODE_MASK);
    }

    void AISysCharacterSetPath(AIPACKET *packet, AIPATH *path) {
        if (packet->path_info.path == path) {
            return;
        }

        memset(&packet->path_info, 0, sizeof(packet->path_info));
        packet->path_info.path = path;
        packet->path_info.path_index = 0xff;
        packet->available_routes = 0;
        packet->inside_path_node = -1;
        packet->current_route = 0xff;
        packet->next_route = 0;
        packet->goal_path_node = NULL;

        if (path == NULL || path->route_count == 0) {
            return;
        }

        const u32 character_mask_low = static_cast<u32>(packet->character_type_mask);
        const u32 character_mask_high = static_cast<u32>(packet->character_type_mask >> 32);
        for (i32 route_index = 0; route_index < path->route_count; ++route_index) {
            AIPATHROUTE &route = path->routes[route_index];
            if ((route.character_mask[0] & character_mask_low) != 0 ||
                (route.character_mask[1] & character_mask_high) != 0) {
                packet->available_routes |= static_cast<u16>(1u << route_index);
            }
        }
    }

    void AISysCharacterSetPathCnx(AIPACKET *packet, NUVEC *position, AIPATHCNX *connection, i32 direction) {
        if (connection == NULL || packet->owner == NULL) {
            packet->path_info.connection = connection;
            packet->current_route = 0xff;
            packet->next_route = 0;
            return;
        }

        if (packet->path_info.connection == connection && packet->path_info.direction == direction &&
            (packet->path_info.flags & (AIPATHINFO_FLAG_ON_PATH | AIPATHINFO_FLAG_ROUTE_CHECKED)) !=
                AIPATHINFO_FLAG_ON_PATH) {
            return;
        }

        const u32 connection_flags = connection->traversal_flags[direction];
        if (connection_flags != 0) {
            if ((connection_flags & AIPATH_CONNECTION_FLAG_RESELECT_ROUTE) != 0) {
                AISysFindRoute(packet);
                return;
            }
            if ((packet->capabilities & connection_flags) == 0) {
                return;
            }
        }

        if (packet->path_info.connection != connection || packet->path_info.direction != direction) {
            packet->path_info.direction = static_cast<u8>(direction);
            packet->path_connection_state = 0;
            packet->path_info.connection = connection;

            AIPATHNODE &node = packet->path_info.path->nodes[connection->node_indices[0]];
            NUVEC delta;
            delta.x = position->x - node.position.x;
            delta.z = position->z - node.position.z;
            NUVEC rotated;
            NuVecRotateY(&rotated, &delta, -connection->rotation);
            packet->path_info.dist = rotated.z / connection->horizontal_distance;
            packet->path_info.width = rotated.x;
            packet->path_info.flags &= static_cast<u8>(~AIPATHINFO_FLAG_ROUTE_CHECKED);
        }

        const u8 route_state = packet->path_info.flags & (AIPATHINFO_FLAG_ON_PATH | AIPATHINFO_FLAG_ROUTE_CHECKED);
        if (route_state == AIPATHINFO_FLAG_ON_PATH) {
            const u8 route = packet->current_route;
            if (route == 0xff || route >= 16 || (connection->route_mask & (1u << route)) == 0) {
                packet->current_route = 0xff;
                AISysFindRoute(packet);
            }
            packet->path_info.flags |= AIPATHINFO_FLAG_ROUTE_CHECKED;
        }
    }

    void AISysCreatureAntinodeInteraction(void) {
    }

    void AISysCreatureInteraction2D(void) {
    }

    void AISysCreatureInteraction3D(void) {
    }

    AIAREA *AISysFindArea(AISYS *sys, char *name) {
        if (sys != NULL) {
            for (i32 i = 0; i < sys->area_count; ++i) {
                if (NuStrICmp(sys->areas[i].name, name) == 0) {
                    return &sys->areas[i];
                }
            }
        }
        return NULL;
    }

    AIPATH *AISysFindPath(AISYS *sys, char *name) {
        if (sys != NULL && sys->path_sys != NULL) {
            for (i32 i = 0; i < sys->path_sys->path_count; ++i) {
                AIPATH *path = sys->path_sys->paths[i];
                if (NuStrICmp(path->name, name) == 0) {
                    return path;
                }
            }
        }
        return NULL;
    }

    void AISysFindRoute(AIPACKET *packet) {
        AIPATH *path = packet->path_info.path;
        if (path == NULL || path->route_count == 0 || packet->path_info.connection == NULL) {
            return;
        }

        const u16 valid_routes = packet->available_routes & packet->path_info.connection->route_mask;
        if (valid_routes == 0) {
            return;
        }

        i32 route = packet->next_route;
        if (route >= path->route_count) {
            route = 0;
        }
        const i32 first_route = route;
        do {
            if ((valid_routes & (1u << route)) != 0) {
                packet->current_route = static_cast<u8>(route);
                packet->next_route = static_cast<u8>(route + 1);
                return;
            }
            ++route;
            if (route >= path->route_count) {
                route = 0;
            }
        } while (route != first_route);
    }

    void AISysGetCharacterPathPos(AISYS *system, APIOBJECT *object, AIPACKET *packet, i32 checks, i32 ground) {
        f32 nearest_distance_squared = 3.402823466e+38f;

        const u8 previous_flags = packet->path_info.flags;
        packet->path_info.flags =
            (previous_flags &
             static_cast<u8>(~(AIPATHINFO_FLAG_ON_PATH | AIPATHINFO_FLAG_WAS_ON_PATH | AIPATHINFO_FLAG_NARROW_PATH))) |
            ((previous_flags & AIPATHINFO_FLAG_ON_PATH) << 1);

        if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
            AISysCharacterSetPath(packet, NULL);
            AISysCharacterSetPathCnx(packet, &object->position, NULL, 0);
            return;
        }

        AIPATH *path = packet->path_info.path;
        if (path == NULL) {
            path = (packet->navigation_flags & AIPACKET_NAVIGATION_FLAG_SEARCH_ALL_PATHS) != 0
                       ? system->path_sys->paths[0]
                       : system->path_sys->active_path;
            AISysCharacterSetPath(packet, path);
            AISysCharacterSetPathCnx(packet, &object->position, NULL, 0);
            packet->path_info.next_check = 0;
            packet->path_info.path_index = 0;
        }
        if (path == NULL) {
            return;
        }

        AISysResetPathSearchConnectionChecks(path);

        AIPATHCNX *current_connection = packet->path_info.connection;
        if (current_connection != NULL) {
            const i32 current_direction = packet->path_info.direction;
            if (AISysCharacterTestPathCnx(system, object, packet, current_connection, current_direction,
                                          &nearest_distance_squared) != 0) {
                return;
            }

            const bool check_adjacent_connections =
                (object->flags_low & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 ||
                (packet->movement_target == NULL && packet->path_connection_state == 0 && ground != 0);
            if (!check_adjacent_connections) {
                return;
            }

            const u8 first_node_index = current_connection->node_indices[current_direction];
            if (first_node_index < path->node_count) {
                AIPATHNODE &first_node = path->nodes[first_node_index];
                for (i32 connection_index = 0; connection_index < first_node.connection_count; ++connection_index) {
                    AIPATHCNX *candidate = first_node.connections[connection_index];
                    if (candidate != current_connection &&
                        AISysCharacterTestPathCnx(system, object, packet, candidate, 0, &nearest_distance_squared) !=
                            0) {
                        return;
                    }
                }
            }

            path = packet->path_info.path;
            const u8 second_node_index = current_connection->node_indices[current_direction == 0];
            if (second_node_index < path->node_count) {
                AIPATHNODE &second_node = path->nodes[second_node_index];
                for (i32 connection_index = 0; connection_index < second_node.connection_count; ++connection_index) {
                    AIPATHCNX *candidate = second_node.connections[connection_index];
                    const i32 direction = candidate->node_indices[0] != second_node_index;
                    if (candidate != current_connection &&
                        AISysCharacterTestPathCnx(system, object, packet, candidate, direction,
                                                  &nearest_distance_squared) != 0) {
                        return;
                    }
                }
            }
        }

        path = packet->path_info.path;
        if ((packet->navigation_flags & AIPACKET_NAVIGATION_FLAG_SEARCH_ALL_PATHS) == 0) {
            i32 remaining_checks = path->node_count < checks ? path->node_count : checks;
            while (remaining_checks-- > 0) {
                if (packet->path_info.next_check >= path->node_count) {
                    packet->path_info.next_check = 0;
                }
                AIPATHNODE &node = path->nodes[packet->path_info.next_check];
                for (i32 connection_index = 0; connection_index < node.connection_count; ++connection_index) {
                    if (AISysCharacterTestPathCnx(system, object, packet, node.connections[connection_index], -1,
                                                  &nearest_distance_squared) != 0) {
                        return;
                    }
                }
                packet->path_info.next_check = (packet->path_info.next_check + 1) % path->node_count;
            }
            return;
        }

        // Characters allowed to search across paths retain their current
        // cursor after probing, just as they retain their current connection.
        AIPATH *original_path = packet->path_info.path;
        AIPATHCNX *original_connection = packet->path_info.connection;
        const u8 original_direction = packet->path_info.direction;

        if (packet->path_info.path_index == 0xff) {
            packet->path_info.path_index = original_path->index;
        }
        if (packet->path_info.path_index >= system->path_sys->path_count) {
            packet->path_info.path_index = 0;
        }

        path = system->path_sys->paths[packet->path_info.path_index];
        packet->path_info.path = path;
        if (packet->path_info.next_check >= path->node_count) {
            packet->path_info.next_check = 0;
        }
        AISysResetPathSearchConnectionChecks(path);

        bool position_inside_path_bounds = false;
        for (i32 remaining_checks = checks; remaining_checks > 0; --remaining_checks) {
            if (!position_inside_path_bounds) {
                position_inside_path_bounds = AIPathCheckExtents(path, &packet->owner->apiobj.position) != 0;
            }

            if (position_inside_path_bounds) {
                AIPATHNODE &node = path->nodes[packet->path_info.next_check];
                for (i32 connection_index = 0; connection_index < node.connection_count; ++connection_index) {
                    if (AISysCharacterTestPathCnx(system, object, packet, node.connections[connection_index], -1,
                                                  &nearest_distance_squared) != 0) {
                        return;
                    }
                }

                ++packet->path_info.next_check;
                if (packet->path_info.next_check < path->node_count) {
                    continue;
                }
            }

            packet->path_info.next_check = 0;
            ++packet->path_info.path_index;
            if (packet->path_info.path_index >= system->path_sys->path_count) {
                packet->path_info.path_index = 0;
            }

            path = system->path_sys->paths[packet->path_info.path_index];
            packet->path_info.path = path;
            AISysCharacterSetPathCnx(packet, &object->position, NULL, 0);
            AISysResetPathSearchConnectionChecks(path);
            position_inside_path_bounds = false;
        }

        packet->path_info.path = original_path;
        packet->path_info.connection = original_connection;
        packet->path_info.direction = original_direction;
    }

    void AISysGetCharacterWaypoint(void) {
    }

    void AISysGetPathColour(void) {
    }

    void AISysGetPathColourCount(void) {
    }

    void AISysGetPathPos(void) {
    }

    void AISysGetPathPosEx(void) {
    }

    void *AISysLoadEx(void *buffer, void *buffer_end, i32 storage_size, void *scene, char *directory, char *name,
                      char *parameter, char *load_directory) {
        VARIPTR *cursor = static_cast<VARIPTR *>(buffer);
        VARIPTR *end = static_cast<VARIPTR *>(buffer_end);
        NUGSCN *gscene = static_cast<NUGSCN *>(scene);
        VARIPTR pak_start = *end;
        void *pak = NULL;
        char ai2_path[256];
        char pak_path[256];
        char script_path[256];

        if (ai_usepackfile != 0) {
            sprintf(pak_path, "%sLevels\\%s\\%s\\ai.pak", AiLevelPathName, directory, parameter);
            i32 pak_size = static_cast<i32>(NuFileSize(pak_path));
            if (pak_size > 0) {
                pak_start.addr = end->addr - ALIGN(pak_size + 1, 0x10);
                pak = NuFilePakLoad(pak_path, &pak_start, *end, 0x10);
            }
        }

        sprintf(ai2_path, "%sLevels\\%s\\%s\\%s.ai2", AiLevelPathName, load_directory, parameter, name);

        AISYS *system = static_cast<AISYS *>(AISysBufferAlloc(cursor, end, sizeof(AISYS)));
        if (system == NULL) {
            return NULL;
        }
        memset(system, 0, sizeof(AISYS));

        system->storage_size = storage_size;
        system->storage = AISysBufferAlloc(cursor, end, storage_size);
        system->storage_end = *cursor;
        memset(system->storage, 0, storage_size);
        system->storage_cursor.addr = reinterpret_cast<usize>(system->storage);

        EdFileSetMedia(1);
        i32 is_open = 0;
        if (pak != NULL) {
            sprintf(pak_path, "%s.ai2", name);
            EdFileSetPakFile(pak);
            is_open = EdFileOpen(pak_path, NUFILE_READ);
        }
        if (is_open == 0) {
            EdFileSetPakFile(NULL);
            is_open = EdFileOpen(ai2_path, NUFILE_READ);
        }

        if (is_open != 0) {
            i32 version = EdFileReadInt();
            system->scene = gscene;

            // The shipped TCS assets use version 20. These section readers
            // follow the original version-20 branches and retain the original
            // gates that affect the current layout.
            if (version == 20) {
                system->path_sys = AISysLoadPaths(system, version, gscene);
                AISysLoadAreas(system, version);
                AISysLoadLocators(system, version);
                AISysLoadLocatorSets(system, version);
                AISysLoadCreatures(system, version);
                AISysLoadAntinodes(system, version, gscene);
                if (GameAILoadFn != NULL) {
                    GameAILoadFn(system, version, gscene, cursor, end);
                }
            }
            EdFileClose();
        }

        sprintf(script_path, "%sLevels\\%s\\%s", AiLevelPathName, directory, parameter);
        AIScriptLoadAllPakFile(pak, script_path, cursor, &pak_start, system);
        AISysSetLevelPath(system, NULL);
        return system;
    }

    void AISysProcess(AISYS *system, APIOBJECT *player_1, APIOBJECT *player_2) {
        if (system == NULL) {
            return;
        }

        system->player_1 = player_1;
        system->player_2 = player_2;

        AIANTINODE *dynamic = reinterpret_cast<AIANTINODE *>(dynamic_antinodes);
        const i32 antinode_count = system->antinode_count;
        for (i32 index = 0; index < antinode_count + 64; ++index) {
            AIANTINODE *antinode =
                index < antinode_count ? &system->antinodes[index] : &dynamic[index - antinode_count];
            if (antinode->has_special == 0) {
                continue;
            }

            void *special = antinode->special_handle;
            if (FindAlternativeSpecialObjectFn != NULL && NuSpecialGetVisibilityFn(special) == 0) {
                FindAlternativeSpecialObjectFn(system, reinterpret_cast<nuhspecial_s *>(special));
            }
            if (NuSpecialGetVisibilityFn(special) == 0) {
                continue;
            }

            NUMTX *draw_matrix = NuSpecialGetDrawMtx(special);
            NuVecMtxTransform(&antinode->position, &antinode->special_position, draw_matrix);
            antinode->height = antinode->width + antinode->position.y;
            antinode->max_height = antinode->position.y + antinode->min_height;

            if (antinode->type != 0) {
                NUVEC forward = {0.0f, 0.0f, 1.0f};
                NUVEC rotated;
                NuVecMtxRotate(&rotated, &forward, draw_matrix);
                antinode->flags = NuAngAdd(NuAtan2D(rotated.x, rotated.z), antinode->rotation_offset);
            }
        }

        for (i32 index = 0; index < 16; ++index) {
            AIGROUP *group = &system->groups[index];
            if (group->is_used) {
                group->can_respawn = group->member_is_alive == 0;
            }
        }

        for (i32 index = 0; index < system->locator_count; ++index) {
            AILOCATOR *locator = &system->locators[index];
            if (locator->path == NULL || locator->connection == NULL) {
                continue;
            }

            AIPATHNODE *start = &locator->path->nodes[locator->connection->node_indices[0]];
            AIPATHNODE *end = &locator->path->nodes[locator->connection->node_indices[1]];
            if ((start->runtime_flags & AIPATHNODE_RUNTIME_POSITION_CHANGED) == 0 &&
                (end->runtime_flags & AIPATHNODE_RUNTIME_POSITION_CHANGED) == 0) {
                continue;
            }

            NUVEC delta;
            NUVEC direction;
            NuVecSub(&delta, &end->position, &start->position);
            NuVecNorm(&direction, &delta);

            const f32 position_on_connection = locator->min_distance;
            f32 radius;
            if (position_on_connection < 0.0f) {
                radius = start->radius;
            } else if (position_on_connection <= 1.0f) {
                radius = end->radius * position_on_connection + (1.0f - position_on_connection) * start->radius;
            } else {
                radius = end->radius;
            }

            NUVEC perpendicular = {direction.z * radius, 0.0f, -direction.x * radius};
            locator->position = start->position;

            NUVEC offset;
            NuVecScale(&offset, &delta, position_on_connection);
            NuVecAdd(&locator->position, &locator->position, &offset);
            NuVecScale(&offset, &perpendicular, locator->max_distance);
            NuVecAdd(&locator->position, &locator->position, &offset);

            const i32 path_angle = static_cast<i32>(NuAtan2(delta.x, delta.z) * 10430.378f);
            locator->flags = NuAngAdd(path_angle, locator->locator_flags);
        }
    }

    void AISysProcessCharacter(AISYS *system, APIOBJECT *object, AIPACKET *packet, i32 checks, f32 elapsed,
                               i32 use_three_dimensions, i32 process_ai) {
        AISCRIPTPROCESS *primary_processor = reinterpret_cast<AISCRIPTPROCESS *>(packet);
        AISCRIPTPROCESS *processor = packet->alternate_script_process;
        if (processor == NULL) {
            processor = primary_processor;
        }

        if ((packet->field_0x1e6 & AIPACKET_RUNTIME_INITIALISED) == 0) {
            AISTATE *saved_state = primary_processor->state;
            if (AIScriptSetStateByName(primary_processor, "Initialise") != 0) {
                AIScriptProcess(system, object, packet, primary_processor, 0.0f);
                AIScriptSetState(primary_processor, saved_state);
            }
            packet->field_0x1e6 |= AIPACKET_RUNTIME_INITIALISED;
        }

        if (process_ai != 0) {
            AISysUpdateCharacterPathPos(system, object, packet, checks, elapsed);

            const u8 runtime_flags = packet->field_0x1e6;
            packet->movement_flags &= static_cast<u8>(~AIPACKET_MOVEMENT_OPTION_TRANSIENT);
            packet->field_0x1e5 &= static_cast<u8>(~4u);
            packet->frame_flags = 0;
            packet->field_0x1e6 = runtime_flags & static_cast<u8>(~AIPACKET_RUNTIME_SPECIAL_MOVE);

            if ((object->field_0x1f8 & APIOBJECT_FLAG_AI_PLAYER_MASK) == APIOBJECT_FLAG_PLAYER_ACTIVE) {
                packet->path_connection_state = 0;
                packet->movement_destination = object->position;
                packet->movement_target = NULL;
                packet->field_0x1e6 = runtime_flags & 7;
                packet->field_0x1e7 &= 0x3f;
                packet->navigation_flags &= static_cast<u8>(~AIPACKET_NAVIGATION_FLAG_TRANSIENT);
            } else {
                if (AIRespawnOnPathFn != NULL && AIRespawnOnPathFn(object) != 0) {
                    return;
                }

                NuSpecialClear(&object->field_0x208);
                object->field_0x1fa &= static_cast<u8>(~4u);
                if (system != NULL && processor->script != NULL) {
                    AIScriptProcess(system, object, packet, processor, elapsed);
                }

                packet->navigation_flags &= static_cast<u8>(~AIPACKET_NAVIGATION_FLAG_TRANSIENT);
                packet->frame_state = 0;
                packet->field_0x1e6 &= 0x0f;

                const u8 source_flags = packet->field_0x1e7;
                packet->field_0x1e7 = source_flags & 0x3f;
                if ((source_flags & AIPACKET_MOVEMENT_SOURCE_ACTIVE) != 0) {
                    const u8 source = (source_flags & AIPACKET_MOVEMENT_SOURCE_MASK) >> 2;
                    if (source == 1 && packet->field_0x134 != 0xff) {
                        AICREATURE *creature = &system->creatures[packet->field_0x134];
                        AIMoveInstruction(packet, &creature->pos, 0.0f, &creature->path_info,
                                          AIPACKET_MOVEMENT_TO_DESTINATION, 0.0f);
                    } else if (source == 2 && packet->locator != NULL) {
                        AIPATHINFO *locator_path = reinterpret_cast<AIPATHINFO *>(&packet->locator->path);
                        AIMoveInstruction(packet, &packet->locator->position, 0.0f, locator_path,
                                          AIPACKET_MOVEMENT_TO_DESTINATION, 0.0f);
                    } else {
                        packet->field_0x1e7 = source_flags & 0x2f;
                    }
                }

                AISysCharacterMovement(system, packet, object, checks);

                if ((packet->field_0x1e7 & AIPACKET_MOVEMENT_SOURCE_MASK) != 0 && packet->movement_target == NULL &&
                    packet->movement_target_radius > 0.0f &&
                    (object->field_0x1f4 & APIOBJECT_MOTION_FLAG_AI_CONTROLLED) != 0) {
                    NUVEC *source_position = NULL;
                    const u8 source = (packet->field_0x1e7 & AIPACKET_MOVEMENT_SOURCE_MASK) >> 2;
                    if (source == 1 && packet->field_0x134 != 0xff) {
                        source_position = &system->creatures[packet->field_0x134].pos;
                    } else if (source == 2 && packet->locator != NULL) {
                        source_position = &packet->locator->position;
                    }

                    if (source_position != NULL) {
                        NUVEC source_delta;
                        const f32 distance_squared = NuVecXZDistSqr(source_position, &object->position, &source_delta);
                        const f32 radius = packet->movement_target_radius;
                        const u8 current_source_flags = packet->field_0x1e7;
                        if ((current_source_flags & AIPACKET_MOVEMENT_SOURCE_ACTIVE) == 0) {
                            if (distance_squared > radius * radius) {
                                const f32 release_radius = radius + 0.5f;
                                if (distance_squared > release_radius * release_radius) {
                                    packet->field_0x1e7 = current_source_flags | AIPACKET_MOVEMENT_SOURCE_ACTIVE;
                                } else {
                                    NUVEC movement_delta = {
                                        packet->movement_destination.x - object->position.x,
                                        0.0f,
                                        packet->movement_destination.z - object->position.z,
                                    };
                                    if (NuVecDot(&movement_delta, &source_delta) < 0.0f) {
                                        packet->movement_destination = object->position;
                                        packet->movement_stopping_distance = 0.0f;
                                    }
                                }
                            }
                        } else if (distance_squared < radius * radius) {
                            packet->field_0x1e7 =
                                current_source_flags & static_cast<u8>(~AIPACKET_MOVEMENT_SOURCE_ACTIVE);
                        }
                    }
                }
            }
        }

        f32 clearance = 0.0f;
        if (packet->movement_stopping_distance != 0.0f) {
            clearance = packet->mover_height;
        }

        NUVEC delta;
        f32 distance;
        if (use_three_dimensions == 0) {
            distance = NuVecXZDist(&packet->movement_destination, &object->position, &delta);
        } else {
            distance = NuVecDist(&packet->movement_destination, &object->position, &delta);
        }

        const f32 stopping_clearance = clearance + packet->movement_stopping_distance;
        if (distance <= packet->mover_height + stopping_clearance) {
            const f32 scale = distance != 0.0f && stopping_clearance != 0.0f ? stopping_clearance / distance : 0.0f;
            NuVecScale(&delta, &delta, scale);
            NuVecSub(&packet->movement_position, &packet->movement_destination, &delta);
        } else {
            const f32 scale = distance != 0.0f && packet->mover_height != 0.0f ? packet->mover_height / distance : 0.0f;
            NuVecScale(&delta, &delta, scale);
            NuVecAdd(&packet->movement_position, &object->position, &delta);
        }
    }

    i32 AISysSetLevelPath(AISYS *system, char *path_name) {
        if (system == NULL || system->path_sys == NULL) {
            return 0;
        }

        AIPATH *path = NULL;
        if (path_name == NULL) {
            path = system->path_sys->paths[0];
        } else {
            for (i32 i = 0; i < system->path_sys->path_count; ++i) {
                if (NuStrICmp(system->path_sys->paths[i]->name, path_name) == 0) {
                    path = system->path_sys->paths[i];
                    break;
                }
            }
        }

        if (path == NULL || system->path_sys->active_path == path) {
            return 0;
        }
        system->path_sys->active_path = path;
        return 1;
    }

    void AISysSetPathCylinderCheck(void) {
    }

    void AISysUpdateCharacterPathPos(AISYS *system, APIOBJECT *object, AIPACKET *packet, i32 checks, f32 elapsed) {
        u8 movement_source_flags = packet->field_0x1e7;
        const bool should_find_path =
            (movement_source_flags & AIPACKET_MOVEMENT_FORCE_PATH_REFRESH) == 0 &&
            (((object->flags_low & 4) == 0 || (packet->path_info.flags & AIPATHINFO_FLAG_ON_PATH) == 0) ||
             object->supporting_platform_id != -1);

        if (should_find_path) {
            if (packet->path_info.path == NULL) {
                packet->last_path_position = object->position;
            } else {
                AISysGetCharacterPathPos(system, object, packet, (object->field_0x1fa & 8) == 0, checks);
                movement_source_flags = packet->field_0x1e7;
            }

            if ((packet->path_info.flags & AIPATHINFO_FLAG_ON_PATH) == 0) {
                packet->time_off_path += elapsed;
            } else {
                packet->time_off_path = 0.0f;
            }
        } else {
            AIPATH *path = packet->path_info.path;
            AIPATHCNX *connection = packet->path_info.connection;
            if (path != NULL && connection != NULL) {
                for (i32 endpoint = 0; endpoint < 2; ++endpoint) {
                    const u8 node_index = connection->node_indices[endpoint];
                    AIPATHNODE *node = &path->nodes[node_index];
                    if (node->has_special != 0 &&
                        (path->updated_node_bits[node_index >> 3] & (1u << (node_index & 7))) == 0) {
                        AIPathNodeUpdatePos(system, path, node);
                    }
                }
            }

            if (packet->inside_path_node != -1 && path != NULL) {
                const i32 node_index = packet->inside_path_node;
                path->inside_node_bits[node_index >> 3] |= static_cast<u8>(1u << (node_index & 7));
            }
            packet->time_off_path = 0.0f;
            movement_source_flags = packet->field_0x1e7;
        }

        packet->field_0x1e7 = movement_source_flags & static_cast<u8>(~AIPACKET_MOVEMENT_FORCE_PATH_REFRESH);
    }

    void AddLocalAIMessage(void) {
    }

    void AddToAIGroup(AIGROUP *group, APIOBJECT *object) {
        if (group == NULL || object == NULL || group->member_count >= 16 || group->count_across == 0) {
            return;
        }

        const u8 member_index = group->member_count;
        const i32 row_count = (member_index + group->count_across) / group->count_across;
        if (row_count > 4) {
            return;
        }

        if (member_index == 0) {
            group->leader = object;
            if (group->x_spacing > 0.0f) {
                group->radius = (group->count_across - 1) * group->x_spacing * 0.5f;
                const f32 turn_radius_per_frame = group->radius * 0.016666668f;
                group->rotation_speed = turn_radius_per_frame != 0.0f
                                            ? static_cast<i16>(group->max_speed / turn_radius_per_frame * 10430.378f)
                                            : 0;
            } else {
                group->radius = 0.0f;
                group->rotation_speed = 100;
            }
        }

        group->row_count = static_cast<u8>(row_count);
        object->ai->group = group;
        object->ai->group_row = member_index;
        object->ai->group_member = member_index / group->count_across;
        object->ai->group_column = member_index % group->count_across;
        group->members[member_index] = object;
        group->member_count = member_index + 1;
    }

    void AiRndrLine3d(void) {
    }

    void AiRndrLine3dDbg(void) {
    }

    void AiSysOnlyUsePakFile(void) {
    }

    void AiSysSetStateDebugee(void) {
    }

    void AiSysUsePackFile(void) {
    }

    void CalculateLocatorDirection(void) {
    }

    void CheckAIMessage(void) {
    }

    void ClearAIMessageSys(void) {
    }

    AIGROUP *CreateAIGroup(AISYS *system, u8 count_across, f32 x_spacing, f32 z_spacing, f32 max_speed) {
        if (system == NULL || count_across == 0) {
            return NULL;
        }

        for (i32 index = 0; index < 16; ++index) {
            AIGROUP *group = &system->groups[index];
            if (group->is_used != 0) {
                continue;
            }

            group->is_used = 1;
            group->count_across = count_across;
            group->x_spacing = x_spacing;
            group->z_spacing = z_spacing;
            group->is_in_formation = 0;
            group->max_speed = max_speed;
            return group;
        }
        return NULL;
    }

    void CreateAIMessageSys(void) {
    }

    void DestroyAIGroup(void) {
    }

    void FindAIDirectionedRandomPointOnNetwork2D(void) {
    }

    void FindLocalAIMessage(void) {
    }

    void FollowAPIObject(void) {
    }

    void GetAIMessage(void) {
    }

    void InitFn_AIPathDeleted(void) {
    }

    void InitFn_AIPathNodeDeleted(void) {
    }

    void InitFn_AIPathNodeMoved(void) {
    }

    void InitFn_GameAISYSRebuildFromEditorData(void) {
    }

    void InitFn_GameAISave(void) {
    }

    void InitFn_GameParamToFloat(void) {
    }

    void InitFn_ScriptProcessFirstTimeAction(SCRIPTPROCESSFIRSTTIMEACTION *function) {
        ScriptProcessFirstTimeActionFn = function;
    }

    void LEGO_AISysCreatureInteraction2D(void) {
    }

    void QueryAIMessage(void) {
    }

    void QueryLocalMessage(void) {
    }

    void RemoveAIMessage(void) {
    }

    void ResetAIMessageSys(void) {
    }

    void SetAIMessage(void) {
    }

} // extern "C"
