#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

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

void SetLevelLights(void *, float) {
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

void SetLights_RTLDATA(rtldata_s *, float) {
}

void FreeGameObjectLights() {
}

void TurnEpisodeDoorLightsOn(i32) {
}

void LightSabre_ColourFromObj(i32, i32 *) {
}

void SetLights(nucolour3_s *, nuvec_s *, nucolour3_s *, nuvec_s *, nucolour3_s *, nuvec_s *, nuvec_s *) {
}

void LoadLights(WORLDINFO_s *world, char *path) {
    (void)world;
    (void)path;
}

extern "C" {

        void IndexLights(void) {
        }

} // extern "C"

