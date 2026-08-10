#pragma once

#include "legoapi/gameobject.h"

struct BOLT_s;
struct LEVEL_PROGRESS_s;
struct bgprocinfo_s;

struct SOCKPOSITION_s {
    u8 locator_index;
    i8 sock_index;
    i16 sock_vertex;
    u32 socket_data;
    NUVEC mid_position;
    NUVEC camera_position;
    f32 socket_distance;
    u16 yaw;
    u16 pitch;
    u8 unknown_28[2];
    u8 candidate_count;
    u8 flags;
    u8 unknown_2c[8];
    u32 sock_mask;
};

extern GameObject_s *Player[8];
extern GameObject_s *player;
extern GameObject_s *player2;

void AICreatureResumeScript(GameObject_s *object);
void FixUpCharacters(CHARFIXUP *fixups);
void ExtraCharacterFixUpAfterConfig();
u32 MakePlayerList(i32 count);
void ResetPlayerPacket(PLAYERPACKET *packet, CHARACTERDATA *character_data);
void ResetPlayerAI(GameObject_s *object);
void InitPlayerAI(GameObject_s *object);
void StoreProgressAICharacter(LEVEL_PROGRESS_s *progress);
GameObject_s *AddCreature(i32 character_id, i32 initialize_ai);
void InitAICreatures(AISYS *ai_system);
void ResetPlayerMoves(GameObject_s *object);
void ResetAICreatures(AISYS *ai_system);
GameObject_s *ActivateCharacter(char *name, NUVEC *position, i32 heading);
void SetPlayerGroupPosition(f32 x, f32 y, f32 z);
void DrawWeapon_SetSabreObjects(GameObject_s *object, i32 right_sabre, i32 left_sabre, i32 right_blaster,
                                i32 left_blaster, i32 *objects, i32 *weapon_objects);
void SetLayers_BOB(GameObject_s *object);
void SetLayers_MOSEISLEYCITIZEN(GameObject_s *object);
void LoadSingleCharacter(bgprocinfo_s *process);
void TerrainPlayer(GameObject_s *object);
void AnimatePlayer(GameObject_s *object);
NUVEC *Player_StartPos(GameObject_s *object);
void SetPlayer();
GameObject_s *Player_FindByID(i32 id);
void RememberPlayerIDs(i32 force, i32 first_id, i32 second_id);
i32 Players_AveragePos(NUVEC *position, SOCKPOSITION_s *sock_position);
GameObject_s *GetOtherActivePlayer(GameObject_s *object);
i32 ActivePlayerInRange(NUVEC *position, f32 distance_squared, f32 *actual_distance_squared);
i32 AvailableToPlayer(u32 character_flags, i32 character_type, i32 detail, i32 require_all);
bool FindNearestPlayerToVec(NUVEC *position, GameObject_s **nearest, f32 &distance_squared, bool check_character_flags,
                            u32 character_flags);
bool FindFurthestPlayerFromVec(NUVEC *position, GameObject_s **furthest, f32 &distance_squared,
                               bool check_character_flags, u32 character_flags);
void FindPlayerAndSetWeapon(i32 id, i32 enabled);
void PlayerTakeHit(GameObject_s *object, GameObject_s *attacker);
void ActivatePlayer(GameObject_s *object);
i32 DeactivatePlayer(GameObject_s *object, f32 duration, GameObject_s *attacker);
i32 KillPlayer(GameObject_s *object, i32 reason, i32 immediate, NUVEC *direction = NULL);
i32 NewPlayerCharacter(GameObject_s *object, i32 character_id, i32 previous_character_id, i32 unused = 0);
bool CanPullLevers(i32 character_id);
bool CanWearHatsInFreePlay(i32 character_id);
void Player_ClearContext(GameObject_s *object, i32 context);
bool MovingBackwards(GameObject_s *object);
bool Players_BothActive();
i32 Player_HasInvincibility(GameObject_s *object);
i32 Player_HasFastBuild(GameObject_s *object);
i32 Player_HasDeflectBolts(GameObject_s *object);
i32 Player_HasDoubleBoltDamage(GameObject_s *object);
i32 Player_HasDoubleBoltDamage_FromBolt(BOLT_s *bolt);
i32 Player_HasDoubleWeaponDamage(GameObject_s *object);
bool Player_HasPurpleForce(GameObject_s *object);
