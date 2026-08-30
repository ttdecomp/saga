#include "legoapi/props/system/socksys.h"

#include <stdio.h>
#include <string.h>

#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
struct GameObject_s;

extern "C" void PerspectMidPoint(NUVEC *result, NUVEC *first, NUVEC *second, NUVEC *camera_position);

// Spline name prefixes for a socket's rails; the socket index is appended
// zero-padded, e.g. "sock_cam_03". Looked up against the scene splines in
// SockSysFindInScene.
static const char *tSockCAM = "sock_cam_";
static const char *tSockA = "sock_a_";
static const char *tSockB = "sock_b_";
static const char *tSockC = "sock_c_";
static const char *tSockD = "sock_d_";
static const char *tSockMID = "sock_mid_";
static const char *tSockLEFT = "sock_left_";
static const char *tSockRIGHT = "sock_right_";
static const char *tSockLOOK = "sock_look_";
static const char *tSockLATERAL = "sock_lateral_";
static const char *tSockTRACKIN = "sock_trackin_";
static const char *tSockLIMIT = "sock_limit_";

// Grows `min`/`max` to include `pnt`.
static void AdjustMinMaxBox(NUVEC *pnt, NUVEC *min, NUVEC *max) {
    if (pnt->x < min->x) {
        min->x = pnt->x;
    }
    if (max->x < pnt->x) {
        max->x = pnt->x;
    }
    if (pnt->y < min->y) {
        min->y = pnt->y;
    }
    if (max->y < pnt->y) {
        max->y = pnt->y;
    }
    if (pnt->z < min->z) {
        min->z = pnt->z;
    }
    if (max->z < pnt->z) {
        max->z = pnt->z;
    }
}

extern "C" {

    void SockSysFindInScene(SOCKSYS *sock_sys, NUGSCN *gscn) {
        i32 i;
        i32 j;
        SOCK *sock;
        i32 len;
        float extent_z;
        char name[256];

        if ((gscn != NULL) && (sock_sys != NULL)) {
            sock = sock_sys->sock;
            for (i = 0; i < 0x40; i++, sock++) {
                if (sock->cam != NULL) {
                    continue;
                }
                sock->unknown_7c = 1.0f;
                sock->unknown_80 = 1.0f;
                sock->unknown_84 = 1.0f;
                sock->unknown_88 = 1.0f;
                sock->unknown_8c = 1.0f;
                sock->unknown_90 = 5.0f;
                sock->unknown_94 = 5.0f;
                sprintf(sock->name, "%.2i", i);
                sock->unknown_f4 = 0.5f;
                sprintf(name, "%s%.2i", tSockCAM, i);
                sock->cam = NuSplineFind(gscn, name);
                if (sock->cam == NULL) {
                    continue;
                }
                len = (i32)(i16)sock->cam->length;
                if (1 < len) {
                    sock->length = len - 1;
                    sprintf(name, "%s%.2i", tSockA, i);
                    sock->a = NuSplineFind(gscn, name);
                    if (sock->a == NULL) {
                        continue;
                    }
                    if ((i32)(i16)sock->a->length != len) {
                        continue;
                    }
                    sprintf(name, "%s%.2i", tSockB, i);
                    sock->b = NuSplineFind(gscn, name);
                    if (sock->b == NULL) {
                        continue;
                    }
                    if ((i32)(i16)sock->b->length != len) {
                        continue;
                    }
                    sprintf(name, "%s%.2i", tSockC, i);
                    sock->c = NuSplineFind(gscn, name);
                    if (sock->c == NULL) {
                        sock->flags |= 1;
                    } else if ((i32)(i16)sock->c->length != len) {
                        sock->flags |= 1;
                    }
                    if ((sock->flags & 1) == 0) {
                        sprintf(name, "%s%.2i", tSockD, i);
                        sock->d = NuSplineFind(gscn, name);
                        if (sock->d == NULL) {
                            sock->flags |= 1;
                        } else if ((i32)(i16)sock->d->length != len) {
                            sock->flags |= 1;
                        }
                    }
                    sprintf(name, "%s%.2i", tSockMID, i);
                    sock->mid = NuSplineFind(gscn, name);
                    if ((sock->mid != NULL) && ((i32)(i16)sock->mid->length != len)) {
                        sock->mid = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLEFT, i);
                    sock->left = NuSplineFind(gscn, name);
                    if ((sock->left != NULL) && ((i32)(i16)sock->left->length != len)) {
                        sock->left = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockRIGHT, i);
                    sock->right = NuSplineFind(gscn, name);
                    if ((sock->right != NULL) && ((i32)(i16)sock->right->length != len)) {
                        sock->right = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLOOK, i);
                    sock->look = NuSplineFind(gscn, name);
                    if ((sock->look != NULL) && ((i32)(i16)sock->look->length != len)) {
                        sock->look = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLATERAL, i);
                    sock->lateral = NuSplineFind(gscn, name);
                    if ((sock->lateral != NULL) && ((i32)(i16)sock->lateral->length != len)) {
                        sock->lateral = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockTRACKIN, i);
                    sock->trackin = NuSplineFind(gscn, name);
                    if ((sock->trackin != NULL) && ((i32)(i16)sock->trackin->length != len)) {
                        sock->trackin = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLIMIT, i);
                    sock->limit = NuSplineFind(gscn, name);
                    sock->max = sock->a->pts[0];
                    sock->min = sock->max;
                    for (j = 0; j < len; j++) {
                        AdjustMinMaxBox(&sock->a->pts[j], &sock->min, &sock->max);
                        AdjustMinMaxBox(&sock->b->pts[j], &sock->min, &sock->max);
                        if ((sock->flags & 1) == 0) {
                            AdjustMinMaxBox(&sock->c->pts[j], &sock->min, &sock->max);
                            AdjustMinMaxBox(&sock->d->pts[j], &sock->min, &sock->max);
                        }
                    }
                    sock->center.x = (sock->min.x + sock->max.x) * 0.5f;
                    sock->center.y = (sock->min.y + sock->max.y) * 0.5f;
                    sock->center.z = (sock->min.z + sock->max.z) * 0.5f;
                    sock->extent = sock->max.x - sock->center.x;
                    extent_z = sock->max.z - sock->center.z;
                    if (extent_z < sock->extent) {
                        sock->extent = extent_z;
                    }
                    sock->valid = 1;
                    sock_sys->count++;
                }
            }
        }
    }

    SOCKSYS *SockSysInit(VARIPTR *buf, VARIPTR buf_end, NUGSCN *gscn) {
        buf->addr = ALIGN(buf->addr, 4);
        if (buf->addr + 0x4f08 >= buf_end.addr) {
            return NULL;
        }

        SOCKSYS *sys = (SOCKSYS *)buf->void_ptr;
        buf->void_ptr = (char *)buf->void_ptr + 8;
        sys->sock = (SOCK *)buf->void_ptr;
        buf->void_ptr = (char *)buf->void_ptr + 0x4f00;
        sys->count = 0;
        memset(sys->sock, 0, 0x4f00);
        SockSysFindInScene(sys, gscn);
        return sys;
    }
}

