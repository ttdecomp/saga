#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nuvec.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern TERRSET *CurTerr;
extern TerrainQuery_s *TerI;
extern NUVEC ShadNorm;
extern TERRAIN_SHAPE *ShadPoly;
extern i16 castnum;

i32 CheckPosAIArea(AIAREA_s *area, nuvec_s *position, float tolerance) {
    if (position == NULL || area == NULL) {
        return 0;
    }

    NUVEC local_position;
    NuVecSub(&local_position, position, &area->position);
    NuVecRotateY(&local_position, &local_position, -area->rotation);

    return local_position.x + tolerance >= -area->half_width && local_position.y + tolerance >= -0.1f &&
           local_position.z + tolerance >= -area->half_depth && local_position.x - tolerance <= area->half_width &&
           local_position.y - tolerance <= area->height && local_position.z - tolerance <= area->half_depth;
}

void GetTakeOverPos(GameObject_s *, nuvec_s *) {
}

void HandleGroupLimit(i32) {
}

void ResetAdaptiveDifficulty() {
}

namespace {

    const f32 NO_TERRAIN_HEIGHT = 2000000.0f;
    const f32 NO_SECOND_TERRAIN_NORMAL = 65536.0f;

    struct ShadowSurfaceCandidate {
        f32 height;
        TERRAIN_SHAPE *surface;
        NUVEC normal;
        i16 terrain_group;
    };

    static f32 TriangleSideXZ(f32 point_x, f32 point_z, const NUVEC &start, const NUVEC &end) {
        return (end.x - start.x) * (point_z - start.z) - (end.z - start.z) * (point_x - start.x);
    }

    static bool PointInsideTriangleXZ(f32 point_x, f32 point_z, const NUVEC &a, const NUVEC &b, const NUVEC &c) {
        const f32 side_ab = TriangleSideXZ(point_x, point_z, a, b);
        const f32 side_bc = TriangleSideXZ(point_x, point_z, b, c);
        const f32 side_ca = TriangleSideXZ(point_x, point_z, c, a);
        return (side_ab >= 0.0f && side_bc >= 0.0f && side_ca >= 0.0f) ||
               (side_ab <= 0.0f && side_bc <= 0.0f && side_ca <= 0.0f);
    }

    static void ConsiderShadowTriangle(const NUVEC &local_position, const TERRAIN_GROUP &group, TERRAIN_SHAPE *surface,
                                       const NUVEC &origin, const NUVEC &edge_a, const NUVEC &edge_b,
                                       const NUVEC &normal, i16 terrain_group, ShadowSurfaceCandidate *floor_candidate,
                                       ShadowSurfaceCandidate *above_candidate) {
        if (normal.y == 0.0f || !PointInsideTriangleXZ(local_position.x, local_position.z, origin, edge_a, edge_b)) {
            return;
        }

        const f32 plane_height =
            origin.y + group.origin.y -
            ((local_position.x - origin.x) * normal.x + (local_position.z - origin.z) * normal.z) / normal.y;
        const f32 query_height = local_position.y + group.origin.y;
        if (plane_height <= query_height) {
            if (normal.y > 0.0f && plane_height > floor_candidate->height) {
                floor_candidate->height = plane_height;
                floor_candidate->surface = surface;
                floor_candidate->normal = normal;
                floor_candidate->terrain_group = terrain_group;
            }
        } else if (plane_height < above_candidate->height ||
                   (plane_height == above_candidate->height && normal.y < above_candidate->normal.y)) {
            // The target keeps the nearest ordinary surface above the cast
            // origin as well as the conventional floor below it.  NewCast's
            // final range check decides whether this surface is reachable.
            above_candidate->height = plane_height;
            above_candidate->surface = surface;
            above_candidate->normal = normal;
            above_candidate->terrain_group = terrain_group;
        }
    }

} // namespace

f32 NewCast(nuvec_s *position, f32 height_above, f32) {
    ShadowSurfaceCandidate floor_candidate = {
        -NO_TERRAIN_HEIGHT,
        NULL,
        {0.0f, 1.0f, 0.0f},
        -1,
    };
    ShadowSurfaceCandidate above_candidate = {
        NO_TERRAIN_HEIGHT,
        NULL,
        {0.0f, 1.0f, 0.0f},
        -1,
    };

    u8 *scan = TerI->scan_list_storage;
    i16 shape_count = *reinterpret_cast<i16 *>(scan);
    while (shape_count > 0) {
        const i16 group_index = *reinterpret_cast<i16 *>(scan + sizeof(i16));
        TERRAIN_GROUP &group = CurTerr->groups[group_index];
        TERRAIN_SHAPE **surfaces = reinterpret_cast<TERRAIN_SHAPE **>(scan) + 1;
        const NUVEC local_position = {
            position->x - group.origin.x,
            position->y - group.origin.y,
            position->z - group.origin.z,
        };

        for (i32 shape_index = 0; shape_index < shape_count; ++shape_index) {
            TERRAIN_SHAPE *surface = surfaces[shape_index];
            // material[1] marks the target's extended-shadow category.  Those
            // candidates feed EShad*, not NewCast's ordinary Shad* result.
            if (surface->material[1] != 0) {
                continue;
            }
            ConsiderShadowTriangle(local_position, group, surface, surface->vectors[0], surface->vectors[1],
                                   surface->vectors[2], surface->normals[0], group_index, &floor_candidate,
                                   &above_candidate);
            if (surface->normals[1].y < NO_SECOND_TERRAIN_NORMAL) {
                ConsiderShadowTriangle(local_position, group, surface, surface->vectors[3], surface->vectors[2],
                                       surface->vectors[1], surface->normals[1], group_index, &floor_candidate,
                                       &above_candidate);
            }
        }

        scan = reinterpret_cast<u8 *>(surfaces + shape_count);
        shape_count = *reinterpret_cast<i16 *>(scan);
    }

    const ShadowSurfaceCandidate *selected_candidate = NULL;
    if (above_candidate.surface != NULL && above_candidate.normal.y > 0.0f &&
        above_candidate.height < position->y + height_above) {
        selected_candidate = &above_candidate;
    } else if (floor_candidate.surface != NULL) {
        selected_candidate = &floor_candidate;
    }

    if (selected_candidate == NULL) {
        position->y = NO_TERRAIN_HEIGHT;
        ShadPoly = NULL;
        ShadNorm.x = 0.0f;
        ShadNorm.y = 1.0f;
        ShadNorm.z = 0.0f;
        castnum = -1;
        return 0.0f;
    }

    position->y = selected_candidate->height;
    ShadNorm = selected_candidate->normal;
    ShadPoly = selected_candidate->surface;
    castnum = selected_candidate->terrain_group;
    return 0.0f;
}

void LoopCode(GameObject_s *, i32, i32, GAMEPAD_s *, i32) {
}
