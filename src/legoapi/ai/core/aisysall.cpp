#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// The target computes two tangent points for a corner. Until that curved-path
// helper is recovered, stage a turn at the shared node so ordinary followers
// cannot cut directly across level geometry.
static i32 CalculateIntersection(AISYS_s *, AIPACKET_s *packet, APIOBJECT_s *object, AIPATHCNX_s *current_connection,
                                 AIPATHCNX_s *target_connection) {
    if (packet->intersection_connection != current_connection ||
        packet->intersection_target_connection != target_connection) {
        packet->movement_flags &=
            static_cast<u8>(~(AIPACKET_MOVEMENT_DIVERSION_RIGHT | AIPACKET_MOVEMENT_DIVERSION_LEFT));
        packet->intersection_connection = current_connection;
        packet->intersection_target_connection = target_connection;
    }

    AIPATH *path = packet->path_info.path;
    if (path == NULL || path->nodes == NULL || current_connection == NULL || target_connection == NULL) {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        return 0;
    }

    u8 shared_node_index = 0xff;
    for (i32 current_end = 0; current_end < 2 && shared_node_index == 0xff; ++current_end) {
        for (i32 target_end = 0; target_end < 2; ++target_end) {
            if (current_connection->node_indices[current_end] == target_connection->node_indices[target_end]) {
                shared_node_index = current_connection->node_indices[current_end];
                break;
            }
        }
    }
    if (shared_node_index >= path->node_count) {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        return 0;
    }

    NUANG incoming_angle = current_connection->rotation;
    if (shared_node_index == current_connection->direction_a) {
        incoming_angle = NuAngAdd(incoming_angle, NUANG_180DEG);
    }
    NUANG outgoing_angle = target_connection->rotation;
    if (shared_node_index == target_connection->direction_b) {
        outgoing_angle = NuAngAdd(outgoing_angle, NUANG_180DEG);
    }

    const i32 turn = static_cast<i16>(NuAngSub(outgoing_angle, incoming_angle));
    if (turn < -0x100 || turn > 0x100) {
        const NUVEC &shared_position = path->nodes[shared_node_index].position;
        packet->right_diversion = shared_position;
        packet->left_diversion = shared_position;
        packet->movement_flags |= AIPACKET_MOVEMENT_DIVERSION_RIGHT | AIPACKET_MOVEMENT_DIVERSION_LEFT;
        packet->runtime_flags &= static_cast<u8>(~AIPACKET_RUNTIME_PATH_BLOCKED);
        return 1;
    }
    packet->runtime_flags &= static_cast<u8>(~AIPACKET_RUNTIME_PATH_BLOCKED);
    return 0;
}

void AISysGetPathPos2(AISYS_s *, nuvec_s *, AIPATHINFO_s *, nuvec_s *, AIPATH_s *, i32) {
}

void ClearAICreatures() {
}

void AIMoveCanReachPath(AISYS_s *, AIPATH_s *, AIPATH_s *) {
}

