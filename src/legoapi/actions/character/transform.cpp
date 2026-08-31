#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern TerrainQuery_s *TerI;

void DeRotatePoint(nuvec_s *point) {
    TerrainQuery_s *query = TerI;

    const f32 sin_pitch = NuTrigTable[(static_cast<i32>(-query->movement_pitch) >> 1) & 0x7fff];
    const f32 cos_pitch = NuTrigTable[(static_cast<i32>(16384.0f - query->movement_pitch) >> 1) & 0x7fff];
    const f32 sin_yaw = NuTrigTable[(static_cast<i32>(-query->movement_yaw) >> 1) & 0x7fff];
    const f32 cos_yaw = NuTrigTable[(static_cast<i32>(16384.0f - query->movement_yaw) >> 1) & 0x7fff];

    const f32 relative_z = point->z - query->position.z;
    const f32 relative_x = point->x - query->position.x;

    const f32 rotated_x = relative_z * sin_yaw + relative_x * cos_yaw;
    const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;

    point->x = rotated_x;
    const f32 relative_y = point->y + query->collision_height_scale - query->position.y;
    point->z = relative_y * sin_pitch + rotated_z * cos_pitch;
    point->y = relative_y * cos_pitch - rotated_z * sin_pitch;
}

void RotateVec(NUVEC *source, NUVEC *destination) {
    TerrainQuery_s *query = TerI;
    const f32 pitch = query->movement_pitch;
    const f32 sin_pitch = NuTrigTable[(static_cast<i32>(pitch) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_pitch = NuTrigTable[(static_cast<i32>(pitch + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)];

    const f32 rotated_z = source->y * sin_pitch + source->z * cos_pitch;
    destination->y = source->y * cos_pitch - source->z * sin_pitch;

    const f32 yaw = query->movement_yaw;
    const f32 sin_yaw = NuTrigTable[(static_cast<i32>(yaw) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_yaw = NuTrigTable[(static_cast<i32>(yaw + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)];

    destination->z = rotated_z * cos_yaw - source->x * sin_yaw;
    destination->x = rotated_z * sin_yaw + source->x * cos_yaw;
}

void ApplyExtraRotation(GameObject_s *, numtx_s *) {
}

void Transform_MoveCode(WORLDINFO_s *, GameObject_s *) {
}

void Transform_DrawTarget(nuvec_s *, float, float) {
}

void DerotateMovementVector() {
    TerrainQuery_s *query = TerI;

    query->movement_yaw = static_cast<f32>(NuAtan2DA(query->movement.x, query->movement.z));
    const f32 horizontal_length =
        NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z);
    query->movement_pitch = static_cast<f32>(NuAtan2DA(-query->movement.y, horizontal_length));
    query->movement_length = NuFsqrt(query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                                     query->movement.z * query->movement.z);
}

void Transform_TargettedByObj(void *) {
}

void InterpolateRotationMatrix(numtx_s *, numtx_s *, numtx_s *, float) {
}

void QuatInterpolateRotationMatrix(numtx_s *, numtx_s *, numtx_s *, float) {
}
