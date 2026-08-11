#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct GameObject_s;
struct GAMEMESSAGE_s;

struct ADDGAMEMSG_s {
    u8 unknown_00[0x38];
    void (*end_delay)(GAMEMESSAGE_s *message);
};

ADDGAMEMSG_s AddGameMsg_Default = {{0}, NULL};
GameObject_s *(*GameMsg_GetExtraObjFn)(GAMEMESSAGE_s *message);

void AddGameMessage(char *, NUVEC *, f32, NUVEC *, f32, u8, u8, u8, u32, f32) {
}
