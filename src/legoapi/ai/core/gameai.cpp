#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nuvec.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

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

f32 NewCast(nuvec_s *, f32, f32) {
    return 0.0f;
}

void LoopCode(GameObject_s *, i32, i32, GAMEPAD_s *, i32) {
}
