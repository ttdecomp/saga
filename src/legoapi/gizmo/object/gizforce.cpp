#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void GetZapOrigin(GameObject_s *) {
}

void ReleaseForce(GameObject_s *, i32) {
}

void SetForceBack(GameObject_s *, nuvec_s *, float, i32) {
}

void ResetForceBack() {
    ForceBackObj = NULL;
    ForceBackPos = NULL;
}

void ResetForceGlow(PLAYERPACKET_s *packet) {
    packet->force_glow_x = 0.0f;
    packet->force_glow_y = 0.0f;
    packet->force_glow_z = 0.0f;
    packet->force_glow_mode = 0;
    packet->force_glow_state = 0;
    packet->force_glow_intensity = 0.2f;
}

void ForceLightning_Origin(GameObject_s *, nuvec_s *, nuvec_s *) {
}

void EndForce(GameObject_s *, i32) {
}

void GizForceSFX_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
