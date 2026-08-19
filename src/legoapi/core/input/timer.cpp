#include "legoapi/core/input/timer.h"

#include "globals.h"
#include "nu2api/numath/nufloat.h"

void ResetTimer(TIMER *timer, f32 reset_time) {
    timer->last_time_elapsed = reset_time;
    timer->time_elapsed = reset_time;
    timer->time_elapsed_mod_seconds = NuFmod(reset_time, 1.0f);
    timer->update_count = 0;
}

void UpdateTimer(TIMER *timer) {
    timer->last_time_elapsed = timer->time_elapsed;
    timer->time_elapsed += FRAMETIME;
    timer->time_elapsed_mod_seconds += FRAMETIME;

    while (timer->time_elapsed_mod_seconds >= 1.0f) {
        timer->time_elapsed_mod_seconds -= 1.0f;
    }

    if (timer->update_count == -1)
        return;

    timer->update_count++;
}
