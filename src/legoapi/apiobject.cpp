#include "legoapi/apiobject.h"

#include <stddef.h>

extern "C" void APIObjectDestroyAll(APIOBJECTSYS *) {}

extern "C" void *APIObjectCreate(APIOBJECTSYS *) {
    return NULL;
}

extern "C" void APIObjectDestroy(APIOBJECTSYS *, void *) {}
