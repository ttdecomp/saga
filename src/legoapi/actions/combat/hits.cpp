#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nufloat.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern TerrainQuery_s *TerI;
extern TERRSET *CurTerr;
extern tertype **TerrOverRideScan;
extern i32 PlatCrush;
extern i32 curSphereter;
extern i32 plathitid;
extern TERRAIN_SPHERE SphereData[16];

void HitWallSpline();
void DeRotatePoint(NUVEC *point);
void DeRotateTerrain(tertype *surface);
void RotateVec(NUVEC *source, NUVEC *destination);
i16 InsidePolLines(f32 point_x, f32 point_y, f32 point_z, f32 edge_a_x, f32 edge_a_y, f32 edge_a_z, f32 edge_b_x,
                   f32 edge_b_y, f32 edge_b_z, NUVEC *normal);
i32 CheckCylinder(i32 first_vertex, i32 second_vertex, i32 *vertex_mask, i32 remaining_vertex_mask);
i32 CheckSphere(i32 vertex_index);
i32 CheckSphereTer(NUVEC *position, f32 radius);
i32 HitPoly(f32 primary_start, f32 primary_end, f32 secondary_start, f32 secondary_end, tertype *surface);

void CannotKill(GameObject_s *) {
}

i32 HitTerrain() {
    u8 *scan = reinterpret_cast<u8 *>(TerrOverRideScan);
    if (scan == NULL) {
        scan = TerI->scan_list_storage;
    }

    TerI->hit_time = 999.9f;
    const f32 radius = TerI->collision_radius;
    TerI->shape_adjusted = 0;
    TerI->hit_type = TERRAIN_HIT_TYPE_NONE;
    TerI->terrain_group_index = -1;

    HitWallSpline();

    TerI->horizontal_movement_length =
        NuFsqrt(TerI->movement.x * TerI->movement.x + TerI->movement.z * TerI->movement.z);

    i32 collision_found = 0;
    f32 primary_start_distance = 0.0f;
    f32 secondary_start_distance = 0.0f;
    f32 secondary_end_distance = 0.0f;
    const f32 negative_radius = -radius;

    i16 shape_count = *reinterpret_cast<i16 *>(scan);
    for (;;) {
        while (shape_count > 0) {
            // A scan group packs its count and terrain-group index into the first
            // pointer-sized slot, followed by the selected shape pointers.
            TERRAIN_GROUP *group = &CurTerr->groups[*reinterpret_cast<i16 *>(scan + sizeof(i16))];
            tertype **scan_entries = reinterpret_cast<tertype **>(scan);
            tertype **surfaces = scan_entries + 1;

            TerrainQuery_s *query = TerI;
            query->local_start.x = query->position.x - group->origin.x;
            query->local_start.y = query->position.y - group->origin.y;
            query->local_start.z = query->position.z - group->origin.z;
            query->local_end.y = query->position.y + query->movement.y - group->origin.y;
            query->local_end.x = query->position.x + query->movement.x - group->origin.x;
            query->local_end.z = query->position.z + query->movement.z - group->origin.z;

            const i32 last_shape_index = shape_count - 1;
            i32 shape_index = 0;
            for (;;) {
                tertype *surface = scan_entries[shape_index + 1];
                const NUVEC &primary_point = surface->vectors[0];
                const NUVEC &primary_normal = surface->normals[0];

                f32 primary_end_distance = (query->local_end.x - primary_point.x) * primary_normal.x +
                                           (query->local_end.y - primary_point.y) * primary_normal.y +
                                           (query->local_end.z - primary_point.z) * primary_normal.z - radius -
                                           query->compare_epsilon;

                i32 crosses_surface = 0;
                if (0.0f > primary_end_distance) {
                    primary_start_distance = (query->local_start.x - primary_point.x) * primary_normal.x +
                                             (query->local_start.y - primary_point.y) * primary_normal.y +
                                             (query->local_start.z - primary_point.z) * primary_normal.z - radius;
                    crosses_surface = primary_start_distance > negative_radius;
                }

                const NUVEC &secondary_point = surface->vectors[3];
                const NUVEC &secondary_normal = surface->normals[1];
                if (65536.0f > secondary_normal.y) {
                    secondary_end_distance = (query->local_end.x - secondary_point.x) * secondary_normal.x +
                                             (query->local_end.y - secondary_point.y) * secondary_normal.y +
                                             (query->local_end.z - secondary_point.z) * secondary_normal.z - radius -
                                             query->compare_epsilon;
                    if (0.0f > secondary_end_distance) {
                        secondary_start_distance = (query->local_start.x - secondary_point.x) * secondary_normal.x +
                                                   (query->local_start.y - secondary_point.y) * secondary_normal.y +
                                                   (query->local_start.z - secondary_point.z) * secondary_normal.z -
                                                   radius;
                        if (secondary_start_distance > negative_radius) {
                            crosses_surface = 1;
                        }
                    }
                }

                if (crosses_surface != 0 && HitPoly(primary_start_distance, primary_end_distance,
                                                    secondary_start_distance, secondary_end_distance, surface) != 0) {
                    query = TerI;
                    query->terrain_group_index = *reinterpret_cast<u16 *>(scan + sizeof(i16));
                    collision_found = 1;
                    tertype *hit_surface = query->surface;
                    if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) != 0 &&
                        (hit_surface->flags & TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS) != 0) {
                        PlatCrush = hit_surface->material[0] + 1;
                    }
                }
                query = TerI;
                if (shape_index == last_shape_index) {
                    break;
                }
                ++shape_index;
            }

            const i16 next_shape_count = *reinterpret_cast<i16 *>(surfaces + shape_index + 1);
            scan = reinterpret_cast<u8 *>(surfaces + shape_count);
            shape_count = next_shape_count;
        }

        if (shape_count == 0) {
            break;
        }
        do {
            scan += (1 - shape_count) * sizeof(tertype *);
            shape_count = *reinterpret_cast<i16 *>(scan);
        } while (shape_count < 0);
    }

    NUVEC sphere_position;
    for (i32 sphere_index = 0; sphere_index < curSphereter; ++sphere_index) {
        const TERRAIN_SPHERE &sphere = SphereData[sphere_index];
        const f32 scaled_collision_radius = TerI->collision_radius * TerI->object_scale;
        sphere_position = sphere.position;
        sphere_position.y *= TerI->inverse_object_scale;
        sphere_position.y -= scaled_collision_radius;
        DeRotatePoint(&sphere_position);
        collision_found |= CheckSphereTer(&sphere_position, sphere.radius);
    }

    TerrainQuery_s *query = TerI;
    if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index != -1) {
        TERRAIN_GROUP *group = &CurTerr->groups[query->terrain_group_index];
        if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
            plathitid = group->scene_index;
        }
    }

    TerrOverRideScan = NULL;
    return collision_found;
}

