#pragma once

#include "decomp.h"
#include "nu2api/nucore/common.h"

struct CHARFIXUP {
    char *name;
    i16 *id;
};

typedef struct CHARFIXUP CHARFIXUP;

struct GameObject_s;
struct nugscn_s;
struct CHARACTERMODEL_s;
struct ANIMPACKET_s;
struct NUJOINTANIM_s;
struct numtx_s;
struct WORLDINFO_s;
using CHARACTERUPDATEFN = void (*)(GameObject_s *);
using MAKELAYERLISTFN = i32 (*)(CHARACTERMODEL_s *, i16 *, u32);

struct CHARACTERANIM_s {
    char *name;
    u32 flags;
    i16 animation_id;
    u8 data_0xa[0x42];
};

DECOMP_ASSERT(sizeof(CHARACTERANIM_s) == 0x4c, "CHARACTERANIM_s size");

struct CHARSCENE_s {
    nugscn_s *scene;
    void *special_scene;
    void *special;
    void *display_special;
};

DECOMP_ASSERT(sizeof(CHARSCENE_s) == 0x10, "CHARSCENE_s size");

extern CHARSCENE_s *CharScene_Area;

struct gamecharacterdata_s {         /* PlaceHolder Structure */
    MAKELAYERLISTFN make_layer_list; // 0x00
    undefined field4_0x4;
    undefined field5_0x5;
    undefined field6_0x6;
    undefined field7_0x7;
    undefined field8_0x8;
    undefined field9_0x9;
    undefined field10_0xa;
    undefined field11_0xb;
    undefined field12_0xc;
    undefined field13_0xd;
    undefined field14_0xe;
    undefined field15_0xf;
    undefined field16_0x10;
    undefined field17_0x11;
    undefined field18_0x12;
    undefined field19_0x13;
    undefined field20_0x14;
    undefined field21_0x15;
    undefined field22_0x16;
    undefined field23_0x17;
    undefined field24_0x18;
    undefined field25_0x19;
    undefined field26_0x1a;
    undefined field27_0x1b;
    undefined field28_0x1c;
    undefined field29_0x1d;
    undefined field30_0x1e;
    undefined field31_0x1f;
    undefined field32_0x20;
    undefined field33_0x21;
    undefined field34_0x22;
    undefined field35_0x23;
    undefined field36_0x24;
    undefined field37_0x25;
    undefined field38_0x26;
    undefined field39_0x27;
    undefined field40_0x28;
    undefined field41_0x29;
    undefined field42_0x2a;
    undefined field43_0x2b;
    undefined field44_0x2c;
    undefined field45_0x2d;
    undefined field46_0x2e;
    undefined field47_0x2f;
    undefined field48_0x30;
    undefined field49_0x31;
    undefined field50_0x32;
    undefined field51_0x33;
    undefined field52_0x34;
    undefined field53_0x35;
    undefined field54_0x36;
    undefined field55_0x37;
    undefined field56_0x38;
    undefined field57_0x39;
    undefined field58_0x3a;
    undefined field59_0x3b;
    undefined field60_0x3c;
    undefined field61_0x3d;
    undefined field62_0x3e;
    undefined field63_0x3f;
    undefined field64_0x40;
    undefined field65_0x41;
    undefined field66_0x42;
    undefined field67_0x43;
    undefined field68_0x44;
    undefined field69_0x45;
    undefined field70_0x46;
    undefined field71_0x47;
    undefined field72_0x48;
    undefined field73_0x49;
    undefined field74_0x4a;
    undefined field75_0x4b;
    undefined field76_0x4c;
    undefined field77_0x4d;
    undefined field78_0x4e;
    undefined field79_0x4f;
    undefined field80_0x50;
    undefined field81_0x51;
    undefined field82_0x52;
    undefined field83_0x53;
    undefined field84_0x54;
    undefined field85_0x55;
    undefined field86_0x56;
    undefined field87_0x57;
    undefined field88_0x58;
    undefined field89_0x59;
    undefined field90_0x5a;
    undefined field91_0x5b;
    undefined field92_0x5c;
    undefined field93_0x5d;
    undefined field94_0x5e;
    undefined field95_0x5f;
    undefined field96_0x60;
    undefined field97_0x61;
    undefined field98_0x62;
    undefined field99_0x63;
    f32 viewdistance;  // 0x64
    f32 heardistance;  // 0x68
    f32 maxviewheight; // 0x6c
    f32 minviewheight; // 0x70
    undefined field116_0x74;
    undefined field117_0x75;
    undefined field118_0x76;
    undefined field119_0x77;
    undefined field120_0x78;
    undefined field121_0x79;
    undefined field122_0x7a;
    undefined field123_0x7b;
    undefined field124_0x7c;
    undefined field125_0x7d;
    undefined field126_0x7e;
    undefined field127_0x7f;
    undefined field128_0x80;
    undefined field129_0x81;
    undefined field130_0x82;
    undefined field131_0x83;
    undefined field132_0x84;
    undefined field133_0x85;
    undefined field134_0x86;
    undefined field135_0x87;
    undefined field136_0x88;
    undefined field137_0x89;
    undefined field138_0x8a;
    undefined field139_0x8b;
    undefined field140_0x8c;
    undefined field141_0x8d;
    undefined field142_0x8e;
    undefined field143_0x8f;
    undefined4 field144_0x90;
    undefined field145_0x94;
    undefined field146_0x95;
    undefined field147_0x96;
    undefined field148_0x97;
    undefined field149_0x98;
    undefined field150_0x99;
    undefined field151_0x9a;
    undefined field152_0x9b;
    undefined field153_0x9c;
    undefined field154_0x9d;
    undefined field155_0x9e;
    undefined field156_0x9f;
    undefined field157_0xa0;
    undefined field158_0xa1;
    undefined field159_0xa2;
    undefined field160_0xa3;
    undefined field161_0xa4;
    undefined field162_0xa5;
    undefined field163_0xa6;
    undefined field164_0xa7;
    undefined field165_0xa8;
    undefined field166_0xa9;
    undefined field167_0xaa;
    undefined field168_0xab;
    undefined field169_0xac;
    undefined field170_0xad;
    undefined field171_0xae;
    undefined field172_0xaf;
    undefined field173_0xb0;
    undefined field174_0xb1;
    undefined field175_0xb2;
    undefined field176_0xb3;
    undefined field177_0xb4;
    undefined field178_0xb5;
    undefined field179_0xb6;
    undefined field180_0xb7;
    undefined field181_0xb8;
    undefined field182_0xb9;
    undefined field183_0xba;
    undefined field184_0xbb;
    undefined field185_0xbc;
    undefined field186_0xbd;
    undefined field187_0xbe;
    undefined field188_0xbf;
    undefined field189_0xc0;
    undefined field190_0xc1;
    undefined field191_0xc2;
    undefined field192_0xc3;
    undefined field193_0xc4;
    undefined field194_0xc5;
    undefined field195_0xc6;
    undefined field196_0xc7;
    undefined field197_0xc8;
    undefined field198_0xc9;
    undefined field199_0xca;
    undefined field200_0xcb;
    undefined field201_0xcc;
    undefined field202_0xcd;
    undefined field203_0xce;
    undefined field204_0xcf;
    undefined field205_0xd0;
    undefined field206_0xd1;
    undefined field207_0xd2;
    undefined field208_0xd3;
    undefined field209_0xd4;
    undefined field210_0xd5;
    undefined field211_0xd6;
    undefined field212_0xd7;
    undefined field213_0xd8;
    undefined field214_0xd9;
    undefined field215_0xda;
    undefined field216_0xdb;
    undefined field217_0xdc;
    undefined field218_0xdd;
    undefined field219_0xde;
    undefined field220_0xdf;
    undefined field221_0xe0;
    undefined field222_0xe1;
    undefined field223_0xe2;
    undefined field224_0xe3;
    undefined field225_0xe4;
    undefined field226_0xe5;
    undefined field227_0xe6;
    undefined field228_0xe7;
    undefined field229_0xe8;
    undefined field230_0xe9;
    undefined field231_0xea;
    undefined field232_0xeb;
    undefined field233_0xec;
    undefined field234_0xed;
    undefined field235_0xee;
    undefined field236_0xef;
    undefined field237_0xf0;
    undefined field238_0xf1;
    undefined field239_0xf2;
    undefined field240_0xf3;
    undefined field241_0xf4;
    undefined field242_0xf5;
    undefined field243_0xf6;
    undefined field244_0xf7;
    undefined field245_0xf8;
    undefined field246_0xf9;
    undefined field247_0xfa;
    undefined field248_0xfb;
    undefined field249_0xfc;
    undefined field250_0xfd;
    undefined field251_0xfe;
    undefined field252_0xff;
    undefined field253_0x100;
    undefined field254_0x101;
    undefined field255_0x102;
    undefined field256_0x103;
    undefined field257_0x104;
    undefined field258_0x105;
    undefined field259_0x106;
    undefined field260_0x107;
    undefined field261_0x108;
    undefined field262_0x109;
    undefined field263_0x10a;
    undefined field264_0x10b;
    undefined field265_0x10c;
    undefined field266_0x10d;
    undefined field267_0x10e;
    undefined field268_0x10f;
    undefined field269_0x110;
    undefined field270_0x111;
    undefined field271_0x112;
    undefined field272_0x113;
    undefined field273_0x114;
    undefined field274_0x115;
    undefined field275_0x116;
    undefined field276_0x117;
    undefined field277_0x118;
    undefined field278_0x119;
    undefined field279_0x11a;
    undefined field280_0x11b;
    undefined field281_0x11c;
    undefined field282_0x11d;
    undefined field283_0x11e;
    undefined field284_0x11f;
};
typedef struct gamecharacterdata_s GAMECHARACTERDATA;

