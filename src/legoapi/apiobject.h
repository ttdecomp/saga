#pragma once

typedef struct APIOBJECT_s {
} APIOBJECT;

typedef struct APIOBJECTSYS_s {
} APIOBJECTSYS;

#ifdef __cplusplus
extern "C" {
#endif

void APIObjectDestroyAll(APIOBJECTSYS *object_system);
void *APIObjectCreate(APIOBJECTSYS *object_system);
void APIObjectDestroy(APIOBJECTSYS *object_system, void *object);

#ifdef __cplusplus
}
#endif