void KillRumble(GameObject_s *) {
}

i32 CheckSphere(i32 vertex_index) {
    f32 radius = TerI->collision_radius;
    if (TerI->transformed_vertices[vertex_index].z < -radius ||
        TerI->transformed_vertices[vertex_index].z > TerI->movement_length + radius) {
        return 0;
    }

    f32 radial_distance_sq = TerI->transformed_vertices[vertex_index].x * TerI->transformed_vertices[vertex_index].x +
                             TerI->transformed_vertices[vertex_index].y * TerI->transformed_vertices[vertex_index].y;
    if (radial_distance_sq > TerI->collision_radius_sq) {
        return 0;
    }

    f32 radial_extent = NuFsqrt(TerI->collision_radius_sq - radial_distance_sq);
    f32 hit_distance = TerI->transformed_vertices[vertex_index].z - radial_extent;
    if (0.0f <= hit_distance && TerI->movement_length >= hit_distance) {
        f32 hit_time = hit_distance / TerI->movement_length;
        if (TerI->hit_time <= hit_time) {
            return 0;
        }

        TerI->hit_time = hit_time;
        TerI->hit_type = TERRAIN_HIT_TYPE_VERTEX;
        TerI->movement_normal.x = -TerI->transformed_vertices[vertex_index].x;
        TerI->movement_normal.y = -TerI->transformed_vertices[vertex_index].y;
        TerI->movement_normal.z = -radial_extent;
        return 1;
    }

    f32 distance_sq =
        radial_distance_sq + TerI->transformed_vertices[vertex_index].z * TerI->transformed_vertices[vertex_index].z;
    if (TerI->collision_radius_sq <= distance_sq) {
        return 0;
    }

    f32 distance = NuFsqrt(distance_sq);
    f32 overlap_time = distance - TerI->collision_radius;
    if (TerI->hit_time <= overlap_time) {
        return 0;
    }

    tertype *surface = TerI->working_surface;
    f32 primary_distance = (TerI->local_start.x - surface->vectors[vertex_index].x) * surface->normals[0].x +
                           (TerI->local_start.y - surface->vectors[vertex_index].y) * surface->normals[0].y +
                           (TerI->local_start.z - surface->vectors[vertex_index].z) * surface->normals[0].z;
    if (0.0f >= primary_distance) {
        return 0;
    }

    TerI->hit_time = overlap_time;
    f32 inverse_distance = 0.0f;
    if (distance != 0.0f) {
        inverse_distance = 1.0f / distance;
    }
    TerI->hit_type = TERRAIN_HIT_TYPE_VERTEX | TERRAIN_HIT_TYPE_SECOND_NORMAL;
    TerI->movement_normal.x = -TerI->transformed_vertices[vertex_index].x * inverse_distance;
    TerI->movement_normal.y = -TerI->transformed_vertices[vertex_index].y * inverse_distance;
    TerI->movement_normal.z = -TerI->transformed_vertices[vertex_index].z * inverse_distance;
    return 1;
}

