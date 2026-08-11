#include "legoapi/touchhacks.h"

#include <math.h>

#include "legoapi/area.h"
#include "legoapi/bolts.h"
#include "legoapi/gamepads.h"
#include "legoapi/qrand.h"
#include "legoapi/world.h"
#include "nu2api/numath/nufloat.h"

struct TouchCharacterRuntime {
    u8 unknown_000[0x94];
    u8 unknown_094_0 : 3;
    u8 shooting_disabled : 1;
    u8 unknown_094_4 : 4;
    u8 unknown_095[2];
    i8 poo_flags;
    u8 unknown_098[0x7e];
    u8 can_lunge;
};

struct TouchCharacterData {
    u8 unknown_00[4];
    u32 flags;
    u8 unknown_08[0x1c];
    TouchCharacterRuntime *runtime;
};

struct TouchGameCharacterData {
    u8 unknown_00[0xc];
    void **actions;
};

struct GIZMOBLOWUP_s {
    u8 unknown_000[0x50];
    NUVEC position;
    u8 unknown_05c[0x40];
    u8 state_flags;
    u8 unknown_09d;
    i8 active;
    u8 unknown_09f;
    u32 flags;
    u8 unknown_0a4[8];
    void *object;
    u8 unknown_0b0[0x7c];

    MechObjectInterface *GetMechObjectInterface();
};

struct GIZFORCE_s {
    u8 unknown_000[0x28];
    u8 *type;
    u8 unknown_02c[0xc];
    i32 state_38;
    i32 state_3c;
    u8 *link;
    u8 unknown_044[0x34];
    u32 flags_78;
    u8 unknown_07c[0x2e];
    u8 flags_aa;
};

struct TouchMovementRuntime {
    u8 unknown_000[0x24];
    f32 gravity;
    u8 unknown_028[4];
    f32 jump_velocity;
};

struct TouchTerrainEntry {
    u32 unknown_0;
    u32 flags;
    u32 unknown_8;
};

struct AIPATHNODE_s {
    u8 unknown_00[4];
    NUVEC position;
    f32 radius;
};

struct GAMECAMERA_s;

#if UINTPTR_MAX != UINT64_MAX
static_assert(offsetof(TouchCharacterRuntime, poo_flags) == 0x97, "poo flags offset");
static_assert(offsetof(TouchCharacterRuntime, can_lunge) == 0x116, "lunge flag offset");
static_assert(offsetof(TouchCharacterData, flags) == 0x4, "character flags offset");
static_assert(offsetof(TouchCharacterData, runtime) == 0x24, "character runtime offset");
static_assert(offsetof(TouchGameCharacterData, actions) == 0xc, "character actions offset");
#endif

extern i16 id_ATST;
extern i16 id_ATST_LOWRES;
extern i16 id_ATAT;
extern i16 id_BODYGUARD;
extern i16 id_EWOK;
extern i16 id_GONKDROID;
extern i16 id_GRIEVOUS;
extern i16 id_GRABCONTROL;
extern i16 id_LUKESKYWALKERDAGOBAH;
extern i16 id_SKELETON;
extern i16 id_STAP;
extern i16 id_STAP2;
extern i16 id_WATTO;
extern i16 id_WICKET;
extern i16 LEGOACT_BUILD;
extern i16 LEGOACT_LUNGE;
extern i16 LEGOACT_SLAM;
extern u8 Cheat[];
extern i32 BonusArea;
extern i32 VehicleArea;
extern f32 AreaPickupScale;
extern GameObject_s *Player[8];
extern GameObject_s *player;
extern GAMECAMERA_s *GameCam;
extern NUCOLOUR3 flashCol;
extern u8 *CInfo;
extern f32 NuTrigTable[];
extern TouchTerrainEntry TerLayer[];
extern LEVELDATA_s *SPEEDERCHASEA_LDATA;
extern GAMECHARACTERDATA *GCDataList;
extern CHARACTERDATA *CDataList;

