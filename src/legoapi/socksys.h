#ifndef LEGOAPI_SOCKSYS_H
#define LEGOAPI_SOCKSYS_H

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/common.h"

// SockSys — spline-based rail camera system used during world loading. A
// SOCKSYS is carved out of the giz buffer: an 8-byte header followed by a
// fixed array of SOCK entries (0x4f00 bytes total).
typedef struct SOCK_s SOCK;

typedef struct SOCKSYS_s {
    SOCK *sock; // 0x0 — array of SOCK entries
    i32 count;  // 0x4 — number of sockets with a valid rail
} SOCKSYS;

#ifdef __cplusplus
extern "C" {
#endif

    void SockSysFindInScene(SOCKSYS *sock_sys, NUGSCN *gscn);
    SOCKSYS *SockSysInit(VARIPTR *buf, VARIPTR buf_end, NUGSCN *gscn);

#ifdef __cplusplus
}
#endif

#endif // LEGOAPI_SOCKSYS_H
