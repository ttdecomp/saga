#ifndef NU2API_SAGA_NUMATH_NUVEC_H
#define NU2API_SAGA_NUMATH_NUVEC_H

typedef struct {
    float x;
    float y;
    float z;
} nuvec_s;

typedef nuvec_s NUVEC;

void NuVecAdd(NUVEC* v, NUVEC* v0, NUVEC* v1);
void NuVecSub(NUVEC* v, NUVEC* v0, NUVEC* v1);

#endif // NU2API_SAGA_NUMATH_NUVEC_H