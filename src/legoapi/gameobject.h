#pragma once

#include <stddef.h>

#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/apiobject.h"
#include "legoapi/character.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"

struct HashedKey {
    u32 value;
};

struct GameObject_s;
struct COINPACKET_s;
struct GIZBUILDIT_s;
class MechAddonCollection;

class MechObjectInterface {
  public:
    virtual ~MechObjectInterface();

    void *objects;
};

class CharacterObjectInterface : public MechObjectInterface {
  public:
    CharacterObjectInterface(GameObject_s &object);

    GameObject_s *object;
};

class MechAddon {
  public:
    enum ProcessStage { PROCESS_STAGE_0 };

    enum RenderStage { RENDER_STAGE_0 };

    virtual ~MechAddon();
    virtual void OnAdded();
    virtual void OnRemoved();
    virtual bool OnProcess(ProcessStage stage, f32 elapsed);
    virtual void OnRender(RenderStage stage);

    MechObjectInterface *mech_object_interface;
    void *next_mech_object;
    void *previous_mech_object;
    HashedKey key;
    MechAddon *next_addon;
};

class MechAddonCollection {
  public:
    MechAddonCollection(MechObjectInterface *mech_interface)
        : mech_object_interface(NULL), next_mech_object(NULL), previous_mech_object(NULL), first_addon(NULL) {
        if (mech_interface != NULL) {
            void **node = (void **)&mech_object_interface;
            void **head = (void **)mech_interface->objects;
            if (head != NULL) {
                void **next = (void **)head[1];
                previous_mech_object = head;
                next_mech_object = next;
                head[1] = node;
                next[2] = node;
            } else {
                mech_interface->objects = node;
                next_mech_object = node;
                previous_mech_object = node;
            }
            mech_object_interface = mech_interface;
        }
    }

    ~MechAddonCollection() {
        MechAddon *addon = first_addon;
        while (addon != NULL) {
            MechAddon *next = addon->next_addon;
            addon->OnRemoved();
            delete addon;
            addon = next;
        }

        if (mech_object_interface != NULL) {
            void **node = (void **)&mech_object_interface;
            void **next = (void **)next_mech_object;
            if (node == next) {
                mech_object_interface->objects = NULL;
            } else {
                void **previous = (void **)previous_mech_object;
                next[2] = previous;
                previous[1] = next;
                if (mech_object_interface->objects == node) {
                    mech_object_interface->objects = next;
                }
            }
            mech_object_interface = NULL;
            next_mech_object = NULL;
            previous_mech_object = NULL;
        }
    }

    virtual void Add(MechAddon &addon) {
        if (mech_object_interface != addon.mech_object_interface) {
            return;
        }

        MechAddon *current = first_addon;
        while (current != NULL) {
            if (current == &addon) {
                return;
            }
            current = current->next_addon;
        }

        if (first_addon == NULL) {
            first_addon = &addon;
        } else {
            current = first_addon;
            while (current->next_addon != NULL) {
                current = current->next_addon;
            }
            current->next_addon = &addon;
        }
        addon.OnAdded();
    }

    virtual void Remove(MechAddon &addon) {
        MechAddon *current = first_addon;
        MechAddon *previous = NULL;

        while (current != NULL && current != &addon) {
            previous = current;
            current = current->next_addon;
        }
        if (current == NULL) {
            return;
        }

        if (previous == NULL) {
            first_addon = current->next_addon;
        } else {
            previous->next_addon = current->next_addon;
        }

        current->OnRemoved();
        delete current;
    }

    virtual void Process(MechAddon::ProcessStage stage, f32 elapsed) {
        MechAddon *addon = first_addon;
        while (addon != NULL) {
            MechAddon *next = addon->next_addon;
            if (!addon->OnProcess(stage, elapsed)) {
                Remove(*addon);
            }
            addon = next;
        }
    }

    virtual void Render(MechAddon::RenderStage stage) {
        MechAddon *addon = first_addon;
        while (addon != NULL) {
            addon->OnRender(stage);
        }
    }

    MechObjectInterface *mech_object_interface;
    void *next_mech_object;
    void *previous_mech_object;
    MechAddon *first_addon;
};

class MechEdgeStopAddon : public MechAddon {
  public:
    MechEdgeStopAddon(MechObjectInterface &mech_interface);

    u8 unknown_018[0x10];
};

class MechAutofireAddon : public MechAddon {
  public:
    MechAutofireAddon(MechObjectInterface &mech_interface);

    u8 unknown_018[4];
};

class MechTouchTask {
  public:
    virtual ~MechTouchTask();
    virtual const HashedKey &GetTaskType() = 0;
    virtual void Unknown() = 0;
    virtual void Kill() = 0;

    MechTouchTask *next;
};