void AIMoveToDestination(AISYS_s *system, AIPACKET_s *packet, APIOBJECT_s *object, i32 checks) {
    AIPATH *path = packet->path_info.path;
    AIPATHCNX *connection = packet->path_info.connection;
    AIPATHINFO &destination_path_info = packet->fallback_path_info;

    // The target asks AIMoveFindDivertNode for a cross-path transition and
    // stops at the current position when none is available. Never substitute a
    // straight line to a destination on an unrelated path graph.
    packet->movement_destination = object->position;
    packet->movement_stopping_distance = 0.0f;
    if (path == NULL || path != destination_path_info.path || connection == NULL ||
        destination_path_info.connection == NULL || path->nodes == NULL) {
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        return;
    }

    AIPATHCNX *destination_connection = destination_path_info.connection;
    if (connection == destination_connection) {
        packet->movement_destination = packet->fallback_destination;
        packet->movement_stopping_distance = packet->fallback_stopping_distance;
        packet->goal_path_node = NULL;
        packet->runtime_flags &=
            static_cast<u8>(~(AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT));
        return;
    }

    // The target first adopts the destination connection when the character's
    // terrain origin is already inside its corridor. Its direction is chosen
    // from the endpoint shared with the current connection.
    if (WithinConnection(system, &packet->terrain_origin, path, destination_connection, checks, connection,
                         packet->current_route, object->field_0x289, NULL, object->collision_radius, 0) != 0) {
        const u8 destination_node_a = destination_connection->node_indices[0];
        const i32 destination_direction =
            destination_node_a == connection->node_indices[0] || destination_node_a == connection->node_indices[1] ? 0
                                                                                                                   : 1;
        AISysCharacterSetPathCnx(packet, &object->position, destination_connection, destination_direction);
        if (packet->path_info.connection == destination_connection) {
            packet->movement_event_flags |= AIPACKET_PATH_CONNECTION_CHANGED;
            packet->movement_destination = packet->fallback_destination;
            packet->movement_stopping_distance = packet->fallback_stopping_distance;
            packet->goal_path_node = NULL;
            packet->runtime_flags &=
                static_cast<u8>(~(AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT));
            return;
        }
    }

    const u8 current_node_a = connection->node_indices[0];
    const u8 current_node_b = connection->node_indices[1];
    const u8 destination_node_a = destination_connection->node_indices[0];
    const u8 destination_node_b = destination_connection->node_indices[1];
    if (current_node_a >= path->node_count || current_node_b >= path->node_count ||
        destination_node_a >= path->node_count || destination_node_b >= path->node_count) {
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        return;
    }

    // Match the target's ordinary endpoint solver: compare all four graph
    // routes between the current and destination connection endpoints, adding
    // the partial distance along each connection. This also permits a clean
    // reversal when the valid route leaves behind the character.
    f32 current_dist = packet->path_info.dist;
    f32 distance_to_current_a = connection->distance * current_dist;
    if (distance_to_current_a < 0.0f) {
        distance_to_current_a = -distance_to_current_a;
    }
    f32 distance_to_current_b = connection->distance * (1.0f - current_dist);
    if (distance_to_current_b < 0.0f) {
        distance_to_current_b = -distance_to_current_b;
    }
    f32 destination_dist = destination_path_info.dist;
    f32 distance_from_a = destination_connection->distance * destination_dist;
    if (distance_from_a < 0.0f) {
        distance_from_a = -distance_from_a;
    }
    f32 distance_from_b = destination_connection->distance * (1.0f - destination_dist);
    if (distance_from_b < 0.0f) {
        distance_from_b = -distance_from_b;
    }
    u8 waypoint_index = connection->node_indices[(packet->path_info.direction != 0 ? 1 : 0) ^ 1];
    u8 goal_index = destination_connection->node_indices[destination_path_info.direction != 0 ? 1 : 0];
    f32 best_route_distance = (waypoint_index == current_node_a ? distance_to_current_a : distance_to_current_b) +
                              AIPathNodeDistanceToPathNode(path, waypoint_index, goal_index, packet->current_route, 0) +
                              (goal_index == destination_node_a ? distance_from_a : distance_from_b);

    const u8 current_nodes[2] = {current_node_a, current_node_b};
    const u8 destination_nodes[2] = {destination_node_a, destination_node_b};
    const f32 current_endpoint_distances[2] = {distance_to_current_a, distance_to_current_b};
    const f32 destination_endpoint_distances[2] = {distance_from_a, distance_from_b};
    for (i32 current_end = 0; current_end < 2; ++current_end) {
        for (i32 destination_end = 0; destination_end < 2; ++destination_end) {
            const f32 candidate_route_distance =
                current_endpoint_distances[current_end] +
                AIPathNodeDistanceToPathNode(path, current_nodes[current_end], destination_nodes[destination_end],
                                             packet->current_route, 0) +
                destination_endpoint_distances[destination_end];
            if (candidate_route_distance < best_route_distance) {
                best_route_distance = candidate_route_distance;
                waypoint_index = current_nodes[current_end];
                goal_index = destination_nodes[destination_end];
            }
        }
    }

    const i32 route_direction = waypoint_index == current_node_b ? 0 : 1;
    if (packet->path_info.direction != route_direction) {
        AISysCharacterSetPathCnx(packet, &object->position, connection, route_direction);
        if (packet->path_info.direction != route_direction) {
            packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
            return;
        }
        packet->movement_event_flags |= AIPACKET_PATH_CONNECTION_CHANGED;
    }

    AIPATHNODE *waypoint = &path->nodes[waypoint_index];
    packet->goal_path_node = &path->nodes[goal_index];
    packet->movement_destination = waypoint->position;
    packet->movement_stopping_distance = 0.0f;
    packet->runtime_flags |= AIPACKET_RUNTIME_USING_PATH_WAYPOINT;

    AIPATHCNX *next_connection = NULL;
    if (goal_index == waypoint_index) {
        next_connection = destination_connection;
    } else if (path->route_matrix != NULL && path->route_matrix[waypoint_index] != NULL &&
               waypoint->connections != NULL) {
        const u8 next_connection_index = path->route_matrix[waypoint_index][goal_index];
        if (next_connection_index < waypoint->connection_count) {
            next_connection = waypoint->connections[next_connection_index];
        }
    }

    if (next_connection == NULL || next_connection == connection) {
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
        return;
    }

    i32 next_direction;
    if (next_connection->node_indices[0] == waypoint_index) {
        next_direction = 0;
    } else if (next_connection->node_indices[1] == waypoint_index) {
        next_direction = 1;
    } else {
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
        return;
    }

    if (packet->current_route != 0xff) {
        if (packet->current_route >= 16) {
            packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
            return;
        }
        const u16 route_bit = static_cast<u16>(1u << packet->current_route);
        if ((next_connection->route_mask & route_bit) == 0 && (waypoint->value_0x5a & route_bit) == 0) {
            packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
            return;
        }
    }

    const u32 traversal_flags = next_connection->traversal_flags[next_direction];
    if ((traversal_flags & AIPATH_CONNECTION_CAPABILITY_MASK) != 0) {
        if ((traversal_flags & AIPATH_CONNECTION_SPECIAL_MASK) != 0) {
            packet->movement_destination = object->position;
            packet->frame_state = traversal_flags;
            packet->runtime_flags &= static_cast<u8>(~AIPACKET_RUNTIME_PATH_BLOCKED);
            packet->runtime_flags |= AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
            return;
        }
        if ((packet->capabilities & traversal_flags) == 0) {
            packet->frame_state = traversal_flags;
            packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
            return;
        }
        packet->navigation_flags |= AIPACKET_NAVIGATION_FLAG_TRANSIENT;
    }

    // Proximity to the node is insufficient at corners and intersections. The
    // target changes connection only after the collision-sized terrain origin
    // lies inside the next corridor.
    if (WithinConnection(system, &packet->terrain_origin, path, next_connection, checks, connection,
                         packet->current_route, object->field_0x289, NULL, object->collision_radius, 0) == 0) {
        packet->runtime_flags &= static_cast<u8>(~AIPACKET_RUNTIME_PATH_BLOCKED);
        return;
    }

    AISysCharacterSetPathCnx(packet, &object->position, next_connection, next_direction);
    if (packet->path_info.connection != next_connection) {
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED | AIPACKET_RUNTIME_USING_PATH_WAYPOINT;
        return;
    }

    packet->movement_event_flags |= AIPACKET_PATH_CONNECTION_CHANGED;
    packet->runtime_flags &= static_cast<u8>(~AIPACKET_RUNTIME_PATH_BLOCKED);
    if (next_connection == destination_connection) {
        packet->movement_destination = packet->fallback_destination;
        packet->movement_stopping_distance = packet->fallback_stopping_distance;
        packet->goal_path_node = NULL;
        packet->runtime_flags &= static_cast<u8>(~AIPACKET_RUNTIME_USING_PATH_WAYPOINT);
        return;
    }

    const u8 next_waypoint_index = next_connection->node_indices[next_direction ^ 1];
    if (next_waypoint_index < path->node_count) {
        packet->movement_destination = path->nodes[next_waypoint_index].position;
    }
}

