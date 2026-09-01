#include "legoapi/world/world_shared.h"

#include <stdio.h>

#include "gameapi/edtools/edstubs.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

#include <limits.h>
#include <string.h>

struct tertype;
struct terrsitu_s;
struct PLATSKININFO;
struct GAMECAMERA_s;
struct pushblock_s;

namespace {
    struct TERRAIN_SCENE_OBJECT {
        u8 pad_0x00[0x44];
        u8 transform[4];
        void *object;
        u8 pad_0x4c[4];
    };
    DECOMP_ASSERT(sizeof(TERRAIN_SCENE_OBJECT) == 0x50, "TERRAIN_SCENE_OBJECT ABI");

    struct TERRAIN_DISPLAY_OBJECT {
        u8 pad_0x00[0x40];
        u8 embedded_object[0x78];
        u8 transform[8];
        i32 scene_object_index;
        void *object;
        u8 pad_0xc8[8];
    };
    DECOMP_ASSERT(sizeof(TERRAIN_DISPLAY_OBJECT) == 0xd0, "TERRAIN_DISPLAY_OBJECT ABI");
    DECOMP_ASSERT(offsetof(TERRAIN_DISPLAY_OBJECT, transform) == 0xb8, "TERRAIN_DISPLAY_OBJECT transform offset");
    DECOMP_ASSERT(offsetof(TERRAIN_DISPLAY_OBJECT, scene_object_index) == 0xc0,
                  "TERRAIN_DISPLAY_OBJECT scene_object_index offset");

    struct TERRAIN_DISPLAY_SCENE {
        u8 pad_0x00[0x6c];
        i32 object_count;
        TERRAIN_DISPLAY_OBJECT *objects;
    };
    DECOMP_ASSERT(offsetof(TERRAIN_DISPLAY_SCENE, object_count) == 0x6c, "TERRAIN_DISPLAY_SCENE object_count offset");

    struct TERRAIN_GSCENE {
        u8 pad_0x00[0x1c];
        i32 object_count;
        TERRAIN_SCENE_OBJECT *objects;
        u8 pad_0x24[0x34];
        void *object_map_present;
        u16 *object_map;
        u8 pad_0x60[0xb0];
        TERRAIN_DISPLAY_SCENE *display_scene;
    };
    DECOMP_ASSERT(offsetof(TERRAIN_GSCENE, object_count) == 0x1c, "TERRAIN_GSCENE object_count offset");
    DECOMP_ASSERT(offsetof(TERRAIN_GSCENE, object_map_present) == 0x58, "TERRAIN_GSCENE object_map_present offset");
    DECOMP_ASSERT(offsetof(TERRAIN_GSCENE, display_scene) == 0x110, "TERRAIN_GSCENE display_scene offset");

} // namespace

static void *ScaleTerrainT1;
static void *ScaleTerrainT2;
static TERRAIN_SHAPE *ScaleTerrain;
static void *TempScanStack;
static void *WallSplList;
i32 terraincnt;
i32 curSphereter;
i32 platinrange;
TERRAIN_SHAPE *ShadPoly;
TERRAIN_SHAPE *TerrPoly;
u8 TerrWallInfo;
u8 TerrWallTab[4];
NUVEC TerrWallNorm;
static i32 PlatImpactId;
static TERRAIN_SHAPE PlatImpactTer;
static NUVEC PlatImpactNorm;
i32 TerrPolyObj;
static TERRAIN_SHAPE TerrPolyInfo;

extern TERRSET *CurTerr;
extern TerrainQuery_s *TerI;
extern i32 WallSplinesOnly;
extern i32 curPickInst;
extern i16 castnum;
extern f32 wallover;
extern TERRAIN_TRACK_SLOT *CurTrackInfo;
extern void *TerImpactData;
extern i32 *TerImpactDataCount;
extern i32 TerImpactDataMax;

void TerrainSkinAllocate(terrsitu_s *terrain_group);
void ScanWallSplineTerrain(i32 scan_type, i32 terrain_mask, i32 scan_flags);
void NewTerrStoreAnyInfo();
void TerrainMoveImpactData();
void RotateVec(NUVEC *source, NUVEC *destination);
TERRAIN_TRACK_SLOT *AllocTerrId();
void PlatformConnect(char *track_id, NUVEC *position_delta, NUVEC *movement_delta, i32 platform_index);
void FullDeflectSmallY(NUVEC *normal, NUVEC *movement, NUVEC *result);
extern "C" void FullDeflect(NUVEC *normal, NUVEC *movement, NUVEC *result);
void DerotateMovementVector();
i32 HitTerrain();
void TerrainImpactNorm();
void Tag_Check(GameObject_s *object);
void BigJumpCode(GameObject_s *object);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
extern "C" i32 NewShadowOnPlatform();
extern NUVEC ShadNorm;

extern i32 terrhitflags;
extern i32 TERRAINMASK_NONWEAPON;
extern i32 TERRAINMASK_NONDROID;
extern i32 LEGO_AIPATHCNX_BLOCKAGE;
extern i32 LEGO_AIPATHCNX_FULLTERRAIN;

void StorePlatImpact() {
    TerrainQuery_s *query = TerI;
    const u8 hit_type = query->hit_type;
    if (hit_type == TERRAIN_HIT_TYPE_NONE || query->terrain_group_index == -1) {
        return;
    }

    TERRAIN_GROUP *group = &CurTerr->groups[query->terrain_group_index];
    if (group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
        return;
    }

    castnum = query->terrain_group_index;
    NUVEC impact_normal = query->movement_normal;

    const u8 maximum_supported_hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE;
    if (hit_type > maximum_supported_hit_type) {
        return;
    }

    const i32 hit_type_flag = 1 << hit_type;
    const i32 rotated_class_mask =
        (1 << TERRAIN_HIT_TYPE_CYLINDER) | (1 << TERRAIN_HIT_TYPE_VERTEX) | (1 << TERRAIN_HIT_TYPE_SPHERE);
    const i32 rotated_hit_mask = rotated_class_mask | (rotated_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
    const i32 face_class_mask = 1 << TERRAIN_HIT_TYPE_FACE;
    const i32 direct_hit_mask = face_class_mask | (face_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
    if ((hit_type_flag & rotated_hit_mask) != 0) {
        RotateVec(&impact_normal, &impact_normal);
        query = TerI;
        if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
            impact_normal.x *= query->inverse_collision_radius;
            impact_normal.y *= query->inverse_collision_radius;
            impact_normal.z *= query->inverse_collision_radius;
        }
    } else if ((hit_type_flag & direct_hit_mask) == 0) {
        return;
    }

    const f32 unit_scale = 1.0f;
    if (query->object_scale != unit_scale) {
        const f32 normal_length = NuFsqrt(impact_normal.x * impact_normal.x +
                                          impact_normal.y * impact_normal.y * query->inverse_object_scale_sq +
                                          impact_normal.z * impact_normal.z);
        f32 inverse_normal_length = 0.0f;
        if (normal_length != 0.0f) {
            inverse_normal_length = unit_scale / normal_length;
        }

        query = TerI;
        impact_normal.x *= inverse_normal_length;
        impact_normal.y *= query->inverse_object_scale * inverse_normal_length;
        impact_normal.z *= inverse_normal_length;
    }

    query = TerI;
    PlatImpactId = CurTerr->groups[query->terrain_group_index].scene_index;
    PlatImpactNorm = impact_normal;
    PlatImpactTer = *query->surface;
}

extern "C" void PlatImpactInfo(NUVEC *normal, i32 *material0, i32 *material1) {
    if (PlatImpactId != -1) {
        *normal = PlatImpactNorm;
        *material0 = PlatImpactTer.material[0];
        *material1 = PlatImpactTer.material[1];
    }
    PlatImpactId = -1;
}

namespace {

    struct TerrainScanBounds {
        f32 min_x;
        f32 min_y;
        f32 min_z;
        f32 max_x;
        f32 max_y;
        f32 max_z;
    };

    struct TerrainScanWriter {
        u8 *group_header;
        TERRAIN_SHAPE **cursor;
        u8 *limit;
        i32 group_shape_count;
        i32 scaled_shape_count;
    };

    static bool TerrainBoundsOverlap(const TerrainScanBounds &bounds, const NUVEC &minimum, const NUVEC &maximum) {
        return bounds.max_x >= minimum.x && bounds.max_y >= minimum.y && bounds.max_z >= minimum.z &&
               maximum.x >= bounds.min_x && maximum.y > bounds.min_y && maximum.z > bounds.min_z;
    }

    static bool TerrainShapeOverlaps(const TerrainScanBounds &bounds, const TERRAIN_SHAPE &shape) {
        return bounds.max_x >= shape.min_x && shape.max_x > bounds.min_x && bounds.max_y >= shape.min_y &&
               shape.max_y > bounds.min_y && bounds.max_z >= shape.min_z && shape.max_z > bounds.min_z;
    }

    static void TerrainFinishScanGroup(TerrainScanWriter *writer, i32 group_index) {
        if (writer->group_shape_count == 0) {
            return;
        }

        i16 *header = reinterpret_cast<i16 *>(writer->group_header);
        header[0] = static_cast<i16>(writer->group_shape_count);
        header[1] = static_cast<i16>(group_index);
        writer->group_header = reinterpret_cast<u8 *>(writer->cursor);
        // The target's pointers are four bytes, so its packed header occupies
        // one pointer slot.  Keeping that relationship on wider hosts lets the
        // scan consumers use the same `entries + 1` traversal without reading
        // the first pointer halfway through the header.
        writer->cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer->group_header + sizeof(TERRAIN_SHAPE *));
        writer->group_shape_count = 0;
    }

    static TerrainScanBounds TerrainGetScanBounds(const TerrainQuery_s &query) {
        const f32 padding = 0.05f;
        TerrainScanBounds bounds;

        if (query.scan_result != 1) {
            const f32 move_length = NuFsqrt(query.movement.x * query.movement.x + query.movement.y * query.movement.y +
                                            query.movement.z * query.movement.z);
            const f32 reach = move_length + query.collision_radius + 0.1f;
            const f32 vertical_reach = reach * query.object_scale;
            bounds.min_x = query.position.x - padding - reach;
            bounds.max_x = query.position.x + padding + reach;
            bounds.min_y = query.position.y - padding - vertical_reach;
            bounds.max_y = query.position.y + padding + vertical_reach;
            bounds.min_z = query.position.z - padding - reach;
            bounds.max_z = query.position.z + padding + reach;
        } else {
            const f32 reach = padding + query.collision_radius;
            if (query.movement.x > 0.0f) {
                bounds.min_x = query.position.x - reach;
                bounds.max_x = query.position.x + query.movement.x + reach;
            } else {
                bounds.min_x = query.position.x + query.movement.x - reach;
                bounds.max_x = query.position.x + reach;
            }
            if (query.movement.y > 0.0f) {
                bounds.min_y = query.position.y - reach;
                bounds.max_y = query.position.y + query.movement.y + reach;
            } else {
                bounds.min_y = query.position.y + query.movement.y - reach;
                bounds.max_y = query.position.y + reach;
            }
            if (query.movement.z > 0.0f) {
                bounds.min_z = query.position.z - reach;
                bounds.max_z = query.position.z + query.movement.z + reach;
            } else {
                bounds.min_z = query.position.z + query.movement.z - reach;
                bounds.max_z = query.position.z + reach;
            }
        }
        return bounds;
    }

} // namespace

