#include "decomp.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nufloat.h"

extern "C" SOCK *sockpar_sock;

static __used__ void SockParBlend(nufpar_s *, void *) {
}
static __used__ void SockParCam1Zoom(nufpar_s *parser, void *) {
    sockpar_sock->single_player_pullback = NuFParGetFloat(parser);
}
static __used__ void SockParCamAngSeek(nufpar_s *parser, void *) {
    sockpar_sock->camera_angle_seek = NuFabs(NuFParGetFloat(parser));
}
static __used__ void SockParCamArenaOffset(nufpar_s *parser, void *) {
    sockpar_sock->camera_rail_offset = NuFParGetFloat(parser);
}
static __used__ void SockParCamBehind(nufpar_s *parser, void *) {
    const f32 ratio = NuFParGetFloat(parser);
    sockpar_sock->flags |= SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER;
    sockpar_sock->camera_local_x_ratio = ratio;
    sockpar_sock->camera_vertical_ratio = ratio;
    sockpar_sock->camera_lateral_ratio = ratio;
}
static __used__ void SockParCamBlendTime(nufpar_s *parser, void *) {
    sockpar_sock->overlap_blend_ratio = NuFParGetFloat(parser);
}
static __used__ void SockParCamDistRatio(nufpar_s *parser, void *) {
    sockpar_sock->camera_pullback_ratio = NuFParGetFloat(parser);
}
static __used__ void SockParCamDistToTarget(nufpar_s *parser, void *) {
    sockpar_sock->camera_distance_to_target = NuFParGetFloat(parser);
}
static __used__ void SockParCamDistToTargetXZ(nufpar_s *parser, void *) {
    sockpar_sock->camera_distance_to_target = NuFParGetFloat(parser);
    sockpar_sock->flags |= SOCK_FLAG_CAMERA_DISTANCE_XZ;
}
static __used__ void SockParCamHeightAboveTerrain(nufpar_s *parser, void *) {
    sockpar_sock->camera_height_above_ground = NuFParGetFloat(parser);
}
static __used__ void SockParCamLateralRatio(nufpar_s *parser, void *) {
    const f32 ratio = NuFParGetFloat(parser);
    sockpar_sock->camera_local_x_ratio = ratio;
    sockpar_sock->camera_lateral_ratio = ratio;
}
static __used__ void SockParCamLockRatioY(nufpar_s *parser, void *) {
    f32 ratio = NuFParGetFloat(parser);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    sockpar_sock->unknown_84 = ratio;
    sockpar_sock->look_ratio_y = ratio;
}
static __used__ void SockParCamLookRatio(nufpar_s *parser, void *) {
    f32 ratio = NuFParGetFloat(parser);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    sockpar_sock->unknown_80 = ratio;
    sockpar_sock->unknown_84 = ratio;
    sockpar_sock->look_ratio_xz = ratio;
    sockpar_sock->look_ratio_y = ratio;
}
static __used__ void SockParCamOffset(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_offset.x = NuFParGetFloat(parser);
    sockpar_sock->camera_arena_offset.y = NuFParGetFloat(parser);
    sockpar_sock->camera_arena_offset.z = NuFParGetFloat(parser);
}
static __used__ void SockParCamPosSeek(nufpar_s *parser, void *) {
    sockpar_sock->camera_position_seek = NuFabs(NuFParGetFloat(parser));
}
static __used__ void SockParCamPullBackYRef(nufpar_s *, void *) {
    sockpar_sock->flags |= SOCK_FLAG_TWO_PLAYER_VERTICAL_SEPARATION;
}
static __used__ void SockParCamRailOffset(nufpar_s *parser, void *) {
    sockpar_sock->camera_rail_offset = NuFParGetFloat(parser);
}
static __used__ void SockParCamScaleLateral(nufpar_s *, void *) {
    sockpar_sock->flags |= SOCK_FLAG_SCALE_LATERAL_OFFSET;
}
static __used__ void SockParCamSegAhead(nufpar_s *parser, void *) {
    sockpar_sock->look_ahead_segments = static_cast<u8>(NuFParGetInt(parser));
}
static __used__ void SockParCamShake(nufpar_s *parser, void *) {
    sockpar_sock->camera_shake = NuFParGetFloat(parser);
}
static __used__ void SockParCamVerticalRatio(nufpar_s *parser, void *) {
    sockpar_sock->camera_vertical_ratio = NuFParGetFloat(parser);
}
static __used__ void SockParCamXOffset(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_offset.x = NuFParGetFloat(parser);
}
static __used__ void SockParCamYOffset(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_offset.y = NuFParGetFloat(parser);
}
static __used__ void SockParCamZOffset(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_offset.z = NuFParGetFloat(parser);
}
static __used__ void SockParCircuit(nufpar_s *, void *) {
}
static __used__ void SockParCurSpeed(nufpar_s *, void *) {
}
static __used__ void SockParIgnore(nufpar_s *, void *) {
}
static __used__ void SockParMidforce(nufpar_s *, void *) {
}
static __used__ void SockParMidforceYscale(nufpar_s *, void *) {
}
static __used__ void SockParMidForceDown(nufpar_s *, void *) {
}
static __used__ void SockParMidForceYScale(nufpar_s *, void *) {
}
static __used__ void SockParMidRangeInner(nufpar_s *, void *) {
}
static __used__ void SockParMidRangeOuter(nufpar_s *, void *) {
}
static __used__ void SockParMisc1(nufpar_s *, void *) {
}
static __used__ void SockParMisc2(nufpar_s *, void *) {
}
static __used__ void SockParMoveAngle(nufpar_s *, void *) {
}
static __used__ void SockParName(nufpar_s *, void *) {
}
static __used__ void SockParObj(nufpar_s *, void *) {
}
static __used__ void SockParOffsetBlendRatio(nufpar_s *parser, void *) {
    const f32 ratio = NuFParGetFloat(parser);
    sockpar_sock->camera_arena_blend = {ratio, ratio, ratio};
}
static __used__ void SockParOffsetBlendXRatio(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_blend.x = NuFParGetFloat(parser);
}
static __used__ void SockParOffsetBlendYRatio(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_blend.y = NuFParGetFloat(parser);
}
static __used__ void SockParOffsetBlendZRatio(nufpar_s *parser, void *) {
    sockpar_sock->camera_arena_blend.z = NuFParGetFloat(parser);
}
static __used__ void SockParPullbackRatio(nufpar_s *parser, void *) {
    sockpar_sock->two_player_pullback = NuFParGetFloat(parser);
}
static __used__ void SockParPullbackRatioXZ(nufpar_s *parser, void *) {
    sockpar_sock->two_player_pullback = NuFParGetFloat(parser);
    sockpar_sock->flags |= SOCK_FLAG_TWO_PLAYER_PLANAR_PULLBACK;
}
static __used__ void SockParTurnOff(nufpar_s *, void *) {
}
static __used__ void SockParYLimits(nufpar_s *, void *) {
    sockpar_sock->flags |= SOCK_FLAG_CLAMP_TARGET_Y;
}

