#include "decomp.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nucore/nustring.h"

#include <stdio.h>
#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" void *AISysBufferAlloc(VARIPTR *cursor, VARIPTR *buf_end, u32 size);

static char *gizaimessage_prefix = "msg_";

void ResetGizAIMessageSys(GIZAIMESSAGESYS_s *sys);

float GetGizAIMessage(GIZAIMESSAGESYS_s *sys, char const *name, GIZAIMESSAGE_s *out) {
    GIZAIMESSAGE_s *msg = CheckGizAIMessage(sys, name, out);
    return (msg != NULL) ? msg->value : 0.0f;
}

// libTTapp.so 0x4b6db0: fetch (or claim) the message and store its value.
GIZAIMESSAGE_s *SetGizAIMessage(GIZAIMESSAGESYS_s *sys, char const *name, float value, GIZAIMESSAGE_s *out) {
    GIZAIMESSAGE_s *msg = CheckGizAIMessage(sys, name, out);
    if (msg != NULL) {
        msg->value = value;
    }
    return msg;
}

// libTTapp.so 0x4b6c20: find a message by name on the system's active list;
// when no active message matches, move the head of the free list to the
// active list, name it, and return it. Names without the "msg_" prefix get
// one prepended (bounded to 0x1e characters). A non-NULL `out` argument is
// returned as-is, and so is NULL for a NULL system or name.
// libTTapp.so 0x4b6ae0: carve the system header (0x18 bytes) and the message
// pool (count * 0x38 bytes) from the permbuffer, then hand the pool to the
// free list. Returns NULL when the buffer is exhausted.
GIZAIMESSAGESYS_s *CreateGizAIMessageSys(VARIPTR *buf, VARIPTR *buf_end, i32 size) {
    GIZAIMESSAGESYS_s *sys = (GIZAIMESSAGESYS_s *)AISysBufferAlloc(buf, buf_end, 0x18);
    if (sys != NULL) {
        memset(sys, 0, 0x18);
        sys->messages = (GIZAIMESSAGE_s *)AISysBufferAlloc(buf, buf_end, (u32)size * 0x38);
        if (sys->messages != NULL) {
            sys->count = size;
            ResetGizAIMessageSys(sys);
        }
    }
    return sys;
}

GIZAIMESSAGE_s *CheckGizAIMessage(GIZAIMESSAGESYS_s *sys, char const *name, GIZAIMESSAGE_s *out) {
    if (sys == NULL) {
        return NULL;
    }
    if (out != NULL) {
        return out;
    }
    if (name == NULL) {
        return NULL;
    }

    char local[0x20];
    if (NuStrIStr((char *)name, gizaimessage_prefix) != NULL) {
        strcpy(local, name);
    } else {
        if (NuStrLen(name) + NuStrLen(gizaimessage_prefix) > 0x1e) {
            return NULL;
        }
        sprintf(local, "%s%s", gizaimessage_prefix, name);
    }

    for (NULISTLNK *node = NuLinkedListGetHead(&sys->active_list); node != NULL;
         node = NuLinkedListGetNext(&sys->active_list, node)) {
        if (NuStrNICmp(local, ((const GIZAIMESSAGE_s *)node)->name, 0x20) == 0) {
            return (GIZAIMESSAGE_s *)node;
        }
    }

    NULISTLNK *node = NuLinkedListGetHead(&sys->free_list);
    if (node == NULL) {
        return NULL;
    }
    NuLinkedListRemove(&sys->free_list, node);
    NuLinkedListAppend(&sys->active_list, node);
    NuStrNCpy(((GIZAIMESSAGE_s *)node)->name, local, 0x20);
    return (GIZAIMESSAGE_s *)node;
}

// libTTapp.so 0x4b6bd0: reset every active message's value to zero (the
// messages stay on the active list).
void ClearGizAIMessageSys(GIZAIMESSAGESYS_s *sys) {
    if (sys == NULL) {
        return;
    }
    for (NULISTLNK *node = NuLinkedListGetHead(&sys->active_list); node != NULL;
         node = NuLinkedListGetNext(&sys->active_list, node)) {
        ((GIZAIMESSAGE_s *)node)->value = 0.0f;
    }
}

// libTTapp.so 0x4b6ea0: the message's name lives right behind the links.
char *GizAIMessage_GetName(GIZAIMESSAGE_s *msg) {
    return (msg != NULL) ? msg->name : NULL;
}

// libTTapp.so 0x4b6a50: zero the two list headers and the pool, then queue
// every pooled message onto the free list.
void ResetGizAIMessageSys(GIZAIMESSAGESYS_s *sys) {
    if (sys == NULL) {
        return;
    }
    sys->free_list.head = NULL;
    sys->free_list.tail = NULL;
    sys->active_list.head = NULL;
    sys->active_list.tail = NULL;
    memset(sys->messages, 0, (usize)sys->count * 0x38);
    for (i32 i = 0; i < sys->count; i++) {
        NuLinkedListAppend(&sys->free_list, (NULISTLNK *)&sys->messages[i]);
    }
}

// libTTapp.so 0x4b6e50: iterate the active list — pass NULL to get the head.
GIZAIMESSAGE_s *QueryGizAIMessage(GIZAIMESSAGESYS_s *sys, GIZAIMESSAGE_s *msg) {
    if (msg != NULL) {
        return (GIZAIMESSAGE_s *)NuLinkedListGetNext(&sys->active_list, &msg->links);
    }
    return (GIZAIMESSAGE_s *)NuLinkedListGetHead(&sys->active_list);
}
