#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void ClearStill() {
}

void CircleLevel(LEVELDATA_s *) {
}

void CurrentStart(GameObject_s *, i32, i32) {
}

void DoSomeChecks(GameObject_s &, AIPATH_s &, AIPATHCNX_s &, i32) {
}

void CDataFromName(char *) {
}

float CalcValue1648(char *data, i32 quarter, i32 stride, float fraction, ani3_scalemin_s *scale_min) {
    u16 *next = reinterpret_cast<u16 *>(data + stride);
    u16 *keys = reinterpret_cast<u16 *>(data);
    float value;

    if (quarter == 0) {
        float tangent =
            static_cast<float>((keys[2] & 0xfff) - (keys[1] & 0xfff)) * fraction + static_cast<float>(keys[1] & 0xfff);
        value = (static_cast<float>(static_cast<i32>(*next) - static_cast<i32>(keys[0])) * tangent / 4095.0f +
                 static_cast<float>(keys[0]));
    } else if (quarter == 1) {
        float tangent =
            static_cast<float>((keys[3] & 0xfff) - (keys[2] & 0xfff)) * fraction + static_cast<float>(keys[2] & 0xfff);
        value = (static_cast<float>(static_cast<i32>(*next) - static_cast<i32>(keys[0])) * tangent / 4095.0f +
                 static_cast<float>(keys[0]));
    } else if (quarter == 2) {
        u32 start = keys[3] & 0xfff;
        u32 end = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
        float tangent =
            static_cast<float>(static_cast<i32>(end) - static_cast<i32>(start)) * fraction + static_cast<float>(start);
        value = (static_cast<float>(static_cast<i32>(*next) - static_cast<i32>(keys[0])) * tangent / 4095.0f +
                 static_cast<float>(keys[0]));
    } else if (quarter == 3) {
        u32 next_value = *next;
        u32 tangent = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
        float a = static_cast<float>(static_cast<i32>(next_value) - static_cast<i32>(keys[0])) *
                      static_cast<float>(tangent) / 4095.0f +
                  static_cast<float>(keys[0]);
        float b =
            static_cast<float>(static_cast<i32>(reinterpret_cast<u16 *>(data)[stride]) - static_cast<i32>(next_value)) *
                static_cast<float>(next[1] & 0xfff) / 4095.0f +
            static_cast<float>(next_value);
        value = (b - a) * fraction + a;
    } else {
        value = 0.0f;
    }

    return value * scale_min->scale + scale_min->minimum;
}

void ConstantRumble(GameObject_s *, float, float) {
}

void CollectIDUnlocked(i32) {
}

void ClearLastSafeTakeOver(GameObject_s *) {
}

void ClearTakeOverObjectSys() {
}

void CalcValue1648Get2Values(char *, i32, i32, ani3_scalemin_s *, float *, float *) {
}

void GetNativeTextureFormatName(NUTEXFORMAT) {
}

void CatIToX(char *, i32) {
}

void DoInput(WORLDINFO_s *) {
}

void CatI64ToX(char *, i64) {
}

void DieRumble(GameObject_s *) {
}

void charToInt(char const *) {
}

static __used__ i32 _fseek64_wrap(__sFILE *, i64, i32) {
    return 0;
}