i16 debug_index;

i32 ReadTerrain(unsigned char *base_path, i32 first_group, i16 **buffer, TERRSET *terrain);
void NuVecCheckForSNANs(NUVEC *vector);
void TerrFlush();

// Debris and terrain globals — accessed from DebrisSetThinningLevel etc.
f32 debris_thinning_level;
i32 forced_debris_thinning;
i32 debris_detail_level;
// char *debris_name[147] @0x61e860 (.data): the static debris effect names,
// seeded into every world's debris system by InitGameDebris.
char *debris_name[147] = {
    "BULLET_SPARK",   "SABER_RED",     "SABER_GREEN",   "SABER_BLUE",    "SABER_PURPLE",   "EXPLD_1A",
    "JS_THRUST",      "S1_THRUST",     "COIN_BLUE",     "COIN_GOLD",     "COIN_SILVER",    "R2D2THRUSTER",
    "R2Q5THRUSTER",   "SPLASH",        "RIPPLE",        "HEART",         "HEART_BIG",      "EXPLOSION",
    "MINI_01",        "MINI_02",       "MINI_03",       "TER_SPARK",     "TER_SPARK2",     "EXPOL_02",
    "SPEEDERDUST",    "POD_DUST",      "CAVE_DUST",     "JANGOTHRUSTER", "EXPLO_DROID",    "EXPLO_NAB",
    "NAB_SMOKE",      "NAB_BOOST",     "MIKESMOKE",     "EXPLO_05",      "EXPLO_06",       "EXPLO_07",
    "EXPLO_11",       "MIKESTALL",     "TRAINING_01",   "TAG_BLUE",      "TAG_GREEN",      "ZIP_TARGET",
    "DOOKU_BALL",     "WALKER_01",     "WALKER_02",     "ENG_BLOW",      "DogImpact",      "DogEngine",
    "DogSmoke",       "DogTrail",      "YODA_HOVER_01", "K_Bolt",        "DUK_P2",         "PodHaze",
    "PodDust10",      "rock_pop1",     "BlueTrail",     "PurpleTrail",   "LavaDie",        "ComboRed",
    "ComboGreen",     "ComboBlue",     "ComboPurple",   "REPAIR",        "SABER_RED1",     "SABER_GREEN1",
    "SABER_BLUE1",    "SABER_PURPLE1", "XWING_1",       "MOUSE_POP",     "PHOTON",         "PHOTON_EXPLO",
    "PHOTON_EXPLO_S", "EXPLO_ORAN_2A", "EXPLO_ORAN_2B", "EXPLO_ORAN_2C", "BUILD_IT1",      "BUILD_IT2",
    "BUILD_IT3",      "BUILD_IT4",     "BUILD_IT5",     "BUILD_IT6",     "DRAG_POP_1",     "DRAG_POP_2",
    "DRAG_POP_4",     "GenoGun",       "SNOW_SPEEDER1", "LEVER_SPARK",   "ZAPPER_1",       "V_BOLT_RED",
    "V_BOLT_GREEN",   "CHAR_SMOKE1",   "CHAR_SMOKE2",   "POWER_P_1",     "POWER_P_2",      "POWER_P_3",
    "TIE_HIT1",       "BRICK_01",      "BRICK_02",      "BRICK_03",      "BUMP_01",        "BUMP_02",
    "LAND_S_DUST1",   "LAND_S_DUST2",  "LAND_S_THRUST", "LAND_S_HAZE",   "EXPLO_06",       "EXPLO_06b",
    "BUMP_03",        "FORCE_BLUE",    "FORCE_RED",     "SCAN3",         "REPULSOR",       "WEE_POP",
    "EXIT_FIRE_1",    "EXIT_FIRE_2",   "EWOK_POP1",     "EXPLO_02",      "ATAT_POP_1",     "ATAT_POP_2",
    "ATAT_POP_3",     "FIRE_1",        "RAIN_1",        "CHAR_BUBBLE",   "CHAR_GHOST",     "DEATH_1",
    "DEATH_2",        "BOULDER_2",     "DISH_1",        "DISH_2",        "SNOW_POP_1",     "SNOW_POP_2",
    "TREE_POP_1",     "TREE_POP_4",    "SPEEDER_HIT",   "FALCONGLOW",    "FALCONTHRUSTER", "SPEED_SPARK",
    "PUNCH_1",        "PUNCH_2",       "GUNSHIP_01",    "GUNSHIP_02",    "SandBlast",      "BULLET1",
    "BULLET2",        "BULLET_HIT",    "POD_SPARK",
};
i32 Grass_Available = 1;
i32 PDEBCOUNT = 0;
void *PDebNameList = NULL;

extern "C" void DebrisSetThinningLevel(f32 level) {
    debris_thinning_level = level < 1.0f ? 1.0f : level;
}
extern "C" void DebrisSetForcedThinning(i32 forced) {
    forced_debris_thinning = forced;
}
extern "C" void DebrisSetDetailLevel(i32 level) {
    debris_detail_level = level;
}

