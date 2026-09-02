#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/android/nutimebar_plain.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nufile/nufile.h"

#include <math.h>
#include <string.h>

struct nuqtdim_s;
struct nuqthdr_s;
struct rtl_s;
struct rtlidata_s;
struct NUFRUSTRUM;

void rtlSwapSetEndianess(rtlset *);

static __used__ int ElOverlaps(nuqtdim_s *, nuqtdim_s *) {
    return 0;
}

static __used__ rtl_s *GetNextRTL(void *, rtl_s *, char *, int *) {
    return nullptr;
}

static __used__ int InsertData(nuqthdr_s *, int, void *) {
    return 0;
}

static __used__ void InsertLight(rtl_s *, rtlidata_s *, float) {
}

static __used__ void InsertAntiLight(rtl_s *, rtlidata_s *, float) {
}

static __used__ int FindNearestRTL(nuvec_s *, int) {
    return 0;
}

static __used__ bool InsideLineXZ(float, float, float, float, float, float) {
    return false;
}

static f32 ClampUnit(f32 value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    return value > 1.0f ? 1.0f : value;
}

static __used__ int FindNearestFog(nuvec_s *) {
    return 0;
}

static __used__ i32 rtlCalcLights(nuvec_s *, numtx_s *, f32, rtlidata_s *) {
    return 0;
}

static __used__ void rtlCalcShadow(rtlidata_s *) {
}

static __used__ void rtlProcessLight(rtl_s *, f32) {
}

static __used__ void rtlSwapEndianess32(void *) {
}

static __used__ void rtlApplySetScaleLoop(void *, rtlidata_s *, nuvec_s *, numtx_s *, i32, f32) {
}

static __used__ void rtlApplyModifiersToChainLight(rtl_s *) {
}

static __used__ void rtlApplyModifiersToSingleLight(rtl_s *) {
}

static __used__ i32 rtlCmp(rtl_s *, rtl_s *) {
    return 0;
}

static __used__ void transposeClipPlanes(NUFRUSTRUM *) {
}

static __used__ void roomRecursive(nugscn_s *, NUFRUSTRUM *, i16, i16, i32) {
}

