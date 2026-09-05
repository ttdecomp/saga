#include "decomp.h"
#include "globals.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nutrig.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern void GameAudio_PlaySfx(i32, nuvec_s *, i32, i32);
extern void NewRumbleAllPlayers(f32, f32, i32, i32);
extern void GameCam_NewShake(GAMECAMERA_s *, f32, f32, f32);
extern void AddToGoldBricks();
extern void IncreaseScore(u32 *, u64, i32);
extern void SetBonusWinner(i32);
extern void Arcade_CoinCollected(i32, u32 *, u32);
extern "C" void PlaySfx(char *, nuvec_s *);
extern void PlayRepeatSfx(char *, i32, f32, char, f32, nuvec_s *);
extern i32 CoinsGoToMainTotal();
extern "C" void NewMenu(i32, i32, i32);
void ResetGameMessages();
void AddFancyMessageRGB(char *, float, float, float, float, i32, unsigned char, unsigned char, unsigned char);

GAMEMESSAGE_s GameMessage[128] = {};

static i32 GameMessageIndex;

/* The public game-message type intentionally only exposes the fields used by
 * callers.  This is the backing layout consumed by the original message
 * queue; its first 0x7c bytes are either a text pointer or inline text. */
struct GAME_MESSAGE_DATA {
    char *text;
    char text_buffer[0x78];
    nuvec_s position_a; // 0x7c
    nuvec_s target_position;
    nuvec_s position;
    nuvec_s start_position;
    f32 field_0xac;
    f32 target_scale;
    f32 field_0xb4;
    f32 field_0xb8;
    f32 elapsed;
    f32 duration;
    f32 field_0xc4;
    f32 field_0xc8;
    f32 field_0xcc;
    f32 field_0xd0;
    f32 field_0xd4;
    u32 flags;
    u32 score;
    u16 field_0xe0;
    u16 field_0xe2;
    u16 field_0xe4;
    u16 icon;
    nuvec_s color;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
    u8 active;
    u8 field_0xf9;
    u8 field_0xfa;
    u8 field_0xfb;
    u8 field_0xfc;
    u8 field_0xfd;
    u8 field_0xfe;
    u8 field_0xff;
    u32 field_0x100;
    u32 field_0x104;
    void (*update_fn)(GAMEMESSAGE_s *);
    void *field_0x10c;
    void (*end_fn)(GAMEMESSAGE_s *);
};

DECOMP_ASSERT(sizeof(GAME_MESSAGE_DATA) == sizeof(GAMEMESSAGE_s), "game message backing layout");

ADDGAMEMSG AddGameMsg_Default = {
    NULL, &v000, &v000, 1.0f, 1.0f, 0xff, 0xff, 0xff, 0x80, 0,  1.0f, 0.0f, 0, -1,
    NULL, 0,     0.0f,  0.0f, 0.0f, 0.0f, NULL, NULL, NULL, -1, 0,    0xff, 0,
};

