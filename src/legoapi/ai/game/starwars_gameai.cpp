#include "decomp.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct AISYS_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

typedef void (*PREPARINGSPECIALMOVEFN)(AIPACKET_s *, APIOBJECT_s *, i32);
typedef i32 (*MIDSPECIALMOVEFN)(AISYS_s *, AIPACKET_s *, APIOBJECT_s *);
typedef void (*HUBCALLBACK)(WORLDINFO_s *);

extern "C" void InitFn_PreparingForSpecialMove(PREPARINGSPECIALMOVEFN);
extern "C" void InitFn_MidSpecialMove(MIDSPECIALMOVEFN);

extern i32 LEGO_AIPATHCNX_JUMP;
extern i32 LEGO_AIPATHCNX_DOUBLE_JUMP;
extern i32 LEGO_AIPATHCNX_HIGH_JUMP;
extern i32 LEGO_AIPATHCNX_R2D2GLIDE;
extern i32 LEGO_AIPATHCNX_FORGOODIES;
extern i32 LEGO_AIPATHCNX_FORBADDIES;
extern i32 LEGO_AIPATHCNX_BLOCKAGE;
extern i32 LEGO_AIPATHCNX_DONTTOGGLE;
extern i32 LEGO_AIPATHCNX_FULLTERRAIN;
extern i32 LEGO_AIPATHCNX_BIGJUMP;
extern i32 LEGO_AIPATHCNX_REQUIRESPERMISSION;
extern i32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
extern i32 LEGO_AIPATHCNX_JUMP_NOW;
extern i32 LEGO_AIPATHCNX_DONT_JUMP_NOW;
extern i32 mechAutoJumpFlags;
extern i32 mechAutoJumpCantReachFlags;

extern HUBCALLBACK Hub_InitAIFn;
extern HUBCALLBACK Hub_ResetAIFn;
extern HUBCALLBACK Hub_UpdateAIFn;

extern void LSW_Hub_InitAI(WORLDINFO_s *);
extern void LSW_Hub_ResetAI(WORLDINFO_s *);
extern void LSW_Hub_UpdateAI(WORLDINFO_s *);
extern "C" i16 id_SNAKE;

enum STARWARS_AI_CAPABILITY : u32 {
    STARWARS_AI_CAPABILITY_DEFAULT = 0x20180040,
    STARWARS_AI_CAPABILITY_OBJECT_STATE_SET = 0x100,
    STARWARS_AI_CAPABILITY_OBJECT_STATE_CLEAR = 0x80,
    STARWARS_AI_CAPABILITY_MODEL_ATTACHMENT = 0x1,
    STARWARS_AI_CAPABILITY_MODEL_FLAG_8_EXCLUSIVE = 0x1000,
    STARWARS_AI_CAPABILITY_MODEL_FLAG_8 = 0x3,
    STARWARS_AI_CAPABILITY_EXTENDED_JUMP = 0x23,
    STARWARS_AI_CAPABILITY_MODEL_FLAG_40000 = 0x10,
    STARWARS_AI_CAPABILITY_MODEL_FLAG_40 = 0x4004,
    STARWARS_AI_CAPABILITY_MODEL_FLAG_100000 = 0x8,
    STARWARS_AI_CAPABILITY_MODEL_FLAG_2000 = 0x10000,
    STARWARS_AI_CAPABILITY_SPECIAL_MOVEMENT = 0x8000,
    STARWARS_AI_CAPABILITY_PLAYER_SLOT = 0x20000,
    STARWARS_AI_CAPABILITY_SNAKE = 0x4000,
};

static void StarWars_PreparingForSpecialMove(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static i32 StarWars_MidSpecialMove(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
    return 0;
}

void StarWars_PrepareJump(AIPACKET_s *, APIOBJECT_s *, i32) {
}

void StarWars_GameAISysInit() {
    LEGO_AIPATHCNX_JUMP = 1;
    LEGO_AIPATHCNX_DOUBLE_JUMP = 2;
    LEGO_AIPATHCNX_HIGH_JUMP = 0x20;
    LEGO_AIPATHCNX_R2D2GLIDE = 4;
    LEGO_AIPATHCNX_FORGOODIES = 0x80;
    LEGO_AIPATHCNX_FORBADDIES = 0x100;
    LEGO_AIPATHCNX_BLOCKAGE = 0x40000;
    LEGO_AIPATHCNX_DONTTOGGLE = 0x80000;
    LEGO_AIPATHCNX_FULLTERRAIN = 0x100000;
    LEGO_AIPATHCNX_BIGJUMP = 0x1000;
    LEGO_AIPATHCNX_REQUIRESPERMISSION = 0x2000;
    LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x200;
    LEGO_AIPATHCNX_JUMP_NOW = 0x400;
    LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x800;

    InitFn_PreparingForSpecialMove(StarWars_PreparingForSpecialMove);
    InitFn_MidSpecialMove(StarWars_MidSpecialMove);

    Hub_InitAIFn = LSW_Hub_InitAI;
    Hub_ResetAIFn = LSW_Hub_ResetAI;
    Hub_UpdateAIFn = LSW_Hub_UpdateAI;
    mechAutoJumpFlags = 0xe00023;
    mechAutoJumpCantReachFlags = 0xffedfe7f;
}

void StarWars_PrepareR2D2Glide(AIPACKET_s *, APIOBJECT_s *, i32) {
}

void StarWars_ParseAIPathCnxFlag(char *) {
}

void StarWars_AutoSetAICapabilities(GameObject_s *object) {
    CHARACTERDATA *character = object->apiobj.character_data;
    object->ai.capabilities = character->ai_path_capabilities | STARWARS_AI_CAPABILITY_DEFAULT;
    if ((object->apiobj.field_0x1f4 & 1) != 0) {
        object->ai.capabilities =
            character->ai_path_capabilities | STARWARS_AI_CAPABILITY_DEFAULT | STARWARS_AI_CAPABILITY_OBJECT_STATE_SET;
    } else if ((object->apiobj.field_0x1f4 & 4) == 0) {
        object->ai.capabilities = character->ai_path_capabilities | STARWARS_AI_CAPABILITY_DEFAULT |
                                  STARWARS_AI_CAPABILITY_OBJECT_STATE_CLEAR;
    }

    if (object->apiobj.character_model->model_data_b[6] != NULL) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_ATTACHMENT;
    }

    const u32 model_flags = character->model_flags;
    if ((model_flags & 0x00200008) == 0x8) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_FLAG_8_EXCLUSIVE;
    }
    if ((model_flags & 0x8) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_FLAG_8;
    }

    const u32 game_flags = static_cast<GAMECHARACTERDATA *>(character->field11_0x24)->flags_090;
    if ((game_flags & 0x00400000) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_EXTENDED_JUMP;
    }
    if ((model_flags & 0x00040000) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_FLAG_40000;
    }
    if ((model_flags & 0x40) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_FLAG_40;
    }
    if ((model_flags & 0x00100000) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_FLAG_100000;
    }
    if ((model_flags & 0x2000) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_MODEL_FLAG_2000;
    }
    if ((model_flags & 0x88) != 0 || (game_flags & 0x40) != 0) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_SPECIAL_MOVEMENT;
    }
    if (object->apiobj.field_0x27c != -1) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_PLAYER_SLOT;
    }
    if (object->id == id_SNAKE) {
        object->ai.capabilities |= STARWARS_AI_CAPABILITY_SNAKE;
    }
}
