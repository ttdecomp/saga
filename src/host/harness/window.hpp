#pragma once

#include "decomp.h"

struct HostWindowOptions {
    bool capture = false;
    bool script_input = false;
    bool script_load = false;
    bool script_play = false;
    bool script_action = false;
    bool script_pause = false;
    bool camera_orbit = false;
    bool camera_free = false;
    bool offscreen = false;
    bool mute = false;
    bool show_fps = false;
    bool msaa = true;
    bool portals = true;
    u64 script_tail_ms = 8000;
    u64 timeout_ms = 90000;
};

// Host harness command entry point.
i32 host_run_window(const HostWindowOptions &options);