// Constructs the fixed terrain header and its variable-sized group, index and
// platform arrays in the caller's arena. The four bounds arrays and the cell
// assignment array are temporary storage carved backwards from buf_end.
extern "C" void *TerrainInitEx(i32 level_num, void *buf, void *buf_end, i32 options, char *path, void *gscn,
                               i32 scene_id, u32 max_group_indices, u32 max_groups, u32 max_platforms) {
    (void)level_num;
    (void)options;
    (void)scene_id;

    VARIPTR *cursor = static_cast<VARIPTR *>(buf);
    const VARIPTR saved_cursor = *cursor;
    TERRAIN_GSCENE *scene = static_cast<TERRAIN_GSCENE *>(gscn);

    usize scratch = reinterpret_cast<usize>(buf_end) & ~static_cast<usize>(1);
    scratch -= max_groups * sizeof(i16);
    i16 *group_cells = reinterpret_cast<i16 *>(scratch);

    scratch &= ~static_cast<usize>(3);
    scratch -= max_groups * sizeof(f32);
    f32 *group_min_x = reinterpret_cast<f32 *>(scratch);
    scratch -= max_groups * sizeof(f32);
    f32 *group_max_x = reinterpret_cast<f32 *>(scratch);
    scratch -= max_groups * sizeof(f32);
    f32 *group_min_z = reinterpret_cast<f32 *>(scratch);
    scratch -= max_groups * sizeof(f32);
    f32 *group_max_z = reinterpret_cast<f32 *>(scratch);

    if (ScaleTerrainT1 == NULL) {
        ScaleTerrainT1 = NU_ALLOC(0xc800, 4, NuMemoryManager::MEM_ALLOC_SET_TO_ZERO, "", NUMEMORY_CATEGORY_NONE);
    }
    if (ScaleTerrainT2 == NULL) {
        ScaleTerrainT2 = NU_ALLOC(0xc800, 4, NuMemoryManager::MEM_ALLOC_SET_TO_ZERO, "", NUMEMORY_CATEGORY_NONE);
    }
    if (TempScanStack == NULL) {
        TempScanStack = NU_ALLOC(0x2000, 4, NuMemoryManager::MEM_ALLOC_SET_TO_ZERO, "", NUMEMORY_CATEGORY_NONE);
    }
    if (WallSplList == NULL) {
        WallSplList = NU_ALLOC(0x600, 4, NuMemoryManager::MEM_ALLOC_SET_TO_ZERO, "", NUMEMORY_CATEGORY_NONE);
    }

    TERRSET *terrain = reinterpret_cast<TERRSET *>(cursor->u8_ptr);
    cursor->u8_ptr += sizeof(TERRSET);

    terrain->groups = reinterpret_cast<TERRAIN_GROUP *>(cursor->u8_ptr);
    cursor->u8_ptr += (max_groups + 0x80) * sizeof(TERRAIN_GROUP);
    terrain->group_indices = reinterpret_cast<i16 *>(cursor->u8_ptr);
    cursor->u8_ptr += max_group_indices * sizeof(i16);
    terrain->platforms = reinterpret_cast<TERRAIN_PLATFORM *>(cursor->u8_ptr);
    cursor->u8_ptr += max_platforms * sizeof(TERRAIN_PLATFORM);

    terrain->max_group_indices = max_group_indices;
    terrain->max_groups = max_groups;
    terrain->max_platforms = max_platforms;

    for (i32 level = 0; level < TERRAIN_INDEX_LEVEL_COUNT; ++level) {
        terrain->index_levels[level].entry_count = 0;
        terrain->index_levels[level].entries = NULL;
    }

    memset(terrain->groups, 0, max_groups * sizeof(TERRAIN_GROUP));
    for (u32 group_index = 0; group_index < max_groups; ++group_index) {
        terrain->groups[group_index].chunk_type = -1;
    }

    for (i32 cell = 0; cell < TERRAIN_CELL_RECORD_COUNT; ++cell) {
        terrain->cells[cell].group_count = 0;
    }

    for (u32 platform_index = 0; platform_index < max_platforms; ++platform_index) {
        TERRAIN_PLATFORM &platform = terrain->platforms[platform_index];
        platform.flags &= ~TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;
        platform.scene_object = NULL;
        platform.field_0x44 = NULL;
        platform.field_0x54 = 0;
        platform.field_0x58 = 0;
        platform.field_0x5c = 0;
        platform.field_0x60 = 0;
        platform.field_0x64 = 0;
    }

    for (i32 slot = 0; slot < TERRAIN_TRACK_SLOT_COUNT; ++slot) {
        terrain->track_slots[slot].id = NULL;
        terrain->track_slots[slot].wall_contact_state = TERRAIN_TRACK_CONTACT_NONE;
    }

    terrain->field_0x064 = NULL;
    terrain->field_0x12c = NULL;
    terrain->field_0x154 = NULL;

    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    ShadPoly = 0;
    TerrPoly = 0;
    TerrWallInfo = 0;
    PlatImpactId = -1;

    const i32 group_count =
        ReadTerrain(reinterpret_cast<unsigned char *>(path), 0, reinterpret_cast<i16 **>(cursor), terrain);
    terrain->group_count = static_cast<i16>(group_count);
    if (group_count < 0) {
        *cursor = saved_cursor;
        return NULL;
    }

    static const f32 kBoundsMinimum = -200000000.0f;
    static const f32 kBoundsMaximum = 200000000.0f;
    static const f32 kSteepNormalY = 0.707f;

    terrain->minimum_height = INT_MAX;

    for (i32 group_index = 0; group_index < group_count; ++group_index) {
        TERRAIN_GROUP &group = terrain->groups[group_index];
        if (static_cast<u32>(group.chunk_type) > TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }

        f32 min_x = kBoundsMaximum;
        f32 min_y = kBoundsMaximum;
        f32 min_z = kBoundsMaximum;
        f32 max_x = kBoundsMinimum;
        f32 max_y = kBoundsMinimum;
        f32 max_z = kBoundsMinimum;
        f32 radius_squared = 0.0f;

        TERRAIN_SHAPE_BATCH *batch = static_cast<TERRAIN_SHAPE_BATCH *>(group.data);
        while (batch->marker >= 0) {
            TERRAIN_SHAPE *shape = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
            for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index, ++shape) {
                NuVecCheckForSNANs(&shape->normals[0]);
                NuVecCheckForSNANs(&shape->normals[1]);

                if (shape->material[1] > 0xef) {
                    const u8 legacy_material = shape->material[1];
                    shape->material[1] = 0;
                    shape->normal_flags |= static_cast<u8>(legacy_material * 4 + 0x44);
                }

                if (shape->material[0] == 0xff) {
                    shape->flags |= TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS;
                    shape->material[0] = shape->material[1];
                    shape->material[1] = 0;
                } else if (static_cast<i8>(shape->material[0]) < 0) {
                    shape->material[0] &= 0x7f;
                    shape->flags |= TERRAIN_SHAPE_FLAG_MATERIAL_HIGH_BIT;
                }

                if (terrain->file_version == 0) {
                    shape->normal_flags &= static_cast<u8>(
                        ~(TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP | TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP));
                    if (kSteepNormalY >= shape->normals[0].y) {
                        shape->normal_flags |= TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP;
                    }
                    if (kSteepNormalY >= shape->normals[1].y) {
                        shape->normal_flags |= TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP;
                    }
                }

                if (shape->min_x < min_x)
                    min_x = shape->min_x;
                if (shape->max_x > max_x)
                    max_x = shape->max_x;
                if (shape->min_y < min_y)
                    min_y = shape->min_y;
                if (shape->max_y > max_y)
                    max_y = shape->max_y;
                if (shape->min_z < min_z)
                    min_z = shape->min_z;
                if (shape->max_z > max_z)
                    max_z = shape->max_z;

                f32 current_minimum_height = static_cast<f32>(terrain->minimum_height);
                for (i32 vector_index = 0; vector_index < 4; ++vector_index) {
                    const NUVEC &vector = shape->vectors[vector_index];
                    const f32 world_y = group.origin.y + vector.y;
                    if (current_minimum_height > world_y) {
                        terrain->minimum_height = static_cast<i32>(world_y);
                        current_minimum_height = static_cast<f32>(terrain->minimum_height);
                    }

                    const f32 length_squared = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
                    if (length_squared > radius_squared) {
                        radius_squared = length_squared;
                    }
                }
            }

            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(reinterpret_cast<TERRAIN_SHAPE *>(batch + 1) +
                                                            batch->shape_count);
        }

        group.radius = NuFsqrt(radius_squared);
        if (group.chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
            group.bounds_min.x = min_x;
            group.bounds_min.y = min_y;
            group.bounds_min.z = min_z;
            group.bounds_max.x = max_x;
            group.bounds_max.y = max_y;
            group.bounds_max.z = max_z;
        } else {
            group.bounds_min.x = group.origin.x + min_x;
            group.bounds_min.y = group.origin.y + min_y;
            group.bounds_min.z = group.origin.z + min_z;
            group.bounds_max.x = group.origin.x + max_x;
            group.bounds_max.y = group.origin.y + max_y;
            group.bounds_max.z = group.origin.z + max_z;
        }
        ++terraincnt;
    }

    f32 terrain_min_x = kBoundsMaximum;
    f32 terrain_min_z = kBoundsMaximum;
    f32 terrain_max_x = kBoundsMinimum;
    f32 terrain_max_z = kBoundsMinimum;

    for (i32 group_index = 0; group_index < group_count; ++group_index) {
        TERRAIN_GROUP &group = terrain->groups[group_index];
        if (group.chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        TERRAIN_SHAPE_BATCH *batch = static_cast<TERRAIN_SHAPE_BATCH *>(group.data);
        while (batch->marker >= 0) {
            const f32 min_x = group.origin.x + batch->min_x;
            const f32 max_x = group.origin.x + batch->max_x;
            const f32 min_z = group.origin.z + batch->min_z;
            const f32 max_z = group.origin.z + batch->max_z;
            if (min_x < terrain_min_x)
                terrain_min_x = min_x;
            if (max_x > terrain_max_x)
                terrain_max_x = max_x;
            if (min_z < terrain_min_z)
                terrain_min_z = min_z;
            if (max_z > terrain_max_z)
                terrain_max_z = max_z;

            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(reinterpret_cast<TERRAIN_SHAPE *>(batch + 1) +
                                                            batch->shape_count);
        }
    }

    terrain->group_index_count = 0;
    for (i32 group_index = 0; group_index < group_count; ++group_index) {
        TERRAIN_GROUP &group = terrain->groups[group_index];
        if (group.chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        f32 min_x = kBoundsMaximum;
        f32 min_z = kBoundsMaximum;
        f32 max_x = kBoundsMinimum;
        f32 max_z = kBoundsMinimum;
        TERRAIN_SHAPE_BATCH *batch = static_cast<TERRAIN_SHAPE_BATCH *>(group.data);
        while (batch->marker >= 0) {
            const f32 batch_min_x = group.origin.x + batch->min_x;
            const f32 batch_max_x = group.origin.x + batch->max_x;
            const f32 batch_min_z = group.origin.z + batch->min_z;
            const f32 batch_max_z = group.origin.z + batch->max_z;
            if (batch_min_x < min_x)
                min_x = batch_min_x;
            if (batch_max_x > max_x)
                max_x = batch_max_x;
            if (batch_min_z < min_z)
                min_z = batch_min_z;
            if (batch_max_z > max_z)
                max_z = batch_max_z;

            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(reinterpret_cast<TERRAIN_SHAPE *>(batch + 1) +
                                                            batch->shape_count);
        }

        group_min_x[group_index] = min_x;
        group_min_z[group_index] = min_z;
        group_max_x[group_index] = max_x;
        group_max_z[group_index] = max_z;

        const i32 x_distance = static_cast<i32>((min_x + max_x) * 0.5f - terrain_min_x);
        i32 x_cell =
            static_cast<i32>(static_cast<f32>(x_distance * TERRAIN_GRID_WIDTH) / (terrain_max_x - terrain_min_x));
        if (x_cell < 0)
            x_cell = 0;
        else if (x_cell >= TERRAIN_GRID_WIDTH)
            x_cell = TERRAIN_GRID_WIDTH - 1;

        const i32 z_distance = static_cast<i32>((min_z + max_z) * 0.5f - terrain_min_z);
        i32 z_cell =
            static_cast<i32>(static_cast<f32>(z_distance * TERRAIN_GRID_WIDTH) / (terrain_max_z - terrain_min_z));
        if (z_cell < 0)
            z_cell = 0;
        else if (z_cell >= TERRAIN_GRID_WIDTH)
            z_cell = TERRAIN_GRID_WIDTH - 1;

        group_cells[group_index] = static_cast<i16>(z_cell * TERRAIN_GRID_WIDTH + x_cell);
    }

    terrain->used_cell_count = 0;
    terrain->group_index_count = 0;
    for (i32 cell_index = 0; cell_index < TERRAIN_GRID_CELL_COUNT; ++cell_index) {
        TERRAIN_CELL &cell = terrain->cells[terrain->used_cell_count];
        cell.first_group = static_cast<u16>(terrain->group_index_count);

        f32 min_x = kBoundsMaximum;
        f32 min_z = kBoundsMaximum;
        f32 max_x = kBoundsMinimum;
        f32 max_z = kBoundsMinimum;
        for (i32 group_index = 0; group_index < group_count; ++group_index) {
            TERRAIN_GROUP &group = terrain->groups[group_index];
            if (group_cells[group_index] != cell_index || group.chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
                continue;
            }

            if (group_min_x[group_index] < min_x)
                min_x = group_min_x[group_index];
            if (group_min_z[group_index] < min_z)
                min_z = group_min_z[group_index];
            if (group_max_x[group_index] > max_x)
                max_x = group_max_x[group_index];
            if (group_max_z[group_index] > max_z)
                max_z = group_max_z[group_index];

            ++cell.group_count;
            terrain->group_indices[terrain->group_index_count] = static_cast<i16>(group_index);
            ++terrain->group_index_count;
        }

        if (cell.group_count != 0) {
            cell.min_x = min_x;
            cell.min_z = min_z;
            cell.max_x = max_x;
            cell.max_z = max_z;
            ++terrain->used_cell_count;
        }
    }

    TERRAIN_CELL &platform_cell = terrain->cells[TERRAIN_PLATFORM_CELL];
    platform_cell.first_group = static_cast<u16>(terrain->group_index_count);

    i32 platform_count = 0;
    for (i32 group_index = 0; group_index < group_count; ++group_index) {
        TERRAIN_GROUP &group = terrain->groups[group_index];
        if (group.chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }
        if (platform_count >= terrain->max_platforms) {
            group.chunk_type = -1;
            continue;
        }

        u16 scene_object_index = group.scene_index;
        debug_index = static_cast<i16>(scene_object_index);
        if (scene == NULL) {
            group.chunk_type = -1;
            group.scene_index = static_cast<u16>(platform_count);
            continue;
        }

        if (scene->object_map_present != NULL) {
            scene_object_index = scene->object_map[scene_object_index];
            debug_index = static_cast<i16>(scene_object_index);
        }

        group.scene_index = static_cast<u16>(platform_count);
        TERRAIN_DISPLAY_SCENE *display_scene = scene->display_scene;
        if (display_scene == NULL && scene_object_index >= scene->object_count) {
            group.chunk_type = -1;
            continue;
        }

        terrain->group_indices[terrain->group_index_count] = static_cast<i16>(group_index);
        ++terrain->group_index_count;
        ++platform_cell.group_count;

        TERRAIN_PLATFORM &platform = terrain->platforms[static_cast<i16>(group.scene_index)];
        platform.flags = static_cast<u8>((platform.flags & 0xfa) | (group.platform_flags & 1));
        platform.scene_object_index = scene_object_index;
        platform.terrain_group_index = static_cast<i16>(group_index);

        if (display_scene != NULL) {
            TERRAIN_DISPLAY_OBJECT *display_object = NULL;
            for (i32 object_index = 0; object_index < display_scene->object_count; ++object_index) {
                TERRAIN_DISPLAY_OBJECT &candidate = display_scene->objects[object_index];
                if (candidate.scene_object_index == scene_object_index) {
                    display_object = &candidate;
                    break;
                }
            }

            if (display_object == NULL) {
                group.chunk_type = -1;
                continue;
            }

            if (display_object->object == NULL || display_object->object == reinterpret_cast<void *>(-1)) {
                platform.scene_object = display_object->embedded_object;
            } else {
                platform.scene_object = display_object->object;
            }
            platform.scene_transform = display_object->transform;
            platform.flags |= TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;
        } else {
            TERRAIN_SCENE_OBJECT &scene_object = scene->objects[scene_object_index];
            platform.scene_object = scene_object.object != NULL ? scene_object.object : &scene_object;
            platform.scene_transform = scene_object.transform;
        }

        ++platform_count;
    }

    TerrFlush();
    return terrain;
}
void LoadTerrainFile(WORLDINFO *world) {
    char path[256];
    world->terrain = NULL;
    if ((world->current_level->flags & LEVEL_TERRAIN) != 0) {
        NuStrCpy(path, world->config_file);
        LEVELDATA *level = world->current_level;
        if (level == (LEVELDATA *)PLATFORM_LDATA) {
            NuStrCpy(path, "levels\\episode_v\\cloudcityescape\\cloudcityescape_b\\cloudcityescape_b");
            level = world->current_level;
        }
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        void *terrain = TerrainInitEx(world->level_idx, &world->giz_buffer, world->unknown_0108.void_ptr, 0, path,
                                      world->current_gscn, 0, (u32)(u16)level->max_ter_groups,
                                      (u32)(u16)level->max_ter_groups, (u32)(u16)level->max_ter_platforms);
        world->terrain = terrain;
    }
}
void LoadGrassFile(WORLDINFO *world) {
    char path[268];
    world->page_grass = -1;
    Grass_Available = 1;
    char *config = world->config_file;

    if (g_isLowEndDevice != 0) {
        char *found = NuStrIStr(config, "Gungan");
        if (found == NULL)
            found = NuStrIStr(config, "SpeederChase");
        if (found == NULL)
            found = NuStrIStr(config, "EndorBattle");
        if (found == NULL)
            found = NuStrIStr(config, "Retake");

        if (found != NULL) {
            Grass_Available = 0;
        } else {
            Grass_Available = 1;
        }
    }

    if (g_isMidRangeDevice != 0 && NuStrIStr(config, "GunGan_A") != NULL) {
        Grass_Available = 0;
        return;
    }

    char *found = NuStrIStr(config, "SpeederChase");
    if (found != NULL) {
        Grass_Available = 0;
    }

    if (Grass_Available != 0) {
        sprintf(path, "%s.gra", config);
        if (NuFileExists(path)) {
            i32 page = edgraLoadPage(path, world->current_gscn, *(i32 *)&world->terrain, &world->giz_buffer,
                                     &world->unknown_0108);
            world->page_grass = page;
        }
    }
}
void LoadBridgeFile(WORLDINFO *world) {
    char path[256];
    world->page_bridge = -1;
    sprintf(path, "%s.bri", world->config_file);
    if (NuFileExists(path)) {
        i32 page = edbriLoadPage(path, world->current_gscn);
        world->page_bridge = page;
    }
}
void LoadPartFile(WORLDINFO *world) {
    char path[256];
    world->page_part = -1;
    edpartSetParticlePage(world->page_pp);

    if ((world->current_level->flags & (LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0) {
        sprintf(path, "%s.par", world->config_file);
        i32 page = -1;
        if (NuFileExists(path)) {
            page = edpartLoadPage(path, 1, world->current_gscn);
            world->page_part = page;
        }
        world->part_debris_sys = static_cast<PARTDEBSYS_s *>(
            InitPartDebris(&world->giz_buffer, &world->unknown_0108, 0x40, PDEBCOUNT, (char **)PDebNameList, page));
    }
}
TERRAIN_TRACK_SLOT *ScanTerrId(void *id) {
    TERRSET *terrain = CurTerr;
    if (terrain == NULL) {
        return NULL;
    }

    for (i32 slot_index = 0; slot_index < TERRAIN_TRACK_SLOT_COUNT; ++slot_index) {
        if (terrain->track_slots[slot_index].id == id) {
            return &terrain->track_slots[slot_index];
        }
    }

    return NULL;
}
i32 UnderWater(GameObject_s *object) {
    if ((object->apiobj.field_0x27f & static_cast<u8>(~8u)) == 1) {
        return object->apiobj.water_height >= object->apiobj.pos_y;
    }
    return 0;
}
void ScanTerrain(i32 scan_type, i32 terrain_mask, i32 scan_flags) {
    if (WallSplinesOnly != 0) {
        ScanWallSplineTerrain(scan_type, terrain_mask, scan_flags);
        return;
    }

    ScaleTerrain = static_cast<TERRAIN_SHAPE *>(ScaleTerrainT1);
    platinrange = 0;
    TerI->scan_group_index = -1;

    TerrainScanWriter writer;
    writer.group_header = TerI->scan_list_storage;
    writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer.group_header + sizeof(TERRAIN_SHAPE *));
    writer.limit = reinterpret_cast<u8 *>(TerI) + 0x93c;
    writer.group_shape_count = 0;
    writer.scaled_shape_count = 0;

    const TerrainScanBounds bounds = TerrainGetScanBounds(*TerI);

    for (i32 cell_index = 0; cell_index < CurTerr->used_cell_count; ++cell_index) {
        const TERRAIN_CELL &cell = CurTerr->cells[cell_index];
        if (bounds.max_x < cell.min_x || bounds.max_z < cell.min_z || cell.max_x < bounds.min_x ||
            cell.max_z < bounds.min_z) {
            continue;
        }

        const i16 *group_indices = CurTerr->group_indices + cell.first_group;
        for (i32 cell_group = 0; cell_group < cell.group_count; ++cell_group) {
            const i32 group_index = group_indices[cell_group];
            TERRAIN_GROUP &group = CurTerr->groups[group_index];
            if (!TerrainBoundsOverlap(bounds, group.bounds_min, group.bounds_max) || group.chunk_type == -1) {
                continue;
            }

            if (static_cast<i16>(group.scene_index) < 0) {
                TerrainSkinAllocate(reinterpret_cast<terrsitu_s *>(&group));
            }

            TerrainScanBounds group_bounds = bounds;
            group_bounds.min_x -= group.origin.x;
            group_bounds.max_x -= group.origin.x;
            group_bounds.min_y -= group.origin.y;
            group_bounds.max_y -= group.origin.y;
            group_bounds.min_z -= group.origin.z;
            group_bounds.max_z -= group.origin.z;

            TERRAIN_SHAPE_BATCH *batch = static_cast<TERRAIN_SHAPE_BATCH *>(group.data);
            while (batch->marker >= 0) {
                TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                if (group_bounds.max_x >= batch->min_x && batch->max_x > group_bounds.min_x &&
                    group_bounds.max_z >= batch->min_z && batch->max_z > group_bounds.min_z) {
                    for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index) {
                        TERRAIN_SHAPE *candidate = &shapes[shape_index];
                        if (!TerrainShapeOverlaps(group_bounds, *candidate) ||
                            reinterpret_cast<u8 *>(writer.cursor) >= writer.limit) {
                            continue;
                        }
                        if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((candidate->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        if (TerI->object_scale != 1.0f) {
                            TERRAIN_SHAPE *source = candidate;
                            candidate = &ScaleTerrain[writer.scaled_shape_count++];
                            candidate->material[0] = source->material[0];
                            candidate->material[1] = source->material[1];
                            candidate->flags = source->flags;
                            candidate->normal_flags = source->normal_flags;

                            for (i32 vector_index = 0; vector_index < 3; ++vector_index) {
                                candidate->vectors[vector_index].x = source->vectors[vector_index].x;
                                candidate->vectors[vector_index].y =
                                    (source->vectors[vector_index].y + group.origin.y) * TerI->inverse_object_scale -
                                    group.origin.y;
                                candidate->vectors[vector_index].z = source->vectors[vector_index].z;
                            }

                            if (source->normals[1].y < 65535.0f) {
                                candidate->vectors[3].x = source->vectors[3].x;
                                candidate->vectors[3].y =
                                    (source->vectors[3].y + group.origin.y) * TerI->inverse_object_scale -
                                    group.origin.y;
                                candidate->vectors[3].z = source->vectors[3].z;

                                const f32 length =
                                    NuFsqrt(source->normals[1].x * source->normals[1].x +
                                            source->normals[1].y * source->normals[1].y * TerI->object_scale_sq +
                                            source->normals[1].z * source->normals[1].z);
                                const f32 inverse_length = length == 0.0f ? 0.0f : 1.0f / length;
                                candidate->normals[1].x = source->normals[1].x * inverse_length;
                                candidate->normals[1].y = source->normals[1].y * TerI->object_scale * inverse_length;
                                candidate->normals[1].z = source->normals[1].z * inverse_length;
                            } else {
                                candidate->normals[1].y = 65536.0f;
                            }

                            const f32 length =
                                NuFsqrt(source->normals[0].x * source->normals[0].x +
                                        source->normals[0].y * source->normals[0].y * TerI->object_scale_sq +
                                        source->normals[0].z * source->normals[0].z);
                            const f32 inverse_length = length == 0.0f ? 0.0f : 1.0f / length;
                            candidate->normals[0].x = source->normals[0].x * inverse_length;
                            candidate->normals[0].y = source->normals[0].y * TerI->object_scale * inverse_length;
                            candidate->normals[0].z = source->normals[0].z * inverse_length;
                        }
                        *writer.cursor++ = candidate;
                        ++writer.group_shape_count;
                    }
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + batch->shape_count);
            }
            TerrainFinishScanGroup(&writer, group_index);
        }
    }

    for (i32 pick_index = 0; pick_index < curPickInst; ++pick_index) {
        const i32 group_index = CurTerr->max_groups + pick_index;
        TERRAIN_GROUP &group = CurTerr->groups[group_index];
        TerrainScanBounds local_bounds = bounds;
        local_bounds.min_x -= group.origin.x;
        local_bounds.max_x -= group.origin.x;
        local_bounds.min_y -= group.origin.y;
        local_bounds.max_y -= group.origin.y;
        local_bounds.min_z -= group.origin.z;
        local_bounds.max_z -= group.origin.z;
        if (!TerrainBoundsOverlap(local_bounds, group.bounds_min, group.bounds_max) || group.chunk_type == -1) {
            continue;
        }

        TERRAIN_SHAPE_BATCH *batch = static_cast<TERRAIN_SHAPE_BATCH *>(group.data);
        while (batch->marker >= 0) {
            TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
            if (local_bounds.max_x >= batch->min_x && batch->max_x > local_bounds.min_x &&
                local_bounds.max_z >= batch->min_z && batch->max_z > local_bounds.min_z) {
                for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index) {
                    TERRAIN_SHAPE *candidate = &shapes[shape_index];
                    if (!TerrainShapeOverlaps(local_bounds, *candidate) ||
                        reinterpret_cast<u8 *>(writer.cursor) >= writer.limit) {
                        continue;
                    }
                    if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                        continue;
                    }
                    if ((candidate->flags & scan_flags) == 0x40) {
                        continue;
                    }

                    if (TerI->object_scale != 1.0f) {
                        TERRAIN_SHAPE *source = candidate;
                        candidate = &ScaleTerrain[writer.scaled_shape_count++];
                        candidate->material[0] = source->material[0];
                        candidate->material[1] = source->material[1];
                        candidate->flags = source->flags;
                        candidate->normal_flags = source->normal_flags;

                        for (i32 vector_index = 0; vector_index < 3; ++vector_index) {
                            candidate->vectors[vector_index].x = source->vectors[vector_index].x;
                            candidate->vectors[vector_index].y =
                                (source->vectors[vector_index].y + group.origin.y) * TerI->inverse_object_scale -
                                group.origin.y;
                            candidate->vectors[vector_index].z = source->vectors[vector_index].z;
                        }

                        if (source->normals[1].y < 65535.0f) {
                            candidate->vectors[3].x = source->vectors[3].x;
                            candidate->vectors[3].y =
                                (source->vectors[3].y + group.origin.y) * TerI->inverse_object_scale - group.origin.y;
                            candidate->vectors[3].z = source->vectors[3].z;

                            const f32 length =
                                NuFsqrt(source->normals[1].x * source->normals[1].x +
                                        source->normals[1].y * source->normals[1].y * TerI->object_scale_sq +
                                        source->normals[1].z * source->normals[1].z);
                            const f32 inverse_length = length == 0.0f ? 0.0f : 1.0f / length;
                            candidate->normals[1].x = source->normals[1].x * inverse_length;
                            candidate->normals[1].y = source->normals[1].y * TerI->object_scale * inverse_length;
                            candidate->normals[1].z = source->normals[1].z * inverse_length;
                        } else {
                            candidate->normals[1].y = 65536.0f;
                        }

                        const f32 length = NuFsqrt(source->normals[0].x * source->normals[0].x +
                                                   source->normals[0].y * source->normals[0].y * TerI->object_scale_sq +
                                                   source->normals[0].z * source->normals[0].z);
                        const f32 inverse_length = length == 0.0f ? 0.0f : 1.0f / length;
                        candidate->normals[0].x = source->normals[0].x * inverse_length;
                        candidate->normals[0].y = source->normals[0].y * TerI->object_scale * inverse_length;
                        candidate->normals[0].z = source->normals[0].z * inverse_length;
                    }
                    *writer.cursor++ = candidate;
                    ++writer.group_shape_count;
                }
            }
            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + batch->shape_count);
        }
        TerrainFinishScanGroup(&writer, group_index);
    }

    TerI->scan_list = TerI->scan_list_storage;
    i16 *terminator = reinterpret_cast<i16 *>(writer.group_header);
    terminator[0] = 0;
    terminator[1] = 0;
}
void TerrainSkin(PLATSKININFO *, nuvec_s *, float, i32) {
}
void TerrDrawPlat(tertype *, i16) {
}
void TerrDrawSitu(tertype *, terrsitu_s *) {
}
void RotateTerrain(tertype *) {
}

