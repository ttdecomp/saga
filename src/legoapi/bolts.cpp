#include "legoapi/bolts.h"

#include <string.h>

#include "legoapi/cheat.h"
#include "legoapi/gamepads.h"
#include "legoapi/gizmo.h"
#include "legoapi/players.h"
#include "legoapi/qrand.h"
#include "legoapi/touchhacks.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numusic/sfx.h"

void GameAudio_PlaySfx(i32 sfx_id, NUVEC *position, i32 unknown_2, i32 unknown_3);
void GameAudio_PlaySfxById(i32 sfx_id, NUVEC *position, i32 unknown_2, i32 unknown_3);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 distance, i32 flags);
void FindAnglesZX(NUVEC *direction, u16 *x_angle, u16 *z_angle);
f32 FindReflectionNoPlatforms(NUVEC *position);
extern "C" void AddGameDebris(void *system, i32 debris_id, NUVEC *position);
extern "C" void AddVariableShotDebrisEffectTimed5(void *debris, NUVEC *position, void *unknown_2, NUVEC *velocity,
                                                  i32 speed, f32 time, void *unknown_6, void *unknown_7, i16 count,
                                                  u8 unknown_9);
extern "C" void NewRayCastGetImpactNormal(NUVEC *normal);
extern "C" void NewTerrHitInfo(void *hit_info);
extern "C" u32 ShadowInfo();
extern "C" i32 NewShadowOnPlatform();
struct PARTDEBSYS_s;
void AddPartDebris(PARTDEBSYS_s *system, i32 debris_id, NUVEC *position);
extern "C" NUFPAR *NuFParCreateMem(char *name, char *data, i32 size);
extern "C" i32 NuSpecialFind(NUGSCN *scene, BOLTSPECIAL *special, char *name);
extern "C" i32 NuSpecialExistsFn(BOLTSPECIAL *special);
extern "C" i32 NuSpecialDrawAt(BOLTSPECIAL *special, NUMTX *matrix);

i32 FindPartDebris(PARTDEBSYS_s *system, char *name);
i32 FindGameDebris(void *system, char *name);
i32 LevelObject_GetReflection(i32 object_id);
void PlatOnOff(i32 platform_id, i32 enabled);
i32 GameRayCast(NUVEC *origin, NUVEC *direction, f32 distance, i32 flags);
extern "C" i32 TerrainPlatId();
extern "C" i8 NewRayCastGetImpactTerrainType();
i32 LineIntersectSphere(NUVEC *line_start, NUVEC *line_end, NUVEC *center, f32 radius_squared, NUVEC *hit);
bool LineIntersectCircle(NUVEC *line_start, NUVEC *line_end, NUVEC *center, f32 radius_squared);
bool SphereSphereOverlapScaleY(NUVEC *first, f32 first_radius, f32 first_y_radius, NUVEC *second, f32 second_radius,
                               f32 second_y_radius);
void FindAnglesXY(NUVEC *direction, u16 *x_angle, u16 *y_angle);
extern "C" void NuMtxSetRotationXYVU0(NUMTX *matrix, i32 *angles);
void CalculateInterceptVector(NUVEC *origin, NUVEC *target, NUVEC *velocity, f32 speed, NUVEC *direction,
                              NUVEC *impact_position);
class NetMessage {
  public:
    i32 swap_endian;
    u8 *data;
    i32 read_position;
    i32 write_position;

    static u8 sm_poolMessageData[];
};

u8 NetMessage::sm_poolMessageData[512 * 0x4b4];

void Bolt_Init(void *bolt, NetMessage &message);
extern "C" void EdFileSwapEndianess16(u8 *data);
extern "C" void EdFileSwapEndianess32(u8 *data);
i32 Bolt_HitGameObject(BOLT *bolt, GameObject_s *object, NUVEC *hit_positions, NUVEC *bounds_min, NUVEC *bounds_max,
                       f32 radius, u8 *hit_bolts);
void Bolt_HitGameObjectRC(NetMessage &message);
i32 MatrixReflectionVU0_AXISY(NUMTX *matrix, f32 plane_y, f32 level_y, NUMTX *reflection);
i32 ObjHitObj_Flags(GameObject_s *object);
i32 CannotKill(GameObject_s *object);
i32 NewBlockAction(GameObject_s *object);
i32 ObjHitObj(GameObject_s *source, GameObject_s *target, i32 damage, u16 hit_flags, i32 unknown_0, i32 unknown_1);
void ObjHitShield(GameObject_s *source, GameObject_s *target, i32 damage, BOLT *bolt);
extern "C" f32 NewRayCastGetTOFI();
i32 GizmoSys_BoltHit(GIZMOSYS *system, void *world, BOLT *bolt, NUVEC *hit_positions, NUVEC *bounds_min,
                     NUVEC *bounds_max, f32 radius, u8 *hit_bolts);
i32 GizmoBlowUp_Hit(GameObject_s *source, NUVEC *hit_positions, i32 position_count, f32 radius, NUVEC *bounds_min,
                    NUVEC *bounds_max, BOLT *bolt, u32 damage, u8 *hit_bolts);
void KillPart(void *part, i32 mode);
i32 GetLevelExBlowupFlags();
void *GizmoBlowUp_FindFromPlatID(WORLDINFO *world, i32 platform_id);
i32 GizmoBlowupBlowup(void *blowup, i32 unknown_1, i32 unknown_2, i32 damage, GameObject_s *object);
void GameCam_HitJudder();
void NewRumble(NUPAD *pad, f32 strength, i32 frames);

extern i32 LevBoltIgnorePlatIds[2];
extern i32 IgnoreWallSplines;
extern NUVEC ShadNorm;
extern i32 MAXPARTS;
extern u8 *Part;
extern u8 TerSurface[];
extern i32 addbolt_nosfx;
extern i32 addbolt_newsfx;
extern i32 addbolt_noobjmom;
extern NUVEC addbolt_newpos;
extern i32 Paused;
extern i32 VehicleArea;
extern i32 BonusArea;
extern i32 i_temp_xrot;
extern u16 temp_xrot;
extern i16 temp_yrot;
extern f32 FRAMETIME;
extern f32 BOLT_SHOOTFLASHTIME;
extern i16 LEGOACT_SHOOTRIGHT;
extern i16 LEGOACT_SHOOTLEFT;
extern i16 LEGOACT_SHOOTBACK;
extern i16 LEGOACT_DEACTIVATED;
extern i32 LEGOCONTEXT_HOLD;
extern i32 LEGOCONTEXT_BLOCK;
extern f32 DEACTIVATEDTIME;
extern i32 netclient;
extern BOLT *objhitobj_bolt;
extern i32 (*BoltInitSfxFn)(GameObject_s *object);
extern i32 (*InitBolt_AddMomentumType)(BOLT *bolt, GameObject_s *object, NUVEC *momentum);
extern void (*Bolt_HitCustomFn)(BOLT *bolt, NUVEC *hit_positions);
extern i32 (*Bolt_HitPlatFn)(BOLT *bolt);
extern NUGSCN *area_scene;
extern NUGSCN *vehicle_scene;
extern NUGSCN *things_scene;

struct BoltCharacterInfo {
    u8 unknown_00[8];
    u32 flags;
    u8 unknown_0c[4];
};

extern BoltCharacterInfo *CInfo;

static void Bolt_Debris_Default(BOLT *bolt, NUVEC *position, i32 debris_id, NUVEC *normal, i32 moving);
static void Bolt_GetShootOrigin_Default(GameObject_s *object, NUVEC *origin);
static u16 Bolt_GetShootDirection_Default(GameObject_s *object, NUVEC *direction);

BOLTTYPE GlobalBoltType_Default = {
    "null",
    4.0f,
    2.0f,
    0.0f,
    0.125f,
    1.0f,
    0.1f,
    -1,
    -1,
    -1,
    -1,
    {-1, -1},
    {0, 0},
    -1,
    0,
    1,
    -1,
    0,
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    -1,
    -1,
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
};

static BOLTSYS BoltSys_Default = {
    &GlobalBoltType_Default,        1,    NULL, Bolt_Debris_Default, Bolt_GetShootOrigin_Default,
    Bolt_GetShootDirection_Default, NULL, NULL,
};

BOLTSYS *BoltSys = &BoltSys_Default;
f32 BOLT_OVERRIDE_PLAYERBOLTDURATION;
f32 BOLT_OVERRIDE_PLAYERBOLTSPEED;
i32 addbolt_nosfx;
i32 addbolt_newsfx = -1;
i32 addbolt_noobjmom;
NUVEC addbolt_newpos;
f32 BOLT_SHOOTFLASHTIME;
i32 netclient;
i32 (*BoltInitSfxFn)(GameObject_s *object);
i32 (*InitBolt_AddMomentumType)(BOLT *bolt, GameObject_s *object, NUVEC *momentum);
void (*Bolt_HitCustomFn)(BOLT *bolt, NUVEC *hit_positions);
i32 (*Bolt_HitPlatFn)(BOLT *bolt);
i32 i_bolt;
BOLT Bolt[32];

static WORLDINFO *BT_worldinfo;
static BOLTTYPE *BT_bolttype;
static NUGSCN *BT_scene;
static i32 BT_gdeb_moving_count;

static void Bolt_Debris_Default(BOLT *bolt, NUVEC *positions, i32 selection, NUVEC *, i32) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    bool use_all = (bolt->flags & 0x40000200) == 0;
    void *game_debris_system = *(void **)((u8 *)world + 0x134);

    if (selection == 0 || (selection == -1 && use_all)) {
        AddGameDebris(game_debris_system, bolt->debris_id, &positions[0]);
    }
    if (selection == 1 || selection == -1) {
        AddGameDebris(game_debris_system, bolt->debris_id, &positions[1]);
    }
    if (selection == 2 || (selection == -1 && use_all)) {
        AddGameDebris(game_debris_system, bolt->debris_id, &positions[2]);
    }

    if (bolt->part_debris_id != -1) {
        PARTDEBSYS_s *part_debris_system = *(PARTDEBSYS_s **)((u8 *)world + 0x138);
        AddPartDebris(part_debris_system, bolt->part_debris_id, &bolt->position);
    }
}

static void BT_nodeflect(NUFPAR *) {
    BT_bolttype->flags |= 0x100;
}

static void BT_converge(NUFPAR *) {
    BT_bolttype->flags |= 0x400;
}

static void BT_canonlyhitplayers(NUFPAR *) {
    BT_bolttype->flags |= 0x80;
}

static void BT_no_terrain(NUFPAR *) {
    BT_bolttype->flags |= 4;
}

static void BT_trooper_bolt(NUFPAR *) {
    BT_bolttype->flags |= 0x80000;
}

static void BT_no_collide(NUFPAR *) {
    BT_bolttype->flags |= 0x10000;
}

static void BT_single_debris(NUFPAR *) {
    BT_bolttype->flags |= 0x40000000;
}

static void Bolt_GetShootOrigin_Default(GameObject_s *object, NUVEC *origin) {
    *origin = object->origin;
}

static u16 Bolt_GetShootDirection_Default(GameObject_s *object, NUVEC *direction) {
    if (direction != NULL) {
        direction->x = NU_SIN_LUT(object->desired_heading);
        direction->y = 0.0f;
        direction->z = NU_COS_LUT(object->desired_heading);
    }
    return object->desired_heading;
}

