#pragma once

#include "decomp.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nu3d/nuspecial.h"

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
struct bgprocinfo_s;
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
    nuhspecial_s special_scene;
};

DECOMP_ASSERT(sizeof(CHARSCENE_s) == 0x10, "CHARSCENE_s size");

extern CHARSCENE_s *CharScene_Area;

struct gamecharacterdata_s {         /* PlaceHolder Structure */
    MAKELAYERLISTFN make_layer_list; // 0x00
    u32 field_0x04;
    u32 field_0x08;
    f32 field_0x0c;
    f32 field_0x10;
    f32 field_0x14;
    f32 field_0x18;
    f32 movement_speed; // 0x1c, ordinary directional top speed
    f32 field_0x20;
    f32 field_0x24;
    f32 field_0x28;
    f32 field_0x2c;
    f32 field_0x30;
    f32 field_0x34;
    f32 field_0x38;
    f32 field_0x3c;
    f32 field_0x40;
    f32 field_0x44;
    f32 field_0x48;
    f32 field_0x4c;
    f32 field_0x50;
    f32 field_0x54;
    f32 field_0x58;
    f32 field_0x5c;
    f32 field_0x60;
    f32 viewdistance;  // 0x64
    f32 heardistance;  // 0x68
    f32 maxviewheight; // 0x6c
    f32 minviewheight; // 0x70
    f32 field_0x74;
    f32 field_0x78;
    f32 field_0x7c;
    f32 field_0x80;
    f32 field_0x84;
    f32 field_0x88;
    f32 field_0x8c;
    u32 flags_090;
    u32 field_0x94;
    u32 field_0x98;
    u32 field_0x9c;
    u32 layer_mask; // 0xa0, hierarchy render-part visibility mask
    u32 field_0xa4;
    u32 field_0xa8;
    u32 field_0xac;
    u32 field_0xb0;
    f32 field_0xb4;
    f32 field_0xb8;
    f32 field_0xbc;
    f32 field_0xc0;
    u32 field_0xc4;
    u32 field_0xc8;
    u32 field_0xcc;
    u32 field_0xd0;
    u32 field_0xd4;
    u32 field_0xd8;
    u32 field_0xdc;
    u32 field_0xe0;
    u32 field_0xe4;
    u32 field_0xe8;
    u32 field_0xec;
    u32 field_0xf0;
    u8 hitpoints; // 0xf4
    u8 field_0xf5;
    u8 field_0xf6;
    u8 field_0xf7;
    u32 field_0xf8;
    u32 field_0xfc;
    u32 field_0x100;
    u32 field_0x104;
    u32 field_0x108;
    u32 field_0x10c;
    u32 field_0x110;
    u16 field_0x114;
    u8 field275_0x116;
    u8 field_0x117;
    u32 field_0x118;
    u16 field_0x11c;
    u16 field_0x11e;
};
typedef struct gamecharacterdata_s GAMECHARACTERDATA;

DECOMP_ASSERT(sizeof(GAMECHARACTERDATA) == 0x120, "GAMECHARACTERDATA size");
DECOMP_ASSERT(offsetof(GAMECHARACTERDATA, layer_mask) == 0xa0, "GAMECHARACTERDATA layer-mask offset");
DECOMP_ASSERT(offsetof(GAMECHARACTERDATA, hitpoints) == 0xf4, "GAMECHARACTERDATA hitpoints offset");

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
    extern i32 hub_character_ready;
    extern u8 Hub_LowEnd_IconsInsteadOfModels;

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
    extern i16 id_BARMAN;
    extern i16 id_CANTINABAND;
    extern i16 id_WEIRDO1;
    extern i16 id_WEIRDO2;
    extern i16 id_JABBA;
    extern i16 id_WOMPRAT;
    extern i16 id_DRAGBOMB;
    extern i16 id_DROIDEKA;
    extern i16 id_CLOUDCITYCITIZEN;
    extern i16 id_GEONOSIAN;
    extern i16 id_BOB;
}

void LoadSingleCharacter(bgprocinfo_s *info);
void UpdateCharacterLoad(void);
