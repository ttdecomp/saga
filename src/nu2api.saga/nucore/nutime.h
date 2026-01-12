#ifndef NU2API_SAGA_NUCORE_NUTIME_H
#define NU2API_SAGA_NUCORE_NUTIME_H

struct nutime_s {
    unsigned int low;
    unsigned int high;
};

typedef struct nutime_s NUTIME;

#ifdef __cplusplus
void NuTimeGetTicksPS(unsigned int *low, unsigned int *high);
unsigned long long NuGetCurrentTimeMilisecondsPS(void);

extern "C" {
#endif

void NuTimeInitPS(void);
void NuTimeGet(NUTIME *t);
void NuTimeSub(NUTIME *t, NUTIME *a, NUTIME *b);

#ifdef __cplusplus
}
#endif

#endif // NU2API_SAGA_NUCORE_NUTIME_H