typedef struct PLAYERPACKET_s {
    u8 unknown_000[0xb4];
    f32 balloon_height;
    u8 unknown_0b8[0x14];
    GameObject_s *opponent;
    void *opponent_target;
    GIZBUILDIT_s *buildit;
    u8 unknown_0d8[0xe];
    i16 action;
    u8 unknown_0e8[7];
    u8 balloon_type;
    u8 buildit_step;
    i8 character_state;
    u8 unknown_0f2[7];
    u8 combat_flags_0;
    u8 unknown_0fa[0x3e];
    COINPACKET_s *coin_packet;
    u8 unknown_13c[4];
    NUMTX joint_matrices[18];
    u8 unknown_5c0[0x20];
    void *game_pad;
    u8 unknown_5e4[0x14];
    void *equipment;
    u8 unknown_5fc[4];
    void *torpedo_packet;
    void *attack_target;
    u8 unknown_608[4];
    GameObject_s *takeover_object;
    u8 unknown_610[0x28];
    void *unknown_638;
    u8 unknown_63c[0x18];
    GameObject_s *force_glow_target;
    GameObject_s *force_glow_candidate;
    u8 unknown_65c[0x14];
    f32 damage_scale;
    u8 unknown_674[0x34];
    f32 tag_cooldown;
    u8 unknown_6ac[0x1c];
    f32 origin_y_offset;
    f32 force_glow_current;
    f32 force_glow_target_value;
    f32 force_glow_step;
    u8 unknown_6d8[4];
    f32 force_glow_hold_time;
    u8 unknown_6e0[0x10];
    f32 toggle_hold_timer;
    u8 unknown_6f4[0x44];
    f32 cheat_powerup_time;
    u8 unknown_73c[8];
    NUVEC cached_velocity;
    u8 unknown_750[0x1c];
    u32 runtime_flags_0;
    u32 runtime_flags_4;
    u8 unknown_774[0x24];
} PLAYERPACKET;

struct GameObject_s {
    GameObject_s *self;
    AISCRIPTPROCESS *ai_process;
    f32 animation_frame;
    f32 previous_animation_frame;
    u8 unknown_010[0x28];
    u8 animation_flags;
    u8 animation_suppressed;
    i16 fallback_animation_index;
    i16 suppressed_animation_index;
    i16 animation_index;
    i16 animation_state;
    i16 previous_animation_state;
    u8 unknown_044[0xc];
    GAMECHARACTERDATA *game_character_data;
    CHARACTERDATA *character_data;
    u16 heading;
    u16 desired_heading;
    NUVEC position;
    NUVEC velocity;
    NUVEC frame_offset;
    NUVEC origin;
    NUVEC old_position;
    NUVEC spawn_position;
    f32 model_scale;
    f32 scale;
    f32 collision_radius;
    f32 collision_half_height;
    f32 model_height;
    NUMTX transform;
    u8 unknown_0f8[0x80];
    NUVEC bounds_min;
    NUVEC bounds_max;
    NUVEC bounds_top_center;
    NUVEC bounds_bottom_center;
    NUVEC predicted_bottom_center;
    NUVEC last_safe_position;
    NUVEC position_history_0;
    NUVEC position_history_1;
    f32 jump_stuck_timer;
    f32 bounds_radius;
    f32 bounds_half_height;
    u32 self_mask_low;
    u32 self_mask_high;
    u32 collision_mask_low;
    u32 collision_mask_high;
    u32 object_flags;
    union {
        u32 state_flags;
        struct {
            u16 state_flags_low;
            u16 state_flags_high;
        };
    };
    NUVEC surface_normal;
    u8 unknown_208[0xc];
    f32 previous_floor_height;
    f32 floor_height;
    f32 edge_shadow_height;
    f32 roof_height;
    u8 unknown_224[0x52];
    u16 yaw;
    u8 unknown_278[2];
    i16 shadow_platform_id;
    i8 player_index;
    u8 ground_contact_flags;
    u8 previous_ground_contact_flags;
    i8 edge_surface_type;
    i8 roof_surface_type;
    i8 ground_surface_type;
    u8 under_water;
    u8 intersects_water;
    u8 visible;
    u8 draw_orientation_mode;
    u8 unknown_286;
    i8 death_state;
    u8 drawn_this_frame;
    u8 mask_bit_index;
    u8 unknown_28a[0x16];
    u32 opponent_mask_low;
    u32 opponent_mask_high;
    u32 in_range_mask_low;
    u32 in_range_mask_high;
    f32 idle_elapsed;
    f32 idle_timer;
    f32 idle_duration;
    i16 idle_action;
    i16 previous_idle_action;
    AISCRIPTPROCESS ai_script_process;
    u8 unknown_388[8];
    GameObject_s *ai_owner_object;
    u8 unknown_394[0x10];
    void *unknown_3a4;
    u8 unknown_3a8[0x4c];
    i8 ai_spawn_index;
    u8 unknown_3f5[5];
    u8 ai_lifecycle_state;
    u8 unknown_3fb[9];
    u8 ai_group_member_index;
    u8 unknown_405[3];
    NUVEC ground_position;
    AIPATHINFO path_info;
    u8 unknown_42c[0x234];
    u8 unknown_660;
    i8 sock_index;
    u8 unknown_662[0x52];
    PLAYERPACKET player_packet;
    MechTouchTask *task_list;
    MechObjectInterface *mech_object_interface;
    MechAddonCollection *addons;
    u8 unknown_e58[0x64];
    u32 seen_mask_low;
    u32 seen_mask_high;
    u32 can_see_mask_low;
    u32 can_see_mask_high;
    void *unknown_ecc;
    u8 unknown_ed0[0x2a];
    u8 portal_flags;
    u8 unknown_efb;
    u8 movement_flags;
    u8 unknown_efd;
    u8 head_movement_flags;
    u8 unknown_eff[9];
    void *usable_object;
    u8 use_action;
    u8 use_frame;
    u8 unknown_f0e[2];
    f32 use_distance;
    u8 unknown_f14[0xd0];
    NUVEC *head_target;
    NUVEC head_target_position;
    f32 head_target_time;
    f32 head_target_duration;
    f32 model_bottom;
    f32 model_top;
    f32 height_scale;
    f32 base_collision_radius;
    f32 spawn_protection_timer;
    f32 half_height_ratio;
    f32 death_elapsed;
    f32 death_duration;
    f32 removal_delay;
    f32 reflection_plane_y;
    f32 damage_cooldown;
    f32 camera_distance;
    u8 unknown_102c[0x1c];
    f32 interaction_timer;
    u8 unknown_104c[4];
    u32 secondary_flags;
    u32 layer_mask;
    u8 unknown_1058[0x18];
    i16 character_id;
    i16 previous_character_id;
    i16 previous_suit_index;
    i16 room_id;
    i16 ground_platform_id;
    i16 ignored_platform_id;
    i16 level_object_index;
    u8 unknown_107e[7];
    i8 terrain_contact_age;
    u8 unknown_1086[3];
    u8 animation_modifier_count;
    u8 maximum_hit_points;
    u8 hit_points;
    i8 head_target_priority;
    u8 unknown_108d[0x3b];
    NUVEC previous_safe_position;
    u8 unknown_10d4[0x10];