void FloatRumble(GameObject_s *) {
}

void HitTerrPoly(tertype *, i32) {
}

i16 InsideLineF(f32 point_u, f32 point_v, f32 line_start_u, f32 line_start_v, f32 line_end_u, f32 line_end_v) {
    const f32 side =
        (point_u - line_start_u) * (line_end_v - line_start_v) + (point_v - line_start_v) * (line_start_u - line_end_u);
    if (side < 0.0f) {
        return 0;
    }
    return 1;
}

i16 InsideLineXZ(f32 point_u, f32 point_v, f32 line_start_u, f32 line_start_v, f32 line_end_u, f32 line_end_v) {
    const f32 side =
        (point_u - line_start_u) * (line_end_v - line_start_v) + (point_v - line_start_v) * (line_start_u - line_end_u);
    return side >= 0.0f;
}

void ObjHitShield(GameObject_s *, GameObject_s *, i32, BOLT_s *) {
}

i32 CheckCylinder(i32 first_vertex, i32 second_vertex, i32 *vertex_mask, i32 remaining_vertex_mask) {
    TerrainQuery_s *query = TerI;

    // Reject an edge when both endpoints lie outside the swept sphere's
    // expanded bounds. Once an edge is rejected this way, its endpoint bits
    // can be removed from the later vertex tests as well.
    if ((query->transformed_vertices[first_vertex].x > query->collision_radius &&
         query->transformed_vertices[second_vertex].x > query->collision_radius) ||
        (query->transformed_vertices[first_vertex].x < -query->collision_radius &&
         query->transformed_vertices[second_vertex].x < -query->collision_radius) ||
        (query->transformed_vertices[first_vertex].y > query->collision_radius &&
         query->transformed_vertices[second_vertex].y > query->collision_radius) ||
        (query->transformed_vertices[first_vertex].y < -query->collision_radius &&
         query->transformed_vertices[second_vertex].y < -query->collision_radius) ||
        (query->transformed_vertices[first_vertex].z < -query->collision_radius &&
         query->transformed_vertices[second_vertex].z < -query->collision_radius) ||
        (query->transformed_vertices[first_vertex].z > query->movement_length + query->collision_radius &&
         query->transformed_vertices[second_vertex].z > query->movement_length + query->collision_radius)) {
        *vertex_mask &= remaining_vertex_mask;
        return 0;
    }

    NUVEC edge_direction = {
        query->transformed_vertices[second_vertex].x - query->transformed_vertices[first_vertex].x,
        query->transformed_vertices[second_vertex].y - query->transformed_vertices[first_vertex].y,
        query->transformed_vertices[second_vertex].z - query->transformed_vertices[first_vertex].z,
    };
    const f32 horizontal_length_sq = edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y;

    // An almost vertical edge has no stable radial cylinder in the XY plane.
    // It can only overlap the sphere at the start of the sweep when it crosses
    // z = 0.
    if (horizontal_length_sq < 1.0e-12f) {
        if ((query->transformed_vertices[first_vertex].z < 0.0f &&
             query->transformed_vertices[second_vertex].z < 0.0f) ||
            (query->transformed_vertices[first_vertex].z > 0.0f &&
             query->transformed_vertices[second_vertex].z > 0.0f)) {
            return 0;
        }

        const f32 distance_sq =
            query->transformed_vertices[second_vertex].x * query->transformed_vertices[second_vertex].x +
            query->transformed_vertices[second_vertex].y * query->transformed_vertices[second_vertex].y;
        if (distance_sq > query->collision_radius_sq) {
            return 0;
        }

        const f32 distance = NuFsqrt(distance_sq);
        const f32 overlap_time = distance - query->collision_radius;
        if (query->hit_time <= overlap_time) {
            return 0;
        }

        NUVEC local_normal = {
            -query->transformed_vertices[second_vertex].x,
            -query->transformed_vertices[second_vertex].y,
            0.0f,
        };
        RotateVec(&local_normal, &local_normal);
        const NUVEC &surface_normal = query->working_surface->normals[0];
        if (local_normal.x * surface_normal.x + local_normal.y * surface_normal.y + local_normal.z * surface_normal.z <=
            0.0f) {
            return 0;
        }

        query = TerI;
        query->hit_time = overlap_time;
        f32 inverse_distance = 0.0f;
        if (distance != 0.0f) {
            inverse_distance = 1.0f / distance;
        }
        query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
        query->movement_normal.x = -query->transformed_vertices[second_vertex].x * inverse_distance;
        query->movement_normal.y = -query->transformed_vertices[second_vertex].y * inverse_distance;
        query->movement_normal.z = 0.0f;
        return 1;
    }

    // Squared radial distance from the movement axis to the infinite line
    // containing the edge. This avoids a square root for the common miss.
    const f32 line_cross = query->transformed_vertices[first_vertex].x * edge_direction.y -
                           query->transformed_vertices[first_vertex].y * edge_direction.x;
    if (line_cross * line_cross > query->collision_radius_sq * horizontal_length_sq) {
        *vertex_mask &= remaining_vertex_mask;
        return 0;
    }

    f32 line_distance_sq = 0.0f;
    if (horizontal_length_sq != 0.0f && line_cross != 0.0f) {
        line_distance_sq = line_cross * (line_cross / horizontal_length_sq);
    }

    const f32 edge_length = NuFsqrt(horizontal_length_sq + edge_direction.z * edge_direction.z);
    f32 inverse_edge_length = 0.0f;
    if (edge_length != 0.0f) {
        inverse_edge_length = 1.0f / edge_length;
    }
    edge_direction.x *= inverse_edge_length;
    edge_direction.y *= inverse_edge_length;
    edge_direction.z *= inverse_edge_length;

    NUVEC line_cross_vector = {
        -query->transformed_vertices[first_vertex].x,
        -query->transformed_vertices[first_vertex].y,
        -query->transformed_vertices[first_vertex].z,
    };
    NuVecCross(&line_cross_vector, &line_cross_vector, &edge_direction);

    NUVEC horizontal_perpendicular = {-edge_direction.y, edge_direction.x, 0.0f};
    const f32 horizontal_direction_sq = horizontal_perpendicular.x * horizontal_perpendicular.x +
                                        horizontal_perpendicular.y * horizontal_perpendicular.y;
    const f32 height_numerator =
        line_cross_vector.x * horizontal_perpendicular.x + line_cross_vector.y * horizontal_perpendicular.y;
    f32 negative_closest_height = 0.0f;
    if (horizontal_direction_sq != 0.0f && height_numerator != 0.0f) {
        negative_closest_height = height_numerator / horizontal_direction_sq;
    }
    const f32 closest_height = -negative_closest_height;

    // This normal lies in the plane formed by the edge and the movement axis.
    // Its z component tells how far along the sweep the cylinder surface is
    // reached.
    NUVEC cylinder_side = {
        edge_direction.x * edge_direction.z,
        -edge_direction.y * edge_direction.z,
        -(edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y),
    };
    const f32 cylinder_side_length = NuFsqrt(cylinder_side.x * cylinder_side.x + cylinder_side.y * cylinder_side.y +
                                             cylinder_side.z * cylinder_side.z);
    if (cylinder_side_length != 0.0f && cylinder_side.z != 0.0f) {
        cylinder_side.z /= cylinder_side_length;
    } else {
        cylinder_side.z = 0.0f;
    }

    const f32 radial_extent = NuFsqrt(query->collision_radius_sq - line_distance_sq);
    f32 height_offset = 0.0f;
    if (radial_extent != 0.0f && cylinder_side.z != 0.0f) {
        height_offset = radial_extent / cylinder_side.z;
    }

    f32 hit_distance;
    if (height_offset >= 0.0f) {
        hit_distance = closest_height - height_offset;
    } else {
        hit_distance = height_offset - negative_closest_height;
    }
    if (hit_distance >= 0.0f && hit_distance <= query->movement_length) {
        const f32 edge_parameter = (hit_distance - query->transformed_vertices[first_vertex].z) * edge_direction.z -
                                   query->transformed_vertices[first_vertex].x * edge_direction.x -
                                   query->transformed_vertices[first_vertex].y * edge_direction.y;
        if (edge_parameter > 0.0f && edge_parameter <= edge_length) {
            f32 hit_time = 0.0f;
            if (hit_distance != 0.0f && query->movement_length != 0.0f) {
                hit_time = hit_distance / query->movement_length;
            }
            if (query->hit_time <= hit_time) {
                *vertex_mask &= remaining_vertex_mask;
                return 0;
            }

            query->hit_type = TERRAIN_HIT_TYPE_CYLINDER;
            query->hit_time = hit_time;
            query->movement_normal.x =
                -(query->transformed_vertices[first_vertex].x + edge_direction.x * edge_parameter);
            query->movement_normal.y =
                -(query->transformed_vertices[first_vertex].y + edge_direction.y * edge_parameter);
            query->movement_normal.z =
                hit_distance - query->transformed_vertices[first_vertex].z - edge_direction.z * edge_parameter;
            *vertex_mask &= remaining_vertex_mask;
            return 1;
        }
    }

    // No forward-time hit was found. Test whether the sphere already overlaps
    // the finite edge at the start of the sweep.
    if ((query->transformed_vertices[first_vertex].z < -query->collision_radius &&
         query->transformed_vertices[second_vertex].z < -query->collision_radius) ||
        (query->transformed_vertices[first_vertex].z > query->collision_radius &&
         query->transformed_vertices[second_vertex].z > query->collision_radius)) {
        return 0;
    }

    const f32 closest_parameter = -query->transformed_vertices[first_vertex].x * edge_direction.x -
                                  query->transformed_vertices[first_vertex].y * edge_direction.y -
                                  query->transformed_vertices[first_vertex].z * edge_direction.z;
    const f32 edge_projection =
        (query->transformed_vertices[second_vertex].x - query->transformed_vertices[first_vertex].x) *
            edge_direction.x +
        (query->transformed_vertices[second_vertex].y - query->transformed_vertices[first_vertex].y) *
            edge_direction.y +
        (query->transformed_vertices[second_vertex].z - query->transformed_vertices[first_vertex].z) * edge_direction.z;
    if (closest_parameter < 0.0f || closest_parameter > edge_projection) {
        return 0;
    }

    // The normalized direction is no longer needed after the closest
    // parameter has been bounded, so reuse it for the closest point itself.
    edge_direction = {
        query->transformed_vertices[first_vertex].x + edge_direction.x * closest_parameter,
        query->transformed_vertices[first_vertex].y + edge_direction.y * closest_parameter,
        query->transformed_vertices[first_vertex].z + edge_direction.z * closest_parameter,
    };
    const f32 distance_sq =
        edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y + edge_direction.z * edge_direction.z;
    if (distance_sq >= query->collision_radius_sq) {
        return 0;
    }

    const f32 distance = NuFsqrt(distance_sq);
    const f32 overlap_time = distance - query->collision_radius;
    if (query->hit_time <= overlap_time) {
        return 0;
    }

    RotateVec(&edge_direction, &horizontal_perpendicular);
    const NUVEC &surface_normal = query->working_surface->normals[0];
    if (horizontal_perpendicular.x * surface_normal.x + horizontal_perpendicular.y * surface_normal.y +
            horizontal_perpendicular.z * surface_normal.z <
        0.0f) {
        query = TerI;
        query->hit_time = overlap_time;
        f32 inverse_distance = 0.0f;
        if (distance != 0.0f) {
            inverse_distance = 1.0f / distance;
        }
        query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
        query->movement_normal.x = -edge_direction.x * inverse_distance;
        query->movement_normal.y = -edge_direction.y * inverse_distance;
        query->movement_normal.z = -edge_direction.z * inverse_distance;
        return 1;
    }
    return 0;
}

