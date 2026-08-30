#include "legoapi/world/world_shared.h"

#include "decomp.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

// Forward declarations for local (static) game-object helper stubs.
struct GameObject_s;
struct nuvec_s;
struct WORLDINFO_s;
struct BOLT_s;
struct debinftype;

static __used__ void SetLayers_BOB(GameObject_s *obj);
static void SetLayers_MOSEISLEYCITIZEN(u32 *layers);

static u32 LayerBit(u8 layer) {
    return 1u << (layer & 31);
}

extern void SetGameObjectCharacterData(GameObject_s *obj);
extern void GetTopBot(GameObject_s *obj);
extern void GameObjectDimensions(GameObject_s *obj);
extern void GameObjectOrigin(GameObject_s *obj);
extern i32 GetDefaultIdle(GameObject_s *obj);
extern void ResetCharacterIdle(GameObject_s *obj, i32 mode, i32 idle);
extern void *Suit_GetDefault(i32 id);
extern void ResetLights(NUVEC *position, rtldata_s *data, void *set);
extern "C" void ResetAnimPacket(void *packet, i32 enabled);
extern void ResetPlayerPacket(PLAYERPACKET_s *packet, CHARACTERDATA_s *data);

extern "C" {
    extern i16 id_MOSEISLEYCITIZEN;
    extern i16 id_CANTINAALIEN;
    extern i16 id_CLOUDCITYCITIZEN;
    extern i16 id_GEONOSIAN;
    extern i16 id_BOB;
}

i32 addcreature_override_id_check;
f32 default_mover_extra = 0.05f;

void ClearGameObjects(APIOBJECTSYS_s *api_object_sys) {
    for (i32 i = 0; i < 64; i++) {
        Obj[i].KillTasks();
        Obj[i].ClearAddons();
        Obj[i].ClearMechObjectInterface();
    }
    memset(Obj, 0, sizeof(GameObject_s) * 64);
    APIObjectDestroyAll(api_object_sys);
    HIGHGAMEOBJECT = 0;
}

