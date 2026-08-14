#include "decomp.h"
#include "legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void SkinPlatform(terrsitu_s *, unsigned char *, PLATSKININFO *) {
}

void Platform_Init(WORLDINFO_s *) {
}

void PlatformChecks(i32, nuvec_s *) {
}

void Platform_Reset(WORLDINFO_s *) {
}

void PlatformConnect(char *, nuvec_s *, nuvec_s *, i32) {
}

void SkinPlatformSize(i32, unsigned char *, PLATSKININFO *) {
}

void CharPlatforms_Reset(CHARPLATFORMSYS_s *) {
}

void CharPlatforms_Update(CHARPLATFORMSYS_s *) {
}

void FindReflectionNoPlatforms(nuvec_s *) {
}

void CharPlatform_FindObjFromPlatID(CHARPLATFORMSYS_s *, i32) {
}

void CharPlatforms_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}

