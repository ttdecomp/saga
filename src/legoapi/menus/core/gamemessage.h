#ifndef LEGOAPI_MENUS_CORE_GAMEMESSAGE_H
#define LEGOAPI_MENUS_CORE_GAMEMESSAGE_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// In-world HUD message spawner.

void *AddGameMessage(char *text, nuvec_s *pos, float a, nuvec_s *b, float c, unsigned char r, unsigned char g,
                     unsigned char blue, u32 flags, float unknown);

#endif
