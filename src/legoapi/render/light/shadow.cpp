#include "legoapi/render/light/shadow.h"
#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nufloat.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern i32 VehicleArea;

namespace {
    struct ESHADOW_POLY_s {
        u8 unknown_00[0x61];
        u8 terrain_layer;
    };
} // namespace

static ESHADOW_POLY_s *EShadPoly;

extern "C" void NuRndrShadowDirCol(const NUVEC *direction, u32 colour, f32 near_distance, f32 far_distance);

f32 BlobShadowFade(NUVEC *position, f32 fade_start, f32 fade_end, f32 alpha) {
    if (alpha <= 0.0f) {
        return 0.0f;
    }

    NUVEC camera_delta;
    NuVecSub(&camera_delta, position, NUMTX_GET_ROW_VEC(&pNuCam->mtx, 3));
    const f32 distance_squared =
        camera_delta.x * camera_delta.x + camera_delta.y * camera_delta.y + camera_delta.z * camera_delta.z;

    if (distance_squared >= fade_end * fade_end) {
        return 0.0f;
    }
    if (distance_squared <= fade_start * fade_start) {
        return alpha;
    }

    const f32 distance = NuFsqrt(distance_squared);
    return (1.0f - (distance - fade_start) / (fade_end - fade_start)) * alpha;
}

void SetShadowLight() {
}

void Shadow_SetMode() {
    ShadowMode = 0;
    for (i32 i = 0; i < 8; ++i) {
        GameObject_s *player = Player[i];
        if (player == NULL || player->suit == NULL) {
            continue;
        }

        const SUIT_s *suit = static_cast<const SUIT_s *>(player->suit);
        if ((suit->store_flag & SUIT_STORE_FLAG_SHADOW_MODE) != 0) {
            ShadowMode = 1;
            return;
        }
    }
}

void InitShadowLights() {
}

void RenderShadowLights(i32) {
}

void ConfigureComplexShadow(GameObject_s *object) {
    NUVEC shadow_direction;
    if (object != NULL) {
        shadow_direction.x = 65536.0f;
        shadow_direction.y = object->apiobj.collision_position.y;
        shadow_direction.z = object->apiobj.field_0x218 - 0.1f;
    } else {
        shadow_direction = {0.0f, -6.25f, 0.0f};
    }

    const f32 shadow_distance = VehicleArea != 0 ? 150.0f : 30.0f;
    NuRndrShadowDirCol(&shadow_direction, 0x17000000, shadow_distance, shadow_distance);
}

void ResetShadowMapRendering() {
}

void EnableShadowMapRendering(i32) {
}

void ResetShadowMapRenderingFn() {
    ResetShadowMapRendering();
}

void EnableShadowMapRenderingFn() {
}

extern "C" {

    i32 EShadowInfo() {
        return EShadPoly != NULL ? EShadPoly->terrain_layer : -1;
    }

    void EShadowRoofInfo(void) {
    }

} // extern "C"
