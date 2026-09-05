#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

static VARIPTR fstack;
static i8 fstackmem[12288];
static VARIPTR fstack_limit;
static VARIPTR fstack_top;

extern NUCAMERA *cam;
extern NUMTX local_inv_view_mtx;
extern NUVEC world_campos;
extern i16 camera_roomid;
extern NUPLANE cam_plane;
extern NUFRUSTRUM **frustra;
extern i32 *nfrustra;
extern i32 draw_portals;

extern "C" void clipRoomAgainstFrustrum(NUGSCN *scene, NUROOM *room, NUFRUSTRUM *frustum);

static i16 rooms_visited[16];

static NUFRUSTRUM *allocateFrustrum(i16 plane_count, i16 room_id) {
    NUFRUSTRUM *frustum = reinterpret_cast<NUFRUSTRUM *>(ALIGN(fstack_top.addr, 16));
    frustum->planes = reinterpret_cast<NUPLANE *>(reinterpret_cast<u8 *>(frustum) + sizeof(NUFRUSTRUM));
    fstack_top.addr = reinterpret_cast<usize>(frustum->planes + plane_count);
    frustum->plane_count = plane_count;
    frustum->room_id = room_id;
    frustra[*nfrustra] = frustum;
    ++*nfrustra;
    return frustum;
}

void transposeClipPlanes(NUFRUSTRUM *frustum) {
    for (i32 i = 0; i < 8; ++i) {
        const NUPLANE &plane = i < frustum->plane_count ? frustum->planes[i] : cam_plane;
        frustum->transposed_planes[i] = plane.a;
        frustum->transposed_planes[8 + i] = plane.b;
        frustum->transposed_planes[16 + i] = plane.c;
        frustum->transposed_planes[24 + i] = plane.d;
    }
}

static void transformFrustrumPlane(NUPLANE *plane) {
    const f32 x = plane->a;
    const f32 y = plane->b;
    const f32 z = plane->c;
    plane->a = x * local_inv_view_mtx.m00 + y * local_inv_view_mtx.m10 + z * local_inv_view_mtx.m20;
    plane->b = x * local_inv_view_mtx.m01 + y * local_inv_view_mtx.m11 + z * local_inv_view_mtx.m21;
    plane->c = x * local_inv_view_mtx.m02 + y * local_inv_view_mtx.m12 + z * local_inv_view_mtx.m22;
    plane->d =
        -(local_inv_view_mtx.m30 * plane->a + local_inv_view_mtx.m31 * plane->b + local_inv_view_mtx.m32 * plane->c);
}

NUFRUSTRUM *buildFrustrum(NUVEC *minimum, NUVEC *maximum, i16 room_id) {
    NUFRUSTRUM *frustum = allocateFrustrum(4, room_id);
    frustum->minimum = *minimum;
    frustum->maximum = *maximum;

    const f32 tangent = NU_TAN_LUT(cam->fov * 0.5f * 10430.378f);
    const f32 horizontal_tangent = tangent / cam->aspect;

    f32 edge = maximum->x * horizontal_tangent;
    f32 inverse_length = 1.0f / NuFsqrt(edge * edge + 1.0f);
    frustum->planes[0] = {-inverse_length, 0.0f, edge * inverse_length, 0.0f};

    edge = minimum->x * horizontal_tangent;
    inverse_length = 1.0f / NuFsqrt(edge * edge + 1.0f);
    frustum->planes[1] = {inverse_length, 0.0f, -edge * inverse_length, 0.0f};

    edge = minimum->y * tangent;
    inverse_length = 1.0f / NuFsqrt(edge * edge + 1.0f);
    frustum->planes[2] = {0.0f, inverse_length, -edge * inverse_length, 0.0f};

    edge = maximum->y * tangent;
    inverse_length = 1.0f / NuFsqrt(edge * edge + 1.0f);
    frustum->planes[3] = {0.0f, -inverse_length, edge * inverse_length, 0.0f};

    for (i32 i = 0; i < 4; ++i) {
        transformFrustrumPlane(&frustum->planes[i]);
    }
    transposeClipPlanes(frustum);
    return frustum;
}

