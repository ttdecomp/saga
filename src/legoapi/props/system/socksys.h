#ifndef LEGOAPI_SOCKSYS_H
#define LEGOAPI_SOCKSYS_H

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nucore/common.h"

// SockSys — spline-based rail camera system used during world loading. A
// SOCKSYS is carved out of the giz buffer: an 8-byte header followed by a
// fixed array of 64 SOCK entries (0x4f00 bytes total).
//
// Each SOCK describes one camera "socket": rails in the scene are named
// `sock_cam_00`..`sock_cam_3f` (plus `sock_a_`, `sock_b_`, `sock_c_`,
// `sock_d_`, `sock_mid_`, `sock_left_`, `sock_right_`, `sock_look_`,
// `sock_lateral_`, `sock_trackin_`, `sock_limit_`) and are resolved against
// the scene splines by SockSysFindInScene.
typedef struct SOCK_s {
    NUGSPLINE *cam;     // 0x00 — sock_cam_ rail; NULL until found in scene
    NUGSPLINE *a;       // 0x04 — sock_a_ rail spline
    NUGSPLINE *b;       // 0x08 — sock_b_ rail spline
    NUGSPLINE *c;       // 0x0c — sock_c_ rail spline (optional)
    NUGSPLINE *d;       // 0x10 — sock_d_ rail spline (optional)
    NUGSPLINE *mid;     // 0x14 — sock_mid_ centre spline (optional)
    NUGSPLINE *left;    // 0x18 — sock_left_ rail spline (optional)
    NUGSPLINE *right;   // 0x1c — sock_right_ rail spline (optional)
    NUGSPLINE *look;    // 0x20 — sock_look_ spline (optional)
    NUGSPLINE *lateral; // 0x24 — sock_lateral_ spline (optional)
    NUGSPLINE *trackin; // 0x28 — sock_trackin_ spline (optional)
    NUGSPLINE *limit;   // 0x2c — sock_limit_ spline (optional)
    u16 length;         // 0x30 — rail point count - 1
    u8 valid;           // 0x32 — 1 once the socket has been populated
    u8 unknown_33;      // 0x33
    u32 unknown_34;     // 0x34
    u32 unknown_38;     // 0x38 — per-point angle table (u16 entries)
    u32 unknown_3c;     // 0x3c
    NUVEC min;          // 0x40 — min of the A/B(/C/D) rail points
    NUVEC max;          // 0x4c — max of the A/B(/C/D) rail points
    NUVEC center;       // 0x58 — midpoint of min and max
    f32 extent;         // 0x64 — half of the smaller of the x/z extents
    u16 flags;          // 0x68 — bit 0: missing C or D rail (length mismatch)
    u8 unknown_6a;      // 0x6a
    u8 unknown_6b;      // 0x6b
    u8 unknown_6c;      // 0x6c
    u8 unknown_6d;      // 0x6d
    u8 unknown_6e;      // 0x6e
    u8 unknown_6f;      // 0x6f
    u8 unknown_70;      // 0x70
    u8 unknown_71;      // 0x71
    u8 unknown_72;      // 0x72
    u8 unknown_73;      // 0x73
    u8 unknown_74;      // 0x74
    u8 unknown_75;      // 0x75
    u8 unknown_76;      // 0x76
    u8 unknown_77;      // 0x77
    u8 unknown_78;      // 0x78
    u8 unknown_79;      // 0x79
    u8 unknown_7a;      // 0x7a
    u8 unknown_7b;      // 0x7b
    f32 unknown_7c;     // 0x7c — default 1.0
    f32 unknown_80;     // 0x80 — default 1.0
    f32 unknown_84;     // 0x84 — default 1.0
    f32 unknown_88;     // 0x88 — default 1.0; blend weight toward look pos
    f32 unknown_8c;     // 0x8c — default 1.0; blend weight toward look pos
    f32 unknown_90;     // 0x90 — default 5.0
    f32 unknown_94;     // 0x94 — default 5.0
    f32 unknown_98;     // 0x98
    f32 unknown_9c;     // 0x9c
    f32 unknown_a0;     // 0xa0
    f32 unknown_a4;     // 0xa4
    f32 unknown_a8;     // 0xa8
    f32 unknown_ac;     // 0xac
    f32 unknown_b0;     // 0xb0
    f32 unknown_b4;     // 0xb4
    f32 unknown_b8;     // 0xb8
    f32 unknown_bc;     // 0xbc
    f32 unknown_c0;     // 0xc0
    f32 unknown_c4;     // 0xc4
    f32 unknown_c8;     // 0xc8
    f32 unknown_cc;     // 0xcc
    f32 unknown_d0;     // 0xd0
    f32 unknown_d4;     // 0xd4
    f32 unknown_d8;     // 0xd8
    u32 unknown_dc;     // 0xdc
    u32 unknown_e0;     // 0xe0
    char name[16];      // 0xe4 — zero-padded socket index, e.g. "03"
    f32 unknown_f4;     // 0xf4 — default 0.5
    u32 unknown_f8;     // 0xf8 — array of scene specials shown on the socket
    u16 unknown_fc;     // 0xfc — count of specials at 0xf8
    u8 unknown_fe;      // 0xfe
    u8 unknown_ff;      // 0xff
    u8 unknown_100[16]; // 0x100 — exception entries (2 bytes each)
    u32 unknown_110;    // 0x110 — exception entry count
    u8 unknown_114[40]; // 0x114
} SOCK;

typedef struct SOCKSYS_s {
    SOCK *sock; // 0x0 — array of 64 SOCK entries
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