static void BT_damage(NUFPAR *parser) {
    BOLTTYPE *type = BT_bolttype;
    type->damage = NuFParGetInt(parser);
}

static void BT_rand_angle(NUFPAR *parser) {
    BOLTTYPE *type = BT_bolttype;
    type->random_angle = NuFParGetFloat(parser) * (65536.0f / 360.0f);
}

static void BT_scale(NUFPAR *parser) {
    BOLTTYPE *type = BT_bolttype;
    type->scale = NuFParGetFloat(parser);
}

static void BT_radius(NUFPAR *parser) {
    BOLTTYPE *type = BT_bolttype;
    type->radius = NuFParGetFloat(parser);
}

static void BT_duration(NUFPAR *parser) {
    BOLTTYPE *type = BT_bolttype;
    type->duration = NuFParGetFloat(parser);
}

static void BT_speed(NUFPAR *parser) {
    BOLTTYPE *type = BT_bolttype;
    type->speed = NuFParGetFloat(parser);
}

static void BT_sfx_hit(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0) {
        BT_bolttype->hit_sfx_id = GetSfxId(parser->word_buf);
    }
}

static void BT_sfx_shoot(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0) {
        BT_bolttype->shoot_sfx_id = GetSfxId(parser->word_buf);
    }
}

static void BT_sceneconfig(NUFPAR *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }

    if (NuStrICmp(parser->word_buf, "level") == 0) {
        BT_scene = BT_worldinfo->current_gscn;
    } else if (NuStrICmp(parser->word_buf, "area") == 0) {
        BT_scene = area_scene;
    } else if (NuStrICmp(parser->word_buf, "vehicle") == 0) {
        BT_scene = vehicle_scene;
    } else {
        BT_scene = things_scene;
    }
}

static void BT_part_hit(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0) {
        PARTDEBSYS_s *system = *(PARTDEBSYS_s **)((u8 *)BT_worldinfo + 0x138);
        BT_bolttype->hit_part = FindPartDebris(system, parser->word_buf);
    }
}

static void BT_debris(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0) {
        void *system = *(void **)((u8 *)BT_worldinfo + 0x134);
        BT_bolttype->debris_id = FindGameDebris(system, parser->word_buf);
    }
}

static void BT_debris_moving(NUFPAR *parser) {
    if (BT_gdeb_moving_count >= 2 || NuFParGetWord(parser) == 0) {
        return;
    }

    void *system = *(void **)((u8 *)BT_worldinfo + 0x134);
    i32 index = BT_gdeb_moving_count;
    BT_bolttype->moving_debris_ids[index] = FindGameDebris(system, parser->word_buf);
    i32 speed = (i32)NuFParGetFloat(parser);
    BT_bolttype->moving_debris_speeds[index] = speed < 0 ? -speed : speed;
    BT_gdeb_moving_count++;
}

static void BT_shadow_obj(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        NuSpecialFind(BT_scene, &BT_bolttype->shadow_object, parser->word_buf);
    }
}

static void BT_ref_glow_obj(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        NuSpecialFind(BT_scene, &BT_bolttype->reflected_glow_object, parser->word_buf);
    }
}

static void BT_glow_obj(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL &&
        NuSpecialFind(BT_scene, &BT_bolttype->glow_object, parser->word_buf) != 0) {
        BT_bolttype->reflected_glow_object = BT_bolttype->glow_object;
    }
}

static void BT_ref_obj(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        NuSpecialFind(BT_scene, &BT_bolttype->reflected_object, parser->word_buf);
    }
}

static void BT_obj(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL &&
        NuSpecialFind(BT_scene, &BT_bolttype->object, parser->word_buf) != 0) {
        BT_bolttype->reflected_object = BT_bolttype->object;
    }
}

static void BT_name(NUFPAR *parser) {
    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buf) < 15) {
        NuStrCpy(BT_bolttype->name, parser->word_buf);
    }
}

static NUFPCOMJMP BoltType_ConfigKeywords[] = {
    {(char *)"name", BT_name},
    {(char *)"speed", BT_speed},
    {(char *)"duration", BT_duration},
    {(char *)"radius", BT_radius},
    {(char *)"scale", BT_scale},
    {(char *)"rand_angle", BT_rand_angle},
    {(char *)"scene", BT_sceneconfig},
    {(char *)"obj", BT_obj},
    {(char *)"ref_obj", BT_ref_obj},
    {(char *)"glow_obj", BT_glow_obj},
    {(char *)"ref_glow_obj", BT_ref_glow_obj},
    {(char *)"shadow_obj", BT_shadow_obj},
    {(char *)"debris", BT_debris},
    {(char *)"debris_moving", BT_debris_moving},
    {(char *)"part_hit", BT_part_hit},
    {(char *)"sfx", BT_sfx_shoot},
    {(char *)"sfx_shoot", BT_sfx_shoot},
    {(char *)"sfx_hit", BT_sfx_hit},
    {(char *)"damage", BT_damage},
    {(char *)"nodeflect", BT_nodeflect},
    {(char *)"converge", BT_converge},
    {(char *)"canonlyhitplayers", BT_canonlyhitplayers},
    {(char *)"no_terrain", BT_no_terrain},
    {(char *)"no_collide", BT_no_collide},
    {(char *)"trooper_bolt", BT_trooper_bolt},
    {(char *)"single_debris", BT_single_debris},
    {NULL, NULL},
};

void Bolts_Reset() {
    memset(Bolt, 0, sizeof(Bolt));
    i_bolt = 0;
    BOLT_OVERRIDE_PLAYERBOLTSPEED = 0.0f;
    BOLT_OVERRIDE_PLAYERBOLTDURATION = 0.0f;
}

void Bolts_Draw(WORLDINFO *world) {
    NUVEC scale = {0.0f, 0.0f, 0.0f};

    BOLT *bolt = Bolt;
    while (bolt != Bolt + 32) {
        if (bolt->active != 0) {
            BOLTTYPE *type = bolt->type;
            f32 draw_scale = type->draw_scale_fn == NULL ? bolt->draw_scale : type->draw_scale_fn(bolt);
            NUMTX matrix = bolt->draw_matrix;
            if (type->fade_time > bolt->elapsed) {
                scale.y = (type->flags & 0x02000000) == 0
                              ? bolt->elapsed / type->fade_time * draw_scale
                              : draw_scale;
                scale.x = scale.z = (bolt->flags & 0x200) == 0 ? draw_scale : scale.y;
                NuMtxPreScale(&matrix, &scale);
            } else if (bolt->draw_scale != 1.0f) {
                scale.x = draw_scale;
                scale.y = draw_scale;
                scale.z = draw_scale;
                NuMtxPreScale(&matrix, &scale);
            }

            NuMtxTranslate(&matrix, &bolt->position);
            i32 drew_object = 0;
            if (NuSpecialExistsFn(&type->object) != 0) {
                drew_object = NuSpecialDrawAt(&type->object, &matrix);
            }
            i32 drew_glow = 0;
            if (NuSpecialExistsFn(&type->glow_object) != 0) {
                drew_glow = NuSpecialDrawAt(&type->glow_object, &matrix);
            }

            NUMTX reflection;
            if (bolt->reflection_plane_y != 2000000.0f &&
                MatrixReflectionVU0_AXISY(&matrix, bolt->reflection_plane_y,
                                          *(f32 *)((u8 *)world->current_level + 0xcc), &reflection) != 0) {
                if (NuSpecialExistsFn(&type->reflected_object) != 0) {
                    NuSpecialDrawAt(&type->reflected_object, &reflection);
                }
                if (NuSpecialExistsFn(&type->reflected_glow_object) != 0) {
                    NuSpecialDrawAt(&type->reflected_glow_object, &reflection);
                }
            }

            if (bolt->shadow_plane_y != 2000000.0f) {
                scale.x = scale.z = scale.y;
                NuMtxSetScale(&matrix, &scale);
                i32 shadow_roll = bolt->shadow_roll;
                f32 sin_roll = NU_SIN_LUT(shadow_roll);
                f32 cos_roll = NU_COS_LUT(shadow_roll);
                f32 row0_y = matrix.m00 * sin_roll + matrix.m01 * cos_roll;
                matrix.m00 = matrix.m00 * cos_roll - matrix.m01 * sin_roll;
                f32 row1_y = matrix.m10 * sin_roll + matrix.m11 * cos_roll;
                matrix.m10 = matrix.m10 * cos_roll - matrix.m11 * sin_roll;
                f32 row2_y = matrix.m20 * sin_roll + matrix.m21 * cos_roll;
                matrix.m20 = matrix.m20 * cos_roll - matrix.m21 * sin_roll;

                i32 shadow_pitch = bolt->shadow_pitch;
                f32 sin_pitch = NU_SIN_LUT(shadow_pitch);
                f32 cos_pitch = NU_COS_LUT(shadow_pitch);
                matrix.m01 = row0_y * cos_pitch - matrix.m02 * sin_pitch;
                matrix.m02 = row0_y * sin_pitch + matrix.m02 * cos_pitch;
                matrix.m11 = row1_y * cos_pitch - matrix.m12 * sin_pitch;
                matrix.m12 = row1_y * sin_pitch + matrix.m12 * cos_pitch;
                matrix.m21 = row2_y * cos_pitch - matrix.m22 * sin_pitch;
                matrix.m22 = row2_y * sin_pitch + matrix.m22 * cos_pitch;
                matrix.m30 = bolt->position.x;
                matrix.m31 = bolt->shadow_plane_y + 0.005f;
                matrix.m32 = bolt->position.z;
                NuSpecialDrawAt(&type->shadow_object, &matrix);
            }

            if (drew_object != 0) {
                bolt->rendered = 1;
            } else if (drew_glow != 0) {
                bolt->rendered = 1;
            } else if (Paused == 0 && bolt->rendered != 0 && (bolt->flags & 0x20) == 0) {
                Bolt_End(bolt, 0);
            }
        }
        bolt++;
    }
}

BOLT *Bolt_Alloc() {
    if (Bolt[i_bolt].active != 0 && (Bolt[i_bolt].flags & 8) != 0) {
        i32 count = 0;
        do {
            i_bolt++;
            if (i_bolt == 32) {
                i_bolt = 0;
            }
            count++;
        } while (Bolt[i_bolt].active != 0 && (Bolt[i_bolt].flags & 8) != 0 && count != 32);
    }

    BOLT *bolt = &Bolt[i_bolt];
    i_bolt = (i_bolt + 1) % 32;
    return bolt;
}