static __used__ void SockDataError() {
}

static i32 RotDiff(u16 from, u16 to) {
    i32 difference = (i32)to - (i32)from;
    if (difference > 0x8000) {
        difference -= 0x10000;
    } else if (difference < -0x8000) {
        difference += 0x10000;
    }
    return difference;
}

static void SockEdgeAnglesXY(NUVEC *from, NUVEC *to, u16 *x, u16 *y) {
    f32 dx = to->x - from->x;
    f32 dz = to->z - from->z;
    *x = (u16)-NuAtan2D(to->y - from->y, NuFsqrt(dx * dx + dz * dz));
    *y = (u16)NuAtan2D(dx, dz);
}

static void SockMidpointAt(const SOCK *sock, i32 point, NUVEC *midpoint) {
    if (sock->mid != NULL) {
        *midpoint = sock->mid->pts[point];
        return;
    }

    NuVecAdd(midpoint, &sock->a->pts[point], &sock->b->pts[point]);
    if ((sock->flags & 1) == 0) {
        NuVecAdd(midpoint, midpoint, &sock->c->pts[point]);
        NuVecAdd(midpoint, midpoint, &sock->d->pts[point]);
        NuVecScale(midpoint, midpoint, 0.25f);
    } else {
        NuVecScale(midpoint, midpoint, 0.5f);
    }
}