GameObject_s *AddGameObject(i32 id) {
    GameObject_s *object = reinterpret_cast<GameObject_s *>(APIObjectCreate(WORLD->api_object_sys));
    if (object == NULL) {
        return NULL;
    }

    const u8 object_index = object->apiobj.field_0x289;
    object->field_0x661 = 0xff;
    object->apiobj.field_0x27f = 0xff;
    object->apiobj.field_0x280 = 0xff;
    object->apiobj.field_0x281 = 0xff;
    object->field_0x1086 = 2;
    object->apiobj.field_0x1f8 |= 0x1001;
    object->field_0x1054 = 1;
    object->apiobj.field_0x1e4 = object_index < 32 ? 1u << object_index : 0;
    object->apiobj.field_0x1e8 = object_index < 32 ? 0 : 1u << (object_index - 32);
    object->apiobj.field_0xa8 = 1.0f;
    object->field_0x1004 = 1.0f;
    object->field_0x1020 = 2000000.0f;
    object->apiobj.field_0x218 = 2000000.0f;
    object->apiobj.field_0x21c = 2000000.0f;
    object->apiobj.field_0x220 = 2000000.0f;

    HIGHGAMEOBJECT = 0;
    for (i32 i = 0; i < 64; i++) {
        if ((Obj[i].apiobj.field_0x1f8 & 1) != 0) {
            HIGHGAMEOBJECT = i + 1;
        }
    }

    object->ai.owner = object;
    object->field_0x2a8 = 0;
    object->field_0x2ac = 0;
    object->apiobj.objptr = object;
    object->apiobj.ai = &object->ai;

    // The remainder of the original function attaches the Android touch
    // edge-stop/autofire addons.  The gameplay object itself is complete at
    // this point; those addon constructors are reconstructed separately.
    (void)id;
    return object;
}
i32 InitCreature(GameObject_s *obj, i32 id, i32 param) {
    addcreature_override_id_check = 0;
    if (id < 0 || id > 0x153 || apicharsys->playermodelids[id] == -1) {
        return 0;
    }

    CHARACTERDATA *character_data = &apicharsys->char_data[id];
    GAMECHARACTERDATA *game_character_data = static_cast<GAMECHARACTERDATA *>(character_data->field11_0x24);
    obj->apiobj.character_data = character_data;
    obj->field_0x1054 = game_character_data->layer_mask;
    obj->apiobj.field_0x1f4 = 0;
    obj->field_0x107c = -1;
    memset(obj->player_packet, 0, 0x798);
    obj->apiobj.field_0x27c = -1;

    NUVEC *start_position = Player_StartPos(obj);
    if (param == 0) {
        obj->apiobj.field_0x1f4 |= 2;
    } else {
        obj->apiobj.field_0x1f4 |= 0x4002;
    }
    if ((WORLD->current_level->flags & 0x40000) == 0) {
        obj->apiobj.field_0x1f4 |= 0x40;
    }

    obj->pad_gamepad = GamePad_Allocate();
    obj->pad_gamepad->unknown_24 |= 0x100;
    obj->hitpoints = game_character_data->hitpoints;
    obj->current_hp = game_character_data->hitpoints;
    ResetPlayerPacket(reinterpret_cast<PLAYERPACKET_s *>(obj->player_packet),
                      reinterpret_cast<CHARACTERDATA_s *>(character_data));

    obj->apiobj.field_0x1fc = v000.x;
    obj->apiobj.field_0x200 = v000.y;
    obj->apiobj.field_0x204 = v000.z;
    obj->field_0xe38 = 4;
    obj->field_0xe37 = game_character_data->field_0xf5;
    obj->id = static_cast<i16>(id);
    obj->apiobj.character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    obj->suit = Suit_GetDefault(id);
    obj->ai.field_0x134 = 0xff;
    SetGameObjectCharacterData(obj);

    obj->apiobj.start_position[0] = start_position->x;
    obj->apiobj.start_position[1] = start_position->y;
    obj->apiobj.start_position[2] = start_position->z;
    obj->apiobj.initial_position[0] = start_position->x;
    obj->apiobj.initial_position[1] = start_position->y;
    obj->apiobj.initial_position[2] = start_position->z;
    obj->apiobj.position = *start_position;
    obj->apiobj.pos_x = start_position->x;
    obj->apiobj.pos_y = start_position->y;
    obj->apiobj.pos_z = start_position->z;
    obj->field_0x1018 = 0.5f;

    GetTopBot(obj);
    GameObjectDimensions(obj);
    obj->apiobj.anim_packet.animation_index = 1;

    i32 reset_animation = 1;
    if (obj->apiobj.character_model != NULL) {
        void **animation_table = *reinterpret_cast<void ***>(reinterpret_cast<u8 *>(obj->apiobj.character_model) + 0xc);
        if (animation_table != NULL && animation_table[1] == NULL) {
            reset_animation = 0;
            for (i32 i = 0; i < 0xe9; i++) {
                if (animation_table[i] != NULL) {
                    obj->apiobj.anim_packet.animation_index = static_cast<u16>(i);
                    reset_animation = 1;
                    break;
                }
            }
        }
    }
    ResetAnimPacket(&obj->apiobj.anim_packet, reset_animation);
    ResetCharacterIdle(obj, 2, GetDefaultIdle(obj));
    ResetLights(&obj->apiobj.position, &obj->light_data, WORLD->rtl_set);

    obj->ai.mover_height = obj->apiobj.field_0x1dc + default_mover_extra;
    GameObjectOrigin(obj);
    obj->apiobj.previous_position[0] = obj->apiobj.position.x;
    obj->apiobj.previous_position[1] = obj->apiobj.position.y;
    obj->apiobj.previous_position[2] = obj->apiobj.position.z;
    obj->field_0x10c8 = obj->apiobj.position.x;
    obj->field_0x10cc = obj->apiobj.position.y;
    obj->field_0x10d0 = obj->apiobj.position.z;

    obj->field_0xf01 = static_cast<u8>((obj->field_0xf01 & ~8u) | (((game_character_data->flags_090 >> 17) & 1) << 3));
    if (id == id_MOSEISLEYCITIZEN) {
        SetLayers_MOSEISLEYCITIZEN(&obj->field_0x1054);
    } else if (id == id_CANTINAALIEN) {
        static const u8 head_layers[3] = {0, 31, 0};
        static const u8 body_layers[3] = {0, 15, 63};
        static const u8 leg_layers[3] = {0, 31, 0};
        obj->field_0x1054 = LayerBit(head_layers[qrand() / 0x5556]) | LayerBit(body_layers[qrand() / 0x5556]) |
                            LayerBit(leg_layers[qrand() / 0x5556]);
    } else if (id == id_CLOUDCITYCITIZEN) {
        static const u8 head_layers[3] = {1, 24, 46};
        static const u8 body_layers[3] = {0, 31, 0};
        static const u8 leg_layers[3] = {2, 16, 62};
        obj->field_0x1054 = LayerBit(head_layers[qrand() / 0x5556]) | LayerBit(body_layers[qrand() / 0x5556]) |
                            LayerBit(leg_layers[qrand() / 0x5556]);
        obj->field_0xf01 = static_cast<u8>((obj->field_0xf01 & ~8u) | (((obj->field_0x1054 >> 7) & 1) << 3));
    } else if (id == id_GEONOSIAN) {
        SetLayers_BOB(obj);
    } else if (id == id_BOB) {
        obj->field_0xefd = static_cast<u8>((obj->field_0xefd & ~2u) | ((qrand() <= 0x7fff) ? 2 : 0));
    }

    if ((game_character_data->flags_090 & 0x8000u) != 0) {
        obj->apiobj.field_0x1f4 |= 0x20000;
    } else {
        obj->apiobj.field_0x1f4 &= ~0x20000u;
    }
    return 1;
}