    MechObjectInterface *GetMechObjectInterface();
    void KillTasks();
    bool IsRunningTaskType(const HashedKey &key);
    void ClearMechObjectInterface();
    MechAddonCollection *GetAddons(bool create);
    void ClearAddons();
};

#if UINTPTR_MAX != UINT64_MAX
static_assert(sizeof(MechAddon) == 0x18, "MechAddon size");
static_assert(sizeof(MechAddonCollection) == 0x14, "MechAddonCollection size");
static_assert(sizeof(MechEdgeStopAddon) == 0x28, "MechEdgeStopAddon size");
static_assert(sizeof(MechAutofireAddon) == 0x1c, "MechAutofireAddon size");
static_assert(sizeof(PLAYERPACKET) == 0x798, "PLAYERPACKET size");
static_assert(sizeof(GameObject_s) == 0x10e4, "GameObject_s size");
static_assert(offsetof(PLAYERPACKET, coin_packet) == 0x138, "PLAYERPACKET coin packet offset");
static_assert(offsetof(PLAYERPACKET, balloon_height) == 0xb4, "PLAYERPACKET balloon height offset");
static_assert(offsetof(PLAYERPACKET, opponent) == 0xcc, "PLAYERPACKET opponent offset");
static_assert(offsetof(PLAYERPACKET, attack_target) == 0x604, "PLAYERPACKET attack target offset");
static_assert(offsetof(PLAYERPACKET, tag_cooldown) == 0x6a8, "PLAYERPACKET tag cooldown offset");
static_assert(offsetof(PLAYERPACKET, toggle_hold_timer) == 0x6f0, "PLAYERPACKET toggle hold timer offset");
static_assert(offsetof(PLAYERPACKET, unknown_638) == 0x638, "PLAYERPACKET unknown 638 offset");
static_assert(offsetof(PLAYERPACKET, force_glow_target) == 0x654, "PLAYERPACKET force glow target offset");
static_assert(offsetof(PLAYERPACKET, force_glow_current) == 0x6cc, "PLAYERPACKET force glow current offset");
static_assert(offsetof(GameObject_s, state_flags) == 0x1f8, "GameObject_s state_flags offset");
static_assert(offsetof(GameObject_s, surface_normal) == 0x1fc, "GameObject_s surface normal offset");
static_assert(offsetof(GameObject_s, ai_script_process) == 0x2c0, "GameObject_s AI process offset");
static_assert(offsetof(GameObject_s, unknown_3a4) == 0x3a4, "GameObject_s unknown 3a4 offset");
static_assert(offsetof(GameObject_s, ai_spawn_index) == 0x3f4, "GameObject_s AI spawn index offset");
static_assert(offsetof(GameObject_s, ai_lifecycle_state) == 0x3fa, "GameObject_s AI lifecycle state offset");
static_assert(offsetof(GameObject_s, ai_group_member_index) == 0x404, "GameObject_s AI group member offset");
static_assert(offsetof(GameObject_s, player_packet) == 0x6b4, "GameObject_s player_packet offset");
static_assert(offsetof(GameObject_s, task_list) == 0xe4c, "GameObject_s task_list offset");
static_assert(offsetof(GameObject_s, unknown_ecc) == 0xecc, "GameObject_s unknown ecc offset");
static_assert(offsetof(GameObject_s, movement_flags) == 0xefc, "GameObject_s movement flags offset");
static_assert(offsetof(GameObject_s, head_movement_flags) == 0xefe, "GameObject_s head movement flags offset");
static_assert(offsetof(GameObject_s, head_target) == 0xfe4, "GameObject_s head target offset");
static_assert(offsetof(GameObject_s, interaction_timer) == 0x1048, "GameObject_s interaction timer offset");
static_assert(offsetof(GameObject_s, secondary_flags) == 0x1050, "GameObject_s secondary flags offset");
static_assert(offsetof(GameObject_s, previous_safe_position) == 0x10c8, "GameObject_s previous_safe_position offset");
#endif

