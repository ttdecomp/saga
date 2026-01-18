#pragma once

typedef unsigned char undefined;

#ifndef __cplusplus
typedef unsigned char bool;
#endif

typedef unsigned char byte;
typedef unsigned char dwfenc;
typedef unsigned int dword;
typedef long long longlong;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined3;
typedef unsigned int undefined4;
typedef unsigned long long undefined5;
typedef unsigned long long undefined6;
typedef unsigned long long undefined7;
typedef unsigned long long undefined8;
typedef unsigned short ushort;
typedef unsigned short word;

union int64 {
    longlong l;
    int i[2];
    uint u[2];
};

typedef struct NuFileBase NuFileBase, *PNuFileBase;

typedef dword T;

struct NuFileBase { /* PlaceHolder Structure */
    undefined field0_0x0;
    undefined field1_0x1;
    undefined field2_0x2;
    undefined field3_0x3;
    undefined4 field4_0x4;
    undefined field5_0x8;
    undefined field6_0x9;
    undefined field7_0xa;
    undefined field8_0xb;
    T field9_0xc;
    char path[256];
};
