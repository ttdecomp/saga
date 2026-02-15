#include "gameapi/ai/aisys/aisys.h"

#include "nu2api.saga/nucore/nulist.h"
#include "nu2api.saga/nucore/nustring.h"

AISTATE *AIStateFind(char *name, AISCRIPT *script) {
    AISTATE *state;

    state = (AISTATE *)NuLinkedListGetHead(&script->states);

    if (name != NULL) {
        while (state != NULL) {
            if (NuStrICmp(name, state->name) == 0) {
                return state;
            }

            state = (AISTATE *)NuLinkedListGetNext(&script->states, &state->list_node);
        }
    }

    return NULL;
}