static void SockRailAngles(SOCK *sock, NUGSPLINE *spline, SOCKROT *rotations) {
    i32 point_count = sock->cam->length;
    for (i32 point = 0; point < point_count; ++point) {
        i32 previous = point - 1;
        if (previous < 0) {
            previous = point_count - 1;
        }
        i32 next = point + 1;
        if (next >= point_count) {
            next = 0;
        }

        NUVEC previous_pos;
        NUVEC position;
        NUVEC next_pos;
        if (spline != NULL) {
            previous_pos = spline->pts[previous];
            position = spline->pts[point];
            next_pos = spline->pts[next];
        } else {
            SockMidpointAt(sock, previous, &previous_pos);
            SockMidpointAt(sock, point, &position);
            SockMidpointAt(sock, next, &next_pos);
        }

        u16 x0 = 0;
        u16 y0 = 0;
        u16 x1 = 0;
        u16 y1 = 0;
        bool average = true;
        if (point == 0 && sock->unknown_33 == 0) {
            SockEdgeAnglesXY(&position, &next_pos, &x0, &y0);
            average = false;
        } else if (point == point_count - 1 && sock->unknown_33 == 0) {
            SockEdgeAnglesXY(&previous_pos, &position, &x0, &y0);
            average = false;
        } else {
            SockEdgeAnglesXY(&previous_pos, &position, &x0, &y0);
            SockEdgeAnglesXY(&position, &next_pos, &x1, &y1);
        }

        rotations[point].x = x0;
        rotations[point].y = y0;
        if (average) {
            rotations[point].x = (u16)(x0 + RotDiff(x0, x1) / 2);
            rotations[point].y = (u16)(y0 + RotDiff(y0, y1) / 2);
        }
    }
}

static f32 SplineLength(NUGSPLINE *spline, i32 closed) {
    if (spline == NULL || spline->length < 2) {
        return 0.0f;
    }
    i32 segment_count = closed == 0 ? spline->length - 1 : spline->length;
    f32 length = 0.0f;
    for (i32 segment = 0; segment < segment_count; ++segment) {
        i32 next = segment == spline->length - 1 ? 0 : segment + 1;
        length += NuVecDist(&spline->pts[segment], &spline->pts[next], NULL);
    }
    return length;
}

static void SetSockBitValue(SOCK *sock, i32 index) {
    sock->overlap_exclusion_mask[index >> 5] |= 1U << (index & 0x1f);
}

static bool SockBitIsSet(const SOCK *sock, i32 index) {
    return (sock->overlap_exclusion_mask[index >> 5] & (1U << (index & 0x1f))) != 0;
}

static bool BoundsOverlap(const NUVEC &min_a, const NUVEC &max_a, const NUVEC &min_b, const NUVEC &max_b,
                          bool ignore_y) {
    return min_b.x <= max_a.x && min_a.x <= max_b.x && min_b.z <= max_a.z && min_a.z <= max_b.z &&
           (ignore_y || (min_b.y <= max_a.y && min_a.y <= max_b.y));
}

SOCKPOSITION TempSPosList[4];
SOCKPOSITION *TempBestSPos;
i32 TempSPosCount;
NUVEC temp_pos;
SOCKPOSITION TempSPos;
u32 TempSLoc[2];
i32 complexsockposition_forcesock = -1;

static NUVEC temp_sockmidpos;
static NUVEC temp_sockcampos;

static bool OnOrOutsidePlane(NUVEC *point, NUVEC *plane_point, NUVEC *normal) {
    return normal->x * (point->x - plane_point->x) + normal->y * (point->y - plane_point->y) +
               normal->z * (point->z - plane_point->z) >=
           0.0f;
}

static bool InsideLineXZ(f32 x, f32 z, f32 x0, f32 z0, f32 x1, f32 z1) {
    return (x - x0) * (z1 - z0) + (x0 - x1) * (z - z0) >= 0.0f;
}

static f32 DistanceToPlane(NUVEC *point, NUVEC *plane_point, NUVEC *normal) {
    return normal->x * (point->x - plane_point->x) + normal->y * (point->y - plane_point->y) +
           normal->z * (point->z - plane_point->z);
}

static f32 RatioBetweenPlanes(NUVEC *point, NUVEC *point_a, NUVEC *normal_a, NUVEC *point_b, NUVEC *normal_b) {
    f32 distance_a = DistanceToPlane(point, point_a, normal_a);
    f32 distance_b = DistanceToPlane(point, point_b, normal_b);
    return distance_a / (distance_a + distance_b);
}

static f32 DistanceToLineXZ(NUVEC *point, NUVEC *line_a, NUVEC *line_b) {
    f32 dx = line_b->x - line_a->x;
    f32 dz = line_b->z - line_a->z;
    f32 length = NuFsqrt(dx * dx + dz * dz);
    if (length == 0.0f) {
        return 0.0f;
    }
    f32 cross = (point->x - line_a->x) * dz - (point->z - line_a->z) * dx;
    return cross < 0.0f ? -cross / length : cross / length;
}

