#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nu3d/nutex.h"

#include <stdio.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void CharVariant_Find(char *) {
}

void CharVariants_Init(CHARVARIANT *, i32) {
}

void CharCategories_Init(CHARCATEGORY *) {
}

void CanWearHatsInFreePlay(i32) {
}

void CharCategory_FindByName(char *) {
}

void CharCategory_IsCategory(GameObject_s *, i32) {
}

static void ConfigureCharacterIcon(CHARACTERDATA &character, NUFPAR *parser, i32 permanent) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }

    character.field20_0x42 = static_cast<i16>(LevelObject_FindIndexFromName(parser->word_buf));
    if (character.field20_0x42 != -1 || permanent == 0) {
        return;
    }

    if (LevelObject_AddExtra(parser->word_buf, 3) != 0) {
        character.field20_0x42 = static_cast<i16>(LevelObject_FindIndexFromName(parser->word_buf));
        NuStrCat(parser->word_buf, "1");
        LevelObject_AddExtra(parser->word_buf, 3);
    }
}

static bool ConfigureLayerMask(NUFPAR *parser, u32 &mask) {
    mask = 0;
    bool configured = false;
    while (NuFParGetWord(parser) != 0) {
        const i32 layer = NuAToI(parser->word_buf);
        if (static_cast<u32>(layer) <= 31) {
            mask |= 1u << layer;
            configured = true;
        }
    }
    return configured;
}

static void ConfigureCharacterLayers(GAMECHARACTERDATA &data, NUFPAR *parser) {
    if (NuStrICmp(parser->word_buf, "layers_special") == 0) {
        if (ConfigureLayerMask(parser, data.layer_mask_special)) {
            data.layer_mask = data.layer_mask_special;
            data.layer_mask_medium = data.layer_mask_special;
            data.layer_mask_low = data.layer_mask_special;
            data.layer_mask_dead = data.layer_mask_special;
        }
    } else if (NuStrICmp(parser->word_buf, "layers_high") == 0) {
        if (ConfigureLayerMask(parser, data.layer_mask)) {
            data.layer_mask_medium = data.layer_mask;
            data.layer_mask_low = data.layer_mask;
        }
    } else if (NuStrICmp(parser->word_buf, "layers_medium") == 0) {
        if (ConfigureLayerMask(parser, data.layer_mask_medium)) {
            data.layer_mask_low = data.layer_mask_medium;
        }
    } else if (NuStrICmp(parser->word_buf, "layers_low") == 0) {
        ConfigureLayerMask(parser, data.layer_mask_low);
    } else if (NuStrICmp(parser->word_buf, "layers_dead") == 0) {
        ConfigureLayerMask(parser, data.layer_mask_dead);
    } else if (NuStrICmp(parser->word_buf, "ride_layersoff") == 0) {
        ConfigureLayerMask(parser, data.ride_layers_off);
    }
}

void CharConfig_ConfigureAll(i32 permanent, nufpcomjmp_s *) {
    if (CDataList == NULL || GCDataList == NULL || CHARCOUNT <= 0) {
        return;
    }

    for (i32 character_id = 0; character_id < CHARCOUNT; ++character_id) {
        CHARACTERDATA &character = CDataList[character_id];
        if (character.dir == NULL || character.file == NULL) {
            continue;
        }

        char path[256];
        snprintf(path, sizeof(path), "chars\\%s\\%s.txt", character.dir, character.file);
        NUFPAR *parser = NuFParCreate(path);
        if (parser == NULL) {
            continue;
        }

        parser->separator_list = const_cast<char *>("=");
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            ConfigureCharacterLayers(GCDataList[character_id], parser);
            if (NuStrICmp(parser->word_buf, "icon") == 0) {
                ConfigureCharacterIcon(character, parser, permanent);
            }
        }
        NuFParDestroy(parser);
    }
}

void CharConfig_CalculateJumpStats(float, float, float *, float *) {
}

void ExtraCharacterFixUpAfterConfig() {
}
