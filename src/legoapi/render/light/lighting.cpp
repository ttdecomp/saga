#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nutex.h"

#include <stdio.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void SetFlicker(GameObject_s *, float) {
}

void ResetLights(nuvec_s *, rtldata_s *, void *) {
}

void SetZeroLights() {
}

rtldata_s lev_rtldata;

extern "C" {
    void rtlApplySetScale(void *, rtldata_s *, NUVEC *, NUMTX *, i32, f32);
    void rtlSetLights(rtldata_s *);
}

void SetLevelLights(void *set, float) {
    rtlApplySetScale(set, &lev_rtldata, reinterpret_cast<NUVEC *>(&global_camera.mtx.m30), NULL, 0x10, 1.0f);
    rtlSetLights(&lev_rtldata);
}

void LightGameObject(GameObject_s *, void *) {
}

void FindAndSetLights(nuvec_s *, float, void *) {
}

void LightSabreDebris(GameObject_s *) {
}

void SetSpotLightMode() {
}

void SetCreatureLights(APIOBJECT_s *) {
}

void SetLights_RTLDATA(rtldata_s *data, float scale) {
    if (scale == 1.0f) {
        rtlSetLights(data);
        return;
    }
    rtldata_s scaled = *data;
    for (i32 offset = 0x78; offset < 0x9c; offset += 4) {
        *reinterpret_cast<f32 *>(scaled.data + offset) *= scale;
    }
    for (i32 offset = 0xc0; offset < 0xcc; offset += 4) {
        *reinterpret_cast<f32 *>(scaled.data + offset) *= scale;
    }
    rtlSetLights(&scaled);
}

void FreeGameObjectLights() {
}

void TurnEpisodeDoorLightsOn(i32) {
}

void LightSabre_ColourFromObj(i32, i32 *) {
}

void SetLights(nucolour3_s *, nuvec_s *, nucolour3_s *, nuvec_s *, nucolour3_s *, nuvec_s *, nuvec_s *) {
}

extern "C" rtlset *rtlLoadSet(char *, VARIPTR *, i32);

void LoadLights(WORLDINFO_s *world, char *path) {
    char filename[268];
    sprintf(filename, "%s.rtl", path);
    world->rtl_set = rtlLoadSet(filename, &world->giz_buffer, world->unknown_0108.addr);
}

extern "C" {

    void IndexLights(rtlset *, VARIPTR *, i32) {
    }

} // extern "C"