static f32 RatioBetweenEdgesXZ(NUVEC *point, NUVEC *edge_a0, NUVEC *edge_a1, NUVEC *edge_b0, NUVEC *edge_b1) {
    f32 distance_a = DistanceToLineXZ(point, edge_a0, edge_a1);
    f32 distance_b = DistanceToLineXZ(point, edge_b0, edge_b1);
    return distance_a / (distance_a + distance_b);
}

static void SockSysPointAlongMID(SOCK *sock, SOCKPOSITION *position, NUVEC *result) {
    NUVEC *from;
    NUVEC *to;
    if (sock->mid == NULL) {
        from = &sock->segments[position->location.segment].midpoint;
        to = &sock->segments[position->next_segment].midpoint;
    } else {
        from = &sock->mid->pts[position->location.segment];
        to = &sock->mid->pts[position->next_segment];
    }
    if (result == NULL) {
        result = &temp_sockmidpos;
    }
    result->x = from->x + (to->x - from->x) * position->ratio;
    result->y = from->y + (to->y - from->y) * position->ratio;
    result->z = from->z + (to->z - from->z) * position->ratio;
}

static void FillSockPosition(SOCKSYS *sock_sys, SOCKPOSITION *position) {
    if (sock_sys == NULL) {
        return;
    }
    SOCK *sock = &sock_sys->sock[position->location.sock];
    i32 segment = position->location.segment;
    i32 next = position->next_segment;
    SockSysPointAlongSpline(&temp_sockcampos, sock->cam, segment, next, position->ratio);

    SOCKROT *cam_from = &sock->cam_rotations[segment];
    SOCKROT *cam_to = &sock->cam_rotations[next];
    position->camera_rotation.x = (u16)(cam_from->x + (f32)RotDiff(cam_from->x, cam_to->x) * position->ratio);
    position->camera_rotation.y = (u16)(cam_from->y + (f32)RotDiff(cam_from->y, cam_to->y) * position->ratio);

    SOCKROT *mid_from = &sock->mid_rotations[segment];
    SOCKROT *mid_to = &sock->mid_rotations[next];
    position->midpoint_rotation.x = (u16)(mid_from->x + (f32)RotDiff(mid_from->x, mid_to->x) * position->ratio);
    position->midpoint_rotation.y = (u16)(mid_from->y + (f32)RotDiff(mid_from->y, mid_to->y) * position->ratio);

    position->distance = (f32)segment + position->ratio;
    i32 divisor = sock->length + (sock->unknown_33 != 0 ? 1 : 0);
    position->normalized_distance = position->distance / (f32)divisor;
}

