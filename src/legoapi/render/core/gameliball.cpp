#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

u8 *crashdata;

namespace {

    struct TERRAIN_FILE_CHUNK {
        i16 chunk_count; // only meaningful in the first descriptor
        u16 version;
        u32 data_word_count;
        NUVEC origin;
        i16 chunk_type;
        u16 field_0x16;
        u8 pad_0x18[0x14];
        u32 field_0x2c;
        u16 field_0x30;
        u16 field_0x32;
    };
    DECOMP_ASSERT(sizeof(TERRAIN_FILE_CHUNK) == 0x34, "TERRAIN_FILE_CHUNK ABI");

    inline void BuildTerrainSpatialBounds(TERRAIN_SPATIAL_NODE *node) {
        // Every block covers sixteen terrain cells and therefore seventeen
        // boundary points.  The original stores the aggregate X/Z bounds in
        // the otherwise-unused Y components of the first four points.
        static const f32 kUnusedBlock = 2147483648.0f;

        while (node != NULL) {
            const i32 point_count = node->point_count;
            if (point_count != 0) {
                TERRAIN_SPATIAL_NODE *block = node;
                i32 first_point = 0;

                do {
                    if (block->points[0].y != kUnusedBlock) {
                        if (first_point + 15 < point_count) {
                            f32 min_x = block->points[0].x;
                            f32 max_x = block->points[0].x;
                            f32 min_z = block->points[0].z;
                            f32 max_z = block->points[0].z;
                            for (i32 point = 1; point < 17; ++point) {
                                min_x = MIN(block->points[point].x, min_x);
                                max_x = MAX(block->points[point].x, max_x);
                                min_z = MIN(block->points[point].z, min_z);
                                max_z = MAX(block->points[point].z, max_z);
                            }
                            block->points[0].y = min_x;
                            block->points[1].y = max_x;
                            block->points[2].y = min_z;
                            block->points[3].y = max_z;
                        } else {
                            block->points[0].y = kUnusedBlock;
                        }
                    }

                    first_point += 16;
                    block =
                        reinterpret_cast<TERRAIN_SPATIAL_NODE *>(reinterpret_cast<u8 *>(block) + 16 * sizeof(NUVEC));
                } while (first_point < point_count);
            }

            // The file starts each spatial chunk with a link word followed by
            // the node itself. ReadTerrain replaces that word with the prior
            // list head while linking the chunks.
            node = *reinterpret_cast<TERRAIN_SPATIAL_NODE **>(reinterpret_cast<u8 *>(node) - sizeof(void *));
        }
    }

} // namespace

i32 ReadTerrain(unsigned char *base_path, i32 first_group, i16 **buffer, TERRSET *terrain) {
    crashdata = NULL;

    char path[100];
    strcpy(path, reinterpret_cast<char *>(base_path));
    strcat(path, ".ter");
    if (NuFileLoadBuffer(path, *buffer, 0x7fffffff) == 0) {
        return -1;
    }

    u8 *file_start = reinterpret_cast<u8 *>(*buffer);
    TERRAIN_FILE_CHUNK *chunk =
        reinterpret_cast<TERRAIN_FILE_CHUNK *>(file_start + 2 * *reinterpret_cast<u32 *>(file_start));
    const i16 chunk_count = chunk->chunk_count;
    *buffer = reinterpret_cast<i16 *>(file_start + sizeof(u32));

    if (chunk_count <= 0) {
        terrain->file_version = chunk->version;
        terrain->spatial_nodes = NULL;
        return 0;
    }

    terrain->file_version = chunk->version;
    terrain->spatial_nodes = NULL;

    i32 chunk_index = 0;
    i32 group_count = 0;
    do {
        u8 *chunk_data = reinterpret_cast<u8 *>(*buffer);

        if (chunk->chunk_type == TERRAIN_CHUNK_SPATIAL_INDEX) {
            TERRAIN_SPATIAL_NODE **link = reinterpret_cast<TERRAIN_SPATIAL_NODE **>(chunk_data);
            if (*reinterpret_cast<u32 *>(link) == 0x12345678) {
                *link = terrain->spatial_nodes;
                terrain->spatial_nodes = reinterpret_cast<TERRAIN_SPATIAL_NODE *>(link + 1);
                --terrain->spatial_nodes->point_count;
            }
        } else if (chunk->chunk_type <= TERRAIN_CHUNK_SPATIAL_INDEX) {
            if (chunk->chunk_type >= TERRAIN_CHUNK_GROUP_PRIMARY) {
                const f32 origin_x = chunk->origin.x;
                terrain->groups[first_group].field_0x32 = chunk->field_0x32;
                terrain->groups[first_group].chunk_type = chunk->chunk_type;
                terrain->groups[first_group].origin.x = origin_x;
                terrain->groups[first_group].origin.y = chunk->origin.y;
                terrain->groups[first_group].field_0x30 = chunk->field_0x16;
                terrain->groups[first_group].origin.z = chunk->origin.z;
                terrain->groups[first_group].data = chunk_data;
                terrain->groups[first_group].field_0x28 = chunk->field_0x2c;
                ++group_count;
                ++first_group;
            }
        } else if (chunk->chunk_type == TERRAIN_CHUNK_CRASH_DATA) {
            crashdata = chunk_data;
        }

        *buffer = reinterpret_cast<i16 *>(chunk_data + 2 * chunk->data_word_count);
        ++chunk_index;
        ++chunk;
    } while (chunk_index != chunk_count);

    BuildTerrainSpatialBounds(terrain->spatial_nodes);
    return group_count;
}

void ReadInstanceIDs(i32, nugscn_s *) {
}

void ReadTerrainPickup(unsigned char *, i16 **, TERRPICKUPSET *) {
}