BOLT *Bolt_Add(GameObject_s *source, NUVEC *position, NUMTX *matrix, i32 type_id, i32 hit_flags) {
    BOLTTYPE *type = BoltType_FindByID(type_id, WORLD);
    if (type == NULL || NuSpecialExistsFn(&type->object) == 0) {
        return NULL;
    }

    BOLT *bolt = Bolt_Alloc();
    if (bolt == NULL) {
        return NULL;
    }

    u8 *data = NULL;
    for (i32 i = 0; i < 512; i++) {
        u8 *candidate = NetMessage::sm_poolMessageData + i * 0x4b4;
        u32 *reference_count = (u32 *)(candidate + 0x4b0);
        if (*reference_count == 0) {
            *reference_count = 1;
            data = candidate;
            break;
        }
    }

    NetMessage message = {1, data, 0x20, 0x20};
    i16 source_index = source == NULL ? -1 : source - Obj;
    if (data != NULL) {
        memmove(data + message.write_position, &source_index, sizeof(source_index));
        EdFileSwapEndianess16(data + message.write_position);
        message.write_position += sizeof(source_index);

        memmove(data + message.write_position, position, sizeof(*position));
        for (i32 offset = 0; offset < (i32)sizeof(*position); offset += 4) {
            EdFileSwapEndianess32(data + message.write_position + offset);
        }
        message.write_position += sizeof(*position);

        memmove(data + message.write_position, matrix, sizeof(*matrix));
        for (i32 offset = 0; offset < (i32)sizeof(*matrix); offset += 4) {
            EdFileSwapEndianess32(data + message.write_position + offset);
        }
        message.write_position += sizeof(*matrix);

        memmove(data + message.write_position, &type_id, sizeof(type_id));
        EdFileSwapEndianess32(data + message.write_position);
        message.write_position += sizeof(type_id);
        memmove(data + message.write_position, &hit_flags, sizeof(hit_flags));
        EdFileSwapEndianess32(data + message.write_position);
        message.write_position += sizeof(hit_flags);
    }

    Bolt_Init(bolt, message);
    if (data != NULL) {
        u32 *reference_count = (u32 *)(data + 0x4b0);
        if (*reference_count < 2) {
            *reference_count = 0;
        } else {
            (*reference_count)--;
        }
    }
    return bolt;
}

static void BoltMessage_Read(NetMessage &message, void *destination, i32 size) {
    if (message.data == NULL) {
        return;
    }
    memmove(destination, message.data + message.read_position, size);
    if (message.swap_endian != 0) {
        if (size == 2) {
            EdFileSwapEndianess16((u8 *)destination);
        } else {
            for (i32 offset = 0; offset < size; offset += 4) {
                EdFileSwapEndianess32((u8 *)destination + offset);
            }
        }
    }
    message.read_position += size;
}

void Bolt_Init(void *bolt_pointer, NetMessage &message) {
    BOLT *bolt = (BOLT *)bolt_pointer;
    i16 source_index;
    NUVEC position;
    NUMTX launch_matrix;
    i32 type_id;
    i32 hit_flags;
    BoltMessage_Read(message, &source_index, sizeof(source_index));
    BoltMessage_Read(message, &position, sizeof(position));
    BoltMessage_Read(message, &launch_matrix, sizeof(launch_matrix));
    BoltMessage_Read(message, &type_id, sizeof(type_id));
    BoltMessage_Read(message, &hit_flags, sizeof(hit_flags));

    GameObject_s *source = source_index < 0 ? NULL : &Obj[source_index];
    BOLTTYPE *type = BoltType_FindByID(type_id, WORLD);
    NUVEC momentum = {0.0f, 0.0f, 0.0f};
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 no_sfx = addbolt_nosfx;
    i32 no_object_momentum = addbolt_noobjmom;
    addbolt_nosfx = 0;
    addbolt_noobjmom = 0;
    addbolt_newsfx = -1;

    if (type == NULL) {
        return;
    }
    if (NuSpecialExistsFn(&type->object) == 0) {
        return;
    }

    NUVEC hit_positions[3];
    hit_positions[1] = bolt->position;
    if (bolt->active != 0) {
        if ((bolt->flags & 0x200) == 0) {
            NUVEC offset = {0.0f, 0.0f, bolt->unknown_0d8 * 2.0f};
            NuVecMtxRotate(&offset, &offset, &bolt->launch_matrix);
            NuVecSub(&hit_positions[0], &bolt->position, &offset);
            NuVecAdd(&hit_positions[2], &bolt->position, &offset);
        }
        BoltSys->debris_fn(bolt, hit_positions, -1, NULL, 0);
    }

    bolt->type = type;
    bolt->type_id = type_id;
    bolt->source = source;
    bolt->speed = BOLT_OVERRIDE_PLAYERBOLTSPEED != 0.0f && BOLT_OVERRIDE_PLAYERBOLTDURATION != 0.0f
                      ? BOLT_OVERRIDE_PLAYERBOLTSPEED
                      : type->speed;
    bolt->duration = BOLT_OVERRIDE_PLAYERBOLTSPEED != 0.0f && BOLT_OVERRIDE_PLAYERBOLTDURATION != 0.0f
                         ? BOLT_OVERRIDE_PLAYERBOLTDURATION
                         : type->duration;
    bolt->acceleration = type->acceleration;
    bolt->radius = type->radius;
    bolt->draw_scale = type->scale;
    bolt->debris_id = type->moving_debris_ids[0];
    bolt->part_debris_id = type->hit_part;
    bolt->index = bolt - Bolt;

    if (hit_flags == 0) {
        hit_flags = ObjHitObj_Flags(source);
    }
    u32 flags = type->flags;
    bolt->hit_flags = hit_flags | 0x80;
    if (source != NULL) {
        if ((source->state_flags & 0x80) != 0) {
            if (source->player_index == 0) {
                flags |= 0x29;
            } else if (source->player_index == 1) {
                flags |= 0x2a;
            } else {
                flags |= 0x28;
            }
        }

        u32 character_flags = *(u32 *)((u8 *)source->character_data + 4);
        bool add_momentum = false;
        if (InitBolt_AddMomentumType != NULL && InitBolt_AddMomentumType(bolt, source, &momentum) != 0) {
            add_momentum = true;
        } else if ((character_flags & 0x04000000) != 0 ||
                   ((source->state_flags & 0x80) != 0 && (character_flags & 0x2000) != 0)) {
            add_momentum = true;
        }
        if (add_momentum && no_object_momentum == 0) {
            NUVEC *forward = (NUVEC *)((u8 *)source + 0xf3c);
            f32 forward_speed = forward->x * source->velocity.x + forward->z * source->velocity.z;
            if (forward_speed > 0.0f) {
                bolt->speed += forward_speed;
            }
        }
        if (source->player_packet.takeover_object != NULL) {
            flags |= 0x40000;
        }

        addbolt_newsfx = BoltInitSfxFn == NULL ? -1 : BoltInitSfxFn(source);
        if (addbolt_newsfx == -1) {
            u8 *runtime_data = *(u8 **)((u8 *)source->character_data + 0x24);
            addbolt_newsfx = *(i16 *)(runtime_data + 0xe4);
        }
    }
    if (addbolt_newsfx == -1) {
        addbolt_newsfx = type->shoot_sfx_id;
    }
    if (no_sfx == 0 && addbolt_newsfx != -1) {
        GameAudio_PlaySfxById(addbolt_newsfx, &position, 0, 0);
    }

    bolt->source = source;
    bolt->flags = flags;
    bolt->elapsed = 0.0f;
    bolt->active = 1;
    bolt->rendered = 0;
    bolt->platform_id = -1;
    bolt->terrain_type = -1;
    bolt->position = position;
    bolt->previous_position = position;
    addbolt_newpos = position;
    bolt->shadow_plane_y = 2000000.0f;
    bolt->reflection_plane_y = 2000000.0f;
    if (source != NULL) {
        if (source->floor_height != 2000000.0f) {
            bolt->shadow_plane_y = source->floor_height;
            bolt->shadow_pitch = *(u16 *)((u8 *)source + 0x105e);
            bolt->shadow_roll = *(u16 *)((u8 *)source + 0x1060);
        }
        if (source->reflection_plane_y != 2000000.0f && ((u8 *)source)[0x1087] == 2) {
            bolt->reflection_plane_y = source->reflection_plane_y;
        }
    }

    f32 base_radius = bolt->radius;
    f32 half_radius = (flags & 0x200) == 0 ? base_radius * 0.33333334f : base_radius;
    bolt->radius = base_radius * bolt->draw_scale;
    bolt->unknown_0d8 = bolt->draw_scale * half_radius;
    bolt->unknown_0dc = 0.2f;
    bolt->travel_distance = bolt->unknown_0d8;

    bolt->velocity.x = 0.0f;
    bolt->velocity.y = 0.0f;
    bolt->velocity.z = bolt->speed;
    bolt->launch_matrix = launch_matrix;
    NuVecMtxRotate(&bolt->velocity, &bolt->velocity, &bolt->launch_matrix);
    NuVecAdd(&bolt->velocity, &bolt->velocity, &momentum);
    NuVecNorm(&bolt->direction, &bolt->velocity);

    NuMtxSetRotationX(&bolt->draw_matrix, 0x4000);
    if ((flags & 0x1000) == 0) {
        NuMtxMulR(&bolt->draw_matrix, &bolt->draw_matrix, &bolt->launch_matrix);
    } else {
        u16 pitch;
        u16 yaw;
        FindAnglesXY(&bolt->direction, &pitch, &yaw);
        NuMtxSetRotationX(&bolt->draw_matrix, pitch + 0x4000);
        NuMtxRotateY(&bolt->draw_matrix, yaw);
        NuMtxSetRotationX(&bolt->launch_matrix, pitch);
        NuMtxRotateY(&bolt->launch_matrix, yaw);
    }

    if ((flags & 0x200) == 0) {
        NUVEC offset = {0.0f, 0.0f, bolt->unknown_0d8 * 2.0f};
        NuVecMtxRotate(&offset, &offset, &bolt->launch_matrix);
        NuVecSub(&hit_positions[0], &position, &offset);
        NuVecAdd(&hit_positions[2], &position, &offset);
    } else {
        hit_positions[0] = position;
        hit_positions[2] = position;
    }
    hit_positions[1] = position;

    f32 radius = bolt->radius;
    bolt->bounds_min.x = position.x - radius;
    bolt->bounds_min.y = position.y - radius;
    bolt->bounds_min.z = position.z - radius;
    bolt->bounds_max.x = position.x + radius;
    bolt->bounds_max.y = position.y + radius;
    bolt->bounds_max.z = position.z + radius;

    NUVEC step;
    NuVecScale(&step, &bolt->direction, bolt->speed * 0.2f);
    if (bolt->acceleration != 0.0f) {
        step.y += bolt->velocity.y * 0.2f + bolt->acceleration * 0.5f * 0.040000003f;
    }

    f32 source_radius = 0.01f;
    if (source != NULL) {
        u8 *runtime_data = *(u8 **)((u8 *)source->character_data + 0x24);
        source_radius = (((u8 *)source->character_data)[5] & 0x20) == 0 && (runtime_data[0x90] & 0x80) == 0
                            ? source->collision_radius * 3.0f
                            : source->collision_radius;
    }
    NUVEC source_offset;
    NuVecScale(&source_offset, &bolt->direction, source_radius);
    NUVEC ray_origin;
    NuVecSub(&ray_origin, &position, &source_offset);
    NuVecAdd(&step, &step, &source_offset);

    if ((flags & 4) == 0 && Bolt_RayCast(bolt, &ray_origin, &step, bolt->duration)) {
        f32 start_time = source_radius / (bolt->speed * bolt->duration) * bolt->duration;
        bolt->duration = bolt->unknown_0dc + start_time;
        bolt->duration = NewRayCastGetTOFI() * bolt->duration - start_time;
        if (bolt->duration <= 0.0f) {
            NuVecAdd(&ray_origin, &ray_origin, &step);
            AddGameDebris(*(void **)((u8 *)world + 0x134), bolt->debris_id, &ray_origin);
            AddGameDebris(*(void **)((u8 *)world + 0x134), bolt->debris_id, &bolt->position);
            Bolt_End(bolt, 1);
        }
    }

    if ((bolt->hit_flags & 1) == 0 &&
        Bolt_HitGameObjects(bolt, hit_positions, &bolt->bounds_min, &bolt->bounds_max, bolt->unknown_0d8, NULL) == 0) {
        if ((flags & 0x13) == 0 &&
            (source == NULL || ((((u8 *)source)[0xefb] & 0x10) == 0 && ((u8 *)source)[0xf0c] != 5))) {
            Bolt_PlayHitSfx(bolt);
        } else if ((bolt->platform_id == -1 || Bolt_HitPlat(bolt, NULL, world) == 0) &&
                   GizmoSys_BoltHit(*(GIZMOSYS **)((u8 *)world + 0x2ac8), world, bolt, hit_positions, &bolt->bounds_min,
                                    &bolt->bounds_max, bolt->unknown_0d8, NULL) == 0) {
            i32 position_count = (flags & 0x200) == 0 ? 3 : 1;
            NUVEC *positions = (flags & 0x200) == 0 ? hit_positions : &bolt->position;
            if (GizmoBlowUp_Hit(source, positions, position_count, bolt->unknown_0d8, &bolt->bounds_min,
                                &bolt->bounds_max, bolt, 1, NULL) == 0) {
                i32 part_mode = Bolt_HitPartMode(bolt);
                if (Bolt_HitParts(bolt, hit_positions, &bolt->bounds_min, &bolt->bounds_max, bolt->unknown_0d8,
                                  part_mode) == 0) {
                    Bolt_PlayHitSfx(bolt);
                }
            } else {
                BoltSys->debris_fn(bolt, hit_positions, -1, NULL, 0);
                if (source != NULL) {
                    NewRumble(*(NUPAD **)source->player_packet.game_pad, 0.7f, 0);
                }
                Bolt_End(bolt, 1);
                Bolt_PlayHitSfx(bolt);
            }
        }
    }

    if (BoltSys->post_hit_fn != NULL && source != NULL && (source->state_flags & 0x80) != 0) {
        ((void (*)(GameObject_s *, NUVEC *))BoltSys->post_hit_fn)(source, &bolt->position);
    }
    if (type->init_fn != NULL) {
        type->init_fn(bolt);
    }
}

