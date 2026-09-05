#ifndef GAME_TIMER_H
#define GAME_TIMER_H

#include "decomp.h"
#include "nu2api/numath/nuvec.h"
struct TIMER_s {
    union {
        struct {
            f32 time_elapsed;
            f32 last_time_elapsed;
            f32 time_elapsed_mod_seconds;
        };
        NUVEC elapsed_components;
    };
    i32 update_count;
};

typedef struct TIMER_s TIMER;

extern TIMER GlobalTimer;
extern f32 LastTouchTime;

#ifdef __cplusplus
void ResetTimer(TIMER *timer, f32 reset_time);
void UpdateTimer(TIMER *timer);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // GAME_TIMER_H