extern "C" void PlaySfxAndSetVolume(char *name, NUVEC *position, f32 volume);
extern "C" i32 PlaySfx(const char *name, NUVEC *position);
void AddExplosion(NUVEC *position, f32 radius, f32 scale, i32 unknown, i32 character_id, i32 flags);
extern "C" i32 AnimPlaying(void *object_instance, i32 animation, i32 unknown1, i32 unknown2);
i32 Cheat_IsOn(i32 cheat);
i32 InCollectList_Index(i32 id, i16 *list, i32 count);
GameObject_s **GameObjIsCableTied(GameObject_s *object);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 distance, i32 flags);
void GameCam_Judder(GAMECAMERA_s *camera, f32 strength, i32 unknown, NUVEC *position);
void GameCam_NewShake(GAMECAMERA_s *camera, f32 x, f32 y, f32 time);
void Hint_SetComplete(i32 hint_id);
i32 GameRayCast(NUVEC *origin, NUVEC *direction, f32 distance, i32 flags);
extern "C" i32 EShadowInfo();
extern "C" void NewRayCastGetImpactNormal(NUVEC *normal);
void Move_CHARACTER();
void Move_DEFAULT();
void Move_DROIDGENERIC();
void Move_GEONOSIAN();
void Move_JAWA();
void Move_JEDI();
void Move_WEIRDO();
i32 GizForce_Complete(GIZFORCE_s *force);
i32 GizForce_StoodOnForce(GIZFORCE_s *force, GameObject_s *object);
i32 SuperWeirdo(GameObject_s *object);
i32 ObjLandReady(GameObject_s *object);

namespace TouchHacks {

    static TouchCharacterData *GetCharacterData(GameObject_s &object) {
        return (TouchCharacterData *)object.character_data;
    }

    static TouchGameCharacterData *GetGameCharacterData(GameObject_s &object) {
        return (TouchGameCharacterData *)object.game_character_data;
    }

    i32 GetLoseStudsDieValue() {
        return BonusArea != 0 ? 10000 : 1000;
    }

    i32 GetLoseStudsFallValue() {
        return 0;
    }

    bool ShouldAutoGrabDragBomb(GameObject_s &object) {
        if (object.character_id != id_ATST && object.character_id != id_ATST_LOWRES) {
            return TouchControlsActive;
        }
        return false;
    }

    f32 GetIncomingPartRange() {
        return 16.0f;
    }

    bool ShouldBlock(GameObject_s &object) {
        if (TouchControlsActive && object.player_packet.unknown_638 != NULL && object.player_index == -1) {
            return qrand() > 14999;
        }
        return true;
    }

    bool AiPlayerTakeDamageOnKillRescue(GameObject_s &) {
        return TouchControlsActive;
    }

    bool CanUseVehicleSmartBomb(GameObject_s &object) {
        TouchCharacterData *character = GetCharacterData(object);
        return Cheat_IsOn(0x14) != 0 && (i8)object.state_flags < 0 && (character->flags & 0x2000) != 0 &&
               InCollectList_Index(object.character_id, NULL, 0) != -1;
    }

    void TriggerVehicleSmartBomb(GameObject_s &object) {
        AddExplosion(&object.origin, AreaPickupScale * 7.5f, 1.0f, 0, -1, 0x4021);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);

