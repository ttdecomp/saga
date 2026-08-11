#pragma once

#include <stddef.h>

#include "legoapi/gameobject.h"
#include "legoapi/world.h"
#include "nu2api/numath/nuvec.h"

struct BOLT_s;

typedef void (*BOLTUPDATEFN)(BOLT_s *bolt);
typedef void (*BOLTENDFN)(BOLT_s *bolt);
typedef f32 (*BOLTDRAWSCALEFN)(BOLT_s *bolt);

typedef struct BOLTSPECIAL_s {
    void *object;
    void *matrix;
    void *instance;
} BOLTSPECIAL;

typedef struct BOLTTYPE_s {
    char name[0x10];
    f32 speed;
    f32 duration;
    f32 acceleration;
    f32 radius;
    f32 scale;
    f32 fade_time;
    i16 object_id;
    i16 glow_object_id;
    i16 shadow_object_id;
    i16 debris_id;
    i16 moving_debris_ids[2];
    u16 moving_debris_speeds[2];
    i16 hit_part;
    i16 unknown_03a;
    i8 damage;
    i8 deflected_type;
    i16 unknown_03e;
    i32 random_angle;
    BOLTUPDATEFN init_fn;
    BOLTUPDATEFN update_fn;
    BOLTENDFN end_fn;
    void *hit_fn;
    BOLTDRAWSCALEFN draw_scale_fn;
    const char *shoot_sfx_name;
    const char *hit_sfx_name;
    u32 flags;
    i16 shoot_sfx_id;
    i16 hit_sfx_id;
    BOLTSPECIAL object;
    BOLTSPECIAL glow_object;
    BOLTSPECIAL reflected_object;
    BOLTSPECIAL reflected_glow_object;
    BOLTSPECIAL shadow_object;
} BOLTTYPE;

typedef struct BOLT_s {
    BOLTTYPE *type;
    GameObject_s *source;
    NUMTX launch_matrix;
    NUMTX draw_matrix;
    NUVEC position;
    NUVEC previous_position;
    NUVEC velocity;
    NUVEC direction;
    NUVEC ray_end;
    f32 draw_scale;
    f32 elapsed;
    f32 speed;
    f32 duration;
    f32 radius;
    f32 unknown_0d8;
    f32 unknown_0dc;
    f32 travel_distance;
    f32 shadow_plane_y;
    f32 reflection_plane_y;
    f32 acceleration;
    u32 flags;
    u16 shadow_pitch;
    u16 shadow_roll;
    u16 hit_flags;
    i16 platform_id;
    i16 debris_id;
    i16 part_debris_id;
    u8 active;
    u8 type_id;
    u8 rendered;
    u8 index;
    i8 terrain_type;
    u8 unknown_105[3];
    NUVEC bounds_min;
    NUVEC bounds_max;
    NUVEC impact_normal;
    NUVEC impact_position;
} BOLT;

typedef void (*BOLTDEBRISFN)(BOLT *bolt, NUVEC *position, i32 debris_id, NUVEC *normal, i32 moving);
typedef void (*BOLTSHOOTORIGINFN)(GameObject_s *object, NUVEC *origin);
typedef u16 (*BOLTSHOOTDIRECTIONFN)(GameObject_s *object, NUVEC *direction);
typedef i32 (*BOLTPARTHITFN)(BOLT *bolt, void *part);
typedef i32 (*BOLTSHOOTJOINTFN)(GameObject_s *object, i32 joint_slot);

typedef struct BOLTSYS_s {
    BOLTTYPE *types;
    i32 type_count;
    void *post_hit_fn;
    BOLTDEBRISFN debris_fn;
    BOLTSHOOTORIGINFN shoot_origin_fn;
    BOLTSHOOTDIRECTIONFN shoot_direction_fn;
    BOLTPARTHITFN part_hit_fn;
    BOLTSHOOTJOINTFN shoot_joint_fn;
} BOLTSYS;

