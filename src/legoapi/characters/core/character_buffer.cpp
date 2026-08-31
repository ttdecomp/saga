#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/world/area.h"

#include <string.h>

extern VARIPTR characterbuffer_base;
extern VARIPTR characterbuffer_ptr;

void ResetCharacterBuffer(i32 force_reset) {
    if (force_reset == 0 && Area != -1 && Area == last_area) {
        return;
    }

    characterbuffer_ptr = characterbuffer_base;
    memset(characterbuffer_ptr.void_ptr, 0, CHARACTERBUFFERSIZE);

    apicharsys->loaded_model_count = apicharsys->permanent_model_count;
    apicharsys->animation_load_attempts = apicharsys->area_animation_count;
    apicharsys->loaded_animation_count = apicharsys->area_animation_count;
}
