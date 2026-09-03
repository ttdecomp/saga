#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <stddef.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern TERRSET *CurTerr;
extern TerrainQuery_s *TerI;
extern i32 curPickInst;

void TerrainSkinAllocate(terrsitu_s *terrain_group);

namespace {

    // Target NewScanRot 0x378fb0 uses 0.1f on both horizontal axes when
    // selecting the terrain shapes that can contain the cast point.
    const f32 SHADOW_SCAN_HALF_EXTENT = 0.1f;

    struct ShadowScanWriter {
        u8 *group_header;
        TERRAIN_SHAPE **cursor;
        u8 *limit;
        i32 shape_count;
    };

    static bool ShadowBoundsOverlap(f32 min_x, f32 min_z, f32 max_x, f32 max_z, const NUVEC &minimum,
                                    const NUVEC &maximum) {
        return max_x >= minimum.x && maximum.x >= min_x && max_z >= minimum.z && maximum.z > min_z;
    }

    static void ShadowFinishGroup(ShadowScanWriter *writer, i32 group_index) {
        if (writer->shape_count == 0) {
            return;
        }

        i16 *header = reinterpret_cast<i16 *>(writer->group_header);
        header[0] = static_cast<i16>(writer->shape_count);
        header[1] = static_cast<i16>(group_index);
        writer->group_header = reinterpret_cast<u8 *>(writer->cursor);
        writer->cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer->group_header + sizeof(TERRAIN_SHAPE *));
        writer->shape_count = 0;
    }

    static void ShadowScanGroup(i32 group_index, f32 world_min_x, f32 world_min_z, f32 world_max_x, f32 world_max_z,
                                i32 terrain_mask, bool bounds_are_local, ShadowScanWriter *writer) {
        TERRAIN_GROUP &group = CurTerr->groups[group_index];
        if (group.chunk_type == -1) {
            return;
        }

        f32 local_min_x = world_min_x - group.origin.x;
        f32 local_max_x = world_max_x - group.origin.x;
        f32 local_min_z = world_min_z - group.origin.z;
        f32 local_max_z = world_max_z - group.origin.z;
        if (bounds_are_local) {
            if (!ShadowBoundsOverlap(local_min_x, local_min_z, local_max_x, local_max_z, group.bounds_min,
                                     group.bounds_max)) {
                return;
            }
        } else if (!ShadowBoundsOverlap(world_min_x, world_min_z, world_max_x, world_max_z, group.bounds_min,
                                        group.bounds_max)) {
            return;
        }

        if (group.scene_index < 0) {
            TerrainSkinAllocate(reinterpret_cast<terrsitu_s *>(&group));
        }

        TERRAIN_SHAPE_BATCH *batch = static_cast<TERRAIN_SHAPE_BATCH *>(group.data);
        while (batch->marker >= 0) {
            TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
            if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                batch->max_z > local_min_z) {
                for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index) {
                    TERRAIN_SHAPE *shape = &shapes[shape_index];
                    if (local_max_x < shape->min_x || shape->max_x <= local_min_x || local_max_z < shape->min_z ||
                        shape->max_z <= local_min_z) {
                        continue;
                    }
                    if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0) {
                        continue;
                    }
                    if (reinterpret_cast<u8 *>(writer->cursor + 1) > writer->limit) {
                        continue;
                    }
                    *writer->cursor++ = shape;
                    ++writer->shape_count;
                }
            }
            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + batch->shape_count);
        }
        ShadowFinishGroup(writer, group_index);
    }

} // namespace

void NewScanRot(nuvec_s *position, i32 terrain_mask) {
    ShadowScanWriter writer;
    writer.group_header = TerI->scan_list_storage;
    writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer.group_header + sizeof(TERRAIN_SHAPE *));
    // Target stops at TerI + 0x93c, leaving the final header slot available.
    writer.limit = reinterpret_cast<u8 *>(TerI) + 0x93c;
    writer.shape_count = 0;

    const f32 min_x = position->x - SHADOW_SCAN_HALF_EXTENT;
    const f32 max_x = position->x + SHADOW_SCAN_HALF_EXTENT;
    const f32 min_z = position->z - SHADOW_SCAN_HALF_EXTENT;
    const f32 max_z = position->z + SHADOW_SCAN_HALF_EXTENT;

    for (i32 cell_index = 0; cell_index < CurTerr->used_cell_count; ++cell_index) {
        const TERRAIN_CELL &cell = CurTerr->cells[cell_index];
        if (max_x < cell.min_x || cell.max_x < min_x || max_z < cell.min_z || cell.max_z < min_z) {
            continue;
        }

        const i16 *group_indices = CurTerr->group_indices + cell.first_group;
        for (i32 cell_group = 0; cell_group < cell.group_count; ++cell_group) {
            const i32 group_index = group_indices[cell_group];
            ShadowScanGroup(group_index, min_x, min_z, max_x, max_z, terrain_mask, false, &writer);
        }
    }

    for (i32 pick_index = 0; pick_index < curPickInst; ++pick_index) {
        const i32 group_index = CurTerr->max_groups + pick_index;
        ShadowScanGroup(group_index, min_x, min_z, max_x, max_z, terrain_mask, true, &writer);
    }

    i16 *terminator = reinterpret_cast<i16 *>(writer.group_header);
    terminator[0] = 0;
    terminator[1] = 0;
    TerI->scan_list = TerI->scan_list_storage;
}

void AndroidMain(void *) {
}

void ObjZappedBlue(GameObject_s *) {
}

void PeriscodeCode(GameObject_s *) {
}

void NewScanHandelFull(nuvec_s *, nuvec_s *, float, i32, i32) {
}

void NewScanHandelSubset(i16 *, nuvec_s *, nuvec_s *, float, i32) {
}

void RegisterGizmoTypes_Batman(variptr_u *, variptr_u *) {
}

void NewScan(nuvec_s *, i32, i32) {
}

static __used__ void remapParent(i32) {
}

static __used__ void remapChildren(i32) {
}