namespace {

    struct TERRAIN_IMPACT_RECORD {
        NUVEC position;
        NUVEC normal;
        u8 material[2];
        u8 flags;
        u8 normal_flags;
    };
    DECOMP_ASSERT(sizeof(TERRAIN_IMPACT_RECORD) == 0x1c, "TERRAIN_IMPACT_RECORD ABI");

    enum TERRAIN_IMPACT_RESULT_FLAGS {
        TERRAIN_IMPACT_RESULT_WALL = 0x01,
        TERRAIN_IMPACT_RESULT_GROUND = 0x02,
        TERRAIN_IMPACT_RESULT_STATIC_WALL = 0x10,
    };

} // namespace

void TerrainImpact(NUVEC *position, NUVEC *movement, u8 *hit_flags) {
    TerrainMoveImpactData();

    TerrainQuery_s *query = TerI;
    u8 hit_type = query->hit_type;
    f32 walkable_normal_y = 0.0f;

    if (hit_type == TERRAIN_HIT_TYPE_NONE) {
        hit_flags[0] = 0;
        position->x = query->position.x + query->movement.x;
        position->y = (query->position.y + query->movement.y) * query->object_scale -
                      query->object_scale * query->collision_radius;
        position->z = query->position.z + query->movement.z;
        query->flags &= static_cast<u8>(~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL);
    } else {
        TERRAIN_SHAPE *surface = query->surface;
        f32 surface_alignment = 0.707f;
        if (surface != NULL) {
            surface_alignment = query->movement_normal.x * surface->normals[0].x +
                                query->movement_normal.y * surface->normals[0].y +
                                query->movement_normal.z * surface->normals[0].z;
        }

        TERRAIN_GROUP *group = NULL;
        if (query->terrain_group_index != -1) {
            group = &CurTerr->groups[query->terrain_group_index];
        }
        const bool platform_group = group != NULL && group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY;

        if (platform_group) {
            const u8 base_hit_type = hit_type & TERRAIN_HIT_TYPE_CLASS_MASK;
            if (base_hit_type > TERRAIN_HIT_TYPE_FACE && surface_alignment < 0.95f) {
                walkable_normal_y = 0.98f;
            } else {
                walkable_normal_y = 0.707f;
            }
        } else {
            walkable_normal_y = query->shape_adjusted != 0 ? 1.1f : -1.1f;
        }

        // A non-negative group index on a hit always comes with a surface.
        const bool wall_override_surface =
            query->terrain_group_index >= 0 &&
            (surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE;
        if (wall_override_surface) {
            query->position.x += query->impact_normal.x * 0.0015f;
            query->position.z += query->impact_normal.z * 0.0015f;
            walkable_normal_y = 1.1f;
        }
        if (wallover != 0.0f) {
            walkable_normal_y = wallover;
        }

        const bool second_normal = (hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) != 0;
        const u8 base_hit_type = hit_type & TERRAIN_HIT_TYPE_CLASS_MASK;
        const bool supported_hit_type = hit_type <= (TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE) &&
                                        base_hit_type >= TERRAIN_HIT_TYPE_FACE &&
                                        base_hit_type <= TERRAIN_HIT_TYPE_SPHERE;

        if (supported_hit_type && !second_normal) {
            if (base_hit_type != TERRAIN_HIT_TYPE_FACE && !wall_override_surface) {
                walkable_normal_y = 0.707f;
            }

            query->hit_time -= query->separation_epsilon;
            if (query->hit_time < 0.0f) {
                query->hit_time = 0.0f;
            }

            const f32 advance = query->hit_time;
            const NUVEC travelled = {
                query->movement.x * advance,
                query->movement.y * advance,
                query->movement.z * advance,
            };
            query->position.x += travelled.x;
            query->position.y += travelled.y;
            query->position.z += travelled.z;
            query->movement.x -= travelled.x;
            query->movement.y -= travelled.y;
            query->movement.z -= travelled.z;

            if (query->impact_normal.y >= walkable_normal_y) {
                FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                FullDeflect(&query->impact_normal, movement, movement);

                terrhitflags |= TERRAIN_IMPACT_RESULT_GROUND;
                hit_flags[0] = 1;
                hit_flags[1] = 1;
                query->position.y += query->movement_normal.y * 0.0003f;

                query = TerI;
                if (query->terrain_group_index != -1) {
                    group = &CurTerr->groups[query->terrain_group_index];
                    if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                        CurTerr->platforms[group->scene_index].flags |= TERRAIN_PLATFORM_FLAG_COLLIDED;
                    }
                }
            } else {
                hit_flags[0] = 0;
                terrhitflags |= TERRAIN_IMPACT_RESULT_WALL;
                if (!platform_group) {
                    terrhitflags |= TERRAIN_IMPACT_RESULT_STATIC_WALL;
                }

                query->position.x += query->movement_normal.x * 0.0006f;
                query->position.z += query->movement_normal.z * 0.0006f;
                query->movement.x += query->movement_normal.x * 0.0006f;
                query->movement.z += query->movement_normal.z * 0.0006f;
                movement->x += query->movement_normal.x * 0.0009f;
                movement->z += query->movement_normal.z * 0.0009f;

                FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                FullDeflectSmallY(&query->impact_normal, movement, movement);

                query = TerI;
                if (CurTrackInfo == NULL) {
                    CurTrackInfo = AllocTerrId();
                    CurTrackInfo->flags = TERRAIN_TRACK_FLAG_NONE;
                    CurTrackInfo->id = query->hit_flags;
                    CurTrackInfo->platform_index = 0;
                    CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;
                }
                CurTrackInfo->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                CurTrackInfo->impact_normal = query->impact_normal;
                TerrWallNorm = query->impact_normal;
            }

            query = TerI;
            query->flags &= static_cast<u8>(~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL);
        } else if (supported_hit_type) {
            if (base_hit_type != TERRAIN_HIT_TYPE_FACE) {
                walkable_normal_y = 0.707f;
            }

            NUVEC response_normal = query->movement_normal;
            if ((query->flags & TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL) != 0) {
                const f32 previous_alignment = response_normal.x * query->previous_movement_normal.x +
                                               response_normal.y * query->previous_movement_normal.y +
                                               response_normal.z * query->previous_movement_normal.z;
                if (previous_alignment < 0.0f) {
                    NUVEC corrected_normal;
                    FullDeflect(&response_normal, &query->previous_movement_normal, &corrected_normal);
                    const f32 correction_alignment = corrected_normal.x * response_normal.x +
                                                     corrected_normal.y * response_normal.y +
                                                     corrected_normal.z * response_normal.z;
                    if (correction_alignment > 0.25f) {
                        response_normal.x = corrected_normal.x / correction_alignment;
                        response_normal.y = corrected_normal.y / correction_alignment;
                        response_normal.z = corrected_normal.z / correction_alignment;
                    }
                }
            }

            query = TerI;
            const f32 contact_push = 0.0035f - query->unclamped_hit_time * 1.05f;
            if (query->impact_normal.y >= walkable_normal_y) {
                query->position.x += response_normal.x * contact_push;
                query->position.y += response_normal.y * contact_push + response_normal.y * 0.01f;
                query->position.z += response_normal.z * contact_push;
                query->movement.y += response_normal.y * 0.01f;

                FullDeflect(&response_normal, &query->movement, &query->movement);
                FullDeflect(&query->impact_normal, movement, movement);
            } else {
                const f32 movement_push = 0.0035f - query->unclamped_hit_time * 0.35f;
                query->position.x += response_normal.x * contact_push;
                query->position.y += response_normal.y * contact_push;
                query->position.z += response_normal.z * contact_push;
                query->movement.x += response_normal.x * movement_push;
                query->movement.y += response_normal.y * movement_push;
                query->movement.z += response_normal.z * movement_push;
                hit_flags[0] = 1;

                FullDeflect(&response_normal, &query->movement, &query->movement);
                FullDeflectSmallY(&query->impact_normal, movement, movement);

                query = TerI;
                if (CurTrackInfo == NULL) {
                    CurTrackInfo = AllocTerrId();
                    CurTrackInfo->flags = TERRAIN_TRACK_FLAG_NONE;
                    CurTrackInfo->id = query->hit_flags;
                    CurTrackInfo->platform_index = 0;
                    CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;
                }
                CurTrackInfo->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                CurTrackInfo->impact_normal = query->impact_normal;
            }

            query = TerI;
            query->flags |= TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
            query->previous_movement_normal = query->movement_normal;
        }
    }

    query = TerI;
    hit_type = query->hit_type;
    if (hit_type >= TERRAIN_HIT_TYPE_FACE && hit_type <= TERRAIN_HIT_TYPE_CLASS_MASK &&
        query->terrain_group_index != -1) {
        TERRAIN_GROUP *group = &CurTerr->groups[query->terrain_group_index];
        if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY && query->impact_normal.y >= walkable_normal_y) {
            PlatformConnect(reinterpret_cast<char *>(query->hit_flags), &query->movement, movement, group->scene_index);
        }
    }

    if (TerImpactData == NULL || *TerImpactDataCount >= TerImpactDataMax) {
        return;
    }

    query = TerI;
    if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
        return;
    }

    TERRAIN_IMPACT_RECORD *records = static_cast<TERRAIN_IMPACT_RECORD *>(TerImpactData);
    TERRAIN_IMPACT_RECORD &record = records[*TerImpactDataCount];
    record.position.x = query->position.x - query->movement_normal.x * query->collision_radius;
    record.position.y = (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
    record.position.z = query->position.z - query->movement_normal.z * query->collision_radius;
    record.normal = query->movement_normal;

    if (query->surface != NULL) {
        record.material[0] = query->surface->material[0];
        record.material[1] = query->surface->material[1];
        record.flags = query->surface->flags;
        record.normal_flags = query->surface->normal_flags;
    } else if (TerrWallInfo != 0) {
        record.material[0] = TerrWallTab[0];
        record.material[1] = TerrWallTab[1];
        record.flags = 0;
        record.normal_flags = 0;
    }
    ++*TerImpactDataCount;
}
void TerrainPlayer(GameObject_s *object) {
    if (object == NULL || object->pad_gamepad == NULL) {
        return;
    }

    APIOBJECT &api = object->apiobj;
    const AIPATHINFO &path_info = object->ai.path_info;
    const AIPATHCNX *path_connection = path_info.connection;
    const bool ordinary_ai_path =
        (api.field_0x1f4 & APIOBJECT_MOTION_FLAG_AI_CONTROLLED) != 0 && object->movement_spline == NULL &&
        object->context_target_position == NULL &&
        (path_info.flags & (AIPATHINFO_FLAG_ON_PATH | AIPATHINFO_FLAG_NARROW_PATH)) == AIPATHINFO_FLAG_ON_PATH &&
        (api.field_0x1fa & 4) == 0 && path_connection != NULL &&
        (path_connection->original_traversal_flags[0] & static_cast<u32>(LEGO_AIPATHCNX_BLOCKAGE)) == 0 &&
        (path_connection->traversal_flags[0] & static_cast<u32>(LEGO_AIPATHCNX_FULLTERRAIN)) == 0;

    // TerrainPlayer selects either the inexpensive shadow-grounding path used
    // by ordinary AI or the swept capsule resolver required by players and
    // special traversal connections, then refreshes floor/contact state before
    // dispatching the character movement callback.
    const f32 lower_bound = object->character_bottom * api.field_0xa8;
    const i32 terrain_mask = TERRAINMASK_NONWEAPON | TERRAINMASK_NONDROID;

    if (ordinary_ai_path) {
        // Ordinary path-following AI uses the target's inexpensive shadow
        // grounding path.  Full swept collision is reserved for path
        // connections whose traversal flags require special collision.
        api.respawn_timer = 0.0f;
        object->field_0xe20 |= 2;
        api.supporting_platform_id = -1;
        api.position.x += api.velocity.x * FRAMETIME;
        api.position.y += api.velocity.y * FRAMETIME;
        api.position.z += api.velocity.z * FRAMETIME;

        api.field_0x218 = GameShadow(object, &api.collision_position, 5.0f, terrain_mask | 0x1f);
        api.supporting_platform_id = static_cast<i16>(NewShadowOnPlatform());
        if (api.field_0x218 != 2000000.0f && api.position.y + lower_bound < api.field_0x218) {
            api.velocity.y = 0.0f;
            api.position.y = api.field_0x218 - lower_bound;
        }

        api.field_0x27d = 0;
        if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
            api.field_0x27d |= APIOBJECT_TERRAIN_CONTACT_NEAR_FLOOR;
        }
        if (api.collision_min.y <= api.field_0x218) {
            api.field_0x27d |= APIOBJECT_TERRAIN_CONTACT_FLOOR;
        }
        if ((api.field_0x27d & APIOBJECT_TERRAIN_CONTACT_FLOOR) != 0) {
            object->field_0x1084 = 1;
            object->contact_position.x = api.position.x;
            object->contact_position.y = api.field_0x218;
            object->contact_position.z = api.position.z;
            object->field_0x6b0 = api.field_0x281;
            object->contact_normal = object->surface_normal;
        } else {
            object->field_0x6b0 = 0;
        }
    } else {
        NUVEC collision_position = api.position;
        collision_position.y += lower_bound;

        NUVEC movement;
        NuVecScale(&movement, &api.velocity, FRAMETIME);

        // The full resolver retains the previous contact bits while preparing
        // its query; they are rebuilt from field_0x105c at the common epilogue.
        // The clears at 0x10343b/0x104447 belong to the alternate integration
        // paths, while this path overwrites platform support from its shadow hit.
        object->field_0x6b0 = 0;
        const i32 object_index = Obj != NULL ? static_cast<i32>(object - Obj) : -1;

        NewTerrainScaleYMask(&collision_position, &movement, reinterpret_cast<u8 *>(&object->field_0x105c),
                             object_index, 0.0f, api.collision_radius, object->collision_y_scale, 0, 0, terrain_mask);
        api.supporting_platform_id = static_cast<i16>(NewShadowOnPlatform());

        api.position.x = collision_position.x;
        api.position.y = collision_position.y - lower_bound;
        api.position.z = collision_position.z;
        if (FRAMETIME != 0.0f) {
            NuVecScale(&api.velocity, &movement, 1.0f / FRAMETIME);
        }

        object->field_0x1084 = static_cast<u8>(TerrImpact);
        if (TerrImpact != 0) {
            object->contact_position = TerrImpactPos;
            object->contact_normal = TerrImpactNormal;
        }
        // The full-resolver path reaches 0x105215 only after the resolved
        // position and velocity have been copied back (via the backward edge at
        // 0x105dc6).  This is distinct from the simple-integration shadow probe.
        api.field_0x218 = GameShadow(object, &api.position, 5.0f, terrain_mask | 0x1f);
        // Full-resolver epilogue 0x1037b9..0x103806 rebuilds contact from the
        // resolver result and the common near-floor test.
        api.field_0x27d = object->field_0x105c != 0 ? APIOBJECT_TERRAIN_CONTACT_FLOOR : 0;
        if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
            api.field_0x27d |= APIOBJECT_TERRAIN_CONTACT_NEAR_FLOOR;
        }
    }
    if (api.field_0x27d != 0) {
        // Target 0x103816 marks the object as terrain-supported here; the
        // collision resolver owns the contact normal fields.
        object->field_0xeff |= 2;
    }

    // Target 0x102e97..0x102ee3 runs this block after the normal swept-terrain
    // and contact path.  The model flag controls whether saved pad state is
    // restored or BigJumpCode consumes the updated contact state.
    const u32 held_buttons = object->pad_gamepad->buttons_held;
    const u32 pressed_buttons = object->pad_gamepad->buttons_pressed;
    Tag_Check(object);
    PreResetCode(object);
    if (api.character_data != NULL && api.character_data->move_fn != NULL) {
        api.character_data->move_fn(object);
    }
    PostResetCode(object);
    if (api.character_data != NULL && (api.character_data->model_flags & 0x00200000) != 0) {
        object->pad_gamepad->buttons_held = held_buttons;
        object->pad_gamepad->buttons_pressed = pressed_buttons;
    } else {
        BigJumpCode(object);
    }

    // Common epilogue at 0x102b60.
    object->pad_gamepad->allocated_5a &= 0xe9;
    object->field_0xd14 = 0;
    object->pad_gamepad->operator_data = NULL;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, api.field_0x276);
}
static inline void MakePlayCorner(GAMECAMERA_s *camera, f32 screen_x, f32 screen_y, NUVEC *near_corner,
                                  NUVEC *far_corner) {
    NUVEC ray = {screen_x, screen_y, 1.0f};
    NuVecMtxRotate(&ray, &ray, &camera->target_mtx);
    NuVecAdd(near_corner, &ray, &camera->pos);
    NuVecNorm(&ray, &ray);
    far_corner->x = near_corner->x + ray.x * 5.0f;
    far_corner->y = near_corner->y + ray.y * 5.0f;
    far_corner->z = near_corner->z + ray.z * 5.0f;
}

