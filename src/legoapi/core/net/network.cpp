#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/legoapi_types.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/numath/nufloat.h"

void TTNetwork::Broadcast(NetMessage, unsigned char) {
}

void TTNetwork::ClearMyHostAddress() {
}

void TTNetwork::Display(ThingRenderData *) {
}

void TTNetwork::GetMyAddress() const {
}

void TTNetwork::GetMyHostAddress() const {
}

void TTNetwork::Initialise() {
}

void TTNetwork::ProcessEvenWhenPaused(ThingProcessData *) {
}

void TTNetwork::ReliableBroadcast(NetMessage, unsigned char) {
}

void TTNetwork::ReliableSend(NetMessage, unsigned char, NetPeer &, char const *, u32) {
}

void TTNetwork::Resume() {
}

void TTNetwork::Send(NetMessage, unsigned char, NetPeer &) {
}

void TTNetwork::SetMyHostAddress(NetAddress const &) {
}

void TTNetwork::Shutdown() {
}

void TTNetwork::Suspend() {
}

TTNetwork::TTNetwork() {
}

void TTNetwork::Update() {
}

TTNetwork::~TTNetwork() {
}

static __used__ void NOSGetGuid() {
}

extern "C" {

    i32 WithinConnection(AISYS *system, NUVEC *position, AIPATH *path, AIPATHCNX *connection, i32 checks,
                         AIPATHCNX *previous_connection, i32 route, i32 ground, AIPATHINFO *path_info, f32 radius,
                         i32 update_once) {
        (void)system;
        (void)checks;
        (void)ground;

        if (position == NULL || path == NULL || path->nodes == NULL || connection == NULL) {
            return 0;
        }

        if (update_once != 0) {
            if (connection->last_search_checksum == path->search_checksum) {
                return 0;
            }
            connection->last_search_checksum = path->search_checksum;
        }

        u8 first_node_index = connection->direction_a;
        u8 second_node_index = connection->direction_b;
        if (first_node_index >= path->node_count || second_node_index >= path->node_count) {
            return 0;
        }

        // A connection excluded from the active route can still be entered at
        // a shared node whose per-route bit explicitly permits the transition.
        if (route != 0xff && previous_connection != NULL &&
            ((static_cast<u64>(connection->route_mask) >> route) & 1) == 0) {
            const u8 previous_a = previous_connection->direction_a;
            const u8 previous_b = previous_connection->direction_b;
            if (first_node_index == previous_a || first_node_index == previous_b) {
                // first_node_index already names the shared endpoint.
            } else if (second_node_index == previous_a || second_node_index == previous_b) {
                const u8 shared_node_index = second_node_index;
                second_node_index = first_node_index;
                first_node_index = shared_node_index;
            } else {
                return 0;
            }

            if (((static_cast<u64>(static_cast<u16>(path->nodes[first_node_index].value_0x5a)) >> route) & 1) == 0) {
                return 0;
            }
        }

        AIPATHNODE *first_node = &path->nodes[first_node_index];
        AIPATHNODE *second_node = &path->nodes[second_node_index];
        f32 first_radius = first_node->radius;
        f32 second_radius = second_node->radius;
        const bool first_is_narrow = radius >= first_radius;
        const bool second_is_narrow = radius >= second_radius;
        if (!first_is_narrow) {
            first_radius -= radius;
        }
        if (!second_is_narrow) {
            second_radius -= radius;
        }

        const f32 segment_x = second_node->position.x - first_node->position.x;
        const f32 segment_z = second_node->position.z - first_node->position.z;
        const f32 segment_length_squared = segment_x * segment_x + segment_z * segment_z;
        const f32 position_x = position->x - first_node->position.x;
        const f32 position_z = position->z - first_node->position.z;
        const f32 path_fraction = segment_length_squared == 0.0f
                                      ? 0.0f
                                      : (position_x * segment_x + position_z * segment_z) / segment_length_squared;
        f32 along = path_fraction;
        f32 nearest_x;
        f32 nearest_z;
        f32 allowed_radius;
        f32 min_height;
        f32 max_height;

        if (along <= 0.0f) {
            along = 0.0f;
            nearest_x = first_node->position.x;
            nearest_z = first_node->position.z;
            allowed_radius = first_radius;
            min_height = first_node->min_height;
            max_height = first_node->max_height;
        } else if (along >= 1.0f) {
            along = 1.0f;
            nearest_x = second_node->position.x;
            nearest_z = second_node->position.z;
            allowed_radius = second_radius;
            min_height = second_node->min_height;
            max_height = second_node->max_height;
        } else {
            nearest_x = first_node->position.x + (second_node->position.x - first_node->position.x) * along;
            nearest_z = first_node->position.z + (second_node->position.z - first_node->position.z) * along;
            allowed_radius = first_radius + (second_radius - first_radius) * along;
            min_height = first_node->min_height + (second_node->min_height - first_node->min_height) * along;
            max_height = first_node->max_height + (second_node->max_height - first_node->max_height) * along;
        }

        const f32 distance_x = position->x - nearest_x;
        const f32 distance_z = position->z - nearest_z;
        if (distance_x * distance_x + distance_z * distance_z > allowed_radius * allowed_radius ||
            position->y < min_height || position->y > max_height) {
            return 0;
        }

        if (path_info != NULL) {
            path_info->connection = connection;
            path_info->direction = 0;
            path_info->flags =
                static_cast<u8>((path_info->flags & ~AIPATHINFO_FLAG_NARROW_PATH) | AIPATHINFO_FLAG_ON_PATH |
                                ((first_is_narrow || second_is_narrow) ? AIPATHINFO_FLAG_NARROW_PATH : 0));
            path_info->dist = path_fraction;
            path_info->width = segment_length_squared == 0.0f ? 0.0f
                                                              : (position_x * segment_z - position_z * segment_x) /
                                                                    NuFsqrt(segment_length_squared);
        }
        return 1;
    }

} // extern "C"