void Bolt_Shoot(GameObject_s *object, i32 type_id, i32 joint_mode) {
    BOLTTYPE *type = BoltType_FindByID(type_id, WORLD);
    u32 flags = type->flags;
    f32 speed = type->speed;
    f32 target_distance = type->duration;

    if ((object->state_flags & 0x80) != 0) {
        if (object->player_index == 0) {
            flags |= 0x21;
        } else if (object->player_index == 1) {
            flags |= 0x22;
        } else {
            flags |= 0x20;
        }
    }
    if ((object->movement_flags & 8) != 0) {
        flags |= 0x40;
    }

    NUVEC target_position = *(NUVEC *)((u8 *)object + 0xc70);
    NUVEC shoot_origin;
    NUVEC shoot_direction;
    BoltSys->shoot_origin_fn(object, &shoot_origin);
    u16 shoot_heading = BoltSys->shoot_direction_fn(object, &shoot_direction);

    i16 action = object->player_packet.action;
    if (object->player_packet.character_state != -1 && action != -1) {
        if (action == LEGOACT_SHOOTRIGHT) {
            shoot_heading += 0x4000;
        } else if (action == LEGOACT_SHOOTLEFT) {
            shoot_heading += 0xc000;
        } else if (action == LEGOACT_SHOOTBACK) {
            shoot_heading += 0x8000;
        }
    }

    u8 runtime_flags_1 = ((u8 *)&object->player_packet.runtime_flags_0)[1];
    if ((runtime_flags_1 & 8) == 0) {
        target_distance = 0.0f;
    } else if ((((u8 *)object)[0xef9] & 0x80) != 0 || object->unknown_ecc != NULL || object->player_index == -1 ||
               (*(NUVEC *)((u8 *)object + 0xc80)).x == 0.0f && (*(NUVEC *)((u8 *)object + 0xc80)).y == 0.0f &&
                   (*(NUVEC *)((u8 *)object + 0xc80)).z == 0.0f) {
        target_distance = NuVecDist(&shoot_origin, &target_position, NULL);
    } else {
        target_distance *= speed;
        NUVEC direction;
        NuVecSub(&direction, &target_position, &shoot_origin);
        NuVecNorm(&direction, &direction);
        NuVecScale(&direction, &direction, target_distance);
        NuVecAdd(&target_position, &shoot_origin, &direction);
    }

    NUVEC *target = NULL;
    NUVEC *target_velocity = NULL;
    NUVEC adjusted_target;
    NUVEC ai_velocity;
    if ((((u8 *)object)[0xef9] & 0x80) != 0) {
        target = (NUVEC *)((u8 *)object + 0xe58);
        if ((object->portal_flags & 3) == 2) {
            NuVecSub(&ai_velocity, target, (NUVEC *)((u8 *)object + 0xe64));
            NuVecScale(&ai_velocity, &ai_velocity, 1.0f / FRAMETIME);
            target_velocity = &ai_velocity;
        }
    } else if (object->unknown_ecc != NULL) {
        GameObject_s *previous_target = (GameObject_s *)object->unknown_ecc;
        target = &previous_target->origin;
        target_velocity = &previous_target->velocity;
        if ((((u8 *)object->character_data)[5] & 0x20) == 0 && (object->object_flags & 1) != 0) {
            adjusted_target = previous_target->origin;
            adjusted_target.y += Bolt_ObjTargetPosYAdjust(previous_target);
            target = &adjusted_target;
        }
    } else if ((runtime_flags_1 & 8) != 0) {
        target = &target_position;
    }

    i16 shoot_sfx[8];
    NUVEC shoot_sfx_positions[8];
    u8 new_bolts[5];
    i32 shoot_sfx_count = 0;
    i32 bolt_count = 0;
    bool found_joint = false;

    for (i32 slot = 0; slot < 5; slot++) {
        if ((slot == 1 && joint_mode == 1) || (slot == 0 && joint_mode == 2)) {
            continue;
        }
        if (BoltSys->shoot_joint_fn != NULL && BoltSys->shoot_joint_fn(object, slot) == 0) {
            continue;
        }

        NUVEC *origin = NULL;
        if (slot == 4) {
            if (found_joint) {
                break;
            }
            origin = &shoot_origin;
        } else if (object->visible != 0) {
            u8 *runtime_data = *(u8 **)((u8 *)object->character_data + 0x24);
            i32 joint = (i8)runtime_data[0xfc + slot];
            if (joint == -1 || *(void **)((u8 *)object->game_character_data + 0x14 + joint * 4) == NULL) {
                joint = (i8)runtime_data[0xf8 + slot];
            }
            if (joint != -1 && *(void **)((u8 *)object->game_character_data + 0x14 + joint * 4) != NULL) {
                found_joint = true;
                origin = (NUVEC *)&object->player_packet.joint_matrices[joint].m30;
            }
        }
        if (origin == NULL) {
            continue;
        }

        NUMTX matrix;
        if ((flags & 0x600000) == 0) {
            if (target != NULL) {
                NUVEC aim_direction;
                if (target_velocity == NULL || (object->portal_flags & 2) == 0) {
                    NuVecSub(&aim_direction, target, origin);
                } else {
                    CalculateInterceptVector(origin, target, target_velocity, speed, &aim_direction, NULL);
                }
                FindAnglesXY(&aim_direction, NULL, NULL);
                i32 angles[2] = {i_temp_xrot, (u16)temp_yrot};
                NuMtxSetRotationXYVU0(&matrix, angles);
            } else if (((u8 *)object)[0x1086] == 4) {
                FindAnglesXY(&shoot_direction, NULL, NULL);
                i32 angles[2] = {i_temp_xrot, (u16)temp_yrot};
                NuMtxSetRotationXYVU0(&matrix, angles);
            } else {
                NuMtxSetRotationY(&matrix, shoot_heading);
            }
        } else {
            u16 pitch;
            if ((flags & 0x80000000) != 0 && (object->state_flags & 0x80) == 0 && object->unknown_ecc != NULL) {
                GameObject_s *previous_target = (GameObject_s *)object->unknown_ecc;
                f32 distance = NuVecXZDist(&previous_target->origin, &object->origin, NULL);
                if (distance > 2.0f) {
                    distance = 2.0f;
                }
                f32 angle = (flags & 0x200000) == 0 ? -20.0f : -10.0f;
                pitch = (i32)(angle * distance * 0.5f * (65536.0f / 360.0f));
            } else {
                pitch = (flags & 0x200000) == 0 ? 0xf1c8 : 0xf8e4;
            }

            u16 yaw = shoot_heading;
            if (target != NULL) {
                yaw = NuAtan2D(target->x - origin->x, target->z - origin->z);
                shoot_heading = yaw;
            }
            i32 angles[2] = {pitch, yaw};
            NuMtxSetRotationXYVU0(&matrix, angles);
        }

        addbolt_nosfx = 1;
        BOLT *bolt = Bolt_Add(object, origin, &matrix, type_id, 0);
        if (bolt != NULL && bolt_count < 5) {
            new_bolts[bolt_count++] = bolt->index;
        }
        if (addbolt_newsfx != -1 && shoot_sfx_count < 8) {
            shoot_sfx[shoot_sfx_count] = addbolt_newsfx;
            shoot_sfx_positions[shoot_sfx_count] = addbolt_newpos;
            shoot_sfx_count++;
        }
        if ((type->flags & 0x20000000) != 0) {
            *(f32 *)((u8 *)object + 0xd50) = BOLT_SHOOTFLASHTIME;
        }
    }

    ((u8 *)&object->player_packet.runtime_flags_0)[2] |= 4;
    if (shoot_sfx_count != 0) {
        i32 sound = 0;
        if (shoot_sfx_count != 1) {
            sound = qrand() / (0xffff / shoot_sfx_count + 1);
        }
        GameAudio_PlaySfxById(shoot_sfx[sound], &shoot_sfx_positions[sound], 0, 0);
    }

    if ((runtime_flags_1 & 8) != 0 && bolt_count > 1) {
        for (i32 i = 0; i < bolt_count; i++) {
            BOLT *bolt = &Bolt[new_bolts[i]];
            f32 maximum_distance = bolt->speed * bolt->duration;
            f32 duration = maximum_distance == 0.0f || target_distance == 0.0f
                               ? 0.0f
                               : target_distance / maximum_distance * type->duration;
            if (duration < bolt->duration) {
                bolt->duration = duration;
            }
        }
    }
}

void Bolt_Free(BOLT *bolt) {
    bolt->active = 0;
}

void Bolt_Reflect(NUVEC *normal, NUVEC *direction, NUVEC *reflection) {
    f32 scale = 2.0f * (normal->x * direction->x + normal->y * direction->y + normal->z * direction->z);
    reflection->x = direction->x - normal->x * scale;
    reflection->y = direction->y - normal->y * scale;
    reflection->z = direction->z - normal->z * scale;
}

void Bolt_PlayHitSfx(BOLT *bolt) {
    if (bolt->type->hit_sfx_id != -1) {
        GameAudio_PlaySfxById(bolt->type->hit_sfx_id, &bolt->position, 0, 0);
    } else {
        GameAudio_PlaySfx(0x28, &bolt->position, 0, 0);
    }
}

void Bolt_End(BOLT *bolt, i32 run_callback) {
    bolt->active = 0;
    if (run_callback != 0 && bolt->type != NULL && bolt->type->end_fn != NULL) {
        bolt->type->end_fn(bolt);
    }
}

