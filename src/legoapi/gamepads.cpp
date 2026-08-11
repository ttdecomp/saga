#include "legoapi/gamepads.h"

#include "globals.h"
#include "legoapi/players.h"
#include "legoapi/timer.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"

struct MovementData {
    u8 unknown_00[0x1c];
    f32 run_speed;
};

struct SocketData {
    u8 unknown_00[0x68];
    u8 flags;
    u8 unknown_69[5];
    u16 angle_offset;
    u8 unknown_70[0xcc];
};

struct GamePadWorld {
    u8 unknown_0000[0x2988];
    SocketData **socket_data;
};

extern "C" GamePadWorld *WORLD;
extern u8 *GameCam;
extern TIMER GameTimer;

bool (*GamePads_IgnoreInputFn)();

i32 RotDiff(u16 first, u16 second);
void NewRumble(NUPAD *pad, f32 strength, i32 frames);
void NewBuzz(NUPAD *pad, f32 strength, i32 frames);
void NewBuzzFrames(NUPAD *pad, i32 frames, i32 delay);
void NewRumbleAllPlayers(f32 strength, f32 buzz, i32 rumble_frames, i32 buzz_frames);

f32 PadOldSpeed2[2];
f32 PadOldSpeed[2];
u16 PadOldAngle2[2];
u16 PadOldAngle[2];

static GAMEPAD_s *GetGamePad(GameObject_s *object) {
    return (GAMEPAD_s *)object->player_packet.game_pad;
}

f32 GamePad_Rotate(GameObject_s *object) {
    i32 player_index;
    if (object == Player[0]) {
        player_index = 0;
    } else if (object == Player[1]) {
        player_index = 1;
    } else {
        return 0.0f;
    }

    GAMEPAD_s *game_pad = GetGamePad(object);
    if (game_pad->speed != 0.0f && PadOldSpeed[player_index] != 0.0f && PadOldSpeed2[player_index] != 0.0f) {
        i32 current_difference = RotDiff(PadOldAngle[player_index], game_pad->direction);
        i32 previous_difference = RotDiff(PadOldAngle2[player_index], PadOldAngle[player_index]);
        if ((previous_difference < 0 && current_difference < 0 && previous_difference > -0x2000 &&
             current_difference > -0x2000) ||
            (previous_difference > 0 && current_difference > 0 && previous_difference < 0x2000 &&
             current_difference < 0x2000)) {
            return ((f32)current_difference / 65536.0f) / FRAMETIME;
        }
    }
    return 0.0f;
}

void DieRumble(GameObject_s *object) {
    if (object != NULL && (i8)object->state_flags < 0) {
        NewRumble(GetGamePad(object)->pad, 1.0f, 0);
        NewBuzz(GetGamePad(object)->pad, 0.3f, 0);
    }
}

void KillRumble(GameObject_s *object) {
    if (object != NULL && (i8)object->state_flags < 0) {
        NewRumble(GetGamePad(object)->pad, 0.7f, 0);
        NewBuzz(GetGamePad(object)->pad, 0.1f, 0);
    }
}

void HitRumble(GameObject_s *object) {
    if (object != NULL && (i8)object->state_flags < 0) {
        NewRumble(GetGamePad(object)->pad, 0.5f, 0);
        NewBuzzFrames(GetGamePad(object)->pad, 2, 0);
    }
}

void TakeHitRumble(GameObject_s *object, f32 strength) {
    if (object != NULL && (i8)object->state_flags < 0) {
        NewRumble(GetGamePad(object)->pad, strength, 0);
        NewBuzz(GetGamePad(object)->pad, 0.1f, 0);
    }
}

void FloatRumble(GameObject_s *object) {
    if (object != NULL && (i8)object->state_flags < 0) {
        MovementData *movement = *(MovementData **)((u8 *)object->character_data + 0x24);
        f32 strength = NuFabs(object->velocity.y) / movement->run_speed * 0.3f + 0.25f;
        if (strength > 1.0f) {
            strength = 1.0f;
        }
        NewRumble(GetGamePad(object)->pad, strength, 0);
        NewBuzzFrames(GetGamePad(object)->pad, 1, 0);
    }
}

void ConstantRumble(GameObject_s *object, f32 strength, f32 time_offset) {
    f32 phase = NuFmod(GameTimer.time_elapsed + time_offset, 1.25f);
    f32 amplitude = 0.0f;
    if (phase < 1.0f) {
        i32 angle = (i32)(phase * 32768.0f + 16384.0f);
        amplitude = 1.0f - NuFabs(NuTrigTable[(angle >> 1) & 0x7fff]);
    }
    amplitude *= strength;

    if (object == NULL) {
        NewRumbleAllPlayers(amplitude, 0.0f, 0, 0);
    } else if ((i8)object->state_flags < 0) {
        NewRumble(GetGamePad(object)->pad, amplitude, 0);
    }
}

u16 GamePad_InputAngle(GameObject_s *object, GAMEPAD_s *game_pad) {
    if ((i8)object->state_flags < 0 && object->sock_index != -1) {
        SocketData *socket = *WORLD->socket_data + object->sock_index;
        if ((socket->flags & 0x40) != 0) {
            return *(u16 *)((u8 *)object + 0x686) + game_pad->direction + socket->angle_offset;
        }
    }
    return game_pad->direction + *(u16 *)(GameCam + 0x1fc);
}