GAMEMESSAGE_s *AddGameMsg(ADDGAMEMSG *message) {
    GAME_MESSAGE_DATA *slot;
    i32 slot_index = -1;
    f32 best = 0.0f;
    i32 index = GameMessageIndex;

    for (i32 count = 128; count != 0; --count) {
        slot = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[index]);
        if (slot->active == 0) {
            slot_index = index;
            break;
        }

        if ((slot->flags & 0x10) == 0) {
            f32 value;
            if (slot->field_0xfa != 0) {
                value = 1.0f;
            } else if (slot->duration == 0.0f) {
                value = 0.0f;
            } else {
                value = slot->elapsed / slot->duration;
            }
            if (slot_index == -1 || value > best) {
                best = value;
                slot_index = index;
            }
        }

        index++;
        if (index == 128) {
            index = 0;
        }
    }

    if (slot_index == -1) {
        return NULL;
    }

    GameMessageIndex = slot_index;
    slot = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[slot_index]);
    if (slot->active != 0 && slot->score != 0 && slot->field_0xfd <= 1 && slot->end_fn != NULL) {
        slot->end_fn(reinterpret_cast<GAMEMESSAGE_s *>(slot));
    }

    slot->text = NULL;
    i32 text_index = 0;
    if (message->extra_position != NULL) {
        if (NuSpecialExistsFn(message->extra_position) == 0) {
            return NULL;
        }
    } else {
        if (message->text == NULL) {
            return NULL;
        }

        i32 length = NuStrLen(message->text);
        if (length <= 0) {
            return NULL;
        }
        if ((message->flags & 0x20) != 0) {
            slot->text = message->text;
        } else {
            for (; text_index < length && text_index <= 0x76; ++text_index) {
                slot->text_buffer[text_index] = message->text[text_index];
            }
        }
    }
    slot->text_buffer[text_index] = '\0';

    slot->field_0xd4 = 0.0f;
    slot->position = *message->position;
    slot->position_a = slot->position;
    slot->field_0xac = message->scale;
    slot->field_0xb4 = message->scale;
    slot->elapsed = 0.0f;
    if ((message->flags & 0x80) == 0) {
        slot->duration = message->duration;
    } else {
        slot->duration = 0.0f;
    }

    slot->alpha = message->alpha;
    slot->red = message->red;
    slot->green = message->green;
    slot->blue = message->blue;
    slot->flags = message->flags;

    if (message->target_position != NULL) {
        slot->target_position = *message->target_position;
    } else {
        slot->target_position = slot->position;
    }
    slot->target_scale = message->target_scale;
    slot->icon = static_cast<u16>(message->icon);
    if (message->extra_position != NULL) {
        slot->color = *message->extra_position;
    } else {
        slot->color.x = 0.0f;
        slot->color.y = 0.0f;
        slot->color.z = 0.0f;
    }

    slot->field_0xfd = static_cast<u8>(message->player_index);
    slot->score = message->score;
    slot->field_0xc4 = *reinterpret_cast<f32 *>(&message->field_0x30);
    slot->field_0xc8 = *reinterpret_cast<f32 *>(&message->field_0x34);
    slot->field_0xe0 = 0;
    slot->field_0xe2 = 0;
    slot->field_0xe4 = 0;
    slot->field_0x100 = *reinterpret_cast<u32 *>(&message->field_0x38);
    slot->field_0x104 = *reinterpret_cast<u32 *>(&message->field_0x3c);
    slot->update_fn = message->update_fn;
    slot->field_0x10c = message->field_0x44;
    slot->end_fn = message->end_fn;
    slot->field_0xff = message->field_0x4d;
    slot->field_0xfe = message->field_0x4e;
    slot->field_0xfc = message->field_0x4f;
    slot->field_0xd0 = message->field_0x20;
    slot->field_0xcc = 1.0f;

    if ((message->flags & 1) == 0) {
        slot->field_0xb8 = message->scale;
        slot->start_position = slot->position;
        slot->field_0xfb = 0;
    }
    slot->field_0xf9 = static_cast<u8>((message->flags & 1) == 0);
    slot->field_0xfa = 0;
    slot->active = 1;
    return reinterpret_cast<GAMEMESSAGE_s *>(slot);
}

void *AddGameMessage(char *text, nuvec_s *position, float scale, nuvec_s *target_position, float target_scale,
                     unsigned char red, unsigned char green, unsigned char blue, u32 flags, float duration) {
    ADDGAMEMSG message = AddGameMsg_Default;
    message.text = text;
    message.position = position;
    message.target_position = target_position;
    message.scale = scale;
    message.target_scale = target_scale;
    message.red = red;
    message.green = green;
    message.blue = blue;
    message.flags = flags;
    message.duration = duration;
    return AddGameMsg(&message);
}

void AddFancyMessage(char *text, float x, float y, float scale, float duration, i32 message_id, i32 colour_type) {
    const i32 red = 0xff;
    const i32 green = 0xff - ((static_cast<u32>(colour_type) < 1) << 6);
    const i32 blue = ~(-static_cast<i32>(static_cast<u32>(colour_type) < 1)) & 0xff;
    AddFancyMessageRGB(text, x, y, scale, duration, message_id, red, green, blue);
}