BOLTTYPE *BoltType_FindByID(i32 id, WORLDINFO *world) {
    if (id >= 0 && id < BoltSys->type_count) {
        return &BoltSys->types[id];
    }
    if (world != NULL && id >= BoltSys->type_count && id <= BoltSys->type_count + 7) {
        BOLTTYPE *world_types = (BOLTTYPE *)((u8 *)world + 0x4b18);
        return &world_types[id - BoltSys->type_count];
    }
    return NULL;
}

void BoltTypes_Reset(WORLDINFO *world) {
    memset((u8 *)world + 0x4b18, 0, sizeof(BOLTTYPE) * 8);
}

f32 Bolt_ObjTargetPosYAdjust(GameObject_s *object) {
    f32 half_height = object->collision_half_height;
    return qrand() * ((half_height * 2.0f) / 65535.0f) - half_height;
}

i8 Bolt_HitPartMode(BOLT *bolt) {
    if (bolt->source == NULL) {
        return 3;
    }
    if (bolt->source->player_index == 0) {
        return 4;
    }
    return bolt->source->player_index == 1 ? 5 : 3;
}

void BoltSys_Init(BOLTSYS *bolt_system) {
    for (i32 i = 0; i < bolt_system->type_count; i++) {
        BOLTTYPE *type = &bolt_system->types[i];
        type->shoot_sfx_id = -1;
        if (type->shoot_sfx_name != NULL) {
            type->shoot_sfx_id = GetSfxId(type->shoot_sfx_name);
        }
        type->hit_sfx_id = -1;
        if (type->hit_sfx_name != NULL) {
            type->hit_sfx_id = GetSfxId(type->hit_sfx_name);
        }
    }

    BoltSys = bolt_system;
    if (bolt_system->debris_fn == NULL) {
        bolt_system->debris_fn = Bolt_Debris_Default;
    }
    if (bolt_system->shoot_origin_fn == NULL) {
        bolt_system->shoot_origin_fn = Bolt_GetShootOrigin_Default;
    }
    if (bolt_system->shoot_direction_fn == NULL) {
        bolt_system->shoot_direction_fn = Bolt_GetShootDirection_Default;
    }
}

bool Bolt_RayCast(BOLT *bolt, NUVEC *origin, NUVEC *direction, f32 distance) {
    NUVEC ray_end;
    NuVecAdd(&ray_end, origin, direction);

    i32 source_platform = -1;
    if (bolt->source != NULL && bolt->source->ignored_platform_id != -1) {
        source_platform = bolt->source->ignored_platform_id;
        PlatOnOff(source_platform, 0);
    }
    if (LevBoltIgnorePlatIds[0] != -1) {
        PlatOnOff(LevBoltIgnorePlatIds[0], 0);
    }
    if (LevBoltIgnorePlatIds[1] != -1) {
        PlatOnOff(LevBoltIgnorePlatIds[1], 0);
    }

    i32 hit = GameRayCast(origin, direction, distance, 0);
    if (hit != 0) {
        bolt->platform_id = TerrainPlatId();
        bolt->terrain_type = NewRayCastGetImpactTerrainType();
        NuVecAdd(&bolt->impact_position, origin, direction);
        bolt->impact_normal = ShadNorm;
    } else {
        bolt->platform_id = -1;
        bolt->terrain_type = -1;
    }

    if (source_platform != -1) {
        PlatOnOff(source_platform, 1);
    }
    if (LevBoltIgnorePlatIds[0] != -1) {
        PlatOnOff(LevBoltIgnorePlatIds[0], 1);
    }
    if (LevBoltIgnorePlatIds[1] != -1) {
        PlatOnOff(LevBoltIgnorePlatIds[1], 1);
    }

    bolt->ray_end = ray_end;
    IgnoreWallSplines = 0;
    return hit != 0;
}

BOLT *FindIncomingBolt(GameObject_s *object, i32 filter_player_sources, i32 set_near_flag) {
    BOLT *nearest = NULL;
    f32 nearest_distance = 100000000.0f;
    f32 radius = object->bounds_radius;
    if (radius < object->bounds_half_height) {
        radius = object->bounds_half_height;
    }

    for (i32 i = 0; i < 32; i++) {
        BOLT *bolt = &Bolt[i];
        if (bolt->active == 0) {
            continue;
        }
        if (filter_player_sources != 0 && bolt->source != NULL && (bolt->source->state_flags & 0x1001) == 0x1001 &&
            bolt->source->death_state == 0 && bolt->source->player_index != -1) {
            continue;
        }

        f32 distance_scale = TouchHacks::TouchControlsActive == 0 ? 0.5f : 1.5f;
        f32 maximum_distance = distance_scale * bolt->travel_distance;
        f32 distance_squared = NuVecDistSqr(&bolt->position, &object->origin, NULL);
        if (distance_squared < maximum_distance * maximum_distance &&
            LineIntersectSphere(&bolt->position, &bolt->direction, &object->origin, radius * radius, NULL) != 0 &&
            distance_squared < nearest_distance) {
            nearest = bolt;
            nearest_distance = distance_squared;
        }
    }

    if (nearest != NULL && set_near_flag != 0) {
        f32 near_radius = object->bounds_radius * 0.75f;
        if (LineIntersectCircle(&nearest->position, &nearest->direction, &object->origin, near_radius * near_radius)) {
            *((u8 *)object + 0xe21) |= 0x20;
        }
    }
    return nearest;
}

BOLT *Bolt_Find(i32 type_id, NUVEC *position, GameObject_s *source) {
    BOLT *nearest = NULL;
    f32 nearest_distance = 1000000.0f;

    for (i32 i = 0; i < 32; i++) {
        BOLT *bolt = &Bolt[i];
        if (bolt->active == 0 || (type_id != -1 && bolt->type_id != type_id) ||
            (source != NULL && bolt->source != source)) {
            continue;
        }
        if (position == NULL) {
            return bolt;
        }

        f32 distance = NuVecDistSqr(&bolt->position, position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = bolt;
        }
    }
    return nearest;
}

