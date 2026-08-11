#include "legoapi/gameobject.h"

typedef void (*CharacterCallback)(GameObject_s *object);

i32 (*Fighting_WeaponInActionFn)(GameObject_s *object);
i32 (*Fighting_WeaponOutActionFn)(GameObject_s *object);

void SetMoveAndAnimateFunctions(u32, u32, u32, u32, i32, CharacterCallback, CharacterCallback, CharacterCallback) {}

#define EMPTY_MOVE(name) \
    void name(GameObject_s *) {}

EMPTY_MOVE(Move_ATAT)
EMPTY_MOVE(Move_BARMAN)
EMPTY_MOVE(Move_BEAST)
EMPTY_MOVE(Move_CANNON)
EMPTY_MOVE(Move_CHARACTER)
EMPTY_MOVE(Move_CRITTER)
EMPTY_MOVE(Move_DRAGBOMB)
EMPTY_MOVE(Move_DROIDEKA)
EMPTY_MOVE(Move_DROIDGENERIC)
EMPTY_MOVE(Move_GEONOSIAN)
EMPTY_MOVE(Move_HOVERDROID)
EMPTY_MOVE(Move_JAWA)
EMPTY_MOVE(Move_JEDI)
EMPTY_MOVE(Move_POD)
EMPTY_MOVE(Move_REPUBLICGUNSHIP)
EMPTY_MOVE(Move_SPEEDERBIKE)
EMPTY_MOVE(Move_SUPERBATTLEDROID)
EMPTY_MOVE(Move_VEHICLE)
EMPTY_MOVE(Move_WALKER)
EMPTY_MOVE(Move_WEIRDO)

#undef EMPTY_MOVE

void Move_CHARACTER() {}
void Move_DEFAULT() {}
void Move_DROIDGENERIC() {}
void Move_GEONOSIAN() {}
void Move_JAWA() {}
void Move_JEDI() {}
void Move_WEIRDO() {}

i32 ObjLandReady(GameObject_s *) {
    return 1;
}

i32 SuperWeirdo(GameObject_s *) {
    return 0;
}

void FastWeaponIn(GameObject_s *, i32) {}
void FastWeaponOut(GameObject_s *, i32) {}
void StartEndOfJump(GameObject_s *) {}
void StartHold(GameObject_s *) {}
void StartJump(GameObject_s *, i32) {}
i32 StartSlide(GameObject_s *, i32) { return 0; }
i32 Slam_Start(GameObject_s *, f32) { return 0; }
bool GameObjIsCableTied(GameObject_s *) { return false; }

void SeekVec(NUVEC *output, NUVEC *, NUVEC *target, f32) {
    if (output != NULL && target != NULL) {
        *output = *target;
    }
}

void TurnCodeCamSafe(GameObject_s *, NUMTX *) {}
