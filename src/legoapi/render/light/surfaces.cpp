#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

NUMTL *SolidMtl;
NUMTL *SolidMtl3D;

extern "C" {
    u32 LAYER_HOVERIGNORE = 0xffffffff;
    volatile u16 SURFACEBITS_DUST;
    u16 SURFACEBITS_NODUST;
    tertype TerSurface[32] = {
        {1.0f, 0, 0xffff},     {1.0f, 65, 0xffff},   {1.0f, 2, 0xffff},      {1.0f, 8, 0xffff},   {1.0f, 65536, 0xffff},
        {1.0f, 66560, 0xffff}, {1.0f, 0, 0xffff},    {1.0f, 2048, 0xffff},   {1.0f, 0, 0xffff},   {1.0f, 8192, 0xffff},
        {1.0f, 10, 0xffff},    {1.0f, 67, 0xffff},   {1.0f, 16, 0xffff},     {1.0f, 16, 0xffff},  {1.0f, 8192, 0xffff},
        {1.0f, 8192, 0xffff},  {0.1f, 1088, 0xffff}, {0.2f, 131074, 0xffff}, {1.0f, 81, 0xffff},  {1.0f, 512, 0xffff},
        {1.0f, 8192, 0xffff},  {1.0f, 514, 0xffff},  {1.0f, 64, 0xffff},     {1.0f, 192, 0xffff}, {1.0f, 8192, 0xffff},
        {1.0f, 4160, 0xffff},  {1.0f, 0, 0xffff},    {1.0f, 5184, 0xffff},   {1.0f, 256, 0xffff}, {1.0f, 320, 0xffff},
        {1.0f, 64, 0xffff},    {1.0f, 64, 0xffff},
    };
}

void AllocTerrId() {
}

NUMTL *CreateCopyMat(NUMTL *source, i32 disable_depth_write, i32 alpha_mode, i32 depth_mode, i32 filter_mode) {
    if (source == NULL) {
        return NULL;
    }

    NUMTL *material = NuMtlCreate3D(1);
    material->diffuse_color.r = 1.0f;
    material->diffuse_color.g = 1.0f;
    material->diffuse_color.b = 1.0f;
    material->opacity = 0.999f;
    material->attribs.alpha_mode = static_cast<u32>(alpha_mode) & 0xf;
    material->attribs.filter_mode = static_cast<u32>(filter_mode) & 3;
    material->attribs.unknown_1_1_2 = 1;
    material->attribs.unknown_1_4_8 = 1;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = static_cast<u32>(depth_mode) & 3;
    material->attribs.unknown_2_1_2 = 1;
    material->attribs.unknown_4_8 = disable_depth_write != 0;
    material->tex_id = source->tex_id;
    material->sort_pri = source->sort_pri;
    NuMtlUpdate(material);
    return material;
}

void SurfaceMaskOn(u32 *) {
}

void GetSurfaceInfo(GameObject_s *, i32, float) {
}

i32 IntersectWater(GameObject_s *object) {
    if ((object->apiobj.field_0x27f & static_cast<u8>(~8u)) != 1) {
        return 0;
    }

    const f32 water_height = object->apiobj.field_0x21c;
    return water_height <= object->apiobj.collision_max.y && object->apiobj.collision_min.y <= water_height;
}

void SurfaceMaskOff(u32 *) {
}

void Surfaces_Reset() {
    LAYER_HOVERIGNORE = 0x10;
    SURFACEBITS_DUST = 0;

    i32 i = 0;
    u32 no_dust = 0;
    const u32 initial_surface_bit = 1;
    u32 dust = 0;
    while (i < 32) {
        const u32 surface_bit = initial_surface_bit << i;
        if ((TerSurface[i].dwFlags & 4) != 0) {
            dust |= surface_bit;
        } else {
            no_dust |= surface_bit;
        }
        ++i;
    }
    SURFACEBITS_DUST = dust;
    SURFACEBITS_NODUST = no_dust;
}

void AdjustLayerBits(u32, GameObject_s *) {
}

void DeRotateTerrain(tertype *) {
}

void InitSurfaceInfo(GameObject_s *) {
}

void Surface_Deflect(nuvec_s *, nuvec_s *, nuvec_s *, i32) {
}

void CreateUsefulMaterials() {
    SolidMtl = NuMtlCreate(1);
    u8 *attrib = reinterpret_cast<u8 *>(&SolidMtl->attribs);
    attrib[1] &= 0x3f;
    attrib[0] &= 0xf0;
    attrib[2] |= 0x04;
    NuMtlUpdate(SolidMtl);

    SolidMtl3D = NuMtlCreate3D(1);
    attrib = reinterpret_cast<u8 *>(&SolidMtl3D->attribs);
    attrib[1] &= 0x3f;
    attrib[0] &= 0xf0;
    attrib[2] |= 0x04;
    NuMtlUpdate(SolidMtl3D);
}

NUMTL *CreateAlphaBlendTexture(VARIPTR *buffer, VARIPTR buffer_end, char *name, i32 disable_depth_write, i32 alpha_mode,
                               i32 sort_priority, i32 depth_mode) {
    const i32 texture_id = NuTexRead(name, buffer, &buffer_end);
    buffer->addr = ALIGN(buffer->addr, 0x10);
    NUMTL *material = NuMtlCreate3D(1);
    material->diffuse_color.r = 1.0f;
    material->diffuse_color.g = 1.0f;
    material->diffuse_color.b = 1.0f;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = static_cast<u32>(depth_mode) & 3;
    material->attribs.unknown_2_1_2 = 2;
    material->opacity = 0.999f;
    material->attribs.alpha_mode = static_cast<u32>(alpha_mode) & 0xf;
    material->attribs.unknown_4_8 = disable_depth_write != 0;
    material->tex_id = static_cast<i16>(texture_id);
    material->sort_pri = static_cast<i16>(sort_priority);
    NuMtlUpdate(material);
    return material;
}

void CreateSubtractiveTexture(char *) {
}
