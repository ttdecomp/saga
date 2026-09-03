#pragma once

#include "decomp.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct dogfight_door_s {
    char name[12];
    f32 distance;
    f32 timer;
};
DECOMP_ASSERT(sizeof(dogfight_door_s) == 0x14, "dogfight_door_s size");

struct dogfight_doors_s {
    dogfight_door_s doors[7];
    u32 unknown_08c;
};
DECOMP_ASSERT(sizeof(dogfight_doors_s) == 0x90, "dogfight_doors_s size");

struct spacelevel_action_config_s {
    f32 unknown_00;
    f32 unknown_04;
    i32 unknown_08;
    f32 unknown_0c;
    f32 unknown_10;
    f32 unknown_14;
    i32 unknown_18;
    f32 unknown_1c;
    f32 unknown_20;
    i32 unknown_24;
    f32 unknown_28;
    i32 unknown_2c;
};
DECOMP_ASSERT(sizeof(spacelevel_action_config_s) == 0x30, "spacelevel_action_config_s size");

struct anakin_action_config_s {
    spacelevel_action_config_s common;
    i32 unknown_30;
    f32 unknown_34;
    f32 unknown_38;
    i32 unknown_3c;
    u32 unknown_40[4];
};
DECOMP_ASSERT(sizeof(anakin_action_config_s) == 0x50, "anakin_action_config_s size");

extern spacelevel_action_config_s Actions_DogFightA;
extern anakin_action_config_s Actions_AnakinA;
extern dogfight_doors_s DogFightDoors;

struct spacelevel_starfighter_s {
    i32 reset_state;
    i32 reset_target;
    u8 unknown_008[0x40 - 0x08];
    i32 reset_timer;
    u8 unknown_044[0x90 - 0x44];
    i32 reset_effect;
    u8 unknown_094[0x98 - 0x94];
    u32 reset_colour;
    i32 reset_effect_timer;
    u8 unknown_0a0[0x128 - 0x0a0];
};
DECOMP_ASSERT(sizeof(spacelevel_starfighter_s) == 0x128, "spacelevel_starfighter_s size");

struct spacelevel_last_starfighter_s {
    i32 reset_state;
    i32 reset_target;
    u8 unknown_008[0x40 - 0x08];
    i32 reset_timer;
    u8 unknown_044[0x90 - 0x44];
    i32 reset_effect;
    u8 unknown_094[0x98 - 0x94];
    u32 reset_colour;
    i32 reset_effect_timer;
    u8 unknown_0a0[0x0a8 - 0x0a0];
};
DECOMP_ASSERT(sizeof(spacelevel_last_starfighter_s) == 0x0a8, "spacelevel_last_starfighter_s size");

struct spacelevel_trooper_team_s {
    i32 reset_state;
    i32 reset_target;
    u8 unknown_008[0x40 - 0x08];
    i32 reset_timer;
    u8 unknown_044[0x90 - 0x44];
    i32 reset_effect;
    u8 unknown_094[0x98 - 0x94];
    u32 reset_colour;
    i32 reset_effect_timer;
    u8 unknown_0a0[0x1b8 - 0x0a0];
};
DECOMP_ASSERT(sizeof(spacelevel_trooper_team_s) == 0x1b8, "spacelevel_trooper_team_s size");

struct spacelevel_fighter_group_s {
    spacelevel_starfighter_s fighters[4];
    spacelevel_trooper_team_s trooper_team;
};
DECOMP_ASSERT(sizeof(spacelevel_fighter_group_s) == 0x658, "spacelevel_fighter_group_s size");

struct spacelevel_large_record_s {
    u8 unknown_000[0x404];
    f32 saved_value;
    f32 reset_value;
    u8 unknown_40c[0x514 - 0x40c];
    i32 saved_state;
    i32 reset_state;
    u8 unknown_51c[0x52c - 0x51c];
};
DECOMP_ASSERT(sizeof(spacelevel_large_record_s) == 0x52c, "spacelevel_large_record_s size");

struct spacelevel_scale_s {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};
DECOMP_ASSERT(sizeof(spacelevel_scale_s) == 0x10, "spacelevel_scale_s size");

struct spacelevel_s {
    NUMTX player_matrix;
    u16 player_matrix_flags;
    u16 player_matrix_state;
    u8 unknown_044[0x48 - 0x44];
    u32 player_colour;
    NUMTX camera_matrix;
    f32 camera_matrix_w;
    u16 camera_matrix_flags;
    u16 camera_matrix_state;
    u8 unknown_094[0x98 - 0x94];
    u32 camera_colour;
    f32 camera_value;
    u8 unknown_0a0[0x1b0 - 0x0a0];
    spacelevel_fighter_group_s fighter_groups[7];
    spacelevel_starfighter_s final_fighters[4];
    spacelevel_last_starfighter_s last_starfighter;
    spacelevel_scale_s draw_scale;
    anakin_action_config_s *unknown_3370;
    i32 unknown_3374;
    i32 unknown_3378;
    i32 unknown_337c;
    u8 unknown_3380[0x338c - 0x3380];
    void *unknown_338c;
    spacelevel_large_record_s large_records[256];
    spacelevel_starfighter_s queued_starfighters[96];
    u8 unknown_5ce90[0x62e90 - 0x5ce90];
    void *reset_buffer;
    i32 reset_buffer_count;
    i32 reset_buffer_used;
    NUVEC player_origin;
    f32 player_origin_padding;
    NUVEC camera_origin;
    i32 unknown_62eb8;
    NUVEC direction;
    f32 unknown_62ec8;
    f32 direction_length;
    f32 inverse_direction_length;
    f32 unknown_62ed4;
    f32 door_time;
    f32 door_countdown;
    f32 door_elapsed;
    u8 unknown_62ee4[0x62ee8 - 0x62ee4];
    f32 value_one_a;
    f32 value_one_b;
};
DECOMP_ASSERT(offsetof(spacelevel_s, fighter_groups) == 0x1b0, "fighter group array offset");
DECOMP_ASSERT(offsetof(spacelevel_s, large_records) == 0x3390, "large record array offset");
DECOMP_ASSERT(offsetof(spacelevel_s, queued_starfighters) == 0x55f90, "queued fighter array offset");
DECOMP_ASSERT(offsetof(spacelevel_s, reset_buffer) == 0x62e90, "reset buffer offset");
DECOMP_ASSERT(offsetof(spacelevel_s, direction) == 0x62ebc, "direction offset");
DECOMP_ASSERT(sizeof(spacelevel_s) == 0x62ef0, "spacelevel_s size");
