#pragma once

#include "nu2api/nucore/common.h"

typedef i32 nurdpgetvarfn(char *, f32 *, i32 *);

#ifdef __cplusplus
extern "C" {
#endif
    f32 NuRDPF(char *input);
    f32 NuRDPFVar(char *input, nurdpgetvarfn *get_var_fn);

    i32 NuRDPI(char *input);
    i32 NuRDPIVar(char *input, nurdpgetvarfn *get_var_fn);
#ifdef __cplusplus
}
#endif
