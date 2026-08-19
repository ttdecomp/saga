#include "decomp.h"
#include "nu2api/nufile/nufpar.h"
#include "legoapi/world/level.h"

static __used__ void LC_AL_backb(nufpar_s *fp) {
    u8 v = (u8)NuFParGetInt(fp);
    *(u8 *)((char *)levelconfig_ldata + 0x9f) = v;
    *(u8 *)((char *)levelconfig_ldata + 0x9e) = v;
}

static __used__ void LC_AL_backb_bottom(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x9f) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backb_top(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x9e) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backg(nufpar_s *fp) {
    u8 v = (u8)NuFParGetInt(fp);
    *(u8 *)((char *)levelconfig_ldata + 0x9d) = v;
    *(u8 *)((char *)levelconfig_ldata + 0x9c) = v;
}

static __used__ void LC_AL_backg_bottom(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x9d) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backg_top(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x9c) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backr(nufpar_s *fp) {
    u8 v = (u8)NuFParGetInt(fp);
    *(u8 *)((char *)levelconfig_ldata + 0x9b) = v;
    *(u8 *)((char *)levelconfig_ldata + 0x9a) = v;
}

static __used__ void LC_AL_backr_bottom(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x9b) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_backr_top(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x9a) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_blobshadow_alpha(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xad) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_blobshadow_fadefar(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xd6) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_blobshadow_fadenear(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xd5) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_cam_tilt(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0xb0) = NuFParGetFloat(fp);
}

static __used__ void LC_AL_camang_seek(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xd8) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_campos_seek(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xd7) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_conveyor(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0xdc) = NuFParGetFloat(fp);
    *(f32 *)((char *)levelconfig_ldata + 0xe0) = NuFParGetFloat(fp);
}

static __used__ void LC_AL_hover_height(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0xb4) = NuFParGetFloat(fp);
}

static __used__ void LC_AL_plat_scan_dist(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xdb) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_raycaststep(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xda) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_reflect_range(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xd9) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_reflect_y(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0xcc) = NuFParGetFloat(fp);
}

static __used__ void LC_AL_sfx_ambient(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xa2) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_a(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xbf) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_b(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xbe) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_g(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xbd) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_endcol_r(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xbc) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_life(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0xc0) = NuFParGetFloat(fp);
}

static __used__ void LC_AL_waterripple_startcol_a(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xbb) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_startcol_b(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xba) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_startcol_g(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xb9) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_AL_waterripple_startcol_r(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xb8) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_attractos(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x102) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_bombgen_objects(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xf2) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_bombgens(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x113) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_bridges(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x114) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_buildit_objects(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xee) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_buildits(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xfc) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_climb_objects(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x103) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_doors(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x10d) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_force(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x10a) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_force_objects(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xf0) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gameantinodes(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xe4) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizmoblowups(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xe6) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizmoblowuptypes(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xe8) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizpanels(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x108) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizrandoms(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x10f) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_gizspecials(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x101) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_grapples(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xfa) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_guidelines(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x104) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_hatmachines(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x109) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_ledges(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x105) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_levers(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xf7) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_minicutParts(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x100) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_minicuts(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xff) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_obstacle_objects(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xec) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_obstacles(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xfb) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_pickups(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xea) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_plugs(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x115) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_pushblock_endpos(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x10c) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_pushblocks(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x10b) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_securitydoors(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x106) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_shards(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xfd) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_signals(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xf6) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_spinneranim_objs(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x111) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_spinners(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xfe) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_technos(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xf8) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_teleports(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x10e) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_ter_groups(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xa6) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_ter_platforms(nufpar_s *fp) {
    *(i16 *)((char *)levelconfig_ldata + 0xa4) = (i16)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_tightropes(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xf4) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_timers(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xf5) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_torpmachines(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x110) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_tubes(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x107) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_turrets(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0x112) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_max_zipups(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xf9) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_mipmapmode(nufpar_s *fp) {
    *(u8 *)((char *)levelconfig_ldata + 0xac) = (u8)NuFParGetInt(fp);
}

static __used__ void LC_BL_wind_size(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0x128) = NuFParGetFloat(fp);
}

static __used__ void LC_BL_wind_speed(nufpar_s *fp) {
    *(f32 *)((char *)levelconfig_ldata + 0x124) = NuFParGetFloat(fp);
}

static __used__ void loadSumBox(nufpar_s *) {
}
static __used__ void LC_AL_metal(nufpar_s *) {
}
static __used__ void LC_AL_music(nufpar_s *) {
}
static __used__ void LC_AL_music_other(nufpar_s *) {
}
static __used__ void LC_AL_farclip(nufpar_s *) {
}
static __used__ void LC_AL_camera_rain(nufpar_s *) {
}
static __used__ void LC_AL_in_space(nufpar_s *) {
}
static __used__ void LC_AL_nearclip(nufpar_s *) {
}
static __used__ void LC_AL_farclip_hack(nufpar_s *) {
}
static __used__ void LC_AL_flat_terrain(nufpar_s *) {
}
static __used__ void LC_AL_hidden_icons(nufpar_s *) {
}
static __used__ void LC_AL_double_score(nufpar_s *) {
}
static __used__ void LC_AL_narrow_socks(nufpar_s *) {
}
static __used__ void LC_AL_terrain_rain(nufpar_s *) {
}
static __used__ void LC_AL_lowendfarclip(nufpar_s *) {
}
static __used__ void LC_AL_lowendcharclip(nufpar_s *) {
}
static __used__ void LC_AL_lowendfogstart(nufpar_s *) {
}
static __used__ void LC_AL_lowendcamerazoom(nufpar_s *) {
}
static __used__ void LC_AL_lowendparticlethin(nufpar_s *) {
}
static __used__ void LC_AL_cam_lateral_dist(nufpar_s *) {
}
static __used__ void LC_AL_cam_pullback_dist(nufpar_s *) {
}
static __used__ void LC_AL_forget_takeovers(nufpar_s *) {
}
static __used__ void LC_AL_pickups_to_panel(nufpar_s *) {
}
static __used__ void LC_AL_override_nopickupgravity(nufpar_s *) {
}
static __used__ void LC_BL_fix_strobing_anims(nufpar_s *) {
}
