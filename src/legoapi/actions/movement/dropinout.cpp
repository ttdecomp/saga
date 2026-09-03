#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nutrig.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void StartDropIn(GameObject_s *) {
}

void StartDropOut(GameObject_s *) {
}

void DropInOutCode(GameObject_s *) {
}

f32 DropInOutScale(GameObject_s *object) {
    const u8 context = static_cast<u8>(object->character_context);
    if (context == CHARACTER_CONTEXT_DROP_IN) {
        const f32 angle = object->drop_transition_time / object->drop_transition_duration * 0.5f;
        return NuTrigTable[(static_cast<i32>(angle) >> 1) & 0x7fff];
    }
    if (context == CHARACTER_CONTEXT_DROP_OUT) {
        const f32 angle = object->drop_transition_time / object->drop_transition_duration * 0.5f + 0.5f;
        return NuTrigTable[(static_cast<i32>(angle) >> 1) & 0x7fff];
    }
    return 1.0f;
}

void FreePlay_DropInToPlayerPos(GameObject_s *) {
}

void DropOut(i32, i32, i32, i32) {
}
