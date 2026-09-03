#include "decomp.h"
#include "nu2api/nucore/common.h"

enum NUPHONE_TOUCH_ACTION {
    NUPHONE_TOUCH_DOWN = 0,
    NUPHONE_TOUCH_UP = 1,
    NUPHONE_TOUCH_MOVE = 2,
    NUPHONE_TOUCH_CANCEL = 3,
};

typedef struct NuPhoneOSMessageData {
    i32 touch_id;
    i32 x;
    i32 y;
    i32 pressure;
    NUPHONE_TOUCH_ACTION touch_action;
} NuPhoneOSMessageData;

DECOMP_ASSERT(sizeof(NuPhoneOSMessageData) == 0x14, "NuPhoneOSMessageData must remain 0x14 bytes");

typedef void PHONEEVENTCALLBACK(const NuPhoneOSMessageData *);

enum {
    PHONE_EVENT_TOUCH = 1,
    PHONE_EVENT_PAUSE = 3,
    PHONE_EVENT_RESUME = 4,
    PHONE_EVENT_BECOME_ACTIVE = 6,
};

#ifdef __cplusplus
extern "C" {
#endif
    void NuPhoneOSRegisterEventCallback(i32 type, PHONEEVENTCALLBACK *callback_fn);
#ifdef __cplusplus
}
#endif