extern "C" {
    NUFPCOMJMPCTX SockSys_ConfigKeywords[] = {
        {const_cast<char *>("circuit"), SockParCircuit},
        {const_cast<char *>("current_speed"), SockParCurSpeed},
        {const_cast<char *>("mid_range_inner"), SockParMidRangeInner},
        {const_cast<char *>("mid_range_outer"), SockParMidRangeOuter},
        {const_cast<char *>("mid_force_yscale"), SockParMidforceYscale},
        {const_cast<char *>("mid_force"), SockParMidforce},
        {const_cast<char *>("mid_force_down"), SockParMidForceDown},
        {const_cast<char *>("mid_force_yscale"), SockParMidForceYScale},
        {const_cast<char *>("cam_scale_lateral"), SockParCamScaleLateral},
        {const_cast<char *>("cam_look_ratio"), SockParCamLookRatio},
        {const_cast<char *>("cam_look_ratio_y"), SockParCamLockRatioY},
        {const_cast<char *>("campos_seek"), SockParCamPosSeek},
        {const_cast<char *>("camang_seek"), SockParCamAngSeek},
        {const_cast<char *>("cam_segments_ahead"), SockParCamSegAhead},
        {const_cast<char *>("cam_lateral_ratio"), SockParCamLateralRatio},
        {const_cast<char *>("cam_vertical_ratio"), SockParCamVerticalRatio},
        {const_cast<char *>("cam_offset"), SockParCamOffset},
        {const_cast<char *>("cam_x_offset"), SockParCamXOffset},
        {const_cast<char *>("cam_y_offset"), SockParCamYOffset},
        {const_cast<char *>("cam_z_offset"), SockParCamZOffset},
        {const_cast<char *>("cam_rail_offset"), SockParCamRailOffset},
        {const_cast<char *>("cam_arena_offset"), SockParCamArenaOffset},
        {const_cast<char *>("offset_blend_ratio"), SockParOffsetBlendRatio},
        {const_cast<char *>("offset_blend_x_ratio"), SockParOffsetBlendXRatio},
        {const_cast<char *>("offset_blend_y_ratio"), SockParOffsetBlendYRatio},
        {const_cast<char *>("offset_blend_z_ratio"), SockParOffsetBlendZRatio},
        {const_cast<char *>("cam_dist_to_target_xz"), SockParCamDistToTargetXZ},
        {const_cast<char *>("cam_pullback_yref"), SockParCamPullBackYRef},
        {const_cast<char *>("cam_dist_to_target"), SockParCamDistToTarget},
        {const_cast<char *>("cam_dist_ratio"), SockParCamDistRatio},
        {const_cast<char *>("ignore_socks"), SockParIgnore},
        {const_cast<char *>("y_limits"), SockParYLimits},
        {const_cast<char *>("turn_off"), SockParTurnOff},
        {const_cast<char *>("name"), SockParName},
        {const_cast<char *>("move_angle"), SockParMoveAngle},
        {const_cast<char *>("cam_behind_setting"), SockParCamBehind},
        {const_cast<char *>("cam_blend_time"), SockParCamBlendTime},
        {const_cast<char *>("cam_1player_zoom"), SockParCam1Zoom},
        {const_cast<char *>("cam_pullback_ratio"), SockParPullbackRatio},
        {const_cast<char *>("cam_pullback_ratio_xz"), SockParPullbackRatioXZ},
        {const_cast<char *>("cam_height_above_terrain"), SockParCamHeightAboveTerrain},
        {const_cast<char *>("cam_shake"), SockParCamShake},
        {const_cast<char *>("set_misc_1"), SockParMisc1},
        {const_cast<char *>("set_misc_2"), SockParMisc2},
        {const_cast<char *>("obj"), SockParObj},
        {const_cast<char *>("blend"), SockParBlend},
        {NULL, NULL},
    };
}