static f32 BestSockPosition(SOCKSYS *sock_sys, NUVEC *point, SOCKPOSITION *result, i32 sock_index, i32 prior_segment) {
    SOCK *sock = &sock_sys->sock[sock_index];
    result->location.sock = -1;
    if (sock->valid == 0) {
        return 0.0f;
    }

    bool within_bounds =
        sock->min.x <= point->x && point->x <= sock->max.x && sock->min.z <= point->z && point->z <= sock->max.z;
    if ((sock->flags & 1) == 0) {
        within_bounds = within_bounds && sock->min.y <= point->y && point->y <= sock->max.y;
    }
    if (!within_bounds) {
        return 0.0f;
    }

    i32 segment_count = sock->length + (sock->unknown_33 != 0 ? 1 : 0);
    i32 initial_segment = prior_segment == -1 ? segment_count / 2 : prior_segment;
    i32 ascending_segment = initial_segment + 1;
    i32 descending_segment = initial_segment - 1;
    i32 direction = 0;
    i32 exhausted_directions = 0;

    for (;;) {
        i32 segment;
        if (direction == 0) {
            segment = initial_segment;
        } else if (direction == 1) {
            segment = ascending_segment++;
        } else {
            segment = descending_segment--;
        }

        if (segment < 0 || segment >= segment_count) {
            exhausted_directions |= direction;
        } else {
            i32 next = segment + 1;
            if (next == segment_count && sock->unknown_33 != 0) {
                next = 0;
            }

            NUVEC *a0 = &sock->a->pts[segment];
            NUVEC *b0 = &sock->b->pts[segment];
            NUVEC *a1 = &sock->a->pts[next];
            NUVEC *b1 = &sock->b->pts[next];
            SOCKSEGMENT *segment_data = &sock->segments[segment];
            bool inside = false;

            if ((sock->flags & 1) == 0) {
                if (BoundsOverlap(segment_data->min, segment_data->max, *point, *point, false)) {
                    NUVEC *c1 = &sock->c->pts[next];
                    NUVEC *d1 = &sock->d->pts[next];
                    inside = OnOrOutsidePlane(point, a0, &segment_data->planes[0]) &&
                             OnOrOutsidePlane(point, a1, &segment_data->planes[1]) &&
                             OnOrOutsidePlane(point, b1, &segment_data->planes[2]) &&
                             OnOrOutsidePlane(point, c1, &segment_data->planes[3]) &&
                             OnOrOutsidePlane(point, d1, &segment_data->planes[4]) &&
                             OnOrOutsidePlane(point, b1, &segment_data->planes[5]);
                }
            } else if (segment_data->min.x <= point->x && point->x <= segment_data->max.x &&
                       segment_data->min.z <= point->z && point->z <= segment_data->max.z) {
                inside = InsideLineXZ(point->x, point->z, b0->x, b0->z, a0->x, a0->z) &&
                         InsideLineXZ(point->x, point->z, a0->x, a0->z, a1->x, a1->z) &&
                         InsideLineXZ(point->x, point->z, a1->x, a1->z, b1->x, b1->z) &&
                         InsideLineXZ(point->x, point->z, b1->x, b1->z, b0->x, b0->z);
            }

            if (inside) {
                result->location.sock = (i8)sock_index;
                result->location.segment = (i16)segment;
                if ((sock->flags & 1) == 0) {
                    result->ratio = RatioBetweenPlanes(point, a0, &segment_data->planes[0], &sock->c->pts[next],
                                                       &segment_data->planes[5]);
                } else {
                    result->ratio = RatioBetweenEdgesXZ(point, b0, a0, b1, a1);
                }
                result->next_segment = (i16)next;
                SockSysPointAlongMID(sock, result, &temp_sockmidpos);
                f32 distance_squared = NuVecDistSqr(point, &temp_sockmidpos, NULL);
                FillSockPosition(sock_sys, result);
                TempSLoc[0] = *(u32 *)&result->location;
                TempSLoc[1] = *(u32 *)&result->ratio;
                return distance_squared;
            }
        }

        if (exhausted_directions == 3) {
            return 0.0f;
        }
        direction = direction == 1 ? 2 : 1;
    }
}