extern "C" {

    void IndexLights(rtlset *, VARIPTR *, i32);
    i32 NuRndrSetAmbientLightPS(const NUCOLOUR3 *);
    i32 NuRndrSetDirectionalLightsPS(const NUVEC *, const NUCOLOUR3 *, const NUVEC *, const NUCOLOUR3 *, const NUVEC *,
                                     const NUCOLOUR3 *);

    void fogAlloc(void) {
    }

    void fogFree(void) {
    }

    void rtlAlloc(void) {
    }

    void rtlResetEx(rtldata_s *data, i32 reset_cached) {
        memset(data, 0, 0x48);
        data->data[0x120] = 0;
        *reinterpret_cast<f32 *>(data->data + 0x120) = 1.0f;
        memset(data->data + 0x78, 0, 0x24);
        const NUVEC default_direction = {1.0f, 0.0f, 0.0f};
        for (i32 i = 0; i < 3; ++i) {
            *reinterpret_cast<NUVEC *>(data->data + 0x9c + i * sizeof(NUVEC)) = default_direction;
        }
        *reinterpret_cast<f32 *>(data->data + 0x134) = 1.0f;
        *reinterpret_cast<f32 *>(data->data + 0x138) = 0.0f;
        *reinterpret_cast<f32 *>(data->data + 0x13c) = 0.0f;
        if (reset_cached != 0) {
            memset(data->data + 0x4c, 0, 0x2c);
            *reinterpret_cast<f32 *>(data->data + 0x130) = 0.0f;
        }
    }

    static void rtlInsertLight(u8 *light, rtldata_s *data, f32 strength) {
        const bool ambient = light[0x58] == 1;
        const i32 pointer_offset = ambient ? 0x18 : 0x00;
        const i32 strength_offset = ambient ? 0x24 : 0x0c;
        for (i32 slot = 0; slot < 3; ++slot) {
            if (*reinterpret_cast<f32 *>(data->data + strength_offset + slot * 4) < strength) {
                for (i32 move = 2; move > slot; --move) {
                    *reinterpret_cast<u8 **>(data->data + pointer_offset + move * 4) =
                        *reinterpret_cast<u8 **>(data->data + pointer_offset + (move - 1) * 4);
                    *reinterpret_cast<f32 *>(data->data + strength_offset + move * 4) =
                        *reinterpret_cast<f32 *>(data->data + strength_offset + (move - 1) * 4);
                }
                *reinterpret_cast<u8 **>(data->data + pointer_offset + slot * 4) = light;
                *reinterpret_cast<f32 *>(data->data + strength_offset + slot * 4) = strength;
                return;
            }
        }
    }

    static f32 rtlDistanceStrength(const u8 *light, const NUVEC *position) {
        const NUVEC *light_position = reinterpret_cast<const NUVEC *>(light);
        const f32 dx = position->x - light_position->x;
        const f32 dy = position->y - light_position->y;
        const f32 dz = position->z - light_position->z;
        const f32 inner = *reinterpret_cast<const f32 *>(light + 0x34);
        const f32 outer = *reinterpret_cast<const f32 *>(light + 0x40);
        const f32 distance_sq = dx * dx + dy * dy + dz * dz;
        if (distance_sq >= outer * outer) {
            return 0.0f;
        }
        if (inner >= outer) {
            return 1.0f;
        }
        const f32 distance = sqrtf(distance_sq);
        return ClampUnit(1.0f - (distance - inner) / (outer - inner));
    }

    void rtlApplySetScale(void *set, rtldata_s *data, NUVEC *position, NUMTX *rotation, i32 identity, f32 scale) {
        (void)identity;
        rtlResetEx(data, 0);
        if (set != NULL) {
            u8 *light = static_cast<u8 *>(set) + 4;
            for (i32 i = 0; i < 0x80 && light[0x58] != 0; ++i, light += 0x8c) {
                f32 strength = light[0x58] == 5 ? 2.0f : rtlDistanceStrength(light, position);
                if (strength != 0.0f && light[0x58] != 7) {
                    rtlInsertLight(light, data, strength);
                }
            }
        }

        for (i32 slot = 0; slot < 3; ++slot) {
            u8 *light = *reinterpret_cast<u8 **>(data->data + slot * 4);
            NUVEC *colour = reinterpret_cast<NUVEC *>(data->data + 0x78 + slot * sizeof(NUVEC));
            NUVEC *direction = reinterpret_cast<NUVEC *>(data->data + 0x9c + slot * sizeof(NUVEC));
            if (light == NULL) {
                *colour = {0.0f, 0.0f, 0.0f};
                *direction = {0.0f, 1.0f, 0.0f};
                continue;
            }
            const f32 strength =
                *reinterpret_cast<f32 *>(data->data + 0x0c + slot * 4) * *reinterpret_cast<f32 *>(light + 0x6c) * scale;
            const NUVEC *source_colour = reinterpret_cast<const NUVEC *>(light + 0x18);
            NuVecScale(colour, const_cast<NUVEC *>(source_colour), strength);
            if (light[0x58] == 2 || light[0x58] == 3 || light[0x58] == 6 || light[0x58] == 8) {
                NuVecSub(direction, reinterpret_cast<NUVEC *>(light), position);
                NuVecNorm(direction, direction);
            } else if (light[0x58] == 4) {
                *direction = *reinterpret_cast<NUVEC *>(light + 0x0c);
            } else {
                *direction = {0.0f, 0.0f, 1.0f};
                NuVecRotateX(direction, direction, *reinterpret_cast<i16 *>(light + 0x5a));
                NuVecRotateY(direction, direction, *reinterpret_cast<i16 *>(light + 0x5c));
                NuVecMtxRotate(direction, direction, &global_camera.mtx);
            }
            if (rotation != NULL) {
                NuVecMtxRotate(direction, direction, rotation);
            }
        }

        NUVEC *ambient = reinterpret_cast<NUVEC *>(data->data + 0xc0);
        *ambient = {0.0f, 0.0f, 0.0f};
        for (i32 slot = 0; slot < 3; ++slot) {
            u8 *light = *reinterpret_cast<u8 **>(data->data + 0x18 + slot * 4);
            if (light == NULL) {
                continue;
            }
            const f32 strength =
                *reinterpret_cast<f32 *>(data->data + 0x24 + slot * 4) * *reinterpret_cast<f32 *>(light + 0x6c) * scale;
            const NUVEC *colour = reinterpret_cast<const NUVEC *>(light + 0x18);
            ambient->x = ClampUnit(ambient->x + colour->x * strength);
            ambient->y = ClampUnit(ambient->y + colour->y * strength);
            ambient->z = ClampUnit(ambient->z + colour->z * strength);
        }
    }

    void rtlDynamicAlloc(void) {
    }

    void rtlDynamicAllocTemplate(void) {
    }

    void rtlDynamicFree(void) {
    }

    void rtlDynamicMasterEnable(void) {
    }

    void rtlDynamicSetColours(void) {
    }

    void rtlDynamicSetDirection(void) {
    }

    void rtlDynamicSetPos(void) {
    }

    void rtlDynamicSetRadii(void) {
    }

    void rtlDynamicSetType(void) {
    }

    void rtlFrameUpdate(f32 frame_time) {
        rtltimer1 = static_cast<u16>(static_cast<i32>(rtltimer1adv * frame_time) + rtltimer1);
        NuTimeBarSlotReset(0, 6);
    }

    void rtlFree(void) {
    }

    void rtlGetCurrentSet(void) {
    }

    void rtlGetEnvPath(void) {
    }

    void rtlGetEnvSceneName(void) {
    }

    void rtlGetEnvSet(void) {
    }

    void rtlGetFogSet(void) {
    }

    void rtlInitDynamic(void) {
    }

    rtlset *rtlLoadSet(char *path, VARIPTR *buffer, i32 buffer_end) {
        buffer->addr = ALIGN(buffer->addr, 4);
        rtlset *set = static_cast<rtlset *>(buffer->void_ptr);
        memset(set, 0, 0x4f84);

        if (NuFileLoadBuffer(path, set, buffer_end - buffer->addr) > 0) {
            rtlSwapSetEndianess(set);
        }

        u8 *bytes = reinterpret_cast<u8 *>(set);
        for (i32 i = 0; i < 0x80; ++i) {
            *reinterpret_cast<i16 *>(bytes + i * 0x8c + 0x6e) = static_cast<i16>(i + 1);
            *reinterpret_cast<rtlset **>(bytes + i * 0x8c + 0x80) = set;
            *reinterpret_cast<f32 *>(bytes + i * 0x8c + 0x70) = 1.0f;
        }
        *reinterpret_cast<u32 *>(bytes) = 5;
        buffer->addr += 0x4f84;
        IndexLights(set, buffer, buffer_end);
        return set;
    }

    void rtlProcessLights(void *, f32) {
    }

    void rtlReset(void) {
    }

    void rtlSaveSet(void) {
    }

    void rtlScaleSetMultipliers(void) {
    }

    void rtlSetAssocName(void) {
    }

    void rtlSetExt(void) {
    }

    void rtlSetLights(rtldata_s *data) {
        const NUVEC *directions = reinterpret_cast<const NUVEC *>(data->data + 0x9c);
        const NUCOLOUR3 *colours = reinterpret_cast<const NUCOLOUR3 *>(data->data + 0x78);
        NuRndrSetDirectionalLightsPS(&directions[0], &colours[0], &directions[1], &colours[1], &directions[2],
                                     &colours[2]);
        NuRndrSetAmbientLightPS(reinterpret_cast<const NUCOLOUR3 *>(data->data + 0xc0));
    }

    void rtlSetMinR(void) {
    }

    void rtlSetModifiers(void) {
    }

    void rtlSetShadowFlickerBlendTime(void) {
    }

    void rtlSetShadowFlickerScale(void) {
    }

    void rtlSetSpecularLight(void) {
    }

    void rtlSetSpecularValue(void) {
    }

    void rtlSetUndoBuffer(void) {
    }

    void rtlSetUserIdName(void) {
    }

    void rtlSpecularValue(void) {
    }

    void rtlResetDynamic(void) {
    }

    i32 rtlFindByUserId(usize rtl_set, i32 user_id) {
        (void)rtl_set;
        (void)user_id;
        return -1;
    }

    void rtlGetDirection(usize rtl_set, i32 id, void **out) {
        (void)rtl_set;
        (void)id;
        (void)out;
    }

} // extern "C"

void SelectNextRTL() {
}

void SelectPrevRTL() {
}

void rtlSwapSetEndianess(rtlset *) {
}