#if UINTPTR_MAX != UINT64_MAX
static_assert(sizeof(BOLTTYPE) == 0xa4, "BOLTTYPE size");
static_assert(offsetof(BOLTTYPE, object_id) == 0x28, "BOLTTYPE object id offset");
static_assert(offsetof(BOLTTYPE, damage) == 0x3c, "BOLTTYPE damage offset");
static_assert(offsetof(BOLTTYPE, flags) == 0x60, "BOLTTYPE flags offset");
static_assert(offsetof(BOLTTYPE, object) == 0x68, "BOLTTYPE object offset");
static_assert(sizeof(BOLT) == 0x138, "BOLT size");
static_assert(offsetof(BOLT, launch_matrix) == 0x8, "BOLT launch matrix offset");
static_assert(offsetof(BOLT, draw_matrix) == 0x48, "BOLT draw matrix offset");
static_assert(offsetof(BOLT, position) == 0x88, "BOLT position offset");
static_assert(offsetof(BOLT, previous_position) == 0x94, "BOLT previous position offset");
static_assert(offsetof(BOLT, velocity) == 0xa0, "BOLT velocity offset");
static_assert(offsetof(BOLT, direction) == 0xac, "BOLT direction offset");
static_assert(offsetof(BOLT, draw_scale) == 0xc4, "BOLT draw scale offset");
static_assert(offsetof(BOLT, elapsed) == 0xc8, "BOLT elapsed offset");
static_assert(offsetof(BOLT, shadow_plane_y) == 0xe4, "BOLT shadow plane offset");
static_assert(offsetof(BOLT, reflection_plane_y) == 0xe8, "BOLT reflection plane offset");
static_assert(offsetof(BOLT, flags) == 0xf0, "BOLT flags offset");
static_assert(offsetof(BOLT, hit_flags) == 0xf8, "BOLT hit flags offset");
static_assert(offsetof(BOLT, platform_id) == 0xfa, "BOLT platform id offset");
static_assert(offsetof(BOLT, active) == 0x100, "BOLT active offset");
static_assert(offsetof(BOLT, terrain_type) == 0x104, "BOLT terrain type offset");
static_assert(offsetof(BOLT, bounds_min) == 0x108, "BOLT bounds min offset");
static_assert(offsetof(BOLT, bounds_max) == 0x114, "BOLT bounds max offset");
static_assert(offsetof(BOLT, ray_end) == 0xb8, "BOLT ray end offset");
static_assert(offsetof(BOLT, impact_normal) == 0x120, "BOLT impact normal offset");
static_assert(offsetof(BOLT, impact_position) == 0x12c, "BOLT impact position offset");
static_assert(sizeof(BOLTSYS) == 0x20, "BOLTSYS size");
#endif

extern BOLTTYPE GlobalBoltType_Default;
extern BOLTSYS *BoltSys;
extern BOLT Bolt[32];
extern i32 i_bolt;
extern f32 BOLT_OVERRIDE_PLAYERBOLTSPEED;
extern f32 BOLT_OVERRIDE_PLAYERBOLTDURATION;

BOLT *Bolt_Alloc();
void Bolt_Free(BOLT *bolt);
void Bolt_Reflect(NUVEC *normal, NUVEC *direction, NUVEC *reflection);
void Bolt_PlayHitSfx(BOLT *bolt);
void Bolt_End(BOLT *bolt, i32 run_callback);
void Bolts_Reset();
BOLTTYPE *BoltType_FindByID(i32 id, WORLDINFO *world);
void BoltTypes_Reset(WORLDINFO *world);
f32 Bolt_ObjTargetPosYAdjust(GameObject_s *object);
i8 Bolt_HitPartMode(BOLT *bolt);
void BoltSys_Init(BOLTSYS *bolt_system);
bool Bolt_RayCast(BOLT *bolt, NUVEC *origin, NUVEC *direction, f32 distance);
BOLT *FindIncomingBolt(GameObject_s *object, i32 filter_player_sources, i32 set_near_flag);
BOLT *Bolt_Find(i32 type_id, NUVEC *position, GameObject_s *source);
i32 BoltType_FindIDByName(char *name, WORLDINFO *world);
void BoltTypes_Init(WORLDINFO *world);
void BoltTypes_Configure(WORLDINFO *world, char *data);
i32 Bolt_HitParts(BOLT *bolt, NUVEC *hit_positions, NUVEC *bounds_min, NUVEC *bounds_max, f32 radius, i32 kill_mode);
void Bolt_AddDeflectedBolt(BOLT *bolt, NUVEC *direction, NUVEC *normal, u8 *hit_bolts);
i32 Bolt_HitPlat(BOLT *bolt, u8 *hit_bolts, WORLDINFO *world);
BOLT *Bolt_Add(GameObject_s *source, NUVEC *position, NUMTX *matrix, i32 type_id, i32 hit_flags);
i32 Bolt_HitGameObjects(BOLT *bolt, NUVEC *hit_positions, NUVEC *bounds_min, NUVEC *bounds_max, f32 radius,
                        u8 *hit_bolts);
void Bolts_Update(WORLDINFO *world);
void Bolts_Draw(WORLDINFO *world);
void Bolt_Shoot(GameObject_s *object, i32 type_id, i32 joint_mode);