struct characterdata_s { /* PlaceHolder Structure */
    i32 field0_0x0;
    u32 model_flags;
    char *dir;
    char *file;
    CHARACTERANIM_s *animations;
    undefined2 field5_0x14;
    undefined field6_0x16;
    undefined field7_0x17;
    CHARACTERUPDATEFN move_fn;
    CHARACTERUPDATEFN animate_fn;
    CHARACTERUPDATEFN draw_fn; // 0x20
    void *field11_0x24;
    undefined4 field12_0x28;
    f32 field13_0x2c;
    f32 field14_0x30;
    f32 field15_0x34;
    f32 field16_0x38;
    f32 field17_0x3c;
    byte flags;
    undefined field19_0x41;
    i16 field20_0x42;
    undefined4 field21_0x44;
    undefined4 field22_0x48;
};
typedef struct characterdata_s CHARACTERDATA;

extern "C" i32 MakeLayerList_Index(CHARACTERMODEL_s *model, i16 *layers, u32 mask);
extern "C" i32 APIDrawCharacterModel(CHARACTERMODEL_s *model, CHARACTERDATA *character_data, ANIMPACKET_s *animation,
                                     numtx_s *matrix, numtx_s *secondary_matrix, numtx_s *reflection_matrix,
                                     i32 unused_zero, numtx_s *auxiliary_matrix, GameObject_s *object, u32 flags,
                                     NUJOINTANIM_s *joint_overrides, i32 joint_override_count, WORLDINFO_s *world,
                                     f32 far_clip, numtx_s *output_matrices, i32 value_15, void *level_model);