void HitWallSpline() {
}

i32 CheckSphereTer(NUVEC *position, f32 radius) {
    const f32 terrain_radius = TerI->collision_radius;
    const f32 combined_radius = radius + terrain_radius;

    if (position->z < -combined_radius || position->z > TerI->movement_length + combined_radius) {
        return 0;
    }

    const f32 radial_distance_sq = position->x * position->x + position->y * position->y;
    const f32 combined_radius_sq = combined_radius * combined_radius;
    if (radial_distance_sq > combined_radius_sq) {
        return 0;
    }

    const f32 radial_extent = NuFsqrt(combined_radius_sq - radial_distance_sq);
    const f32 hit_distance = position->z - radial_extent;
    if (hit_distance >= 0.0f && hit_distance <= TerI->movement_length) {
        const f32 hit_time = hit_distance / TerI->movement_length;
        if (hit_time >= TerI->hit_time) {
            return 0;
        }

        TerI->hit_time = hit_time;
        TerI->hit_type = TERRAIN_HIT_TYPE_SPHERE;
        TerI->movement_normal.x = -position->x;
        TerI->movement_normal.y = -position->y;
        TerI->movement_normal.z = -radial_extent;
        return 1;
    }

    const f32 distance_sq = radial_distance_sq + position->z * position->z;
    if (distance_sq >= combined_radius_sq) {
        return 0;
    }

    const f32 distance = NuFsqrt(distance_sq);
    f32 inverse_distance = 0.0f;
    if (distance != 0.0f) {
        inverse_distance = 1.0f / distance;
    }

    TerI->hit_time = 0.0f;
    TerI->hit_type = TERRAIN_HIT_TYPE_SPHERE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
    TerI->movement_normal.x = -position->x * inverse_distance;
    TerI->movement_normal.y = -position->y * inverse_distance;
    TerI->movement_normal.z = -position->z * inverse_distance;
    return 1;
}