static inline void SetPlayPlane(PLAYPLANE_s *plane, NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, NUVEC *edge_a0,
                                NUVEC *edge_a1, NUVEC *edge_b0, NUVEC *edge_b1) {
    plane->point.x = (a->x + b->x + c->x + d->x) * 0.25f;
    plane->point.y = (a->y + b->y + c->y + d->y) * 0.25f;
    plane->point.z = (a->z + b->z + c->z + d->z) * 0.25f;
    NUVEC first_edge;
    NUVEC second_edge;
    NuVecSub(&first_edge, edge_a0, edge_a1);
    NuVecSub(&second_edge, edge_b0, edge_b1);
    NuVecCross(&plane->normal, &first_edge, &second_edge);
    NuVecNorm(&plane->normal, &plane->normal);
}

void MakePlayPlanes(GAMECAMERA_s *camera) {
    NUVEC near_corner[4];
    NUVEC far_corner[4];
    MakePlayCorner(camera, -0.8f * PANEL3DMULX, 0.85f * PANEL3DMULY, &near_corner[0], &far_corner[0]);
    MakePlayCorner(camera, 0.85f * PANEL3DMULX, 0.85f * PANEL3DMULY, &near_corner[1], &far_corner[1]);
    MakePlayCorner(camera, -0.8f * PANEL3DMULX, -0.8f * PANEL3DMULY, &near_corner[2], &far_corner[2]);
    MakePlayCorner(camera, 0.85f * PANEL3DMULX, -0.8f * PANEL3DMULY, &near_corner[3], &far_corner[3]);

    // Plane order is near, left, right, top, bottom, far.
    SetPlayPlane(&PlayPlane[1], &near_corner[0], &far_corner[0], &far_corner[2], &near_corner[2], &far_corner[0],
                 &near_corner[0], &far_corner[2], &near_corner[2]);
    SetPlayPlane(&PlayPlane[2], &far_corner[1], &near_corner[1], &near_corner[3], &far_corner[3], &near_corner[1],
                 &far_corner[1], &near_corner[3], &far_corner[3]);
    SetPlayPlane(&PlayPlane[3], &near_corner[0], &near_corner[1], &far_corner[1], &far_corner[0], &near_corner[1],
                 &near_corner[0], &far_corner[1], &far_corner[0]);
    SetPlayPlane(&PlayPlane[4], &near_corner[2], &far_corner[2], &far_corner[3], &near_corner[3], &far_corner[2],
                 &near_corner[2], &far_corner[3], &near_corner[2]);
    SetPlayPlane(&PlayPlane[0], &near_corner[0], &near_corner[2], &near_corner[3], &near_corner[1], &near_corner[2],
                 &near_corner[0], &near_corner[3], &near_corner[0]);
    SetPlayPlane(&PlayPlane[5], &far_corner[0], &far_corner[1], &far_corner[3], &far_corner[2], &far_corner[1],
                 &far_corner[0], &far_corner[3], &far_corner[0]);
}
void TerrDrawPlatCol(tertype *, i16, i32) {
}
void TerrShowCamTerr() {
}
void GetIndGrassClump(i32, i32) {
}
void TerrainStaticMtx(PLATSKININFO *, nuvec_s *, i32) {
}
void ScanTerrainHandel(i32, i16 *) {
}
extern "C" void NewShapeInit(NUVEC *offset) {
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 0.0f;
}

