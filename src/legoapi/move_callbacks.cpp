#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct GameObject_s;

i32 (*CanMagnetClimbFn)(GameObject_s *object);
i32 (*CanPushObstaclesFn)(GameObject_s *object);
i32 (*CanSuperCarryFn)(GameObject_s *object);
i32 (*CanPushBlocksFn)(GameObject_s *object);
i32 (*CanGlideFn)(GameObject_s *object);
i32 (*UsingExtraActionsFn)(GameObject_s *object);
i32 (*CanStartHoldFn)(GameObject_s *object);
i32 (*IsWearingBackPackFn)(GameObject_s *object);
i32 (*FindSlamOrigin_UseCPosFn)(GameObject_s *object);
i32 (*Tag_NoHiddenIconFn)(GameObject_s *object);
void (*ExtraDieSfxFn)(GameObject_s *object);
void (*ExtraHurtSfxFn)(GameObject_s *object);
void (*BuckStartExtraFn)(GameObject_s *object);
void (*SurfaceInfo_ExtraReflectFn)(GameObject_s *object);
void (*BigJump_EndOfLandFn)(GameObject_s *object);
void (*Jump_EndOfLandContextFn)(GameObject_s *object);
i32 (*Jump_PreventJumpFn)(GameObject_s *object);
i32 (*Slam_GetDebrisFn)(GameObject_s *object, i32 debris);

void ReleasePush(GameObject_s *) {
}

void DoBuckStart(GameObject_s *) {
}
