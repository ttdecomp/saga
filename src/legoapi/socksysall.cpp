#include "legoapi/socksys.h"

#include <stdio.h>
#include <string.h>

#include "nu2api/numath/nuvec.h"

// Spline name prefixes for a socket's rails; the socket index is appended
// zero-padded, e.g. "sock_cam_03". Looked up against the scene splines in
// SockSysFindInScene.
static const char *tSockCAM = "sock_cam_";
static const char *tSockA = "sock_a_";
static const char *tSockB = "sock_b_";
static const char *tSockC = "sock_c_";
static const char *tSockD = "sock_d_";
static const char *tSockMID = "sock_mid_";
static const char *tSockLEFT = "sock_left_";
static const char *tSockRIGHT = "sock_right_";
static const char *tSockLOOK = "sock_look_";
static const char *tSockLATERAL = "sock_lateral_";
static const char *tSockTRACKIN = "sock_trackin_";
static const char *tSockLIMIT = "sock_limit_";

// Grows `min`/`max` to include `pnt`.
static void AdjustMinMaxBox(NUVEC *pnt, NUVEC *min, NUVEC *max) {
    if (pnt->x < min->x) {
        min->x = pnt->x;
    }
    if (max->x < pnt->x) {
        max->x = pnt->x;
    }
    if (pnt->y < min->y) {
        min->y = pnt->y;
    }
    if (max->y < pnt->y) {
        max->y = pnt->y;
    }
    if (pnt->z < min->z) {
        min->z = pnt->z;
    }
    if (max->z < pnt->z) {
        max->z = pnt->z;
    }
}

extern "C" {

    void SockSysFindInScene(SOCKSYS *sock_sys, NUGSCN *gscn) {
        int i;
        int j;
        SOCK *sock;
        int len;
        float extent_z;
        char name[256];

        if ((gscn != NULL) && (sock_sys != NULL)) {
            sock = sock_sys->sock;
            for (i = 0; i < 0x40; i++, sock++) {
                if (sock->cam != NULL) {
                    continue;
                }
                sock->unknown_7c = 1.0f;
                sock->unknown_80 = 1.0f;
                sock->unknown_84 = 1.0f;
                sock->unknown_88 = 1.0f;
                sock->unknown_8c = 1.0f;
                sock->unknown_90 = 5.0f;
                sock->unknown_94 = 5.0f;
                sprintf(sock->name, "%.2i", i);
                sock->unknown_f4 = 0.5f;
                sprintf(name, "%s%.2i", tSockCAM, i);
                sock->cam = NuSplineFind(gscn, name);
                if (sock->cam == NULL) {
                    continue;
                }
                len = (int)(short)sock->cam->length;
                if (1 < len) {
                    sock->length = len - 1;
                    sprintf(name, "%s%.2i", tSockA, i);
                    sock->a = NuSplineFind(gscn, name);
                    if (sock->a == NULL) {
                        continue;
                    }
                    if ((int)(short)sock->a->length != len) {
                        continue;
                    }
                    sprintf(name, "%s%.2i", tSockB, i);
                    sock->b = NuSplineFind(gscn, name);
                    if (sock->b == NULL) {
                        continue;
                    }
                    if ((int)(short)sock->b->length != len) {
                        continue;
                    }
                    sprintf(name, "%s%.2i", tSockC, i);
                    sock->c = NuSplineFind(gscn, name);
                    if (sock->c == NULL) {
                        sock->flags |= 1;
                    } else if ((int)(short)sock->c->length != len) {
                        sock->flags |= 1;
                    }
                    if ((sock->flags & 1) == 0) {
                        sprintf(name, "%s%.2i", tSockD, i);
                        sock->d = NuSplineFind(gscn, name);
                        if (sock->d == NULL) {
                            sock->flags |= 1;
                        } else if ((int)(short)sock->d->length != len) {
                            sock->flags |= 1;
                        }
                    }
                    sprintf(name, "%s%.2i", tSockMID, i);
                    sock->mid = NuSplineFind(gscn, name);
                    if ((sock->mid != NULL) && ((int)(short)sock->mid->length != len)) {
                        sock->mid = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLEFT, i);
                    sock->left = NuSplineFind(gscn, name);
                    if ((sock->left != NULL) && ((int)(short)sock->left->length != len)) {
                        sock->left = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockRIGHT, i);
                    sock->right = NuSplineFind(gscn, name);
                    if ((sock->right != NULL) && ((int)(short)sock->right->length != len)) {
                        sock->right = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLOOK, i);
                    sock->look = NuSplineFind(gscn, name);
                    if ((sock->look != NULL) && ((int)(short)sock->look->length != len)) {
                        sock->look = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLATERAL, i);
                    sock->lateral = NuSplineFind(gscn, name);
                    if ((sock->lateral != NULL) && ((int)(short)sock->lateral->length != len)) {
                        sock->lateral = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockTRACKIN, i);
                    sock->trackin = NuSplineFind(gscn, name);
                    if ((sock->trackin != NULL) && ((int)(short)sock->trackin->length != len)) {
                        sock->trackin = NULL;
                    }
                    sprintf(name, "%s%.2i", tSockLIMIT, i);
                    sock->limit = NuSplineFind(gscn, name);
                    sock->max = sock->a->pts[0];
                    sock->min = sock->max;
                    for (j = 0; j < len; j++) {
                        AdjustMinMaxBox(&sock->a->pts[j], &sock->min, &sock->max);
                        AdjustMinMaxBox(&sock->b->pts[j], &sock->min, &sock->max);
                        if ((sock->flags & 1) == 0) {
                            AdjustMinMaxBox(&sock->c->pts[j], &sock->min, &sock->max);
                            AdjustMinMaxBox(&sock->d->pts[j], &sock->min, &sock->max);
                        }
                    }
                    sock->center.x = (sock->min.x + sock->max.x) * 0.5f;
                    sock->center.y = (sock->min.y + sock->max.y) * 0.5f;
                    sock->center.z = (sock->min.z + sock->max.z) * 0.5f;
                    sock->extent = sock->max.x - sock->center.x;
                    extent_z = sock->max.z - sock->center.z;
                    if (extent_z < sock->extent) {
                        sock->extent = extent_z;
                    }
                    sock->valid = 1;
                    sock_sys->count++;
                }
            }
        }
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
