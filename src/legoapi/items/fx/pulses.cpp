#include "decomp.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nuspecial.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Pulses_Reset(PULSESYS_s *pulse_sys) {
    PULSESYS_s *current_pulse_sys = pulse_sys;

    if (current_pulse_sys == NULL) {
        return;
    }
    if (current_pulse_sys->pulse_count == 0) {
        return;
    }
    for (i32 i = 0; i < current_pulse_sys->pulse_count; i++) {
        NuSpecialSetVisibility(&current_pulse_sys->pulses[i].special, 0);

        PULSE_s *pulse = &current_pulse_sys->pulses[i];
        pulse->timer = pulse->start_wait;
        pulse->disabled = 0;
        pulse->active = 0;
        if (pulse->gizmo_name[0] != '\0') {
            pulse->gizmo = GizmoFindByName(WorldInfo_CurrentlyActive()->gizmo_sys, -1, pulse->gizmo_name);
        }
    }
}

void Pulses_AddSfx(PULSESYS_s *, i32 *, i32 *, i32) {
}

void Pulses_Update(PULSESYS_s *) {
}

void Pulses_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
