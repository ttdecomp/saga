#include "legoapi/players.h"

struct COINPACKET_s;

struct PlayerProgressEntry {
    u8 data[0x10];
};

i32 FreePlay;
i32 Arcade;
i32 GAMEDEMO;
i32 NOAICREATURES;
i32 BonusWinner = -1;
i32 PLAYERCOUNT;
i32 PlayerID[2];
i32 waiting_for_character = -1;
i32 apiloadcharactermodels_append;
i32 apiloadcharactermodels_nopakfile;
i32 CHARPAK;
void *characterbuffer_ptr;
void *characterbuffer_end;
i32 LEVELOBJECTCOUNT;
u8 ObjTab[1];
char *save_icon[1];
void *CharacterCustomiser;
i32 hub_character_ready;
i32 makeplayerlist_freeplay;
i16 FreePlayModelList[8];
i16 PlayerList[8];
PlayerProgressEntry PlayerProgress[8];
void *PlayerSuit[8];
u8 PlayerTorpedoCount[8];
u8 *Game_CharacterSave;
i16 tUNKNOWN;
u32 (*SpecialRouteCharacterTypeIDFn)(i32 character_type);
NUVEC plr_lastpos;

void FreeTorpedoPacket(void **packet) {
    if (packet != NULL) {
        *packet = NULL;
    }
}

void InitCreature(GameObject_s *, i32, i32) {}
void ResetPlayer(GameObject_s *, i32, NUVEC *, i32) {}
void ResetCoinPacket(COINPACKET_s *) {}
void LightGameObject(GameObject_s *, void *) {}
void Customiser_SaveModelTextureIDs(void *, void *) {}
void StarWars_AutoSetAICapabilities(GameObject_s *) {}
void SetProtocolDroidDeactivatedAction(GameObject_s *) {}
void Arcade_PlayerKilled(i32, i32) {}
void LoseHelmet(GameObject_s *, i32, i32) {}
void DestroySnakeBody(GameObject_s *) {}
void GizForce_ResetLOS(GameObject_s *) {}
void Whip_Release(GameObject_s *) {}
void SuperCarry_Release(GameObject_s *) {}
void SpecialMove_ReleaseVictim(GameObject_s *) {}

void *Suit_GetDefault(i32) {
    return NULL;
}

i32 LoseCoins(GameObject_s *, i32) {
    return 0;
}

struct MISSIONSYS_s;

i32 Mission_Active(MISSIONSYS_s *) {
    return 0;
}

extern "C" void ComplexSockPosition(void *, NUVEC *position, i32, u32, SOCKPOSITION_s *sock_position) {
    if (sock_position != NULL && position != NULL) {
        sock_position->mid_position = *position;
        sock_position->camera_position = *position;
    }
}
