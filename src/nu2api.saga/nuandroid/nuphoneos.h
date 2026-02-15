#include "nu2api.saga/nucore/common.h"

typedef struct NuPhoneOSMessageData {
} NuPhoneOSMessageData;

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
