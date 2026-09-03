#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void StarWars_AutoSetAICapabilities(GameObject_s *object);

void InitPlayerAI(GameObject_s *object) {
    // This is the first operation in the target initializer.  Path traversal
    // reads these flags immediately when the creature starts processing AI.
    StarWars_AutoSetAICapabilities(object);
}
