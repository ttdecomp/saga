#include "legoapi/world/world_shared.h"
#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edtools/edfile.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufilepak.h"

#include <stdio.h>
#include <string.h>

extern "C" {
    AICHARACTERTYPEID *GlobalCharacterTypeIDFn;
    AISPECIALROUTECHARACTERTYPEID *SpecialRouteCharacterTypeIDFn;
    AICHARACTERDISTANCE *GetViewRangeFn;
    AICHARACTERDISTANCE *GetHearDistanceFn;
    AICHARACTERDISTANCE *GetMaxViewHeightFn;
    AICHARACTERDISTANCE *GetMinViewHeightFn;
    GAMEAILOAD *GameAILoadFn;
    char *AiLevelPathName = "";
}

void AIPathCalcExtents(AIPATH *path);

extern "C" void *AISysBufferAlloc(VARIPTR *cursor, VARIPTR *buf_end, u32 size);
extern "C" i32 AISysSetLevelPath(AISYS *system, char *path_name);

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
                node->traversal_flags = static_cast<u8>(EdFileReadChar()) & ~6u;
                node->path_flags = EdFileReadShort();
                node->special_type = static_cast<u8>(EdFileReadChar());

                char special_name[256] = {};
                i32 special_name_length = EdFileReadChar();
                if (special_name_length != 0) {
                    EdFileRead(special_name, special_name_length);
                    node->has_special = static_cast<u8>(NuSpecialFind(scene, &node->special, special_name, 1) != 0);
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
                antinode->has_special = static_cast<u8>(NuSpecialFind(scene, &antinode->special, special_name, 1) != 0);
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

    void AIFormationFollow(void) {
    }

    void AILocatorSet_AssignFurthestLocator(void) {
    }

    void AILocatorSet_AssignNearestLocator(void) {
    }

    void AILocatorSet_AssignRandomLocator(void) {
    }

    void AILocatorSet_CheckLocatorsStillAssigned(void) {
    }

    void AIMoveInstruction(void) {
    }

    void *AIPAthFindPathCnx(AISYS_s *, i32, char *, void *, void *) {
        return NULL;
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

    void *AIPathFindNode(AISYS_s *, char *, i32) {
        return NULL;
    }

    void AIPathFindPathCnxFromIX(void) {
    }

    void AIPathNodeDistanceToPathNode(void) {
    }

    void AIPathNodeUpdatePos(void) {
    }

    void AISYSRebuildFromEditorData(void) {
    }

    void AIScriptForceParamReEval(void) {
    }

    void AIScriptNameFromIx(void) {
    }

    void AIScriptProcess(void) {
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

    void AISysCharacterMovement(void) {
    }

    void AISysCharacterSetPath(void) {
    }

    void AISysCharacterSetPathCnx(void) {
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

    void AISysFindRoute(void) {
    }

    void AISysGetCharacterPathPos(void) {
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

    void AISysProcess(void) {
    }

    void AISysProcessCharacter(void) {
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

    void AISysUpdateCharacterPathPos(void) {
    }

    void AddLocalAIMessage(void) {
    }

#ifdef __EMSCRIPTEN__
    void AddToAIGroup(AIGROUP *, GameObject_s *) {
    }
#else
    void AddToAIGroup(void) {
    }
#endif

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

    void InitFn_AIActionParseSpeed(void) {
    }

    void InitFn_AIBigJumpToDestination(void) {
    }

    void InitFn_AIPathDeleted(void) {
    }

    void InitFn_AIPathNodeDeleted(void) {
    }

    void InitFn_AIPathNodeMoved(void) {
    }

    void InitFn_AIRespawnOnPath(void) {
    }

    void InitFn_APIOBJECTFromObjIDFn(void) {
    }

    void InitFn_ClearAICreatures(void) {
    }

    void InitFn_FindAlternativeSpecialObjectFn(void) {
    }

    void InitFn_GameAILoad(void) {
    }

    void InitFn_GameAISYSRebuildFromEditorData(void) {
    }

    void InitFn_GameAISave(void) {
    }

    void InitFn_GameParamToFloat(void) {
    }

    void InitFn_GetAICreatureOrigin(void) {
    }

    void InitFn_GetNamedAPIObject(void) {
    }

    void InitFn_ScriptProcessFirstTimeAction(void) {
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

    void SetScriptErrorLevel(void) {
    }

} // extern "C"
