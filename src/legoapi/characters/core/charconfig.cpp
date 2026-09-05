#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nu3d/nutex.h"

#include <stdio.h>
#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Move_JEDI(GameObject_s *object);
void Animate_JEDI(GameObject_s *object);
void SetMoveAndAnimateFunctions(u32 model_flag_mask, u32 model_flag_value, u32 game_flag_mask, u32 game_flag_value,
                                i32 movement_type, void *move_function, void *animate_function, void *draw_function);
void CharConfig_CalculateJumpStats(f32 jump_speed, f32 gravity, f32 *duration, f32 *height);

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

static bool ReadOnOff(NUFPAR *parser, bool default_value) {
    if (NuFParGetWord(parser) == 0) {
        return default_value;
    }
    if (NuStrICmp(parser->word_buf, "on") == 0) {
        return true;
    }
    if (NuStrICmp(parser->word_buf, "off") == 0) {
        return false;
    }
    return default_value;
}

static CHARACTERANIM_s DefaultCharacterAnimation(bool bsa_default) {
    CHARACTERANIM_s animation;
    memset(&animation, 0, sizeof(animation));
    animation.flags = CHARACTER_ANIMATION_FLAG_CYCLING | CHARACTER_ANIMATION_FLAG_DEFAULT;
    if (bsa_default) {
        animation.flags |= CHARACTER_ANIMATION_FLAG_BSA;
    }
    animation.animation_id = -1;
    animation.blend_in_time = 0.2f;
    animation.blend_out_time = 0.2f;
    animation.playback_rate = 30.0f;
    animation.locator = 0xff;
    return animation;
}

static void StoreCharacterAnimations(CHARACTERDATA &character, CHARACTERANIM_s *animations, char names[][40],
                                     i32 animation_count) {
    if (character.animations != NULL || animation_count == 0) {
        return;
    }

    const usize descriptor_bytes = static_cast<usize>(animation_count + 1) * sizeof(CHARACTERANIM_s);
    usize name_bytes = 0;
    for (i32 animation_index = 0; animation_index < animation_count; ++animation_index) {
        name_bytes += static_cast<usize>(NuStrLen(names[animation_index])) + 1;
    }

    const usize allocation_start = ALIGN(permbuffer_ptr.addr, 4);
    const usize allocation_end = allocation_start + descriptor_bytes + name_bytes;
    if (allocation_end > permbuffer_end.addr) {
        return;
    }

    character.animations = reinterpret_cast<CHARACTERANIM_s *>(allocation_start);
    char *name_cursor = reinterpret_cast<char *>(allocation_start + descriptor_bytes);
    for (i32 animation_index = 0; animation_index < animation_count; ++animation_index) {
        character.animations[animation_index] = animations[animation_index];
        character.animations[animation_index].name = name_cursor;
        NuStrCpy(name_cursor, names[animation_index]);
        name_cursor += NuStrLen(name_cursor) + 1;
    }

    CHARACTERANIM_s &sentinel = character.animations[animation_count];
    memset(&sentinel, 0, sizeof(sentinel));
    sentinel.animation_id = -1;
    permbuffer_ptr.addr = allocation_end;
}

struct CharacterConfigParseState {
    CHARACTERANIM_s animations[100];
    char animation_names[100][40];
    i32 animation_count;
    bool bsa_default;
    bool in_animation;
    CHARACTERANIM_s animation;
    char animation_name[40];
};