void EndScoreMessage(GAMEMESSAGE_s *message) {
    GAME_MESSAGE_DATA *data = reinterpret_cast<GAME_MESSAGE_DATA *>(message);
    if (CoinsGoToMainTotal() != 0) {
        i32 icon = static_cast<i16>(data->icon);
        if (data->icon != 0xffff) {
            if (static_cast<u32>(icon - 0xb7) < 4) {
                icon -= 4;
            } else if (static_cast<u32>(icon - 0xbf) < 4) {
                icon -= 4;
            } else if (static_cast<u32>(icon - 0xc7) < 4) {
                icon -= 4;
            } else if (static_cast<u32>(icon - 0xd5) < 4) {
                icon += 4;
            }
            cointotal_i_obj[static_cast<i8>(data->field_0xfd)] = icon;
        }
        CoinTotalScale = 1.5f;

        if (SuperStory != 0) {
            IncreaseScore(&SuperStoryScore, data->score, 0);
        } else {
            WORLDINFO_s *world = WorldInfo_CurrentlyActive();
            if (world->area != NULL && (world->area->flags & 0x100) != 0) {
                if (static_cast<u32>(BonusCoinTotal) < BonusCoinTarget) {
                    IncreaseScore(reinterpret_cast<u32 *>(&BonusCoinTotal), data->score, 0);
                    if (static_cast<u32>(BonusCoinTotal) >= BonusCoinTarget) {
                        BonusCoinTotal = static_cast<i32>(BonusCoinTarget);
                        SetBonusWinner(static_cast<i8>(data->field_0xfd));
                        BonusWinFlag = 0;
                        NewMenu(32, -1, -1);
                        PlaySfx("MK-Panel", NULL);
                        ResetGameMessages();
                    }
                }
            } else {
                IncreaseScore(&Game.coins, data->score, 1);
            }
        }
    } else {
        const i8 player_index = static_cast<i8>(data->field_0xfd);
        if (static_cast<u8>(player_index) > 1 || Player[player_index] == NULL ||
            Player[player_index]->coinpacket == NULL) {
            goto play_counter_sound;
        }

        COINPACKET *packet = Player[player_index]->coinpacket;
        const u32 old_coins = packet->coins;
        if (static_cast<i16>(data->icon) != -1) {
            packet->lastcoin = data->icon;
        }

        if (BonusWinner == -1) {
            IncreaseScore(&packet->coins, data->score, 1);
            packet->scale = 1.5f;
        }
        if (BuildUpDone == 0) {
            BuildUpScale = 1.5f;
        }
        if (Arcade != 0) {
            Arcade_CoinCollected(player_index, &packet->coins, old_coins);
            goto play_counter_sound;
        }
        if (BonusArea == 0 || BonusWinner != -1 || packet->coins <= 999999) {
            goto play_counter_sound;
        }

        packet->coins = 1000000;
        SetBonusWinner(player_index);
        BonusWinFlag = 0;
        NewMenu(31, -1, -1);
        PlaySfx("MK-Panel", NULL);
        ResetGameMessages();
    }

play_counter_sound:
    if (DoubleScoreTime > 0.0f) {
        PlayRepeatSfx("Counter", -1, 0.0f, 2, 0.1f, NULL);
    } else {
        PlaySfx("Counter", NULL);
    }
}

void ResetGameMessages() {
    for (i32 i = 0; i < 128; i++) {
        GameMessage[i].active = 0;
    }
}

void AddFancyMessageRGB(char *text, float x, float y, float scale, float duration, i32 message_id, unsigned char red,
                        unsigned char green, unsigned char blue) {
    if (message_id != 0) {
        GameAudio_PlaySfx(0x2c, NULL, 0, 0);
        GameAudio_PlaySfx(0x26, NULL, 0, 0);
        NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
    }

    NUVEC position = {x, y, 1.0f};
    scale += scale;
    const f32 scale_offset = scale * 0.5f;
    AddGameMessage(text, &position, scale, &position, scale + scale_offset, red, green, blue, 0x4020, duration);
    AddGameMessage(text, &position, scale, &position, scale - scale_offset, red, green, blue, 0x4020, duration);
}

i32 FindGameMsgsWithID(i32 id, i32 remove, i32 player, GAMEMESSAGE_s *exclude) {
    i32 count = 0;
    GAME_MESSAGE_DATA *message = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[0]);
    GAME_MESSAGE_DATA *end = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[128]);
    GAME_MESSAGE_DATA *excluded = reinterpret_cast<GAME_MESSAGE_DATA *>(exclude);

    if (remove == 0) {
        if (player != -1) {
            for (; message != end; ++message) {
                if (message->active != 0 && static_cast<i8>(message->field_0xfe) == id && message != excluded &&
                    static_cast<i8>(message->field_0xfd) == player) {
                    ++count;
                }
            }
        } else {
            for (; message != end; ++message) {
                if (message->active != 0 && static_cast<i8>(message->field_0xfe) == id && message != excluded) {
                    ++count;
                }
            }
        }
    } else {
        if (player != -1) {
            for (; message != end; ++message) {
                if (message->active != 0 && static_cast<i8>(message->field_0xfe) == id && message != excluded &&
                    static_cast<i8>(message->field_0xfd) == player) {
                    ++count;
                    message->active = 0;
                }
            }
        } else {
            for (; message != end; ++message) {
                if (message->active != 0 && static_cast<i8>(message->field_0xfe) == id && message != excluded) {
                    ++count;
                    message->active = 0;
                }
            }
        }
    }
    return count;
}