i16 InsidePolLines(f32 point_x, f32 point_y, f32 point_z, f32 edge_a_x, f32 edge_a_y, f32 edge_a_z, f32 edge_b_x,
                   f32 edge_b_y, f32 edge_b_z, NUVEC *normal) {
    // Project onto the plane perpendicular to the dominant normal component.
    // The call order preserves the polygon winding in that projection.
    if (NuFabs(normal->y) >= NuFabs(normal->x) && NuFabs(normal->y) >= NuFabs(normal->z)) {
        if (0.0f > normal->y) {
            if (InsideLineF(point_x, point_z, 0.0f, 0.0f, edge_a_x, edge_a_z) == 0 ||
                InsideLineF(point_x, point_z, edge_b_x, edge_b_z, 0.0f, 0.0f) == 0) {
                return 0;
            }
            return InsideLineF(point_x, point_z, edge_a_x, edge_a_z, edge_b_x, edge_b_z) != 0;
        }

        if (InsideLineF(point_x, point_z, edge_a_x, edge_a_z, 0.0f, 0.0f) == 0 ||
            InsideLineF(point_x, point_z, 0.0f, 0.0f, edge_b_x, edge_b_z) == 0) {
            return 0;
        }
        return InsideLineF(point_x, point_z, edge_b_x, edge_b_z, edge_a_x, edge_a_z) != 0;
    }

    if (NuFabs(normal->y) > NuFabs(normal->x) || NuFabs(normal->z) > NuFabs(normal->x)) {
        if (0.0f > normal->z) {
            if (InsideLineF(point_y, point_x, 0.0f, 0.0f, edge_a_y, edge_a_x) == 0 ||
                InsideLineF(point_y, point_x, edge_b_y, edge_b_x, 0.0f, 0.0f) == 0) {
                return 0;
            }
            return InsideLineF(point_y, point_x, edge_a_y, edge_a_x, edge_b_y, edge_b_x) != 0;
        }

        if (InsideLineF(point_y, point_x, edge_a_y, edge_a_x, 0.0f, 0.0f) == 0 ||
            InsideLineF(point_y, point_x, 0.0f, 0.0f, edge_b_y, edge_b_x) == 0) {
            return 0;
        }
        return InsideLineF(point_y, point_x, edge_b_y, edge_b_x, edge_a_y, edge_a_x) != 0;
    }

    if (0.0f > normal->x) {
        if (InsideLineF(point_y, point_z, edge_a_y, edge_a_z, 0.0f, 0.0f) == 0 ||
            InsideLineF(point_y, point_z, 0.0f, 0.0f, edge_b_y, edge_b_z) == 0) {
            return 0;
        }
        return InsideLineF(point_y, point_z, edge_b_y, edge_b_z, edge_a_y, edge_a_z) != 0;
    }

    if (InsideLineF(point_y, point_z, 0.0f, 0.0f, edge_a_y, edge_a_z) == 0 ||
        InsideLineF(point_y, point_z, edge_b_y, edge_b_z, 0.0f, 0.0f) == 0) {
        return 0;
    }
    return InsideLineF(point_y, point_z, edge_a_y, edge_a_z, edge_b_y, edge_b_z) != 0;
}

