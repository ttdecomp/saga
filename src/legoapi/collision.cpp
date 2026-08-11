#include <math.h>

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

i32 LevBoltIgnorePlatIds[2] = {-1, -1};
i32 IgnoreWallSplines;
NUVEC ShadNorm = {0.0f, 1.0f, 0.0f};

static NUVEC impact_normal = {0.0f, 1.0f, 0.0f};

i32 GameRayCast(NUVEC *, NUVEC *, f32, i32) {
    return 0;
}

extern "C" void NewRayCastGetImpactNormal(NUVEC *normal) {
    if (normal != NULL) {
        *normal = impact_normal;
    }
}

extern "C" i8 NewRayCastGetImpactTerrainType() {
    return -1;
}

extern "C" f32 NewRayCastGetTOFI() {
    return 1.0f;
}

extern "C" void NewTerrHitInfo(void *) {
}

extern "C" i32 TerrainPlatId() {
    return -1;
}

void PlatOnOff(i32, i32) {
}

i32 LineIntersectSphere(NUVEC *, NUVEC *, NUVEC *, f32, NUVEC *) {
    return 0;
}

bool LineIntersectCircle(NUVEC *, NUVEC *, NUVEC *, f32) {
    return false;
}

bool SphereSphereOverlapScaleY(NUVEC *first, f32 first_radius, f32 first_y_radius, NUVEC *second, f32 second_radius,
                               f32 second_y_radius) {
    f32 x = first->x - second->x;
    f32 z = first->z - second->z;
    f32 y_scale = first_y_radius + second_y_radius;
    f32 y = y_scale == 0.0f ? 0.0f : (first->y - second->y) * (first_radius + second_radius) / y_scale;
    f32 radius = first_radius + second_radius;
    return x * x + y * y + z * z <= radius * radius;
}

void FindAnglesXY(NUVEC *direction, u16 *x_angle, u16 *y_angle) {
    f32 horizontal = sqrtf(direction->x * direction->x + direction->z * direction->z);
    if (x_angle != NULL) {
        *x_angle = (u16)(i32)(atan2f(-direction->y, horizontal) * (65536.0f / 6.283185307179586f));
    }
    if (y_angle != NULL) {
        *y_angle = (u16)(i32)(atan2f(direction->x, direction->z) * (65536.0f / 6.283185307179586f));
    }
}

void FindAnglesZX(NUVEC *direction, u16 *x_angle, u16 *z_angle) {
    f32 horizontal = sqrtf(direction->x * direction->x + direction->z * direction->z);
    if (x_angle != NULL) {
        *x_angle = (u16)(i32)(atan2f(-direction->z, direction->y) * (65536.0f / 6.283185307179586f));
    }
    if (z_angle != NULL) {
        *z_angle = (u16)(i32)(atan2f(direction->x, horizontal) * (65536.0f / 6.283185307179586f));
    }
}

void CalculateInterceptVector(NUVEC *origin, NUVEC *target, NUVEC *, f32, NUVEC *direction, NUVEC *impact_position) {
    direction->x = target->x - origin->x;
    direction->y = target->y - origin->y;
    direction->z = target->z - origin->z;
    f32 length = sqrtf(direction->x * direction->x + direction->y * direction->y + direction->z * direction->z);
    if (length != 0.0f) {
        direction->x /= length;
        direction->y /= length;
        direction->z /= length;
    }
    if (impact_position != NULL) {
        *impact_position = *target;
    }
}