extern i32 CHARCOUNT;
extern CHARACTERDATA *CDataList;
extern GAMECHARACTERDATA *GCDataList;
extern GAMECHARACTERDATA GCDATA_DEFAULT;

#ifdef __cplusplus
extern "C" {
#endif
    void APITransparentInit();
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

i32 CharIDFromName(char *name);

extern "C" {
#endif
    extern i32 g_loadingCharacterInHub;

    CHARACTERDATA *ConfigureCharacterList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 count,
                                          i32 *countDest, i32 count2, GAMECHARACTERDATA **dest);
#ifdef __cplusplus
}
#endif

// Model id globals (assigned from the character name table in character.cpp)
extern "C" {
    extern i16 id_KAADU;
    extern i16 id_GUNGAN;
    extern i16 id_FALUMPASET;
    extern i16 id_DARTHMAUL;
    extern i16 id_ANAKINSPOD;
    extern i16 id_STAP;
    extern i16 id_BATTLEDROID;
    extern i16 id_ROYALGUARD;
    extern i16 id_CLONEARC;
    extern i16 id_IMPERIALSHUTTLE;
    extern i16 id_NABOOSTARFIGHTER;
    extern i16 id_XWING;
    extern i16 id_SNOWSPEEDER;
    extern i16 id_MILLENNIUMFALCON;
    extern i16 id_NEW_REPUBLIC_GUNSHIP;
    extern i16 id_COUNTDOOKU;
    extern i16 id_OBIWANKENOBIJEDIMASTER;
    extern i16 id_GRIEVOUS;
    extern i16 id_JANGOFETT;
    extern i16 id_MOSEISLEYCITIZEN;
    extern i16 id_CANTINAALIEN;
    extern i16 id_CLOUDCITYCITIZEN;
    extern i16 id_GEONOSIAN;
    extern i16 id_BOB;
}