void ObjHitObj_Flags(GameObject_s *) {
}

void CollideGameObjects(WORLDINFO_s *) {
}

void CalculateRayBoxIntersection(VuVec const &, VuVec const &, VuVec const &, VuVec const &, float, float &) {
}

void CalcCapsuleIntersectDistance(VuVec const &, VuVec const &, float, VuVec const &, float) {
}

i32 HitPoly(f32 primary_start, f32 primary_end, f32 secondary_start, f32 secondary_end, tertype *surface) {
    TerrainQuery_s *query = TerI;
    const f32 radius = query->collision_radius;
    const f32 no_secondary_normal = 65536.0f;

    i32 collision_found = 0;
    f32 candidate_time = 0.0f;
    NUVEC contact_point;

    const NUVEC &primary_origin = surface->vectors[0];
    NUVEC &primary_normal = surface->normals[0];
    contact_point.x = query->local_start.x - radius * primary_normal.x - primary_origin.x;
    contact_point.y = query->local_start.y - radius * primary_normal.y - primary_origin.y;
    contact_point.z = query->local_start.z - radius * primary_normal.z - primary_origin.z;

    i32 test_primary_face = 0;
    if (primary_start > 0.0f && primary_end < 0.0f) {
        candidate_time = primary_start / (primary_start - primary_end);
        contact_point.x += query->movement.x * candidate_time;
        contact_point.y += query->movement.y * candidate_time;
        contact_point.z += query->movement.z * candidate_time;
        test_primary_face = 1;
    } else if (primary_start <= 0.0f && primary_end <= 0.0f && primary_start >= -radius && primary_end >= -radius) {
        candidate_time = primary_start;
        const f32 projection_distance = -primary_start;
        contact_point.x += primary_normal.x * projection_distance;
        contact_point.y += primary_normal.y * projection_distance;
        contact_point.z += primary_normal.z * projection_distance;
        test_primary_face = 1;
    }

    if (test_primary_face != 0) {
        const NUVEC edge_0_1 = {
            surface->vectors[1].x - primary_origin.x,
            surface->vectors[1].y - primary_origin.y,
            surface->vectors[1].z - primary_origin.z,
        };
        const NUVEC edge_0_2 = {
            surface->vectors[2].x - primary_origin.x,
            surface->vectors[2].y - primary_origin.y,
            surface->vectors[2].z - primary_origin.z,
        };
        if (InsidePolLines(contact_point.x, contact_point.y, contact_point.z, edge_0_1.x, edge_0_1.y, edge_0_1.z,
                           edge_0_2.x, edge_0_2.y, edge_0_2.z, &primary_normal) != 0 &&
            candidate_time <= query->hit_time) {
            query->hit_time = candidate_time;
            query->hit_type =
                primary_start > 0.0f ? TERRAIN_HIT_TYPE_FACE : TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
            query->movement_normal = primary_normal;
            query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
            collision_found = 1;
        }
    }

    NUVEC &secondary_normal = surface->normals[1];
    if (secondary_normal.y < no_secondary_normal) {
        const NUVEC &secondary_origin = surface->vectors[3];
        contact_point.x = query->local_start.x - radius * secondary_normal.x - secondary_origin.x;
        contact_point.y = query->local_start.y - radius * secondary_normal.y - secondary_origin.y;
        contact_point.z = query->local_start.z - radius * secondary_normal.z - secondary_origin.z;

        i32 test_secondary_face = 0;
        if (secondary_start > 0.0f && secondary_end < 0.0f) {
            candidate_time = secondary_start / (secondary_start - secondary_end);
            contact_point.x += query->movement.x * candidate_time;
            contact_point.y += query->movement.y * candidate_time;
            contact_point.z += query->movement.z * candidate_time;
            test_secondary_face = 1;
        } else if (secondary_start <= 0.0f && secondary_end <= 0.0f && secondary_start >= -radius &&
                   secondary_end >= -radius) {
            candidate_time = secondary_start;
            const f32 projection_distance = -secondary_start;
            contact_point.x += secondary_normal.x * projection_distance;
            contact_point.y += secondary_normal.y * projection_distance;
            contact_point.z += secondary_normal.z * projection_distance;
            test_secondary_face = 1;
        }

        if (test_secondary_face != 0) {
            const NUVEC edge_3_2 = {
                surface->vectors[2].x - secondary_origin.x,
                surface->vectors[2].y - secondary_origin.y,
                surface->vectors[2].z - secondary_origin.z,
            };
            const NUVEC edge_3_1 = {
                surface->vectors[1].x - secondary_origin.x,
                surface->vectors[1].y - secondary_origin.y,
                surface->vectors[1].z - secondary_origin.z,
            };
            if (InsidePolLines(contact_point.x, contact_point.y, contact_point.z, edge_3_2.x, edge_3_2.y, edge_3_2.z,
                               edge_3_1.x, edge_3_1.y, edge_3_1.z, &secondary_normal) != 0 &&
                candidate_time < query->hit_time) {
                query->hit_time = candidate_time;
                query->hit_type = secondary_start > 0.0f ? TERRAIN_HIT_TYPE_FACE
                                                         : TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                query->movement_normal = secondary_normal;
                query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                collision_found = 1;
            }
        }
    }

    if (radius != 0.0f) {
        query->working_surface = surface;
        i32 vertex_mask = 0xf;
        DeRotateTerrain(surface);

        if (primary_start >= -radius) {
            if (CheckCylinder(0, 1, &vertex_mask, 0xc) != 0) {
                query = TerI;
                query->hit_edge = TERRAIN_HIT_EDGE_0_1;
                query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                collision_found = 1;
            }
            if (CheckCylinder(2, 0, &vertex_mask, 0xa) != 0) {
                query = TerI;
                query->hit_edge = TERRAIN_HIT_EDGE_2_0;
                query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                collision_found = 1;
            }
        }

        if (CheckCylinder(1, 2, &vertex_mask, 0x9) != 0) {
            query = TerI;
            query->hit_edge = TERRAIN_HIT_EDGE_1_2;
            query->shape_adjusted = (surface->normal_flags & (TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP |
                                                              TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)) != 0;
            collision_found = 1;
        }

        if (secondary_normal.y < no_secondary_normal && secondary_start >= -radius) {
            if (CheckCylinder(1, 3, &vertex_mask, 0x5) != 0) {
                query = TerI;
                query->hit_edge = TERRAIN_HIT_EDGE_1_3;
                query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                collision_found = 1;
            }
            if (CheckCylinder(3, 2, &vertex_mask, 0x3) != 0) {
                query = TerI;
                query->hit_edge = TERRAIN_HIT_EDGE_3_2;
                query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                collision_found = 1;
            }
            if ((vertex_mask & (1 << 3)) != 0 && CheckSphere(3) != 0) {
                TerI->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                collision_found = 1;
            }
        }

        if (primary_start >= -radius) {
            if ((vertex_mask & (1 << 0)) != 0 && CheckSphere(0) != 0) {
                TerI->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                collision_found = 1;
            }
            if ((vertex_mask & (1 << 1)) != 0 && CheckSphere(1) != 0) {
                TerI->shape_adjusted = (surface->normal_flags & (TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP |
                                                                 TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)) != 0;
                collision_found = 1;
            }
        }

        if ((vertex_mask & (1 << 2)) != 0 && CheckSphere(2) != 0) {
            TerI->shape_adjusted = (surface->normal_flags & (TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP |
                                                             TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)) != 0;
            collision_found = 1;
        }
    }

    query = TerI;
    if (collision_found != 0) {
        query->surface = surface;
    }
    query->unclamped_hit_time = query->hit_time;
    if (query->hit_time < 0.0f) {
        query->hit_time = 0.0f;
    }
    return collision_found;
}

void CheckCol(nutex_s *, i32, i32, i32, i32) {
}

void HitRumble(GameObject_s *) {
}

void ObjHitObj(GameObject_s *, GameObject_s *, i32, u16, i32, i32) {
}

void RayImpact(nuvec_s *) {
}