        f32 judder = qrand() < 0x8000 ? -1.5f : 1.5f;
        GameCam_Judder(GameCam, judder, 2, NULL);
        GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
        PlaySfx("Explode1", NULL);
        Hint_SetComplete(0x5e1);
    }

    bool CanThrowBountyBomb(GameObject_s &object) {
        u8 *game_state = *(u8 **)((u8 *)WORLD + 0x2ac0);
        u8 special_state = *((u8 *)&object + 0x108e);
        u8 takeover_state = *((u8 *)&object + 0xe31);
        TouchCharacterData *character = GetCharacterData(object);
        if (game_state[0xe9e] == 0 || (i8)object.state_flags >= 0 ||
            ((character->flags & 0x1000000) == 0 && special_state != 6 && SuperWeirdo(&object) == 0) ||
            (object.ground_contact_flags == 0 && takeover_state != 1)) {
            return false;
        }

        i8 character_state = object.player_packet.character_state;
        if (character_state == 6 || character_state == -1 || character_state == 7) {
            return true;
        }
        return (CInfo[character_state * 0x10 + 8] & 4) != 0;
    }

    bool CheckJumpForLandingSpot(GameObject_s &object, f32 distance) {
        NUVEC position = object.position;
        f32 minimum_y = object.position.y - distance;
        TouchMovementRuntime *runtime = (TouchMovementRuntime *)GetCharacterData(object)->runtime;
        f32 velocity_y = runtime->jump_velocity + object.velocity.y;

        do {
            NUVEC next_position = {position.x + object.velocity.x * 0.2f, position.y + velocity_y * 0.2f,
                                   position.z + object.velocity.z * 0.2f};
            NUVEC direction = {next_position.x - position.x, next_position.y - position.y,
                               next_position.z - position.z};
            if (GameRayCast(&position, &direction, 0.0f, 0) != 0) {
                NUVEC normal = {0.0f, 0.0f, 0.0f};
                NewRayCastGetImpactNormal(&normal);
                if (normal.y > 0.8f && GameShadow(&object, &position, 5.0f, -1) != 2000000.0f) {
                    i32 terrain = EShadowInfo();
                    if (terrain < 17 && (TerLayer[terrain].flags & 1) == 0) {
                        return true;
                    }
                }
            }

            velocity_y += runtime->gravity * 0.2f;
            position = next_position;
        } while (minimum_y <= position.y);

        return false;
    }

    bool CheckForAboutToRunIntoKillTerrain(GameObject_s &object, f32 distance) {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA) {
            return false;
        }

        NUVEC position = {object.position.x + object.velocity.x * distance, object.position.y + 0.3f,
                          object.position.z + object.velocity.z * distance};
        if (GameShadow(&object, &position, 5.0f, -1) == 2000000.0f) {
            return false;
        }

        i32 terrain = EShadowInfo();
        if (terrain > 16 || (TerLayer[terrain].flags & 1) == 0) {
            return false;
        }

        NUVEC direction = {object.velocity.x, 0.0f, object.velocity.z};
        NuVecNorm(&direction, &direction);
        f32 radius = object.collision_radius * 0.8f;
        position.x += direction.x * radius;
        position.z += direction.z * radius;

        if (GameShadow(&object, &position, 5.0f, -1) == 0.0f) {
            return true;
        }
        terrain = EShadowInfo();
        return terrain > 16 || (TerLayer[terrain].flags & 1) != 0;
    }

    bool CheckForAboutToRunOffAnEdge(GameObject_s &object, f32 distance) {
        NUVEC position = {object.position.x + object.velocity.x * distance, object.position.y + 0.3f,
                          object.position.z + object.velocity.z * distance};
        f32 minimum_height = object.position.y - 0.3f;
        if (GameShadow(&object, &position, 5.0f, -1) >= minimum_height) {
            return false;
        }

        NUVEC direction = {object.velocity.x, 0.0f, object.velocity.z};
        NuVecNorm(&direction, &direction);
        f32 radius = object.collision_radius * 0.8f;
        position.x += direction.x * radius;
        position.z += direction.z * radius;
        return GameShadow(&object, &position, 5.0f, -1) < minimum_height;
    }

    bool SolveRoot(f32 a, f32 b, f32 c, f32 &root1, f32 &root2) {
        if (a != 0.0f) {
            f32 discriminant = b * b - a * 4.0f * c;
            if (discriminant >= 0.0f) {
                f32 square_root = NuFsqrt(discriminant);
                root1 = (-b - square_root) / (a + a);
                root2 = (square_root - b) / (a + a);
                return true;
            }
        }
        return false;
    }

    VuVec CalculateXZVelForArcToHitPoint(const VuVec &origin, const VuVec &target, f32 vertical_velocity, f32 gravity) {
        f32 root1;
        f32 root2;
        f32 vertical_distance = -fabsf(origin.y - target.y);
        if (SolveRoot(gravity * 0.5f, vertical_velocity, -vertical_distance, root1, root2)) {
            return VuVec((target.x - origin.x) / root1, 0.0f, (target.z - origin.z) / root1, 0.0f);
        }
        return VuVec_Zero;
    }

    VuVec CalculateJumpVelToHitPoint(GameObject_s &object, const VuVec &target) {
        VuVec origin(object.position.x, object.position.y, object.position.z, 1.0f);
        TouchMovementRuntime *runtime = (TouchMovementRuntime *)GetCharacterData(object)->runtime;
        return CalculateXZVelForArcToHitPoint(origin, target, runtime->jump_velocity, runtime->gravity);
    }

    VuVec CalculateJumpVelToHitPointDblJump(GameObject_s &object, const VuVec &target) {
        TouchMovementRuntime *runtime = (TouchMovementRuntime *)GetCharacterData(object)->runtime;
        TouchMovementRuntime *player_runtime = (TouchMovementRuntime *)GetCharacterData(*player)->runtime;
        f32 height = -((-(player_runtime->jump_velocity * player_runtime->jump_velocity) /
                        (player_runtime->gravity + player_runtime->gravity)) *
                       0.7f);
        f32 first_root1;
        f32 first_root2;
        f32 second_root1;
        f32 second_root2;
        if (SolveRoot(runtime->gravity * 0.5f, runtime->jump_velocity, height, first_root1, first_root2) &&
            SolveRoot(runtime->gravity * 0.5f, runtime->jump_velocity, height, second_root1, second_root2)) {
            f32 duration = first_root1 + second_root1;
            return VuVec((target.x - object.position.x) / duration, 0.0f, (target.z - object.position.z) / duration,
                         0.0f);
        }
        return VuVec_Zero;
    }

    bool CanJumpToPoint(GameObject_s &object, const VuVec &target) {
        TouchMovementRuntime *runtime = (TouchMovementRuntime *)GetCharacterData(object)->runtime;
        f32 root1;
        f32 root2;
        if (!SolveRoot(runtime->gravity * 0.5f, runtime->jump_velocity, -(target.y - object.position.y), root1,
                       root2)) {
            return false;
        }

        f32 x = target.x - object.position.x;
        f32 z = target.z - object.position.z;
        f32 maximum_distance = root1 * 1.4f;
        return x * x + z * z < maximum_distance * maximum_distance;
    }

    bool CanJumpToPoint(GameObject_s &object, const AIPATHNODE_s &node) {
        NUVEC direction = {node.position.x - object.position.x, node.position.y - object.position.y,
                           node.position.z - object.position.z};
        f32 distance = NuVecMag(&direction) - node.radius;
        NuVecNorm(&direction, &direction);
        VuVec target(direction.x * distance + object.position.x, direction.y * distance + object.position.y,
                     direction.z * distance + object.position.z, 1.0f);
        return CanJumpToPoint(object, target);
    }

    bool CanPoo(GameObject_s &object) {
        TouchCharacterData *character = GetCharacterData(object);
        return character->runtime->poo_flags < 0 && object.player_packet.character_state == -1 &&
               object.ground_contact_flags != 0 && (i8)object.state_flags < 0 &&
               (Cheat[0x28] != 0 || Cheat[0x128] != 0);
    }

    void PlaySmartBombBuildupEffects(GameObject_s &, f32 elapsed, f32 duration) {
        PlaySfxAndSetVolume("JForceUse", NULL, elapsed * 3.0f / duration);
    }

    bool CanLunge(GameObject_s &object) {
        TouchCharacterData *character = GetCharacterData(object);
        if (character->runtime->can_lunge == 0 && (character->flags & 8) == 0) {
            return false;
        }
        return LEGOACT_LUNGE != -1 && GetGameCharacterData(object)->actions[LEGOACT_LUNGE] != NULL;
    }

    bool CanSlam(GameObject_s &object) {
        return LEGOACT_SLAM != -1 && GetGameCharacterData(object)->actions[LEGOACT_SLAM] != NULL;
    }

    bool CanShoot(GameObject_s &object) {
        TouchCharacterData *character = GetCharacterData(object);
        return (character->flags & 0x10000000) != 0 && !character->runtime->shooting_disabled;
    }

    bool CanUseTeleport(GameObject_s &object) {
        TouchCharacterData *character = GetCharacterData(object);
        if (character == NULL) {
            return false;
        }
        if ((character->flags & 0x40000) == 0 && SuperWeirdo(&object) == 0) {
            return false;
        }
        return true;
    }

    bool CanUseZipup(GameObject_s &object) {
        if (object.character_data == NULL || ObjLandReady(&object) == 0) {
            return false;
        }

        TouchCharacterData *character = GetCharacterData(object);
        TouchCharacterRuntime *runtime = character->runtime;
        return (character->flags & 0x100000) != 0 || SuperWeirdo(&object) != 0 ||
               ((character->flags & 8) != 0 && (i8)((u8 *)runtime)[0x95] >= 0 && Cheat_IsOn(0xd) != 0);
    }

    bool ShouldKeepWeaponOut(GameObject_s &object) {
        bool keep_weapon_out = false;
        if (TouchControlsActive && object.character_id != id_GRABCONTROL && (i8)object.state_flags < 0 &&
            object.unknown_3a4 != NULL) {
            keep_weapon_out = object.player_packet.character_state == -1;
        }
        return keep_weapon_out;
    }

    bool ShouldPutWeaponAway(GameObject_s &object) {
        f32 weapon_out_time = *(f32 *)((u8 *)&object + 0xd3c);
        u8 takeover_state = *((u8 *)&object + 0xe31);
        return TouchControlsActive && object.character_id != id_GRABCONTROL && object.character_id != id_WICKET &&
               object.character_id != id_EWOK && (i8)object.state_flags < 0 && object.unknown_3a4 == NULL &&
               weapon_out_time > 5.0f && object.player_packet.character_state == -1 && takeover_state != 1;
    }

    bool CanJump(GameObject_s &object) {
        f32 jump_timer = *(f32 *)((u8 *)&object + 0xd18);
        if ((object.ground_contact_flags == 0 && jump_timer <= 0.0f) || object.game_character_data == NULL ||
            ObjLandReady(&object) == 0) {
            return false;
        }

        if (GetGameCharacterData(object)->actions[6] != NULL || (GetCharacterData(object)->flags & 0x40) != 0 ||
            object.character_id == id_WATTO) {
            return true;
        }
        return object.character_id == id_GONKDROID && Cheat_IsOn(8) != 0;
    }

    bool CanUseBuildIt(GameObject_s &object) {
        if (LEGOACT_BUILD == -1 || object.game_character_data == NULL ||
            GetGameCharacterData(object)->actions[LEGOACT_BUILD] == NULL ||
            AnimPlaying((u8 *)&object + 8, LEGOACT_BUILD, 1, 1) != 0 || object.ground_contact_flags == 0) {
            return false;
        }
        return ObjLandReady(&object) != 0;
    }

    bool CanUseGizForce(GameObject_s &object) {
        TouchCharacterData *character = GetCharacterData(object);
        return character != NULL && ((character->flags >> 3) & 1) != 0;
    }

    bool CanUseGizForce(GameObject_s &object, GIZFORCE_s &force) {
        if (force.state_38 != 0 || force.state_3c != 0 || (force.flags_aa & 1) != 0) {
            return false;
        }

        TouchCharacterData *character = GetCharacterData(object);
        bool can_override = SuperWeirdo(&object) != 0 || ((i8)object.state_flags < 0 && Cheat_IsOn(0x19) != 0);
        if ((force.flags_78 & 0x10) != 0 && (character->flags & 4) == 0 && !can_override) {
            return false;
        }

        if (force.link == NULL) {
            if (GizForce_Complete(&force) == 0) {
                return GizForce_StoodOnForce(&force, &object) == 0;
            }
            return false;
        }

        u8 count = force.link[0x25];
        GIZFORCE_s *linked_force = count == 0 ? NULL : *(GIZFORCE_s **)(force.link - 4 + count * 4);
        if (count == 0 || linked_force == &force) {
            return GizForce_StoodOnForce(&force, &object) == 0;
        }
        if ((force.link[0x24] & 1) != 0 || *(i32 *)(force.type + 0xc) == 2) {
            return false;
        }
        if (linked_force == NULL) {
            return GizForce_StoodOnForce(&force, &object) == 0;
        }
        if ((linked_force->type[0xa] & 7) != 0) {
            return false;
        }
        if (linked_force->state_38 == 0) {
            return GizForce_StoodOnForce(&force, &object) == 0;
        }
        return false;
    }

    bool CanTagTo(GameObject_s &object, GameObject_s &target) {
        if (&object == &target || (target.state_flags_low & 0x1001) != 0x1001 || target.death_state != 0 ||
            (*((u8 *)&target + 0x7b5) & 2) != 0) {
            return false;
        }

        i8 character_state = target.player_packet.character_state;
        if (character_state == 0x3d || character_state == 0x17 || (CInfo[character_state * 0x10 + 9] & 0x80) != 0 ||
            ((*((u8 *)&target + 0xf00) & 2) != 0 && player->character_id != id_LUKESKYWALKERDAGOBAH) ||
            target.visible == 0 ||
            ((target.object_flags & 5) != 0 &&
             (HUB_ADATA == NULL || (void *)HUB_ADATA != (void *)WORLD->current_level)) ||
            (i32)target.object_flags < 0) {
            return false;
        }

        f32 height_range = target.collision_half_height;
        if (height_range <= object.collision_half_height) {
            height_range = object.collision_half_height;
        }
        if (fabsf(target.position.y - object.position.y) > height_range) {
            return false;
        }

        f32 x = object.position.x - target.position.x;
        f32 y = object.position.y - target.position.y;
        f32 z = object.position.z - target.position.z;
        return x * x + y * y + z * z <= 4.0f;
    }

    bool CanToggleTo(GameObject_s &object, i32 character_id) {
        if (object.character_id == character_id) {
            return false;
        }

        i8 character_state = object.player_packet.character_state;
        if ((CInfo[character_state * 0x10 + 9] & 1) != 0) {
            return false;
        }

        f32 target_height = *(f32 *)((u8 *)GCDataList + character_id * 0x120 + 0x28);
        if (object.edge_surface_type <= 16 && (TerLayer[object.edge_surface_type].flags & 1) != 0 &&
            target_height <= 0.0f) {
            return false;
        }

        void *move = *(void **)((u8 *)object.character_data + 0x18);
        if (object.floor_height == 2000000.0f || object.roof_height == 2000000.0f || move == (void *)Move_DEFAULT) {
            return true;
        }

        CHARACTERDATA *character = &CDataList[character_id];
        return character->field16_0x38 - character->field15_0x34 < object.roof_height - object.floor_height;
    }

    bool CanTagVehicle(GameObject_s &object, GameObject_s &target) {
        void *move = *(void **)((u8 *)object.character_data + 0x18);
        if (move != (void *)Move_CHARACTER && move != (void *)Move_WEIRDO && move != (void *)Move_JEDI &&
            move != (void *)Move_DROIDGENERIC && move != (void *)Move_JAWA && move != (void *)Move_GEONOSIAN) {
            return false;
        }

        TouchCharacterData *target_character = GetCharacterData(target);
        if ((target_character->flags & 0x40000000) != 0 &&
            (target.player_packet.character_state == 0x3e || target.player_packet.character_state == 0x17)) {
            return false;
        }

        TouchCharacterData *character = GetCharacterData(object);
        if (target.player_packet.takeover_object != NULL || (character->flags & 0x10) != 0 ||
            object.character_id == id_SKELETON || object.character_id == id_GRIEVOUS ||
            object.character_id == id_BODYGUARD) {
            return false;
        }

        bool ignore_height = target.character_id == id_ATST || target.character_id == id_ATST_LOWRES ||
                             target.character_id == id_ATAT || target.character_id == id_STAP ||
                             target.character_id == id_STAP2;
        f32 y = object.position.y - target.position.y;
        if (!ignore_height && fabsf(y) > object.collision_half_height) {
            return false;
        }

        f32 x = object.position.x - target.position.x;
        f32 z = object.position.z - target.position.z;
        return x * x + y * y + z * z <= 4.0f;
    }

    MechObjectInterface *FindBombTarget(GameObject_s &object) {
        f32 forward_x = NuTrigTable[object.heading >> 1];
        f32 forward_z = NuTrigTable[(object.heading + 0x4000) >> 1 & 0x7fff];
        GIZMOBLOWUP_s *blowups = *(GIZMOBLOWUP_s **)((u8 *)WORLD + 0x50cc);
        i32 count = *(i32 *)((u8 *)WORLD + 0x50c4);
        if (blowups == NULL || count <= 0) {
            return NULL;
        }

        MechObjectInterface *best = NULL;
        f32 best_dot = 0.5f;
        for (i32 i = 0; i < count; i++) {
            GIZMOBLOWUP_s &blowup = blowups[i];
            if (blowup.active >= 0 || blowup.object == NULL || (blowup.state_flags & 1) != 0 ||
                ((*((u8 *)blowup.object + 0x7f) & 2) == 0)) {
                continue;
            }

            NUVEC direction = {blowup.position.x - object.position.x, blowup.position.y - object.position.y,
                               blowup.position.z - object.position.z};
            if (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z >= 90000.0f) {
                continue;
            }

            NuVecNorm(&direction, &direction);
            f32 dot = direction.x * forward_x + direction.z * forward_z;
            if (dot > best_dot) {
                best = blowup.GetMechObjectInterface();
                best_dot = dot;
            }
        }
        return best;
    }

    bool CanForceTargetObj(GameObject_s &object, GameObject_s &target) {
        return !TouchControlsActive || !CanTagTo(object, target);
    }

    bool InParty(GameObject_s &object) {
        return (Player[0] != NULL && Player[0] == &object) || (Player[1] != NULL && Player[1] == &object) ||
               (Player[2] != NULL && Player[2] == &object) || (Player[3] != NULL && Player[3] == &object) ||
               (Player[4] != NULL && Player[4] == &object) || (Player[5] != NULL && Player[5] == &object) ||
               (Player[6] != NULL && Player[6] == &object) || (Player[7] != NULL && Player[7] == &object);
    }

    bool ShouldFlash(f32 time) {
        return time > 0.0f && NuFmod(time, 0.3f) < 0.15f;
    }

    NUCOLOUR3 *GetFlashColour() {
        return &flashCol;
    }

    bool ShouldDeflectBolt(GameObject_s &object, BOLT_s &bolt) {
        GameObject_s **cable_tie;
        bool should_deflect;
        if (!TouchControlsActive || VehicleArea == 0 ||
            (object.character_id != id_ATST && object.character_id != id_ATST_LOWRES) || bolt.source == NULL ||
            (i8)bolt.source->state_flags >= 0 || (cable_tie = GameObjIsCableTied(&object)) == NULL) {
            should_deflect = false;
        } else {
            should_deflect = *cable_tie == bolt.source;
        }
        return should_deflect;
    }

    bool CanBlowupBeBlownUp(GIZMOBLOWUP_s &blowup, i32 hit_type) {
        return hit_type != 1 || ((blowup.flags >> 7) & 1) != 0;
    }

    bool CanUseLever(GameObject_s &object) {
        return GetGameCharacterData(object)->actions[0x5d] != NULL && object.ground_contact_flags != 0;
    }

    bool CanUseHatMachine(GameObject_s &object) {
        return GetGameCharacterData(object)->actions[0x5d] != NULL && object.ground_contact_flags != 0 &&
               ObjLandReady(&object) != 0;
    }

} // namespace TouchHacks
