#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

struct ADAPTIVEDIFFICULTY_s {
    i32 difficulty;
    f32 multiplier;
    f32 elapsed;
};

extern i32 adaptivedifficulty[3];

void CheckPosAIArea(AIAREA_s *, nuvec_s *, float) {
}

void GetTakeOverPos(GameObject_s *, nuvec_s *) {
}

void HandleGroupLimit(i32) {
}

void ResetAdaptiveDifficulty() {
    ADAPTIVEDIFFICULTY_s *difficulty = (ADAPTIVEDIFFICULTY_s *)adaptivedifficulty;
    difficulty->multiplier = 0.5f;
    difficulty->elapsed = 0.0f;
    difficulty->difficulty = -4;
}

void NewCast(nuvec_s *, float, float) {
}

void LoopCode(GameObject_s *, i32, i32, GAMEPAD_s *, i32) {
}