static void ParseCharacterConfig(i32 character_id, NUFPAR *parser, i32 permanent, const char *directory,
                                 i32 include_depth, CharacterConfigParseState &state) {
    CHARACTERDATA &character = CDataList[character_id];
    GAMECHARACTERDATA &game_character = GCDataList[character_id];

    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0) {
            continue;
        }

        char key[64];
        if (NuStrLen(parser->word_buf) >= static_cast<i32>(sizeof(key))) {
            continue;
        }
        NuStrCpy(key, parser->word_buf);
        if (state.in_animation) {
            if (NuStrICmp(key, "action") == 0) {
                if (NuFParGetWord(parser) != 0) {
                    state.animation.animation_id = static_cast<i16>(ActionFromName(parser->word_buf));
                }
            } else if (NuStrICmp(key, "blend_in") == 0) {
                state.animation.blend_in_time = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "blend_out") == 0) {
                state.animation.blend_out_time = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "fpsec") == 0) {
                state.animation.playback_rate = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "speed") == 0) {
                state.animation.action_speed = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "frame") == 0 || NuStrICmp(key, "frame1") == 0) {
                state.animation.event_frame_1 = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "frame2") == 0) {
                state.animation.event_frame_2 = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "frame3") == 0) {
                state.animation.event_frame_3 = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "frame4") == 0) {
                state.animation.event_frame_4 = NuFParGetFloat(parser);
            } else if (NuStrICmp(key, "cycle") == 0) {
                if (ReadOnOff(parser, true)) {
                    state.animation.flags |= CHARACTER_ANIMATION_FLAG_CYCLING;
                } else {
                    state.animation.flags &= ~CHARACTER_ANIMATION_FLAG_CYCLING;
                }
            } else if (NuStrICmp(key, "bsa") == 0) {
                if (ReadOnOff(parser, true)) {
                    state.animation.flags |= CHARACTER_ANIMATION_FLAG_BSA;
                } else {
                    state.animation.flags &= ~CHARACTER_ANIMATION_FLAG_BSA;
                }
            } else if (NuStrICmp(key, "no_headturn") == 0 || NuStrICmp(key, "headturn_off") == 0) {
                state.animation.flags |= CHARACTER_ANIMATION_FLAG_NO_HEAD_TURN;
            } else if (NuStrICmp(key, "footsteps") == 0) {
                state.animation.flags |= CHARACTER_ANIMATION_FLAG_FOOTSTEPS;
            } else if (NuStrICmp(key, "anim_end") == 0) {
                if (state.animation.animation_id != -1 && state.animation_count < 100) {
                    state.animations[state.animation_count] = state.animation;
                    NuStrCpy(state.animation_names[state.animation_count], state.animation_name);
                    ++state.animation_count;
                }
                state.in_animation = false;
            }
            continue;
        }

        if (NuStrICmp(key, "txt_file") == 0) {
            if (include_depth < 8 && NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buf) < 128) {
                char path[256];
                snprintf(path, sizeof(path), "chars\\%s\\%s.txt", directory, parser->word_buf);
                NUFPAR *included_parser = NuFParCreate(path);
                if (included_parser != NULL) {
                    included_parser->separator_list = const_cast<char *>("=");
                    ParseCharacterConfig(character_id, included_parser, permanent, directory, include_depth + 1, state);
                    NuFParDestroy(included_parser);
                }
            }
        } else if (NuStrICmp(key, "anim_start") == 0) {
            if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buf) < 40) {
                NuStrCpy(state.animation_name, parser->word_buf);
                state.animation = DefaultCharacterAnimation(state.bsa_default);
                state.in_animation = true;
            }
        } else if (NuStrICmp(key, "bsa_default") == 0) {
            state.bsa_default = ReadOnOff(parser, state.bsa_default);
        } else if (NuStrICmp(key, "jedi") == 0) {
            if (ReadOnOff(parser, true)) {
                character.model_flags |= CHARACTER_MODEL_FLAG_JEDI;
            } else {
                character.model_flags &= ~CHARACTER_MODEL_FLAG_JEDI;
            }
        } else if (NuStrICmp(key, "oldheadmovement") == 0) {
            if (ReadOnOff(parser, true)) {
                character.model_flags |= CHARACTER_MODEL_FLAG_OLD_HEAD_MOVEMENT;
            } else {
                character.model_flags &= ~CHARACTER_MODEL_FLAG_OLD_HEAD_MOVEMENT;
            }
        } else if (NuStrICmp(key, "name_id") == 0) {
            character.name_id = NuFParGetInt(parser);
        } else if (NuStrICmp(key, "tiptoe_speed") == 0) {
            game_character.tiptoe_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "walk_speed") == 0) {
            game_character.walk_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "run_speed") == 0) {
            game_character.run_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "acceleration") == 0) {
            game_character.velocity_seek_rate = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "air_gravity") == 0) {
            game_character.gravity = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "jump_speed") == 0) {
            game_character.jump_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "jump_2_speed") == 0) {
            game_character.second_jump_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "hit_points") == 0) {
            game_character.hitpoints = static_cast<u8>(NuFParGetInt(parser));
        } else if (NuStrICmp(key, "no_tiptoe") == 0) {
            if (ReadOnOff(parser, true)) {
                game_character.flags_090 |= GAMECHARACTER_FLAG_DISABLE_TIPTOE;
            } else {
                game_character.flags_090 &= ~GAMECHARACTER_FLAG_DISABLE_TIPTOE;
            }
        } else if (NuStrICmp(key, "radius") == 0) {
            character.collision_radius = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "miny") == 0) {
            character.bounds_min_y = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "maxy") == 0) {
            character.bounds_max_y = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "scale") == 0) {
            character.model_scale = NuFParGetFloat(parser);
        } else if (NuStrICmp(key, "icon") == 0) {
            ConfigureCharacterIcon(character, parser, permanent);
        } else {
            NuStrCpy(parser->word_buf, key);
            ConfigureCharacterLayers(game_character, parser);
        }
    }
}

static void CharConfig(i32 character_id, NUFPAR *parser, i32 permanent, const char *directory) {
    CHARACTERDATA &character = CDataList[character_id];
    GAMECHARACTERDATA &game_character = GCDataList[character_id];

    CharacterConfigParseState state = {};
    state.animation = DefaultCharacterAnimation(false);
    ParseCharacterConfig(character_id, parser, permanent, directory, 0, state);

    character.model_flags |= CHARACTER_MODEL_FLAG_CONFIGURED;
    CharConfig_CalculateJumpStats(game_character.jump_speed, game_character.gravity, &game_character.jump_duration,
                                  &game_character.jump_height);
    CharConfig_CalculateJumpStats(game_character.second_jump_speed, game_character.gravity,
                                  &game_character.second_jump_duration, &game_character.second_jump_height);
    if (permanent != 0) {
        StoreCharacterAnimations(character, state.animations, state.animation_names, state.animation_count);
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
        CharConfig(character_id, parser, permanent, character.dir);
        NuFParDestroy(parser);
    }
}

void CharConfig_CalculateJumpStats(float jump_speed, float gravity, float *duration, float *height) {
    const f32 ascent_time = gravity != 0.0f ? -jump_speed / gravity : 0.0f;
    if (duration != NULL) {
        *duration = ascent_time * 2.0f;
    }
    if (height != NULL) {
        *height = jump_speed * ascent_time + gravity * 0.5f * ascent_time * ascent_time;
    }
}

void ExtraCharacterFixUpAfterConfig() {
    // This is the first assignment made by the target fix-up routine and
    // covers the ordinary playable Jedi characters in the Cantina.
    SetMoveAndAnimateFunctions(CHARACTER_MODEL_FLAG_JEDI, CHARACTER_MODEL_FLAG_JEDI, 0, 0, -1,
                               reinterpret_cast<void *>(Move_JEDI), reinterpret_cast<void *>(Animate_JEDI), NULL);
}
