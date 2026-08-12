#include "legoapi/socksys.h"

#include <string.h>

extern "C" {

    void SockSysFindInScene(SOCKSYS *sock_sys, NUGSCN *gscn) {
        (void)sock_sys;
        (void)gscn;
    }

    SOCKSYS *SockSysInit(VARIPTR *buf, VARIPTR buf_end, NUGSCN *gscn) {
        buf->addr = ALIGN(buf->addr, 4);
        if (buf->addr + 0x4f08 >= buf_end.addr) {
            return NULL;
        }

        SOCKSYS *sys = (SOCKSYS *)buf->void_ptr;
        buf->void_ptr = (char *)buf->void_ptr + 8;
        sys->sock = (SOCK *)buf->void_ptr;
        buf->void_ptr = (char *)buf->void_ptr + 0x4f00;
        sys->count = 0;
        memset(sys->sock, 0, 0x4f00);
        SockSysFindInScene(sys, gscn);
        return sys;
    }
}
