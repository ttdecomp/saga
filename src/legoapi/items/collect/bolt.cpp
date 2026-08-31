#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static f32 TerrWallDeflectYScale;

extern "C" void TerrainSetWallDeflectYScale(f32 scale) {
    TerrWallDeflectYScale = scale;
}

void FullDeflectSize(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void FullDeflectTest(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void FullReflectTest(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void FindIncomingBolt(GameObject_s *, i32, i32) {
}

void FullDeflectSmallY(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    // Move just far enough out of the surface to retain a small separation.
    const f32 normal_x = normal->x;
    const f32 normal_y = normal->y;
    const f32 normal_z = normal->z;
    const f32 movement_x = movement->x;
    const f32 movement_y = movement->y;
    const f32 movement_z = movement->z;
    const f32 deflection = -movement_y * normal_y - movement_x * normal_x - movement_z * normal_z + 0.0003f;
    result->x = movement_x + normal_x * deflection;
    result->y = movement_y + normal_y * deflection * TerrWallDeflectYScale;
    result->z = movement_z + normal_z * deflection;
}

void GuidedMissile_Kill(PART_s *, i32) {
}

void GuidedMissile_Move(PART_s *, float) {
}

void GuidedMissile_Deflect(PART_s *) {
}

void InitBolt_AddMomentumType_LSW(BOLT_s *, GameObject_s *, nuvec_s *) {
}

extern "C" {

    void FullDeflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        const f32 normal_x = normal->x;
        const f32 normal_y = normal->y;
        const f32 normal_z = normal->z;
        const f32 movement_x = movement->x;
        const f32 movement_y = movement->y;
        const f32 movement_z = movement->z;
        const f32 deflection = -movement_y * normal_y - movement_x * normal_x - movement_z * normal_z + 0.0003f;
        result->x = movement_x + normal_x * deflection;
        result->y = movement_y + normal_y * deflection;
        result->z = movement_z + normal_z * deflection;
    }

    void FullReflect(void) {
    }

} // extern "C"