void AICreatureResumeScript(GameObject_s *) {
}

void AIRetreatFromDestination(AISYS_s *, AIPACKET_s *, APIOBJECT_s *, i32) {
}

void AIRetreatFromDestination2(AISYS_s *, AIPACKET_s *, APIOBJECT_s *, i32) {
}

void AIMoveDirectlyToDestination(AISYS_s *, AIPACKET_s *, APIOBJECT_s *, i32) {
}

void AIMoveToDestinationAvoidingCamera(AISYS_s *, AIPACKET_s *, APIOBJECT_s *, i32) {
}

void AISysNodeCanReachThisJumpConnection(GameObject_s &, AIPATH_s &, unsigned char, AIPATHCNX_s &, i32) {
}

void AISysCharacterCanReachThisJumpConnection(GameObject_s &, AIPATH_s &, AIPATHCNX_s &, i32) {
}

void AICircle(AISYS_s *, AIPACKET_s *, APIOBJECT_s *, i32) {
}

void AIWander(AISYS_s *system, AIPACKET_s *packet, APIOBJECT_s *object, i32 checks) {
    if (packet == NULL || object == NULL || packet->path_info.path == NULL || packet->path_info.path->nodes == NULL ||
        packet->path_info.connection == NULL) {
        if (packet != NULL && object != NULL) {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            packet->movement_target = NULL;
            packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        }
        return;
    }

    AIPATHCNX *target_connection = packet->movement_target;
    AIPATHCNX *current_connection = packet->path_info.connection;
    const u8 current_node_index = current_connection->node_indices[packet->path_info.direction == 0];
    if (current_node_index >= packet->path_info.path->node_count) {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        packet->movement_target = NULL;
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        return;
    }

    if (target_connection != NULL && target_connection->direction_a != current_node_index &&
        target_connection->direction_b != current_node_index) {
        packet->movement_target = NULL;
        target_connection = NULL;
    }

    if (target_connection == NULL) {
        AIPATH *path = packet->path_info.path;
        AIPATHNODE *current_node = &path->nodes[current_node_index];
        const i32 connection_count = current_node->connection_count;

        if (connection_count != 0 && current_node->connections != NULL) {
            const i32 first_connection = static_cast<i32>(NuRandInt() % connection_count);
            for (i32 offset = 0; offset < connection_count; ++offset) {
                AIPATHCNX *candidate = current_node->connections[(first_connection + offset) % connection_count];
                if (candidate == NULL ||
                    (candidate->direction_a != current_node_index && candidate->direction_b != current_node_index)) {
                    continue;
                }
                packet->movement_target = candidate;
                target_connection = candidate;

                if (candidate == current_connection) {
                    packet->movement_target = NULL;
                    target_connection = NULL;
                    continue;
                }

                if (packet->current_route != 0xff &&
                    ((static_cast<u64>(candidate->route_mask) >> packet->current_route) & 1) == 0 &&
                    ((static_cast<u64>(static_cast<u16>(current_node->value_0x5a)) >> packet->current_route) & 1) ==
                        0) {
                    packet->movement_target = NULL;
                    target_connection = NULL;
                    continue;
                }

                const u8 direction = current_node_index != candidate->direction_a;
                packet->movement_target_direction = direction;
                const u32 traversal_flags = candidate->traversal_flags[direction];
                if (traversal_flags == 0) {
                    break;
                }

                packet->movement_target = NULL;
                target_connection = NULL;
            }
        }

        if (target_connection == NULL) {
            current_connection = packet->path_info.connection;
            const u8 node_index = current_connection->node_indices[packet->path_info.direction == 0];
            AIPATHNODE *node = &packet->path_info.path->nodes[node_index];
            NUVEC delta;
            const f32 distance_squared = NuVecXZDistSqr(&object->position, &node->position, &delta);

            if (distance_squared < node->radius_squared) {
                AISysCharacterSetPathCnx(packet, &object->position, current_connection,
                                         packet->path_info.direction == 0);
                current_connection = packet->path_info.connection;
                const u8 updated_node_index = current_connection->node_indices[packet->path_info.direction == 0];
                node = &packet->path_info.path->nodes[updated_node_index];
                packet->goal_path_node = node;
            }

            packet->movement_destination = node->position;
            packet->movement_stopping_distance = 0.0f;
            return;
        }
    }

    packet->field_0x1e6 |= AIPACKET_RUNTIME_SPECIAL_MOVE;
    if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path, target_connection, checks,
                         current_connection, packet->current_route, object->field_0x289, NULL, object->collision_radius,
                         0) != 0) {
        AISysCharacterSetPathCnx(packet, &object->position, target_connection, packet->movement_target_direction);

        current_connection = packet->path_info.connection;
        const u8 node_index = current_connection->node_indices[packet->path_info.direction == 0];
        AIPATHNODE *node = &packet->path_info.path->nodes[node_index];
        packet->goal_path_node = node;
        packet->movement_destination = node->position;
        packet->movement_stopping_distance = 0.0f;
        packet->movement_target = NULL;
        return;
    }

    const u8 destination_node_index = target_connection->node_indices[packet->movement_target_direction == 0];
    if (destination_node_index >= packet->path_info.path->node_count) {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        packet->movement_target = NULL;
        packet->runtime_flags |= AIPACKET_RUNTIME_PATH_BLOCKED;
        return;
    }
    packet->movement_destination = packet->path_info.path->nodes[destination_node_index].position;
    packet->movement_stopping_distance = 0.0f;

    if (CalculateIntersection(system, packet, object, current_connection, target_connection) != 0) {
        NUVEC delta;

        if ((packet->movement_flags & AIPACKET_MOVEMENT_DIVERSION_LEFT) != 0) {
            NuVecSub(&delta, &packet->left_diversion, &object->position);
            const NUANG diversion_angle = NuAtan2D(delta.x, delta.z);
            NuVecSub(&delta, &packet->movement_destination, &object->position);
            const NUANG destination_angle = NuAtan2D(delta.x, delta.z);
            if (NuAngSub(destination_angle, diversion_angle) < 0) {
                packet->movement_destination = packet->left_diversion;
                return;
            }
        }

        if ((packet->movement_flags & AIPACKET_MOVEMENT_DIVERSION_RIGHT) != 0) {
            NuVecSub(&delta, &packet->right_diversion, &object->position);
            const NUANG diversion_angle = NuAtan2D(delta.x, delta.z);
            NuVecSub(&delta, &packet->movement_destination, &object->position);
            const NUANG destination_angle = NuAtan2D(delta.x, delta.z);
            if (NuAngSub(destination_angle, diversion_angle) > 0) {
                packet->movement_destination = packet->right_diversion;
            }
        }
    }
}
