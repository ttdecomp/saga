#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Door_Reset() {
}

void Doors_Check(WORLDINFO_s *, GameObject_s *) {
}

void Door_GoThrough(WORLDINFO_s *, DOOR_s *, i32) {
}

void Door_SetCutCam(DOOR_s *) {
}

void Doors_SetLastDoor(DOOR_s *) {
}

void Doors_Init(WORLDINFO_s *world) {
    (void)world;
}

void Doors_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}

void PortalDoors_Update(WORLDINFO_s *) {
}