extern GameObject_s *Obj;
extern i32 HIGHGAMEOBJECT;
extern GameObject_s BackUpPlayers[2];
extern void (*GameObjectDimensionsExtraFn)(GameObject_s *object);
extern i32 (*LastSafePosExtraFn)(GameObject_s *object);

void GameObjectDimensions(GameObject_s *object);
GameObject_s *AddGameObject(i32 character_id);
void RemoveGameObject(GameObject_s *object, i32 unused);
void ScaleGameObject(GameObject_s *object);
void GetTopBot(GameObject_s *object);
void SetToLastSafePos(GameObject_s *object);
i32 UnderPlayerControl(GameObject_s *object);
GameObject_s *FindGameObject(i32 character_id, u32 object_flags, i32 alive_only, i32 vehicle_only, i32 non_level_only);
GameObject_s *FindNearestGameObject(NUVEC *position, GameObject_s *source, u32 character_flags, f32 maximum_distance,
                                    f32 radius_extra, i32 game_character_index, i32 character_id, i32 player_filter,
                                    f32 *distance_squared, i32 horizontal_only, i32 (*filter)(GameObject_s *),
                                    bool first_match);
i32 GameObjectNearFloor(GameObject_s *object, f32 distance, f32 *floor_delta);
void ClearGameObjects(APIOBJECTSYS *object_system);
void GameObjectToCameraDistances();
void GameObjectOrigin(GameObject_s *object);
void PortalGameObject(GameObject_s *object, i32 enable, i32 secondary, i16 room_id, NUGSCN *scene);
void SnapCreaturePos(GameObject_s *object, NUVEC *position, i32 heading, AIPATHINFO *path_info, i32 set_on_surface);
void UpdateLastSafePosition(GameObject_s *object);
void GameObjectSetCanUse(GameObject_s *object, void *usable_object, u8 action, u8 unused, f32 distance);
void SetFlicker(GameObject_s *object, f32 time);
void SetGameObjectCharacterData(GameObject_s *object);
void SetHitPoints(GameObject_s *object, i32 hit_points);
void Player_CopyEssentials(GameObject_s *source, GameObject_s *destination);
void ClearAICreatures();
void DeactivateGameObject(GameObject_s *object);
void DeactivateCharacter(char *name);
i32 GetDefaultIdle(GameObject_s *object);
void ResetCharacterIdle(GameObject_s *object, i32 reset, i32 action);
void UpdateCharacterIdle(GameObject_s *object);
void AddFootSteps(GameObject_s *object);
void SetHeadTarget(GameObject_s *object, NUVEC *target, i8 priority, f32 time, f32 minimum_duration,
                   f32 maximum_duration);
void SetObjAsHeadTarget(GameObject_s *object, GameObject_s *target, i8 priority, f32 time, f32 minimum_duration,
                        f32 maximum_duration);
void HeadMovement(GameObject_s *object);
void CloakMovement(GameObject_s *object);
void HairMovement(GameObject_s *object);
