#include "nu2api/nucore/common.h"

struct CUTSCENEPLAYERCLIP;

i32 (*CutScenePlayer_AcceptFn)(CUTSCENEPLAYERCLIP *clip);
void (*CutScene_OverrideConfigFileNameFn)(char *name, i32 unknown_1, i32 unknown_2);
