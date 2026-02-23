#pragma once

#include "legoapi/gizmo.h"

#ifdef __cplusplus

typedef struct AI_s {
} AI;

typedef struct {
    char pad[8];
    char *name_ptr; 
} AIGIZMODEF;


typedef struct {
    char name_buffer[0x14]; 

    AIGIZMODEF *def;

    char _pad[152];

    char *suffix_3; // 0xB0
    char *suffix_2; // 0xB4
    char *suffix_1; // 0xB8
} AIGIZMOINTERNAL;


ADDGIZMOTYPE* AI_RegisterGizmo(int type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