static void computePortalExtents(NUPORTAL *portal, NUVEC *minimum, NUVEC *maximum) {
    NUMTX clip;
    NuCameraGetClipMtx(&clip, NULL);
    NuVecMtxTransformH(minimum, &portal->vertices[0], &clip);
    *maximum = *minimum;
    for (i32 i = 1; i < portal->vertex_count; ++i) {
        NUVEC point;
        NuVecMtxTransformH(&point, &portal->vertices[i], &clip);
        NuVecMax(maximum, maximum, &point);
        NuVecMin(minimum, minimum, &point);
    }
    const NUVEC minus_one = {-1.0f, -1.0f, -1.0f};
    const NUVEC one = {1.0f, 1.0f, 1.0f};
    NuVecMin(maximum, maximum, const_cast<NUVEC *>(&one));
    NuVecMax(maximum, maximum, const_cast<NUVEC *>(&minus_one));
    NuVecMin(minimum, minimum, const_cast<NUVEC *>(&one));
    NuVecMax(minimum, minimum, const_cast<NUVEC *>(&minus_one));
}

static f32 planeDistance(const NUPLANE &plane, const NUVEC &point) {
    return plane.a * point.x + plane.b * point.y + plane.c * point.z + plane.d;
}

static NUFRUSTRUM *buildPortalFrustrum(NUPORTAL *portal, i16 room_id) {
    NUFRUSTRUM *frustum = allocateFrustrum(portal->vertex_count, room_id);
    const f32 camera_side = planeDistance(portal->plane, world_campos);
    NUVEC camera_position = {
        local_inv_view_mtx.m30,
        local_inv_view_mtx.m31,
        local_inv_view_mtx.m32,
    };
    for (i32 i = 0; i < portal->vertex_count; ++i) {
        NUVEC first;
        NUVEC second;
        NUVEC normal;
        NuVecSub(&first, &camera_position, &portal->vertices[i]);
        NuVecSub(&second, &camera_position, &portal->vertices[(i + 1) % portal->vertex_count]);
        NuVecCross(&normal, &first, &second);
        NuVecNorm(&normal, &normal);
        if (camera_side >= 0.0f) {
            normal.x = -normal.x;
            normal.y = -normal.y;
            normal.z = -normal.z;
        }
        frustum->planes[i].a = normal.x;
        frustum->planes[i].b = normal.y;
        frustum->planes[i].c = normal.z;
        frustum->planes[i].d =
            -(normal.x * camera_position.x + normal.y * camera_position.y + normal.z * camera_position.z);
    }
    computePortalExtents(portal, &frustum->minimum, &frustum->maximum);
    transposeClipPlanes(frustum);
    return frustum;
}

static NUFRUSTRUM *copyFrustrum(NUFRUSTRUM *source, i16 room_id) {
    NUFRUSTRUM *copy = allocateFrustrum(source->plane_count, room_id);
    memcpy(copy->transposed_planes, source->transposed_planes, sizeof(copy->transposed_planes));
    copy->minimum = source->minimum;
    copy->maximum = source->maximum;
    memcpy(copy->planes, source->planes, sizeof(NUPLANE) * source->plane_count);
    return copy;
}