i32 BoltType_FindIDByName(char *name, WORLDINFO *world) {
    if (NuStrLen(name) == 0) {
        return -1;
    }

    BOLTTYPE *world_types = (BOLTTYPE *)((u8 *)world + 0x4b18);
    for (i32 i = 7; i >= 0; i--) {
        if (NuStrICmp(name, world_types[i].name) == 0) {
            return BoltSys->type_count + i;
        }
    }
    for (i32 i = BoltSys->type_count + 7; i >= 0; i--) {
        if (NuStrICmp(name, BoltSys->types[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

static void BoltType_CopySpecial(BOLTSPECIAL *destination, WORLDINFO *world, i32 object_id) {
    u8 *level_objects = *(u8 **)((u8 *)world + 0x2ac0);
    BOLTSPECIAL *source = (BOLTSPECIAL *)(level_objects + object_id * 0x10);
    *destination = *source;
}

void BoltTypes_Init(WORLDINFO *world) {
    BOLTTYPE *type = BoltSys->types;
    for (i32 i = 0; i < BoltSys->type_count; i++, type++) {
        type->object.object = NULL;
        type->object.matrix = NULL;
        type->object.instance = NULL;
        type->glow_object.object = NULL;
        type->glow_object.matrix = NULL;
        type->glow_object.instance = NULL;
        type->reflected_object.object = NULL;
        type->reflected_object.matrix = NULL;
        type->reflected_object.instance = NULL;
        type->reflected_glow_object.object = NULL;
        type->reflected_glow_object.matrix = NULL;
        type->reflected_glow_object.instance = NULL;

        if (type->object_id != -1) {
            BoltType_CopySpecial(&type->object, world, type->object_id);
            if (type->glow_object_id != -1) {
                BoltType_CopySpecial(&type->glow_object, world, type->glow_object_id);
            }

            i32 reflected_id = LevelObject_GetReflection(type->object_id);
            if (reflected_id != -1) {
                BoltType_CopySpecial(&type->reflected_object, world, reflected_id);
                if (type->glow_object_id != -1) {
                    reflected_id = LevelObject_GetReflection(type->glow_object_id);
                    if (reflected_id != -1) {
                        BoltType_CopySpecial(&type->reflected_glow_object, world, reflected_id);
                    }
                }
            } else {
                type->reflected_object = type->object;
                type->reflected_glow_object = type->glow_object;
            }
        }

        if (type->shadow_object_id != -1) {
            BoltType_CopySpecial(&type->shadow_object, world, type->shadow_object_id);
        } else {
            memset(&type->shadow_object, 0, sizeof(type->shadow_object));
        }
    }
}

void BoltTypes_Configure(WORLDINFO *world, char *data) {
    BoltTypes_Reset(world);
    NUFPAR *parser = NuFParCreateMem((char *)"bolttypes", data, 0xffff);
    if (parser == NULL) {
        return;
    }

    BOLTTYPE type;
    bool reading_type = false;
    NuFParPushCom(parser, BoltType_ConfigKeywords);
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0) {
            continue;
        }
        if (reading_type) {
            if (NuStrICmp(parser->word_buf, "bolttype_end") == 0) {
                if (type.name[0] != '\0') {
                    BOLTTYPE *world_types = (BOLTTYPE *)((u8 *)world + 0x4b18);
                    for (i32 i = 0; i < 8; i++) {
                        if (NuStrLen(world_types[i].name) == 0) {
                            world_types[i] = type;
                            break;
                        }
                    }
                }
                reading_type = false;
            } else {
                NuFParInterpretWord(parser);
            }
        } else if (NuStrICmp(parser->word_buf, "bolttype_start") == 0) {
            BT_worldinfo = world;
            BT_bolttype = &type;
            BT_gdeb_moving_count = 0;
            BT_scene = things_scene;
            type = GlobalBoltType_Default;
            type.name[0] = '\0';
            type.moving_debris_ids[0] = -1;
            type.moving_debris_ids[1] = -1;
            reading_type = true;
        }
    }
    NuFParDestroy(parser);
}

struct BoltPart {
    u8 unknown_000[0x30];
    NUVEC position;
    u8 unknown_03c[0x44];
    NUVEC debris_normal;
    NUVEC bounds_min;
    NUVEC bounds_max;
    u8 unknown_0a4[0x40];
    f32 radius;
    u8 unknown_0e8[0x20];
    u32 flags;
    u8 unknown_10c[0x38];
    u8 active_flags;
    u8 unknown_145[0xdf];
};

#ifndef __x86_64__
static_assert(sizeof(BoltPart) == 0x224, "BoltPart size");
#endif

i32 Bolt_HitParts(BOLT *bolt, NUVEC *hit_positions, NUVEC *bounds_min, NUVEC *bounds_max, f32 radius, i32 kill_mode) {
    BoltPart *parts = (BoltPart *)Part;
    for (i32 i = 0; i < MAXPARTS; i++) {
        BoltPart *part = &parts[i];
        if ((part->active_flags & 1) == 0 || ((part->flags & 0x08000000) == 0 && (part->flags & 0xa) != 0xa) ||
            bounds_min->x > part->bounds_max.x || bounds_max->x < part->bounds_min.x ||
            bounds_min->z > part->bounds_max.z || bounds_max->z < part->bounds_min.z ||
            bounds_min->y > part->bounds_max.y || bounds_max->y < part->bounds_min.y) {
            continue;
        }

        i32 hit_index = -1;
        if ((bolt->flags & 0x200) == 0 &&
            SphereSphereOverlapScaleY(&part->position, part->radius, part->radius, &hit_positions[2], radius, radius)) {
            hit_index = 2;
        } else if (SphereSphereOverlapScaleY(&part->position, part->radius, part->radius, &hit_positions[1], radius,
                                             radius)) {
            hit_index = 1;
        } else if ((bolt->flags & 0x200) == 0 && SphereSphereOverlapScaleY(&part->position, part->radius, part->radius,
                                                                           &hit_positions[0], radius, radius)) {
            hit_index = 0;
        }
        if (hit_index == -1) {
            continue;
        }

        BoltSys->debris_fn(bolt, hit_positions, hit_index, &part->debris_normal, 0);
        Bolt_PlayHitSfx(bolt);
        if (bolt->source != NULL && (bolt->source->state_flags & 1) != 0) {
            NewRumble(*(NUPAD **)bolt->source->player_packet.game_pad, 0.7f, 0);
        }
        Bolt_End(bolt, 1);
        if (BoltSys->part_hit_fn != NULL && BoltSys->part_hit_fn(bolt, part) == 1) {
            return (i32)(usize)part;
        }
        KillPart(part, kill_mode);
        return (i32)(usize)part;
    }
    return 0;
}

void Bolt_AddDeflectedBolt(BOLT *bolt, NUVEC *direction, NUVEC *normal, u8 *hit_bolts) {
    if ((bolt->type->flags & 0x100000) != 0) {
        return;
    }

    NUVEC reflected;
    Bolt_Reflect(normal, direction, &reflected);
    NuVecNorm(&reflected, &reflected);
    FindAnglesXY(&reflected, NULL, NULL);

    if ((u8)bolt->terrain_type > 31 || (TerSurface[bolt->terrain_type * 12 + 5] & 8) == 0) {
        i_temp_xrot += (i32)((f32)qrand() * (1.0f / 65535.0f) * 10922.0f - 5461.0f);
        temp_yrot += (i16)(i32)((f32)qrand() * (1.0f / 65535.0f) * 10922.0f - 5461.0f);
    }

    i32 angles[2] = {i_temp_xrot, (u16)temp_yrot};
    NUMTX matrix;
    NuMtxSetRotationXYVU0(&matrix, angles);

    i32 type_id = bolt->type->deflected_type;
    if (type_id < 0 || type_id >= BoltSys->type_count) {
        type_id = bolt->type_id;
    }
    f32 offset = bolt->travel_distance * 1.01f;
    NUVEC position;
    position.x = bolt->position.x + reflected.x * offset;
    position.y = bolt->position.y + reflected.y * offset;
    position.z = bolt->position.z + reflected.z * offset;
    addbolt_nosfx = 1;
    BOLT *deflected = Bolt_Add(NULL, &position, &matrix, type_id, bolt->hit_flags);
    if (deflected != NULL) {
        deflected->flags |= 0x10000000;
        if (hit_bolts != NULL) {
            hit_bolts[deflected->index] = 1;
        }
    }
}

i32 Bolt_HitPlat(BOLT *bolt, u8 *hit_bolts, WORLDINFO *) {
    u32 level_flags = GetLevelExBlowupFlags();
    Bolt_PlayHitSfx(bolt);

    GIZMOSYS *gizmo_system = *(GIZMOSYS **)((u8 *)WORLD + 0x2ac8);
    if ((bolt->hit_flags & 0x800) != 0 && GizmoSys_BoltHitPlat(gizmo_system, WORLD, bolt, hit_bolts) != 0) {
        return 1;
    }

    void *blowup = GizmoBlowUp_FindFromPlatID(WORLD, bolt->platform_id);
    bool has_blowup = blowup != NULL;
    if (blowup != NULL) {
        u32 blowup_flags = *(u32 *)((u8 *)blowup + 0xa0);
        if ((blowup_flags & 0x8000) != 0) {
            GameObject_s *source = bolt->source;
            if (source != NULL && (blowup_flags & 0x80000) != 0) {
                if ((level_flags & 1) != 0) {
                    u8 *character = *(u8 **)((u8 *)source->character_data + 0x24);
                    if (*(f32 *)(character + 0x28) > 0.0f) {
                        return 0;
                    }
                }
                if (source->player_packet.takeover_object == NULL) {
                    return 0;
                }
            }

            i32 result = GizmoBlowupBlowup(blowup, 1, 3, bolt->type->damage, NULL);
            if (result != 0 && source != NULL) {
                NewRumble(*(NUPAD **)source->player_packet.game_pad, 0.7f, 0);
            }
            return 1;
        }
    }

    if (bolt->terrain_type != -1) {
        u32 terrain_flags = *(u32 *)(TerSurface + bolt->terrain_type * 12 + 4);
        if ((terrain_flags & 0x1800) != 0 || has_blowup) {
            GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
            Bolt_AddDeflectedBolt(bolt, &bolt->direction, &bolt->impact_normal, hit_bolts);
            if (bolt->source != NULL && (bolt->source->state_flags & 0x80) != 0) {
                NewRumble(*(NUPAD **)bolt->source->player_packet.game_pad, 0.7f, 0);
                GameCam_HitJudder();
            }
        }
    }
    return 0;
}

void Bolt_HitGameObjectRC(NetMessage &message) {
    i16 bolt_index;
    i16 object_index;
    NUVEC hit_positions[3];
    i32 hit_index;
    i32 client;
    BoltMessage_Read(message, &bolt_index, sizeof(bolt_index));
    BoltMessage_Read(message, &object_index, sizeof(object_index));
    BoltMessage_Read(message, hit_positions, sizeof(hit_positions));
    BoltMessage_Read(message, &hit_index, sizeof(hit_index));
    BoltMessage_Read(message, &client, sizeof(client));

    BOLT *bolt = &Bolt[bolt_index];
    GameObject_s *object = &Obj[object_index];
    if (BoltSys->post_hit_fn != NULL && bolt->source != NULL && (i8)bolt->source->state_flags < 0) {
        ((void (*)(GameObject_s *, NUVEC *))BoltSys->post_hit_fn)(bolt->source, &bolt->position);
    }

    bool deflected = false;
    if ((i8)object->state_flags < 0) {
        deflected = Player_HasDeflectBolts(object) != 0;
    }
    bool cannot_kill = Cheats_CheckFlags(0x100000) != 0 && (i8)object->state_flags < 0;
    if (!deflected) {
        deflected = TouchHacks::ShouldDeflectBolt(*object, *bolt);
    }

    NewRumble(*(NUPAD **)object->player_packet.game_pad, 0.5f, 0);
    u32 character_flags = CInfo[object->player_packet.character_state].flags;
    u8 combat_flags_1 = ((u8 *)&object->player_packet)[0xfa];
    bool can_deflect = (character_flags & 0x04000000) != 0 ||
                       ((character_flags & 0x08000000) != 0 && (combat_flags_1 & 2) != 0) || deflected ||
                       (character_flags & 0x00800000) != 0 ||
                       ((character_flags & 0x01000000) != 0 && (combat_flags_1 & 1) != 0) || CannotKill(object) != 0;

    if (((bolt->flags & 0x100) == 0 || deflected) && can_deflect) {
        BoltSys->debris_fn(bolt, hit_positions, hit_index, &object->velocity, 1);
        if (LEGOCONTEXT_HOLD != -1 && object->player_packet.character_state == LEGOCONTEXT_HOLD) {
            NewBlockAction(object);
        }
        GameAudio_PlaySfx(0x2a, &hit_positions[hit_index], 0, 0);
        addbolt_nosfx = 1;

        if (!((LEGOCONTEXT_BLOCK == -1 || object->player_packet.character_state != LEGOCONTEXT_BLOCK) && !deflected &&
              bolt->elapsed < 0.1f)) {
            NUVEC direction;
            if (bolt->source == NULL || (bolt->source->state_flags & 1) == 0) {
                NuVecSub(&direction, &bolt->position, &hit_positions[hit_index]);
            } else {
                CalculateInterceptVector(&hit_positions[hit_index], &bolt->source->origin, &bolt->source->velocity,
                                         bolt->speed, &direction, NULL);
            }
            FindAnglesXY(&direction, NULL, NULL);

            bool randomize =
                ((bolt->flags & 0x40) != 0 && !deflected) ||
                ((i8)object->state_flags >= 0 ||
                 ((LEGOCONTEXT_BLOCK == -1 || object->player_packet.character_state != LEGOCONTEXT_BLOCK) &&
                  !cannot_kill)) ||
                ((*(u32 *)((u8 *)object->character_data + 4) & 8) == 0) ||
                (bolt->source != NULL && (bolt->source->state_flags & 1) != 0 &&
                 ((bolt->source->state_flags & 0x1001) != 0x1001 || bolt->source->death_state != 0));
            if (randomize && !TouchHacks::TouchControlsActive) {
                temp_yrot += (i16)(i32)((f32)qrand() * (1.0f / 65535.0f) * 16384.0f - 8192.0f);
                i_temp_xrot += (i32)((f32)qrand() * (1.0f / 65535.0f) * 16384.0f - 5461.0f);
                if (i_temp_xrot < -0x2000) {
                    i_temp_xrot = -0x2000;
                } else if (i_temp_xrot > 0x2000) {
                    i_temp_xrot = 0x2000;
                }
                temp_xrot = i_temp_xrot;
            }

            NewBuzz(*(NUPAD **)object->player_packet.game_pad, 0.1f, 0);
            i32 angles[2] = {temp_xrot, (u16)temp_yrot};
            NUMTX matrix;
            NuMtxSetRotationXYVU0(&matrix, angles);
            BOLT *reflected = Bolt_Add(object, &hit_positions[hit_index], &matrix, bolt->type_id, 0);
            if (reflected != NULL) {
                reflected->flags |= 0x10000000;
            }
        }
    } else {
        u8 *character_runtime = *(u8 **)((u8 *)object->character_data + 0x24);
        bool can_take_damage =
            object->player_packet.damage_scale != 1.0f &&
            ((character_runtime[0x97] & 4) == 0 || VehicleArea == 0 || BonusArea == 0) &&
            (((u8 *)object)[0x108b] != 0 || (((u8 *)object->character_data)[5] & 0x20) == 0) &&
            ((i8)object->state_flags >= 0 || (character_runtime[0x90] & 0x40) == 0 ||
             (bolt->source != NULL && (*(u8 **)((u8 *)bolt->source->character_data + 0x24))[0x90] & 0x40) != 0) &&
            ((((u8 *)object->character_data)[7] & 0x20) == 0 || object->player_packet.takeover_object != NULL);
        if (can_take_damage) {
            BoltSys->debris_fn(bolt, hit_positions, hit_index, &object->velocity, 0);
            u32 damage = 0;
            if ((bolt->flags & 0x00800000) == 0) {
                damage = bolt->type->damage;
                if (Player_HasDoubleBoltDamage_FromBolt(bolt) != 0) {
                    damage *= 2;
                }
            }
            Bolt_PlayHitSfx(bolt);

            GameObject_s *source = bolt->source;
            if (source == NULL || (source->state_flags & 1) == 0 || (i8)source->state_flags < 0 ||
                (source->object_flags & 1) == 0 || ((source->object_flags ^ object->object_flags) & 1) != 0) {
                objhitobj_bolt = bolt;
                ObjHitObj(source, object, damage, bolt->hit_flags, 0, 0);
                character_flags = CInfo[object->player_packet.character_state].flags;
                u8 *actions = *(u8 **)((u8 *)object->game_character_data + 0xc);
                if ((bolt->flags & 0x00800000) != 0 && (character_flags & 0x10008000) == 0 &&
                    (((u8 *)object->character_data)[4] & 0x10) != 0 && LEGOACT_DEACTIVATED != -1 &&
                    *(void **)(actions + LEGOACT_DEACTIVATED * 4) != NULL) {
                    DeactivatePlayer(object, DEACTIVATEDTIME, NULL);
                    source = bolt->source;
                    if (source != NULL) {
                        NewBuzz(*(NUPAD **)source->player_packet.game_pad, 0.1f, 0);
                    }
                }
                source = bolt->source;
            }
            if (source != NULL && source->player_packet.game_pad != NULL) {
                NewRumble(*(NUPAD **)source->player_packet.game_pad, 0.7f, 0);
            }
        } else if ((bolt->flags & 0x04000000) != 0) {
            BoltSys->debris_fn(bolt, hit_positions, hit_index, &object->velocity, 0);
            ObjHitShield(bolt->source, object, bolt->type->damage, bolt);
        } else {
            BoltSys->debris_fn(bolt, hit_positions, hit_index, &object->velocity, 1);
            ObjHitShield(bolt->source, object, 0, bolt);
            GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
            NewBuzz(*(NUPAD **)object->player_packet.game_pad, 0.1f, 0);
            if ((bolt->flags & 0x00100000) == 0) {
                temp_yrot = NuAtan2D(bolt->position.x - object->origin.x, bolt->position.z - object->origin.z);
                temp_yrot += (i16)(i32)((f32)qrand() * (1.0f / 65535.0f) * 16384.0f - 5461.0f);
                i32 angles[2];
                angles[0] = (i32)(-1820.0f - (f32)qrand() * (1.0f / 65535.0f) * 6371.0f);
                temp_xrot = angles[0];
                angles[0] = temp_xrot;
                angles[1] = (u16)temp_yrot;
                NUMTX matrix;
                NuMtxSetRotationXYVU0(&matrix, angles);
                addbolt_nosfx = 1;
                BOLT *reflected = Bolt_Add(object, &hit_positions[hit_index], &matrix, bolt->type_id, 0);
                if (reflected != NULL) {
                    reflected->flags |= 0x10000000;
                }
            }
        }
    }

    if (object->death_state == 0 && (object->state_flags & 2) == 0) {
        f32 momentum = *(f32 *)(*(u8 **)((u8 *)object->character_data + 0x24) + 0x18);
        if (object->model_scale > 1.0f) {
            momentum /= object->model_scale;
        }
        object->velocity.x += bolt->direction.x * momentum;
        object->velocity.z += bolt->direction.z * momentum;
    }
    if ((i8)object->state_flags < 0) {
        GameCam_HitJudder();
    }
    Bolt_End(bolt, 1);
}

i32 Bolt_HitGameObject(BOLT *bolt, GameObject_s *object, NUVEC *hit_positions, NUVEC *, NUVEC *, f32 radius, u8 *) {
    NUVEC object_position = object->origin;
    i32 hit_index = 2;
    NUVEC hit_position = hit_positions[2];
    bool ignore_height = (bolt->flags & 0x08000000) != 0 && bolt->elapsed >= bolt->type->fade_time;
    if (ignore_height) {
        object_position.y = 0.0f;
        hit_position.y = 0.0f;
    }
    if (!((bolt->flags & 0x200) == 0 &&
          SphereSphereOverlapScaleY(&object_position, object->bounds_radius, object->bounds_half_height, &hit_position,
                                    radius, radius))) {
        hit_index = 1;
        hit_position = hit_positions[1];
        if (ignore_height) {
            object_position.y = 0.0f;
            hit_position.y = 0.0f;
        }
        if (!SphereSphereOverlapScaleY(&object_position, object->bounds_radius, object->bounds_half_height,
                                       &hit_position, radius, radius)) {
            hit_index = 0;
            if ((bolt->flags & 0x200) != 0) {
                return 0;
            }
            hit_position = hit_positions[0];
            if (ignore_height) {
                object_position.y = 0.0f;
                hit_position.y = 0.0f;
            }
            if (!SphereSphereOverlapScaleY(&object_position, object->bounds_radius, object->bounds_half_height,
                                           &hit_position, radius, radius)) {
                return 0;
            }
        }
    }

    u8 *data = NULL;
    for (i32 i = 0; i < 512; i++) {
        u8 *candidate = NetMessage::sm_poolMessageData + i * 0x4b4;
        u32 *reference_count = (u32 *)(candidate + 0x4b0);
        if (*reference_count == 0) {
            *reference_count = 1;
            data = candidate;
            break;
        }
    }

    NetMessage message = {1, data, 0x20, 0x20};
    i16 bolt_index = bolt - Bolt;
    i16 object_index = object - Obj;
    if (data != NULL) {
        memmove(data + message.write_position, &bolt_index, sizeof(bolt_index));
        EdFileSwapEndianess16(data + message.write_position);
        message.write_position += sizeof(bolt_index);
        memmove(data + message.write_position, &object_index, sizeof(object_index));
        EdFileSwapEndianess16(data + message.write_position);
        message.write_position += sizeof(object_index);

        memmove(data + message.write_position, hit_positions, sizeof(NUVEC) * 3);
        for (i32 offset = 0; offset < (i32)(sizeof(NUVEC) * 3); offset += 4) {
            EdFileSwapEndianess32(data + message.write_position + offset);
        }
        message.write_position += sizeof(NUVEC) * 3;
        memmove(data + message.write_position, &hit_index, sizeof(hit_index));
        EdFileSwapEndianess32(data + message.write_position);
        message.write_position += sizeof(hit_index);
        memmove(data + message.write_position, &netclient, sizeof(netclient));
        EdFileSwapEndianess32(data + message.write_position);
        message.write_position += sizeof(netclient);
    }

    Bolt_HitGameObjectRC(message);
    if (data != NULL) {
        u32 *reference_count = (u32 *)(data + 0x4b0);
        if (*reference_count < 2) {
            *reference_count = 0;
        } else {
            (*reference_count)--;
        }
    }
    return 1;
}

i32 Bolt_HitGameObjects(BOLT *bolt, NUVEC *hit_positions, NUVEC *bounds_min, NUVEC *bounds_max, f32 radius,
                        u8 *hit_bolts) {
    for (i32 i = 0; i < HIGHGAMEOBJECT; i++) {
        GameObject_s *object = &Obj[i];
        if ((object->state_flags & 0x1001) != 0x1001 || object->death_state != 0 || object == bolt->source ||
            (object->player_packet.runtime_flags_0 & 0x20) != 0) {
            continue;
        }

        GameObject_s *source_takeover = bolt->source == NULL ? NULL : bolt->source->player_packet.takeover_object;
        if (source_takeover != NULL && object == source_takeover) {
            continue;
        }

        if ((bolt->flags & 0x80) != 0 && object->player_index == -1) {
            GameObject_s *takeover = object->player_packet.takeover_object;
            if (takeover == NULL || takeover->player_index == -1) {
                continue;
            }
        }

        if ((CInfo[object->player_packet.character_state].flags & 0x8080) != 0 ||
            ((i8 *)&object->player_packet.runtime_flags_4)[1] < 0) {
            continue;
        }

        if (VehicleArea != 0 && BonusArea == 0 && bolt->source != NULL && bolt->source->player_index != -1 &&
            object->player_index != -1) {
            continue;
        }

        u8 *runtime_data = *(u8 **)((u8 *)object->character_data + 0x24);
        if ((runtime_data[0x91] & 0x80) != 0 || bounds_min->x > object->bounds_max.x ||
            object->bounds_min.x > bounds_max->x || bounds_min->z > object->bounds_max.z ||
            object->bounds_min.z > bounds_max->z ||
            ((bolt->flags & 0x08000000) == 0 &&
             (bounds_min->y > object->bounds_max.y || object->bounds_min.y > bounds_max->y))) {
            continue;
        }

        if (Bolt_HitGameObject(bolt, object, hit_positions, bounds_min, bounds_max, radius, hit_bolts) != 0) {
            return 1;
        }
    }
    return 0;
}

void Bolts_Update(WORLDINFO *world) {
    NUVEC impact_normal = v010;
    u8 hit_bolts[32];
    memset(hit_bolts, 0, sizeof(hit_bolts));

    for (i32 bolt_index = 0; bolt_index < 32; bolt_index++) {
        BOLT *bolt = &Bolt[bolt_index];
        if (bolt->active == 0 || hit_bolts[bolt_index] != 0) {
            continue;
        }

        if (bolt->source != NULL && (bolt->source->state_flags & 0x1001) != 0x1001) {
            bolt->source = NULL;
        }

        BOLTTYPE *type = bolt->type;
        f32 frame_time = FRAMETIME;
        if ((bolt->flags & 4) == 0) {
            bolt->unknown_0dc -= FRAMETIME;
            if (bolt->unknown_0dc <= 0.0f) {
                NUVEC ray_step;
                NuVecScale(&ray_step, &bolt->direction, bolt->speed * 0.2f);
                bolt->unknown_0dc = 0.2f;
                if (bolt->acceleration != 0.0f) {
                    ray_step.y = bolt->velocity.y * 0.2f + bolt->acceleration * 0.5f * 0.040000003f;
                }

                NUVEC next_ray_end;
                NuVecAdd(&next_ray_end, &bolt->position, &ray_step);
                NuVecSub(&ray_step, &next_ray_end, &bolt->ray_end);
                if (Bolt_RayCast(bolt, &bolt->ray_end, &ray_step, bolt->travel_distance)) {
                    u8 hit_info[16];
                    NewRayCastGetImpactNormal(&impact_normal);
                    NewTerrHitInfo(hit_info);
                    bolt->duration = NewRayCastGetTOFI() * bolt->unknown_0dc + bolt->elapsed;
                    if (type->hit_fn != NULL) {
                        ((void (*)(BOLT *, NUVEC *))type->hit_fn)(bolt, &impact_normal);
                    }
                }

                bolt->reflection_plane_y = 2000000.0f;
                f32 shadow_height = GameShadow(NULL, &bolt->position, 5.0f, -1);
                frame_time = FRAMETIME;
                if (shadow_height != 2000000.0f) {
                    if (NuSpecialExistsFn(&type->shadow_object) != 0) {
                        bolt->shadow_plane_y = shadow_height;
                        FindAnglesZX(&ShadNorm, &bolt->shadow_pitch, &bolt->shadow_roll);
                    }
                    u32 terrain = ShadowInfo();
                    if (terrain < 32 && (TerSurface[terrain * 12 + 4] & 2) != 0) {
                        bolt->reflection_plane_y = shadow_height;
                        frame_time = FRAMETIME;
                    } else if (NewShadowOnPlatform() != -1) {
                        bolt->reflection_plane_y = FindReflectionNoPlatforms(&bolt->position);
                        frame_time = FRAMETIME;
                    }
                }
            }
        }

        bolt->elapsed += frame_time;
        if (type->update_fn != NULL) {
            type->update_fn(bolt);
            frame_time = FRAMETIME;
        }

        if (bolt->acceleration != 0.0f) {
            bolt->velocity.y += frame_time * bolt->acceleration;
            bolt->speed = NuVecMag(&bolt->velocity);
            NuVecNorm(&bolt->direction, &bolt->velocity);
            frame_time = FRAMETIME;
            if ((bolt->flags & 0x1000) != 0) {
                u16 pitch;
                u16 yaw;
                FindAnglesXY(&bolt->direction, &pitch, &yaw);
                NuMtxSetRotationX(&bolt->draw_matrix, pitch + 0x4000);
                NuMtxRotateY(&bolt->draw_matrix, yaw);
                NuMtxSetRotationX(&bolt->launch_matrix, pitch);
                NuMtxRotateY(&bolt->launch_matrix, yaw);
                frame_time = FRAMETIME;
            }
        }

        bolt->position.x += bolt->velocity.x * frame_time;
        bolt->position.y += bolt->velocity.y * frame_time;
        bolt->position.z += bolt->velocity.z * frame_time;

        NUVEC hit_positions[3];
        hit_positions[1] = bolt->position;
        f32 position_x = bolt->position.x;
        f32 position_y = bolt->position.y;
        f32 position_z = bolt->position.z;
        if ((bolt->flags & 0x200) == 0) {
            NUVEC offset = {0.0f, 0.0f, bolt->unknown_0d8 * 2.0f};
            NuVecMtxRotate(&offset, &offset, &bolt->launch_matrix);
            NuVecSub(&hit_positions[0], &bolt->position, &offset);
            NuVecAdd(&hit_positions[2], &bolt->position, &offset);
        }

        bolt->bounds_min.x = position_x - bolt->radius;
        bolt->bounds_min.y = position_y - bolt->radius;
        bolt->bounds_min.z = position_z - bolt->radius;
        bolt->bounds_max.x = position_x + bolt->radius;
        bolt->bounds_max.y = position_y + bolt->radius;
        bolt->bounds_max.z = position_z + bolt->radius;

        if (bolt->duration <= bolt->elapsed) {
            bool hit_terrain;
            if (bolt->terrain_type == -1) {
                BoltSys->debris_fn(bolt, hit_positions, -1, NULL, 0);
                hit_terrain = false;
            } else {
                NUVEC reflected_velocity;
                Bolt_Reflect(&bolt->impact_normal, &bolt->velocity, &reflected_velocity);
                NuVecScale(&reflected_velocity, &reflected_velocity, 0.2f);
                i32 moving = bolt->platform_id != -1 && bolt->terrain_type != -1 &&
                             (TerSurface[bolt->terrain_type * 12 + 5] & 0x10) != 0;
                BoltSys->debris_fn(bolt, hit_positions, -1, &reflected_velocity, moving);
                hit_terrain = true;
            }
            Bolt_End(bolt, 1);

            if ((bolt->flags & 0x10000) == 0 && Bolt_HitGameObjects(bolt, hit_positions, &bolt->bounds_min,
                                                                    &bolt->bounds_max, bolt->unknown_0d8, NULL) == 0) {
                u32 flags = bolt->flags;
                bool normal_terrain_hit =
                    (flags & 0x13) == 0 && (bolt->source == NULL || ((((u8 *)bolt->source)[0xefb] & 0x10) == 0 &&
                                                                     ((u8 *)bolt->source)[0xf0c] != 5));
                if (normal_terrain_hit) {
                    if (hit_terrain) {
                        i32 terrain = bolt->terrain_type;
                        if (bolt->platform_id != -1 && terrain != -1 && (TerSurface[terrain * 12 + 5] & 0x10) != 0) {
                            GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
                            Bolt_AddDeflectedBolt(bolt, &bolt->direction, &bolt->impact_normal, hit_bolts);
                            terrain = bolt->terrain_type;
                            flags = bolt->flags;
                        }
                        if ((u8)terrain < 32 && (TerSurface[terrain * 12 + 5] & 8) != 0 && (flags & 0x8000) == 0) {
                            GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
                            Bolt_AddDeflectedBolt(bolt, &bolt->velocity, &bolt->impact_normal, hit_bolts);
                        } else if ((flags & 0x80000) == 0) {
                            Bolt_PlayHitSfx(bolt);
                        }
                    } else if ((flags & 0x80000) == 0) {
                        Bolt_PlayHitSfx(bolt);
                    }
                } else if (bolt->platform_id == -1) {
                    i32 gizmo_hit = GizmoSys_BoltHit(*(GIZMOSYS **)((u8 *)world + 0x2ac8), world, bolt, hit_positions,
                                                     &bolt->bounds_min, &bolt->bounds_max, bolt->unknown_0d8, NULL);
                    if (gizmo_hit == 0) {
                        i32 position_count = (bolt->flags & 0x200) == 0 ? 3 : 1;
                        NUVEC *positions = (bolt->flags & 0x200) == 0 ? hit_positions : &bolt->position;
                        if (GizmoBlowUp_Hit(bolt->source, positions, position_count, bolt->unknown_0d8,
                                            &bolt->bounds_min, &bolt->bounds_max, bolt, 1, NULL) == 0) {
                            i32 part_mode = Bolt_HitPartMode(bolt);
                            if (Bolt_HitParts(bolt, hit_positions, &bolt->bounds_min, &bolt->bounds_max,
                                              bolt->unknown_0d8, part_mode) == 0) {
                                if (hit_terrain) {
                                    i32 terrain = bolt->terrain_type;
                                    flags = bolt->flags;
                                    if ((u8)terrain < 32 && (TerSurface[terrain * 12 + 5] & 8) != 0 &&
                                        (flags & 0x8000) == 0) {
                                        GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
                                        Bolt_AddDeflectedBolt(bolt, &bolt->velocity, &bolt->impact_normal, hit_bolts);
                                    } else if ((flags & 0x80000) == 0) {
                                        Bolt_PlayHitSfx(bolt);
                                    }
                                } else if ((bolt->flags & 0x80000) == 0) {
                                    Bolt_PlayHitSfx(bolt);
                                }
                            }
                        } else {
                            BoltSys->debris_fn(bolt, hit_positions, -1, NULL, 0);
                            if (bolt->source != NULL) {
                                NewRumble(*(NUPAD **)bolt->source->player_packet.game_pad, 0.7f, 0);
                            }
                            Bolt_End(bolt, 1);
                            Bolt_PlayHitSfx(bolt);
                        }
                    }
                } else if (Bolt_HitPlatFn == NULL || Bolt_HitPlatFn(bolt) == 0) {
                    Bolt_HitPlat(bolt, hit_bolts, world);
                }

                if (BoltSys->post_hit_fn != NULL && bolt->source != NULL && (i8)bolt->source->state_flags < 0) {
                    ((void (*)(GameObject_s *, NUVEC *))BoltSys->post_hit_fn)(bolt->source, &bolt->position);
                }
            }
            continue;
        }

        if (Bolt_HitCustomFn != NULL) {
            Bolt_HitCustomFn(bolt, hit_positions);
        }
        if (type->moving_debris_ids[0] != -1 && type->moving_debris_speeds[0] != 0) {
            void *debris = *(void **)(*(u8 **)(*(u8 **)((u8 *)world + 0x134) + 8) + type->moving_debris_ids[0] * 0x14);
            AddVariableShotDebrisEffectTimed5(debris, &bolt->position, NULL, &bolt->velocity,
                                              type->moving_debris_speeds[0], FRAMETIME, NULL, NULL, 20000, 0);
        }
        if (type->moving_debris_ids[1] != -1 && type->moving_debris_speeds[1] != 0) {
            void *debris = *(void **)(*(u8 **)(*(u8 **)((u8 *)world + 0x134) + 8) + type->moving_debris_ids[1] * 0x14);
            AddVariableShotDebrisEffectTimed5(debris, &bolt->position, NULL, &bolt->velocity,
                                              type->moving_debris_speeds[1], FRAMETIME, NULL, NULL, 20000, 0);
        }

        if ((bolt->flags & 0x10000) == 0) {
            bool hit_object = false;
            for (i32 object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
                GameObject_s *object = &Obj[object_index];
                if ((object->state_flags & 0x1001) != 0x1001 || object->death_state != 0 || object == bolt->source ||
                    (((u8 *)&object->player_packet.runtime_flags_0)[0] & 0x20) != 0) {
                    continue;
                }

                GameObject_s *source_takeover =
                    bolt->source == NULL ? NULL : bolt->source->player_packet.takeover_object;
                if (source_takeover != NULL && object == source_takeover) {
                    continue;
                }
                if ((bolt->flags & 0x80) != 0 && object->player_index == -1) {
                    GameObject_s *takeover = object->player_packet.takeover_object;
                    if (takeover == NULL || takeover->player_index == -1) {
                        continue;
                    }
                }
                if ((CInfo[object->player_packet.character_state].flags & 0x8080) != 0 ||
                    (i8)((u8 *)&object->player_packet.runtime_flags_4)[1] < 0) {
                    continue;
                }
                if (VehicleArea != 0 && BonusArea == 0 && bolt->source != NULL && bolt->source->player_index != -1 &&
                    object->player_index != -1) {
                    continue;
                }

                u8 *runtime_data = *(u8 **)((u8 *)object->character_data + 0x24);
                if ((runtime_data[0x91] & 0x80) != 0 || bolt->bounds_min.x > object->bounds_max.x ||
                    object->bounds_min.x > bolt->bounds_max.x || bolt->bounds_min.z > object->bounds_max.z ||
                    object->bounds_min.z > bolt->bounds_max.z ||
                    ((bolt->flags & 0x08000000) == 0 &&
                     (bolt->bounds_min.y > object->bounds_max.y || object->bounds_min.y > bolt->bounds_max.y))) {
                    continue;
                }

                if (Bolt_HitGameObject(bolt, object, hit_positions, &bolt->bounds_min, &bolt->bounds_max,
                                       bolt->unknown_0d8, hit_bolts) != 0) {
                    hit_object = true;
                    break;
                }
            }
            if (hit_object) {
                continue;
            }

            u32 flags = bolt->flags;
            bool test_gizmos = (flags & 0x13) != 0 ||
                               (bolt->source != NULL &&
                                ((((u8 *)bolt->source)[0xefb] & 0x10) != 0 || ((u8 *)bolt->source)[0xf0c] == 5));
            if (test_gizmos &&
                GizmoSys_BoltHit(*(GIZMOSYS **)((u8 *)world + 0x2ac8), world, bolt, hit_positions, &bolt->bounds_min,
                                 &bolt->bounds_max, bolt->unknown_0d8, hit_bolts) == 0) {
                i32 position_count = (bolt->flags & 0x200) == 0 ? 3 : 1;
                NUVEC *positions = (bolt->flags & 0x200) == 0 ? hit_positions : &bolt->position;
                if (GizmoBlowUp_Hit(bolt->source, positions, position_count, bolt->unknown_0d8, &bolt->bounds_min,
                                    &bolt->bounds_max, bolt, 1, hit_bolts) == 0) {
                    i32 part_mode = Bolt_HitPartMode(bolt);
                    Bolt_HitParts(bolt, hit_positions, &bolt->bounds_min, &bolt->bounds_max, bolt->unknown_0d8,
                                  part_mode);
                } else {
                    BoltSys->debris_fn(bolt, hit_positions, -1, NULL, 0);
                    if (bolt->source != NULL) {
                        NewRumble(*(NUPAD **)bolt->source->player_packet.game_pad, 0.7f, 0);
                    }
                    Bolt_End(bolt, 1);
                    Bolt_PlayHitSfx(bolt);
                }
            }
        }
    }
}