void InitGameObjectLights(void) {
}

// Local (static) game-object behaviour codes and per-object helpers. Stubbed
// as local `t` symbols matching res/libTTapp.so.

static __used__ void ShieldCode(GameObject_s *) {
}

static __used__ void TrenchMove(GameObject_s *) {
}

static __used__ void PooCode(GameObject_s *) {
}

static __used__ void ZapCode(GameObject_s *, i32, i32) {
}

static __used__ void PunchCode(GameObject_s *, i32, i32, i32, i32, f32) {
}

static __used__ void ShootCode(GameObject_s *, i32, i32, i32, i32, i32) {
}

static __used__ void SwipeCode(GameObject_s *, i32, i32) {
}

static __used__ void Punch_HitHold(GameObject_s *, GameObject_s *) {
}

static __used__ i32 Punch_GetDamage_LSW(GameObject_s *, GameObject_s *) {
    return 0;
}

static __used__ void Punch_HitExtraCode_LSW(GameObject_s *, nuvec_s *) {
}

static __used__ void TrenchKilledCallback(GameObject_s *) {
}

static __used__ void SetLayers_BOB(GameObject_s *) {
}

static void SetLayers_MOSEISLEYCITIZEN(u32 *) {
}

static __used__ void Tag_NoHiddenIcon(GameObject_s *) {
}

static __used__ void Tag_FindGameObject_TRANSFER(GameObject_s *) {
}

static __used__ void TightRope_Attach(GameObject_s *, WORLDINFO_s *) {
}

static __used__ void TightRope_MoveUpdate(GameObject_s *, i32) {
}

static __used__ i32 UsingExtraActions_Game(GameObject_s *) {
    return 0;
}

static __used__ void ZipUp_GetStartPoint(GameObject_s *, nuvec_s *) {
}

static __used__ void SurfaceInfo_ExtraReflect(GameObject_s *) {
}

static __used__ i32 Slam_GetDebris_Game(GameObject_s *, i32) {
    return 0;
}

static __used__ void PauseGame_ExtraCode() {
}

static __used__ void UpdateTotalPtls(debinftype *) {
}

static __used__ i32 SpecialObjectFilter(void *) {
    return 0;
}

static __used__ void KilledTrooperCannon(GameObject_s *) {
}

static __used__ void DeactivatedCode(GameObject_s *) {
}

static __used__ void BlockCode(GameObject_s *, int, int, int, int) {
}

static __used__ void DodgeCode(GameObject_s *, int, int) {
}

static __used__ void FireCode(GameObject_s *, int, int, float, int) {
}
