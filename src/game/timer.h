#ifndef GAME_TIMER_H
#define GAME_TIMER_H

struct TIMER_s {
    float time_elapsed;
    float last_time_elapsed;
    float time_elapsed_mod_seconds;
    int update_count;
};

typedef struct TIMER_s TIMER;

#ifdef __cplusplus
void ResetTimer(TIMER* timer, float reset_time);
void UpdateTimer(TIMER* timer);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // GAME_TIMER_H