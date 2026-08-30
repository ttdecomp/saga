#pragma once

#include "decomp.h"

enum class HostLoadAction {
    inspect,
    list,
    extract,
};

struct HostLoadOptions {
    HostLoadAction action = HostLoadAction::inspect;
    const char *filter = nullptr;
    const char *dat_path = nullptr;
    const char *output_path = ".work/extracted.bin";
};

// Host harness command entry point.
i32 host_run_load(const HostLoadOptions &options);
