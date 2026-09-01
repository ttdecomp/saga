#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuplane.h"
#include "decomp_assert.h"

typedef struct nuportal_s {
    NUPLANE plane; // 0x00, separates the two adjoining rooms
    u8 pad_10[6];
    u16 front_room; // 0x16
    u16 back_room;  // 0x18
    i8 id;          // 0x1a
    u8 pad_1b;
    u32 is_active; // 0x1c, bit 0 enables traversal
} NUPORTAL;

enum NUPORTAL_FLAGS {
    NUPORTAL_FLAG_ACTIVE = 0x01,
    NUPORTAL_FLAG_DEFAULT_ACTIVE = 0x02,
};

typedef struct nuroom_s {
    i16 *instance_indices; // 0x00, display-scene instances belonging to this room
    NUPLANE *planes;       // 0x04, inward-facing convex-room planes
    i16 *portal_indices;   // 0x08
    i16 instance_count;    // 0x0c
    u8 plane_count;        // 0x0e
    u8 portal_count;       // 0x0f
    u8 flags;              // 0x10, bit 0 marks overlapping/priority rooms
    u8 pad_11[3];
    u8 priority; // 0x14, resolves overlapping rooms
    u8 pad_15[3];
} NUROOM;

enum NUROOM_FLAGS {
    NUROOM_FLAG_OVERLAPPING = 0x01,
    NUROOM_FLAG_VISITED = 0x04,
};

typedef struct NUFRUSTRUM {
    f32 transposed_planes[32]; // 0x00, four SIMD-friendly columns for up to eight planes
    NUPLANE *planes;           // 0x80
    NUVEC minimum;             // 0x84, clipped portal extent minimum
    NUVEC maximum;             // 0x90, clipped portal extent maximum
    i16 plane_count;           // 0x9c
    i16 room_id;               // 0x9e
} NUFRUSTRUM;

DECOMP_ASSERT(sizeof(NUPORTAL) == 0x20, "portal record size");
DECOMP_ASSERT(sizeof(NUROOM) == 0x18, "room record size");
DECOMP_ASSERT(sizeof(NUFRUSTRUM) == 0xa0, "portal frustum size");
DECOMP_ASSERT(offsetof(NUROOM, instance_indices) == 0x00, "room instance-index offset");
DECOMP_ASSERT(offsetof(NUROOM, planes) == 0x04, "room plane-array offset");
DECOMP_ASSERT(offsetof(NUROOM, portal_indices) == 0x08, "room portal-index offset");
DECOMP_ASSERT(offsetof(NUROOM, instance_count) == 0x0c, "room instance-count offset");

struct nugscn_s;

#ifdef __cplusplus
extern "C" {
#endif

    void NuPortalInit(void);
    i32 NuPortalVisibility(struct nugscn_s *scene);
    void NuPortalMaxDepth(struct nugscn_s *scene, i32 depth);
    i32 NuPortalEnabled(i32 enabled);
    i32 NuPortalNumRooms(struct nugscn_s *scene);
    void NuPortalResetActive(struct nugscn_s *scene);
    i32 NuPortalRoomClipTest(struct nugscn_s *scene, i16 room_id);
    void NuPortalRoomClipTestAll(struct nugscn_s *scene, u8 *room_visibility);
    void NuPortalSetActive(struct nugscn_s *scene, i32 portal_id, i32 active);
    void NuPortalSetActiveDirect(NUPORTAL *portal, i32 active);
    i32 NuPortalWhichRoom(struct nugscn_s *scene, NUVEC *position);
    void FlagRoomInstancesAsVisible(NUROOM *room, struct nugscn_s *scene);

#ifdef __cplusplus
}

void NuPortalSetOverride(NUVEC *position);
void NuPortalEnableDebugDraw(i32 enabled);
#endif
