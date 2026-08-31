#pragma once

#include "decomp.h"

struct HostWindowOptions {
    bool capture = false;
    bool script_input = false;
    bool script_load = false;
    bool script_play = false;
    bool offscreen = false;
    bool mute = false;
    u64 script_tail_ms = 8000;
    u64 timeout_ms = 90000;
};

// Host harness command entry point.
i32 host_run_window(const HostWindowOptions &options);