void UpdateGameMessages() {
    using MessageFn = void (*)(GAMEMESSAGE_s *);
    GAME_MESSAGE_DATA *message = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[0]);
    GAME_MESSAGE_DATA *end = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[128]);
    for (; message != end; ++message) {
        if (message->active == 0) {
            continue;
        }
        if (message->field_0xfa != 0) {
            message->active = 0;
            if (message->end_fn != NULL && message->field_0xff == 0) {
                message->end_fn(reinterpret_cast<GAMEMESSAGE_s *>(message));
            }
            continue;
        }

        if ((message->flags & 0x80) != 0) {
            message->duration = 1.0f;
            message->elapsed = 1.0f;
            message->field_0xfa = 1;
        }
        if (message->field_0xd0 > 0.0f) {
            message->field_0xd0 -= FRAMETIME;
            if (message->field_0xd0 > 0.0f) {
                continue;
            }
            MessageFn callback = reinterpret_cast<MessageFn>(message->field_0x100);
            if (callback != NULL) {
                callback(reinterpret_cast<GAMEMESSAGE_s *>(message));
            }
            continue;
        }

        if (message->duration <= message->elapsed) {
            continue;
        }

        message->elapsed += FRAMETIME;
        if (message->elapsed >= message->duration) {
            message->elapsed = message->duration;
            message->field_0xfa = 1;
            if (message->end_fn != NULL && message->field_0xff != 0) {
                message->end_fn(reinterpret_cast<GAMEMESSAGE_s *>(message));
            }
        }

        const u32 flags = message->flags;
        if ((flags & 0x50) != 0) {
            f32 curve = message->elapsed / message->duration;
            if ((flags & 0x100) != 0) {
                curve = 1.0f - NU_SIN_LUT(static_cast<i32>((curve + 1.0f) * 16384.0f));
            } else if ((flags & 0x200) != 0) {
                curve = NU_SIN_LUT(static_cast<i32>(curve * 16384.0f));
            } else if ((flags & 0x400) != 0) {
                curve = NU_SIN_LUT(static_cast<i32>(curve * 32768.0f));
            } else if ((flags & 0x800) != 0) {
                curve = NU_SIN_LUT(static_cast<i32>(curve * 32768.0f));
            }
            if ((flags & 0x10) != 0) {
                message->position.x =
                    message->position_a.x + (message->target_position.x - message->position_a.x) * curve;
                message->position.y =
                    message->position_a.y + (message->target_position.y - message->position_a.y) * curve;
                message->position.z =
                    message->position_a.z + (message->target_position.z - message->position_a.z) * curve;
            }
            if ((flags & 0x40) != 0) {
                message->field_0xb4 = message->field_0xac + (message->target_scale - message->field_0xac) * curve;
            }
        }
        if ((flags & 0x8000) != 0) {
            const f32 alpha_curve =
                NU_SIN_LUT(static_cast<i32>((message->elapsed / message->duration + 1.0f) * 16384.0f));
            message->alpha = static_cast<u8>(alpha_curve * 128.0f);
        } else if ((flags & 0x4000) != 0) {
            message->alpha = static_cast<u8>((1.0f - message->elapsed / message->duration) * 128.0f);
        }
        MessageFn callback = reinterpret_cast<MessageFn>(message->field_0x104);
        if (callback != NULL) {
            callback(reinterpret_cast<GAMEMESSAGE_s *>(message));
        }
    }
}

static i32 GoldBrickMessageCount;

void AddGoldBrickMessage(STATUSPACKET_s *packet, i16 brick) {
    if (GoldBrickMessageCount <= 15) {
        *reinterpret_cast<i16 *>(reinterpret_cast<unsigned char *>(packet) + 0x12c + GoldBrickMessageCount * 2) = brick;
        ++GoldBrickMessageCount;
    }
    AddToGoldBricks();
}

void TransformGameMessages(nuvec_s *camera_position, nuvec_s *camera_direction, nuvec_s *camera_scale) {
    GAME_MESSAGE_DATA *message = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[0]);
    GAME_MESSAGE_DATA *end = reinterpret_cast<GAME_MESSAGE_DATA *>(&GameMessage[128]);
    for (; message != end; ++message) {
        if (message->active == 0 || message->field_0xf9 != 0) {
            continue;
        }
        NuCameraTransformScreenClip(&message->start_position, &message->position, 1, NULL);
        const f32 depth = (message->position.x - camera_position->x) * camera_direction->x +
                          (message->position.y - camera_position->y) * camera_direction->y +
                          (message->position.z - camera_position->z) * camera_direction->z;
        if (depth > 0.0f) {
            message->field_0xfb = 0;
        } else {
            message->field_0xfb = 1;
        }
        if ((message->flags & 4) != 0) {
            const f32 offset = message->field_0xb4;
            NUVEC target = {message->position.x + offset * camera_scale->x,
                            message->position.y + offset * camera_scale->y,
                            message->position.z + offset * camera_scale->z};
            NUVEC transformed;
            NuCameraTransformScreenClip(&transformed, &target, 1, NULL);
            message->field_0xb8 = __builtin_fabsf(message->start_position.x - transformed.x) * 0.525f;
        } else if ((message->flags & 2) == 0) {
            message->field_0xf9 = 1;
        }
        message->start_position.z = 1.0f;
    }
}