void roomRecursive(NUGSCN *scene, NUFRUSTRUM *frustum, i16 room_id, i16 previous_room, i32 depth) {
    if (depth >= 2) {
        for (i32 i = 0; i < depth - 1; ++i) {
            if (rooms_visited[i] == room_id) {
                return;
            }
        }
    }
    if (depth >= 16) {
        return;
    }

    NUROOM *room = &scene->rooms[room_id];
    rooms_visited[depth] = room_id;
    if (room_id == camera_roomid) {
        FlagRoomInstancesAsVisible(room, scene);
    } else {
        clipRoomAgainstFrustrum(scene, room, frustum);
    }
    room->flags |= NUROOM_FLAG_VISITED;
    if (depth >= scene->portal_depth || room->portal_count == 0) {
        return;
    }

    for (i32 portal_index = 0; portal_index < room->portal_count; ++portal_index) {
        NUPORTAL *portal = &scene->portals[room->portal_indices[portal_index]];
        i16 next_room = portal->front_room == room_id ? portal->back_room : portal->front_room;
        if (next_room == previous_room || (portal->is_active & NUPORTAL_FLAG_ACTIVE) == 0) {
            continue;
        }

        NUROOM *next = &scene->rooms[next_room];
        const f32 camera_side = planeDistance(portal->plane, world_campos);
        if (next->priority > room->priority && ((room_id == portal->back_room && camera_side < 0.0f) ||
                                                (room_id == portal->front_room && camera_side > 0.0f))) {
            continue;
        }

        i32 inside_tests = 0;
        bool rejected = false;
        for (i32 plane_index = 0; plane_index < frustum->plane_count; ++plane_index) {
            i32 outside_vertices = 0;
            for (i32 vertex_index = 0; vertex_index < portal->vertex_count; ++vertex_index) {
                if (planeDistance(frustum->planes[plane_index], portal->vertices[vertex_index]) < 0.0f) {
                    ++outside_vertices;
                } else {
                    ++inside_tests;
                }
            }
            if (outside_vertices == portal->vertex_count) {
                rejected = true;
                break;
            }
        }
        if (rejected) {
            continue;
        }

        NUFRUSTRUM *next_frustum = NULL;
        if (inside_tests == frustum->plane_count * portal->vertex_count) {
            next_frustum = buildPortalFrustrum(portal, next_room);
        } else {
            i32 in_front = 0;
            for (i32 i = 0; i < portal->vertex_count; ++i) {
                if (planeDistance(cam_plane, portal->vertices[i]) > 0.0f) {
                    ++in_front;
                }
            }
            if (in_front == 0) {
                continue;
            }
            if (in_front == portal->vertex_count) {
                NUVEC portal_minimum;
                NUVEC portal_maximum;
                computePortalExtents(portal, &portal_minimum, &portal_maximum);
                if (frustum->minimum.x >= portal_maximum.x || frustum->minimum.y >= portal_maximum.y ||
                    portal_minimum.x >= frustum->maximum.x || portal_minimum.y >= frustum->maximum.y) {
                    continue;
                }
                portal_minimum.x = MAX(portal_minimum.x, frustum->minimum.x);
                portal_minimum.y = MAX(portal_minimum.y, frustum->minimum.y);
                portal_maximum.x = MIN(portal_maximum.x, frustum->maximum.x);
                portal_maximum.y = MIN(portal_maximum.y, frustum->maximum.y);
                next_frustum = buildFrustrum(&portal_minimum, &portal_maximum, next_room);
            } else {
                next_frustum = copyFrustrum(frustum, next_room);
            }
        }
        roomRecursive(scene, next_frustum, next_room, room_id, depth + 1);
    }
}

void NuPortalInit(void) {
    fstack.void_ptr = fstackmem;
    fstack_top.addr = ALIGN((usize)fstackmem, 16);
    fstack_limit.void_ptr = fstackmem + 0x3000;
}

void NuPortalSetActiveDirect(NUPORTAL *portal, i32 active) {
    if (portal == NULL) {
        return;
    }

    if (active) {
        portal->is_active |= NUPORTAL_FLAG_ACTIVE;
    } else {
        portal->is_active = 0;
    }
}

void NuPortalSetActive(NUGSCN *scene, i32 portal_id, i32 active) {
    for (u32 index = 0; index < scene->max_portals; ++index) {
        NUPORTAL *portal = &scene->portals[index];
        if (portal->id == portal_id) {
            if (active != 0) {
                portal->is_active |= NUPORTAL_FLAG_ACTIVE;
            } else {
                portal->is_active = 0;
            }
        }
    }
}

void NuPortalMaxDepth(struct nugscn_s *scene, i32 depth) {
    scene->portal_depth = depth;
}
