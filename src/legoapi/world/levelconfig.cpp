#include "decomp.h"
#include "nu2api/nufile/nufpar.h"
#include "legoapi/world/level.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/numusic/numusic.h"
#include "globals.h"

extern i16 GetMusicIndex(char *, nusound_filename_info_s *, i32);

static __used__ void LC_AL_backb(nufpar_s *fp) {
    u8 v = (u8)NuFParGetInt(fp);
    levelconfig_ldata->data_display.bg_blue_bottom = v;
    levelconfig_ldata->data_display.bg_blue_top = v;
}

static __used__ void LC_AL_backb_bottom(nufpar_s *fp) {
    levelconfig_ldata->data_display.bg_blue_bottom = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backb_top(nufpar_s *fp) {
    levelconfig_ldata->data_display.bg_blue_top = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backg(nufpar_s *fp) {
    u8 v = (u8)NuFParGetInt(fp);
    levelconfig_ldata->data_display.bg_green_bottom = v;
    levelconfig_ldata->data_display.bg_green_top = v;
}

static __used__ void LC_AL_backg_bottom(nufpar_s *fp) {
    levelconfig_ldata->data_display.bg_green_bottom = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backg_top(nufpar_s *fp) {
    levelconfig_ldata->data_display.bg_green_top = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backr(nufpar_s *fp) {
    u8 v = (u8)NuFParGetInt(fp);
    levelconfig_ldata->data_display.bg_red_bottom = v;
    levelconfig_ldata->data_display.bg_red_top = v;
}

static __used__ void LC_AL_backr_bottom(nufpar_s *fp) {
    levelconfig_ldata->data_display.bg_red_bottom = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backr_top(nufpar_s *fp) {
    levelconfig_ldata->data_display.bg_red_top = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_blobshadow_alpha(nufpar_s *fp) {
    levelconfig_ldata->blob_shadow_alpha = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_blobshadow_fadefar(nufpar_s *fp) {
    levelconfig_ldata->blob_shadow_fade_far = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_blobshadow_fadenear(nufpar_s *fp) {
    levelconfig_ldata->blob_shadow_fade_near = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_cam_tilt(nufpar_s *fp) {
    levelconfig_ldata->cam_tilt = NuFParGetFloat(fp);
}

static __used__ void LC_AL_camang_seek(nufpar_s *fp) {
    levelconfig_ldata->cam_angle_seek = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_campos_seek(nufpar_s *fp) {
    levelconfig_ldata->cam_pos_seek = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_conveyor(nufpar_s *fp) {
    while (NuFParGetWord(fp) != 0) {
        if (NuStrICmp(fp->word_buf, "xspeed") == 0) {
            levelconfig_ldata->conveyor_x_speed = NuFParGetFloat(fp);
        } else if (NuStrICmp(fp->word_buf, "zspeed") == 0) {
            levelconfig_ldata->conveyor_z_speed = NuFParGetFloat(fp);
        }
    }
}

static __used__ void LC_AL_hover_height(nufpar_s *fp) {
    levelconfig_ldata->hover_height = NuFParGetFloat(fp);
}

static __used__ void LC_AL_plat_scan_dist(nufpar_s *fp) {
    levelconfig_ldata->unknown_0db = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_raycaststep(nufpar_s *fp) {
    levelconfig_ldata->unknown_0da = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_reflect_range(nufpar_s *fp) {
    levelconfig_ldata->unknown_0d9 = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_reflect_y(nufpar_s *fp) {
    levelconfig_ldata->unknown_0cc = NuFParGetFloat(fp);
}

static __used__ void LC_AL_sfx_ambient(nufpar_s *fp) {
    if (NuFParGetWord(fp) != 0) {
        levelconfig_ldata->unknown_0a2 = (i16)GetSfxId(fp->word_buf);
    }
}

static __used__ void LC_AL_waterripple_endcol_a(nufpar_s *fp) {
    levelconfig_ldata->unknown_0bf = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_b(nufpar_s *fp) {
    levelconfig_ldata->unknown_0be = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_g(nufpar_s *fp) {
    levelconfig_ldata->unknown_0bd = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_r(nufpar_s *fp) {
    levelconfig_ldata->unknown_0bc = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_life(nufpar_s *fp) {
    levelconfig_ldata->unknown_0c0 = NuFParGetFloat(fp);
}

static __used__ void LC_AL_waterripple_startcol_a(nufpar_s *fp) {
    levelconfig_ldata->unknown_0bb = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_startcol_b(nufpar_s *fp) {
    levelconfig_ldata->unknown_0ba = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_startcol_g(nufpar_s *fp) {
    levelconfig_ldata->unknown_0b9 = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_startcol_r(nufpar_s *fp) {
    levelconfig_ldata->unknown_0b8 = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_attractos(nufpar_s *fp) {
    levelconfig_ldata->max_attractos = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_bombgen_objects(nufpar_s *fp) {
    levelconfig_ldata->max_bombgen_objs = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_bombgens(nufpar_s *fp) {
    levelconfig_ldata->max_bombgens = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_bridges(nufpar_s *fp) {
    levelconfig_ldata->max_bridges = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_buildit_objects(nufpar_s *fp) {
    levelconfig_ldata->max_buildit_objs = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_buildits(nufpar_s *fp) {
    levelconfig_ldata->max_buildits = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_climb_objects(nufpar_s *fp) {
    levelconfig_ldata->max_climb_objs = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_doors(nufpar_s *fp) {
    levelconfig_ldata->max_doors = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_force(nufpar_s *fp) {
    levelconfig_ldata->max_force = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_force_objects(nufpar_s *fp) {
    levelconfig_ldata->max_force_objs = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gameantinodes(nufpar_s *fp) {
    levelconfig_ldata->max_antinodes = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizmoblowups(nufpar_s *fp) {
    levelconfig_ldata->max_gizmo_blowups = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizmoblowuptypes(nufpar_s *fp) {
    levelconfig_ldata->max_gizmo_blowup_types = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizpanels(nufpar_s *fp) {
    levelconfig_ldata->max_giz_panels = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizrandoms(nufpar_s *fp) {
    levelconfig_ldata->max_giz_randoms = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizspecials(nufpar_s *fp) {
    levelconfig_ldata->max_giz_specials = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_grapples(nufpar_s *fp) {
    levelconfig_ldata->max_grapples = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_guidelines(nufpar_s *fp) {
    levelconfig_ldata->max_guidelines = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_hatmachines(nufpar_s *fp) {
    levelconfig_ldata->max_hat_machines = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_ledges(nufpar_s *fp) {
    levelconfig_ldata->max_ledges = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_levers(nufpar_s *fp) {
    levelconfig_ldata->max_levers = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_minicutParts(nufpar_s *fp) {
    levelconfig_ldata->max_minicut_parts = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_minicuts(nufpar_s *fp) {
    levelconfig_ldata->max_minicuts = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_obstacle_objects(nufpar_s *fp) {
    levelconfig_ldata->max_obstacle_objs = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_obstacles(nufpar_s *fp) {
    levelconfig_ldata->max_obstacles = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_pickups(nufpar_s *fp) {
    levelconfig_ldata->max_pickups = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_plugs(nufpar_s *fp) {
    levelconfig_ldata->max_plugs = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_pushblock_endpos(nufpar_s *fp) {
    levelconfig_ldata->max_push_block_end_pos = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_pushblocks(nufpar_s *fp) {
    levelconfig_ldata->max_push_blocks = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_securitydoors(nufpar_s *fp) {
    levelconfig_ldata->max_security_doors = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_shards(nufpar_s *fp) {
    levelconfig_ldata->max_shards = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_signals(nufpar_s *fp) {
    levelconfig_ldata->max_signals = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_spinneranim_objs(nufpar_s *fp) {
    levelconfig_ldata->max_spinner_anim_objs = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_spinners(nufpar_s *fp) {
    levelconfig_ldata->max_spinners = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_technos(nufpar_s *fp) {
    levelconfig_ldata->max_technos = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_teleports(nufpar_s *fp) {
    levelconfig_ldata->max_teleports = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_ter_groups(nufpar_s *fp) {
    levelconfig_ldata->max_ter_groups = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_ter_platforms(nufpar_s *fp) {
    levelconfig_ldata->max_ter_platforms = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_tightropes(nufpar_s *fp) {
    levelconfig_ldata->max_tightropes = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_timers(nufpar_s *fp) {
    levelconfig_ldata->max_giz_timers = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_torpmachines(nufpar_s *fp) {
    levelconfig_ldata->max_torp_machines = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_tubes(nufpar_s *fp) {
    levelconfig_ldata->max_tubes = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_turrets(nufpar_s *fp) {
    levelconfig_ldata->max_turrets = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_zipups(nufpar_s *fp) {
    levelconfig_ldata->max_zipups = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_mipmapmode(nufpar_s *fp) {
    levelconfig_ldata->mipmap_mode = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_wind_size(nufpar_s *fp) {
    levelconfig_ldata->wind_size = NuFParGetFloat(fp);
}

static __used__ void LC_BL_wind_speed(nufpar_s *fp) {
    levelconfig_ldata->wind_speed = NuFParGetFloat(fp);
}

static u8 load_conditionParam;

static __used__ void loadSumBox(nufpar_s *fp) {
    load_conditionParam = (u8)NuFParGetInt(fp);
}
static __used__ void LC_AL_metal(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x1000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x1000;
    }
}
static __used__ void LC_AL_music(nufpar_s *fp) {
    if (NuFParGetWord(fp) != 0) {
        levelconfig_ldata->music_index = (i16)GetMusicIndex(fp->word_buf, MusicInfo, -1);
        levelconfig_ldata->music_tracks[0][0] = music_man.GetTrackHandle(TRACK_CLASS_QUIET, fp->word_buf);
        levelconfig_ldata->music_tracks[1][0] = music_man.GetTrackHandle(TRACK_CLASS_ACTION, fp->word_buf);
        levelconfig_ldata->music_tracks[2][0] = music_man.GetTrackHandle(TRACK_CLASS_NOMUSIC, fp->word_buf);
    }
}
static __used__ void LC_AL_music_other(nufpar_s *fp) {
    if (NuFParGetWord(fp) != 0) {
        levelconfig_ldata->unknown_0a8 = (i16)GetMusicIndex(fp->word_buf, MusicInfo, -1);
        levelconfig_ldata->music_tracks[0][1] = music_man.GetTrackHandle(TRACK_CLASS_QUIET, fp->word_buf);
        levelconfig_ldata->music_tracks[1][1] = music_man.GetTrackHandle(TRACK_CLASS_ACTION, fp->word_buf);
        levelconfig_ldata->music_tracks[2][1] = music_man.GetTrackHandle(TRACK_CLASS_NOMUSIC, fp->word_buf);
    }
}
static __used__ void LC_AL_farclip(nufpar_s *fp) {
    i32 v = NuFParGetInt(fp);
    i32 farclip;
    if (v > 9) {
        farclip = (v <= 20000) ? v : 20000;
    } else {
        farclip = 10;
    }
    if (NuIOS_IsLowEndDevice() && (f32)farclip <= levelconfig_ldata->data_display.far_clip) {
        farclip = (i32)levelconfig_ldata->data_display.far_clip;
    }
    levelconfig_ldata->data_display.unknown_14 = (i16)farclip;
}
static __used__ void LC_AL_camera_rain(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x4000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x4000;
    }
}
static __used__ void LC_AL_in_space(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x40000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x40000;
    }
}
static __used__ void LC_AL_flat_terrain(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x10;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x10;
    }
}
static __used__ void LC_AL_hidden_icons(nufpar_s *fp) {
    if (NuFParGetWord(fp) != 0) {
        if (NuStrICmp(fp->word_buf, "on") == 0) {
            levelconfig_ldata->flags &= ~0x800000;
            return;
        }
        if (NuStrICmp(fp->word_buf, "off") == 0) {
            levelconfig_ldata->flags |= 0x800000;
        }
    }
}
static __used__ void LC_AL_double_score(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x800;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x800;
    }
}
static __used__ void LC_AL_narrow_socks(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x200000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x200000;
    }
}
static __used__ void LC_AL_terrain_rain(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x8000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x8000;
    }
}
static __used__ void LC_AL_lowendfarclip(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (v < 2.0f) {
        v = 2.0f;
    } else if (v > 20000.0f) {
        v = 20000.0f;
    }
    levelconfig_ldata->data_display.far_clip = v;
    if (NuIOS_IsLowEndDevice()) {
        levelconfig_ldata->data_display.unknown_14 = (i16)v;
    }
}
static __used__ void LC_AL_cam_lateral_dist(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (v < 0.0f) {
        v = 0.0f;
    }
    levelconfig_ldata->cam_lateral_dist = v;
}
static __used__ void LC_AL_cam_pullback_dist(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (v < 0.0f) {
        v = 0.0f;
    }
    levelconfig_ldata->cam_pullback_dist = v;
}
static __used__ void LC_AL_lowendcharclip(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (!NuIOS_IsLowEndDevice()) {
        levelconfig_ldata->unknown_11c = v;
    }
}
static __used__ void LC_AL_lowendfogstart(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (v < 2.0f) {
        v = 2.0f;
    } else if (v > 20000.0f) {
        v = 20000.0f;
    }
    levelconfig_ldata->data_display.fog_start = v;
}
static __used__ void LC_AL_nearclip(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (v < 0.001f) {
        v = 0.001f;
    } else if (v > 1.0f) {
        v = 1.0f;
    }
    levelconfig_ldata->data_display.unknown_04 = v;
}
static __used__ void LC_AL_farclip_hack(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (v < 0.1f) {
        v = 0.1f;
    } else if (v > 50.0f) {
        v = 50.0f;
    }
    levelconfig_ldata->data_display.unknown_00 = v;
}
static __used__ void LC_AL_lowendcamerazoom(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (!NuIOS_IsLowEndDevice()) {
        levelconfig_ldata->unknown_120 = v;
    }
}
static __used__ void LC_AL_lowendparticlethin(nufpar_s *fp) {
    f32 v = NuFParGetFloat(fp);
    if (!NuIOS_IsLowEndDevice()) {
        levelconfig_ldata->data_display.particle_thin = v;
    }
}
static __used__ void LC_AL_forget_takeovers(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x100000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x100000;
    }
}
static __used__ void LC_AL_pickups_to_panel(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x80000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x80000;
    }
}
static __used__ void LC_AL_override_nopickupgravity(nufpar_s *fp) {
    levelconfig_ldata->flags |= 0x400000;
    if (NuFParGetWord(fp) != 0 && NuStrICmp(fp->word_buf, "off") == 0) {
        levelconfig_ldata->flags &= ~0x400000;
    }
}
static __used__ void LC_BL_fix_strobing_anims(nufpar_s *fp) {
    (void)fp;
    levelconfig_ldata->flags |= 0x100;
}