extern "C" {

    void ComplexSockPosition(SOCKSYS *sock_sys, NUVEC *position, i32 prior_sock, i32 prior_segment,
                             SOCKPOSITION *result) {
        if (sock_sys == NULL) {
            result->location.sock = -1;
            result->location.segment = -1;
            result->flags = 0;
            return;
        }

        i32 original_prior_sock = prior_sock;
        i32 forced_sock = complexsockposition_forcesock;
        complexsockposition_forcesock = -1;
        memset(TempSPosList, 0, sizeof(TempSPosList));
        temp_pos = *position;

        i32 sockets_to_check = 0x40;
        if (forced_sock != -1) {
            prior_sock = forced_sock;
            prior_segment = -1;
            sockets_to_check = 1;
        } else if (prior_sock == -1) {
            prior_sock = 0;
            prior_segment = -1;
        }

        bool fallback_search = false;
        for (;;) {
            i32 candidate_count = 0;
            u32 candidate_mask = 0;
            bool excluded_socket_seen = false;
            SOCKPOSITION *best = NULL;
            f32 best_distance = 1000000.0f;

            for (i32 checked = 0; checked < sockets_to_check; ++checked) {
                SOCK *sock = &sock_sys->sock[prior_sock];
                if ((sock->flags & 0x100) == 0) {
                    bool excluded = forced_sock == -1 && !fallback_search && original_prior_sock != -1 &&
                                    prior_sock != original_prior_sock &&
                                    SockBitIsSet(&sock_sys->sock[original_prior_sock], prior_sock);
                    if (excluded) {
                        excluded_socket_seen = true;
                    } else {
                        f32 distance = BestSockPosition(sock_sys, position, &TempSPosList[candidate_count], prior_sock,
                                                        prior_segment);
                        SOCKPOSITION *candidate = &TempSPosList[candidate_count];
                        if (candidate->location.sock != -1) {
                            candidate->midpoint = temp_sockmidpos;
                            candidate->camera_position = temp_sockcampos;
                            if (best == NULL || distance < best_distance) {
                                best = candidate;
                                best_distance = distance;
                            }
                            candidate_mask |= 1U << (prior_sock & 0x1f);
                            ++candidate_count;
                            if (candidate_count == 4) {
                                break;
                            }
                        }
                    }
                }
                prior_sock = (prior_sock + 1) % 0x40;
                prior_segment = -1;
            }

            if (best != NULL) {
                *result = *best;
                result->candidate_mask = candidate_mask;
                result->flags = fallback_search ? 1 : 0;
                result->candidate_count = (i8)candidate_count;
                TempBestSPos = best;
                TempSPosCount = candidate_count;
                memcpy(TempSLoc, result, sizeof(TempSLoc));
                return;
            }
            if (!excluded_socket_seen) {
                result->location.sock = -1;
                result->location.segment = -1;
                result->flags = 0;
                result->candidate_count = 0;
                TempBestSPos = NULL;
                TempSPosCount = 0;
                memcpy(TempSLoc, result, sizeof(TempSLoc));
                return;
            }

            fallback_search = true;
            prior_sock = 0;
            prior_segment = -1;
            forced_sock = -1;
            sockets_to_check = 0x40;
        }
    }

    void ComplexSockAngles(void) {
    }

    void EnforceSockYLimits(void) {
    }

    void FindSock(void) {
    }

    void MidDistanceFromSockStart(void) {
    }

    void MoveSockPosition(void) {
    }

    void SetSockBit(SOCK *sock, i32 index) {
        SetSockBitValue(sock, index);
    }

    void SetSockPostion(void) {
    }

    bool SockBitSet(SOCK *sock, i32 index) {
        return SockBitIsSet(sock, index);
    }

    void SockOff(void) {
    }

    void SockOn(void) {
    }

    void SockRotationMatrix(void) {
    }

    void SockSegmentsAhead(void) {
    }

    i32 SockSysCamera(SOCKSYS *sock_sys, NUVEC *fallback_camera_position, i32 socket_changed,
                      NUVEC *player_camera_positions, NUVEC *player_positions, i32 player_count,
                      SOCKPOSITION *camera_socket_position, NUVEC *camera_position, NUVEC *camera_target,
                      f32 *overlap_blend, f32 *position_seek, f32 *angle_seek, f32 *terrain_clearance,
                      f32 *separation_scale) {
        if (sock_sys == NULL) {
            return 0;
        }

        NUVEC average_camera_position = {0.0f, 0.0f, 0.0f};
        NUVEC average_player_position = {0.0f, 0.0f, 0.0f};
        for (i32 i = 0; i < player_count; ++i) {
            NuVecAdd(&average_camera_position, &average_camera_position, &player_camera_positions[i]);
            NuVecAdd(&average_player_position, &average_player_position, &player_positions[i]);
        }
        f32 inverse_player_count = 1.0f / (f32)player_count;
        NuVecScale(&average_camera_position, &average_camera_position, inverse_player_count);
        NuVecScale(&average_player_position, &average_player_position, inverse_player_count);

        if (player_count == 2 && socket_changed == 0) {
            PerspectMidPoint(camera_target, &player_camera_positions[0], &player_camera_positions[1],
                             fallback_camera_position);
        } else {
            *camera_target = average_camera_position;
        }

        ComplexSockPosition(sock_sys, &average_camera_position, camera_socket_position->location.sock,
                            camera_socket_position->location.segment, camera_socket_position);
        if (camera_socket_position->location.sock == -1) {
            *camera_position = *fallback_camera_position;
            return 0;
        }

        *camera_position = {0.0f, 0.0f, 0.0f};
        NUVEC accumulated_target = {0.0f, 0.0f, 0.0f};
        if (overlap_blend != NULL) {
            *overlap_blend = 0.0f;
        }
        if (position_seek != NULL) {
            *position_seek = 0.0f;
        }
        if (angle_seek != NULL) {
            *angle_seek = 0.0f;
        }
        if (terrain_clearance != NULL) {
            *terrain_clearance = 0.0f;
        }
        if (separation_scale != NULL) {
            *separation_scale = 0.0f;
        }

        i32 contributing_sockets = 0;
        f32 single_player_pullback = 0.0f;
        f32 two_player_pullback = 0.0f;
        for (i32 i = 0; i < TempSPosCount; ++i) {
            SOCKPOSITION *candidate = &TempSPosList[i];
            SOCK *sock = &sock_sys->sock[candidate->location.sock];
            bool include =
                socket_changed != 0 || candidate->location.sock == camera_socket_position->location.sock ||
                !SockBitIsSet(&sock_sys->sock[camera_socket_position->location.sock], candidate->location.sock);
            if (!include) {
                continue;
            }

            NUVEC candidate_camera = candidate->camera_position;
            if (sock->unknown_b4 != 0.0f) {
                NUVEC forward = {0.0f, 0.0f, 1.0f};
                NuVecRotateX(&forward, &forward, candidate->camera_rotation.x);
                NuVecRotateY(&forward, &forward, candidate->camera_rotation.y);
                NuVecAddScale(&candidate_camera, &candidate_camera, &forward, sock->unknown_b4);
            }

            NuVecAdd(camera_position, camera_position, &candidate_camera);
            NuVecAdd(&accumulated_target, &accumulated_target, camera_target);
            if (overlap_blend != NULL) {
                *overlap_blend += sock->unknown_f4;
            }
            if (position_seek != NULL) {
                *position_seek += sock->unknown_90;
            }
            if (angle_seek != NULL) {
                *angle_seek += sock->unknown_94;
            }
            if (terrain_clearance != NULL) {
                *terrain_clearance += sock->unknown_d8;
            }
            single_player_pullback += sock->unknown_cc;
            two_player_pullback += sock->unknown_d0;
            ++contributing_sockets;
        }

        if (contributing_sockets > 0) {
            f32 inverse_socket_count = 1.0f / (f32)contributing_sockets;
            NuVecScale(camera_position, camera_position, inverse_socket_count);
            NuVecScale(camera_target, &accumulated_target, inverse_socket_count);
            if (overlap_blend != NULL) {
                *overlap_blend *= inverse_socket_count;
            }
            if (position_seek != NULL) {
                *position_seek *= inverse_socket_count;
            }
            if (angle_seek != NULL) {
                *angle_seek *= inverse_socket_count;
            }
            if (terrain_clearance != NULL) {
                *terrain_clearance *= inverse_socket_count;
            }
            single_player_pullback *= inverse_socket_count;
            two_player_pullback *= inverse_socket_count;
            if (separation_scale != NULL) {
                *separation_scale = two_player_pullback;
            }
        }

        f32 pullback = player_count == 1 ? single_player_pullback : two_player_pullback;
        if (pullback != 0.0f) {
            NUVEC direction;
            f32 distance = NuVecDist(camera_target, camera_position, &direction);
            if (distance > 1.0f) {
                NuVecNorm(&direction, &direction);
                if (distance - pullback < 1.0f) {
                    pullback = distance - 1.0f;
                }
                NuVecAddScale(camera_position, camera_position, &direction, pullback);
            }
        }
        return 1;
    }

    void SockSysCameraWithOverlapBlend(void) {
    }

    void SockSysConfigureNuFPar(void) {
    }

    void SockSysPointAlongSpline(NUVEC *result, NUGSPLINE *spline, i32 segment, i32 next_segment, f32 ratio) {
        NUVEC *from = &spline->pts[segment];
        NUVEC *to = &spline->pts[next_segment];
        result->x = from->x + (to->x - from->x) * ratio;
        result->y = from->y + (to->y - from->y) * ratio;
        result->z = from->z + (to->z - from->z) * ratio;
    }

    void SockSysSetObjectVisibility(void) {
    }

    void SockSysTrackInSplineInfo(void) {
    }

    void SockSys_Configure(void *sock_sys, char *config, i32 param, void *buf, void *buf_end, void *gscn) {
        (void)sock_sys;
        (void)config;
        (void)param;
        (void)buf;
        (void)buf_end;
        (void)gscn;
    }

    void SockSys_GenerateData(SOCKSYS *sock_sys, VARIPTR *buf, VARIPTR *buf_end) {
        if (sock_sys == NULL) {
            return;
        }

        for (i32 sock_index = 0; sock_index < 0x40; ++sock_index) {
            SOCK *sock = &sock_sys->sock[sock_index];
            if (sock->valid == 0) {
                continue;
            }

            i32 point_count = sock->cam->length;
            usize rotations_size = (usize)point_count * sizeof(SOCKROT);
            if (buf->addr + rotations_size >= buf_end->addr) {
                SockDataError();
                return;
            }
            sock->cam_rotations = (SOCKROT *)buf->void_ptr;
            SockRailAngles(sock, sock->cam, sock->cam_rotations);
            buf->addr += rotations_size;

            if (buf->addr + rotations_size >= buf_end->addr) {
                SockDataError();
                return;
            }
            sock->mid_rotations = (SOCKROT *)buf->void_ptr;
            SockRailAngles(sock, sock->mid, sock->mid_rotations);
            buf->addr += rotations_size;

            usize segments_size = (usize)point_count * sizeof(SOCKSEGMENT);
            if (buf->addr + segments_size >= buf_end->addr) {
                SockDataError();
                return;
            }
            sock->segments = (SOCKSEGMENT *)buf->void_ptr;
            f32 distance_from_start = 0.0f;
            for (i32 segment_index = 0; segment_index < point_count; ++segment_index) {
                i32 next = (segment_index + 1) % point_count;
                SOCKSEGMENT *segment = &sock->segments[segment_index];

                SockMidpointAt(sock, segment_index, &segment->midpoint);
                SockMidpointAt(sock, next, &segment->next_midpoint);

                segment->min = sock->a->pts[segment_index];
                segment->max = segment->min;
                AdjustMinMaxBox(&sock->b->pts[segment_index], &segment->min, &segment->max);
                if ((sock->flags & 1) == 0) {
                    AdjustMinMaxBox(&sock->c->pts[segment_index], &segment->min, &segment->max);
                    AdjustMinMaxBox(&sock->d->pts[segment_index], &segment->min, &segment->max);
                }
                AdjustMinMaxBox(&sock->a->pts[next], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->b->pts[next], &segment->min, &segment->max);
                if ((sock->flags & 1) == 0) {
                    AdjustMinMaxBox(&sock->c->pts[next], &segment->min, &segment->max);
                    AdjustMinMaxBox(&sock->d->pts[next], &segment->min, &segment->max);

                    NuVecSurfaceNormal(&segment->planes[0], &sock->a->pts[segment_index], &sock->b->pts[segment_index],
                                       &sock->d->pts[segment_index]);
                    NuVecSurfaceNormal(&segment->planes[1], &sock->a->pts[next], &sock->b->pts[next],
                                       &sock->b->pts[segment_index]);
                    NuVecSurfaceNormal(&segment->planes[2], &sock->b->pts[next], &sock->c->pts[next],
                                       &sock->c->pts[segment_index]);
                    NuVecSurfaceNormal(&segment->planes[3], &sock->c->pts[next], &sock->d->pts[next],
                                       &sock->d->pts[segment_index]);
                    NuVecSurfaceNormal(&segment->planes[4], &sock->d->pts[next], &sock->a->pts[next],
                                       &sock->a->pts[segment_index]);
                    NuVecSurfaceNormal(&segment->planes[5], &sock->b->pts[next], &sock->a->pts[next],
                                       &sock->c->pts[next]);
                }

                segment->length = NuVecDist(&segment->midpoint, &segment->next_midpoint, NULL);
                segment->distance_from_start = distance_from_start;
                distance_from_start += segment->length;
            }
            sock->unknown_98 = sock->mid == NULL ? 0.0f : SplineLength(sock->mid, sock->unknown_33);
            if (sock->mid == NULL) {
                i32 segment_count = sock->length + (sock->unknown_33 != 0 ? 1 : 0);
                for (i32 segment = 0; segment < segment_count; ++segment) {
                    sock->unknown_98 += sock->segments[segment].length;
                }
            }
            buf->addr += segments_size;
        }

        buf->addr = ALIGN(buf->addr, 16);
        for (i32 sock_index = 0; sock_index < 0x40; ++sock_index) {
            SOCK *sock = &sock_sys->sock[sock_index];
            if (sock->valid == 0) {
                continue;
            }
            for (i32 other_index = 0; other_index < 0x40; ++other_index) {
                SOCK *other = &sock_sys->sock[other_index];
                if (sock_index == other_index || other->valid == 0) {
                    continue;
                }
                bool ignore_y = (sock->flags & 1) != 0 || (other->flags & 1) != 0;
                if (!BoundsOverlap(sock->min, sock->max, other->min, other->max, ignore_y)) {
                    SetSockBitValue(sock, other_index);
                    continue;
                }

                i32 segment_count = sock->length + (sock->unknown_33 != 0 ? 1 : 0);
                i32 other_segment_count = other->length + (other->unknown_33 != 0 ? 1 : 0);
                bool overlap = false;
                for (i32 segment = 0; segment < segment_count && !overlap; ++segment) {
                    for (i32 other_segment = 0; other_segment < other_segment_count; ++other_segment) {
                        if (BoundsOverlap(sock->segments[segment].min, sock->segments[segment].max,
                                          other->segments[other_segment].min, other->segments[other_segment].max,
                                          ignore_y)) {
                            overlap = true;
                            break;
                        }
                    }
                }
                if (!overlap) {
                    SetSockBitValue(sock, other_index);
                }
            }
        }
    }

} // extern "C"

void ForceAlongSock(GameObject_s *) {
}

void GetSockEdgeEnum(char *) {
}

void GoingForwardsAlongNarrowSock(GameObject_s *) {
}
