#include "decomp.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "globals.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nufloat.h"

#include <string.h>

extern "C" {

    void APIObjectCollision(void) {
    }

    void APIObjectCollision2D(void) {
    }

    void APIObjectCollisions(void) {
    }

    void APIObjectSetUsed(APIOBJECT *object, u8 index, i32 used) {
        if (used != 0) {
            object->field_0x1f8 |= APIOBJECT_FLAG_IN_USE;
            object->field_0x289 = index;
        } else {
            object->field_0x1f8 &= ~APIOBJECT_FLAG_IN_USE;
        }
    }

    APIOBJECT *APIObjectCreate(APIOBJECTSYS_s *system) {
        if (system == NULL || system->object_size == 0 || system->objects == NULL) {
            return NULL;
        }

        u8 *slot = reinterpret_cast<u8 *>(system->objects);
        for (u8 index = 0; index < 64; index++, slot += system->object_size) {
            APIOBJECT *object = reinterpret_cast<APIOBJECT *>(slot);
            if ((object->field_0x1f8 & APIOBJECT_FLAG_IN_USE) == 0) {
                memset(object, 0, system->object_size);
                APIObjectSetUsed(object, index, 1);
                return object;
            }
        }
        return NULL;
    }

    void APIObjectDestroyAll(APIOBJECTSYS_s *system) {
        if (system != NULL) {
            memset(system->objects, 0, system->object_size * 64);
        }
    }

    void APIObjectRemoveFromLOSTable(APIOBJECTSYS_s *, APIOBJECT *, APIOBJECT *) {
    }

    void APIObjectDestroy(APIOBJECTSYS_s *system, APIOBJECT *object) {
        if (system == NULL || object == NULL || system->object_size == 0) {
            return;
        }
        for (i32 i = 0; i < 64; i++) {
            APIOBJECT *other =
                reinterpret_cast<APIOBJECT *>(reinterpret_cast<u8 *>(system->objects) + system->object_size * i);
            if ((other->field_0x1f8 & APIOBJECT_FLAG_IN_USE) != 0) {
                APIObjectRemoveFromLOSTable(system, other, object);
            }
        }
        APIObjectRemoveFromLOSTable(system, NULL, object);
        memset(object, 0, system->object_size);
    }

    void APIObjectLOSChecks(void) {
    }

    void APIObjectVelocities(GameObject_s *object) {
        APIOBJECT &api = object->apiobj;
        if (api.velocity.x == 0.0f && api.velocity.z == 0.0f) {
            api.horizontal_velocity_magnitude = 0.0f;
            api.velocity_magnitude = NuFabs(api.velocity.y);
            return;
        }

        const f32 horizontal_squared = api.velocity.x * api.velocity.x + api.velocity.z * api.velocity.z;
        api.horizontal_velocity_magnitude = NuFsqrt(horizontal_squared);

        if (api.velocity.y == 0.0f) {
            api.velocity_magnitude = api.horizontal_velocity_magnitude;
        } else {
            api.velocity_magnitude = NuFsqrt(horizontal_squared + api.velocity.y * api.velocity.y);
        }
    }

    void AddCollisionSphere(void) {
    }

    void FlagRoomInstancesAsVisible(NUROOM *room, NUGSCN *) {
        for (i32 i = 0; i < room->instance_count; ++i) {
            PortalVisiFlags[room->instance_indices[i] >> 3] =
                PortalVisiFlags[room->instance_indices[i] >> 3] | static_cast<u8>(1 << (room->instance_indices[i] & 7));
        }
    }

    void StoreLocatorCoordinates(void) {
    }

    void WindShear(void) {
    }

} // extern "C"
