#include "nu2api/nu3d/nurndrstat.h"

extern "C" void NuRndrStateSetFogEnabled(i32 enabled) {
    render_state.fog_enabled = enabled;
    render_state.fog_state = NULL;
    ++render_state.state.global_id;
    ++render_state.state.fog_id;
}

extern "C" void NuRndrStateSetFogState(f32 near_distance, f32 far_distance, u32 colour, f32 density) {
    render_state.fog_near = near_distance;
    render_state.fog_far = far_distance;
    render_state.fog_rgba = colour;
    render_state.fog_density = density;
    render_state.fog_state = NULL;
    ++render_state.state.global_id;
    ++render_state.state.fog_id;
}