void TerrainImpactNorm() {
    TerrainMoveImpactData();

    const u8 maximum_supported_hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE;
    if (TerI->hit_type > maximum_supported_hit_type) {
        return;
    }

    const i32 hit_type_flag = 1 << TerI->hit_type;
    const i32 sphere_class_mask = 1 << TERRAIN_HIT_TYPE_SPHERE;
    const i32 rotate_and_mark_mask = sphere_class_mask | (sphere_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
    const i32 rotated_surface_mask = (1 << TERRAIN_HIT_TYPE_CYLINDER) | (1 << TERRAIN_HIT_TYPE_VERTEX);
    const i32 rotate_mask = rotated_surface_mask | (rotated_surface_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
    const i32 face_class_mask = 1 << TERRAIN_HIT_TYPE_FACE;
    const i32 direct_normal_mask = face_class_mask | (face_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);

    if ((hit_type_flag & rotate_and_mark_mask) != 0) {
        terrhitflags |= 4;
    }

    const bool rotated_hit = (hit_type_flag & (rotate_and_mark_mask | rotate_mask)) != 0;
    if (rotated_hit) {
        RotateVec(&TerI->movement_normal, &TerI->movement_normal);
    } else if ((hit_type_flag & direct_normal_mask) == 0) {
        return;
    }

    TerrainQuery_s *query = TerI;
    // Curved hits are produced in collision-height-scaled space and need to
    // be transformed back. Face normals already come from the terrain in
    // object space; the target's direct-face branch deliberately bypasses
    // this scaling before copying the normal below.
    if (rotated_hit && (query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
        query->movement_normal.x *= query->inverse_collision_radius;
        query->movement_normal.y *= query->inverse_collision_radius;
        query->movement_normal.z *= query->inverse_collision_radius;
    }

    if (query->object_scale == 1.0f) {
        query->impact_normal = query->movement_normal;
        return;
    }

    const f32 normal_length =
        NuFsqrt(query->movement_normal.x * query->movement_normal.x +
                query->movement_normal.y * query->movement_normal.y * query->inverse_object_scale_sq +
                query->movement_normal.z * query->movement_normal.z);
    f32 inverse_normal_length = 0.0f;
    if (normal_length != 0.0f) {
        inverse_normal_length = 1.0f / normal_length;
    }

    query = TerI;
    query->impact_normal.x = query->movement_normal.x * inverse_normal_length;
    query->impact_normal.y = query->movement_normal.y * query->inverse_object_scale * inverse_normal_length;
    query->impact_normal.z = query->movement_normal.z * inverse_normal_length;
}
void ScanTerrainPlatform(i32, i32) {
}
void TerrainBlockOnBlock(WORLDINFO_s *, pushblock_s *, nuvec_s *, float *) {
}
void TerrainSkinAllocate(terrsitu_s *) {
}
void ScanTerrIDRemovePlat(i32 platform_index) {
    TERRAIN_TRACK_SLOT *slot = CurTerr->track_slots;
    i32 remaining = TERRAIN_TRACK_SLOT_COUNT;
    do {
        if (slot->platform_index == platform_index) {
            slot->id = NULL;
        }
        ++slot;
        --remaining;
    } while (remaining != 0);
}
void ScanWallSplineTerrain(i32, i32, i32) {
}
i32 TerrainImpactPlatform(unsigned char *hit_flags) {
    TerrainMoveImpactData();

    TerrainQuery_s *query = TerI;
    if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
        hit_flags[0] = 0;
        query->position.x += query->movement.x;
        query->position.y += query->movement.y;
        query->position.z += query->movement.z;
        return 0;
    }

    const u8 maximum_platform_hit_type = TERRAIN_HIT_TYPE_SPHERE;
    if (query->hit_type > maximum_platform_hit_type) {
        return 1;
    }

    query->hit_time -= query->separation_epsilon;
    if (query->hit_time < 0.0f) {
        query->hit_time = 0.0f;
    }

    query->position.x += query->movement.x * query->hit_time;
    query->position.y += query->movement.y * query->hit_time;
    query->position.z += query->movement.z * query->hit_time;

    const f32 minimum_walkable_normal_y = 0.707f;
    if (query->impact_normal.y < minimum_walkable_normal_y) {
        hit_flags[0] = 0;
        return 0;
    }

    hit_flags[0] = 1;
    hit_flags[1] = 1;
    query->position.y += query->movement_normal.y * 0.004f;
    return 0;
}
void TerrainMoveImpactData() {
    const i32 stored_class_mask = (1 << TERRAIN_HIT_TYPE_FACE) | (1 << TERRAIN_HIT_TYPE_CYLINDER) |
                                  (1 << TERRAIN_HIT_TYPE_VERTEX) | (1 << TERRAIN_HIT_TYPE_SPHERE);
    const i32 stored_hit_type_mask = stored_class_mask | (stored_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
    const u8 maximum_stored_hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE;

    if (TerI->hit_type > maximum_stored_hit_type) {
        return;
    }

    i32 hit_type_flag = 1 << TerI->hit_type;
    hit_type_flag &= stored_hit_type_mask;
    if (hit_type_flag == 0 || TerI->terrain_group_index == -1) {
        return;
    }

    NewTerrStoreAnyInfo();
    TerrPoly = &TerrPolyInfo;
    TerrPolyObj = TerI->terrain_group_index;
}
i32 TerrainPlatformEmbedded(nuvec_s *) {
    return 0;
}
void TerrainPlatformMoveCheck(nuvec_s *, nuvec_s *, i32, i32, i32) {
}
void TerrDraw(tertype *, i16) {
}
