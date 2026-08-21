#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"
GIZFORCE_s *GizForce_FindByName(GIZFORCESYS_s *, char *);
struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;
static __used__ void PodRaceSnipersReset() {
}
static __used__ void PodRaceSnipersUpdate() {
}

void NegotiationsA_Init(WORLDINFO_s *world) {
    GIZFORCE_s *f = GizForce_FindByName(world->giz_force_sys, "Force3");
    if (f != NULL)
        f->strength_0x6c = 0.4f;
    f = GizForce_FindByName(world->giz_force_sys, "Force2");
    if (f != NULL)
        f->strength_0x6c = 0.4f;
    f = GizForce_FindByName(world->giz_force_sys, "Force72");
    if (f != NULL)
        f->strength_0x6c = 0.75f;
}

void NegotiationsB_Init(WORLDINFO_s *world) {
    GIZFORCE_s *f = GizForce_FindByName(world->giz_force_sys, "Force10");
    if (f != NULL)
        f->strength_0x6c = 0.5f;
}

void GunganA_Init(WORLDINFO_s *) {
}

void GunganA_Update(WORLDINFO_s *) {
}

void RescueA_Init(WORLDINFO_s *) {
}

void RescueB_Init(WORLDINFO_s *) {
}

void RescueC_Init(WORLDINFO_s *) {
}

void RescueE_Init(WORLDINFO_s *) {
}

void PodRaceInit(WORLDINFO_s *) {
}

void PodRaceADraw(WORLDINFO_s *) {
}

void PodRaceAInit(WORLDINFO_s *) {
}

void PodRaceBInit(WORLDINFO_s *) {
}

void PodRaceCInit(WORLDINFO_s *) {
}

void PodRacePanel(WORLDINFO_s *) {
}

void PodRaceReset() {
}

void PodRaceAReset(WORLDINFO_s *) {
}

void PodRaceBReset(WORLDINFO_s *) {
}

void PodRaceCReset(WORLDINFO_s *) {
}

void PodRaceUpdate(WORLDINFO_s *, float) {
}

void PodRaceAUpdate(WORLDINFO_s *) {
}

void PodRaceBUpdate(WORLDINFO_s *) {
}

void PodRaceCUpdate(WORLDINFO_s *) {
}

void PodRaceAlwasyUpdate(WORLDINFO_s *) {
}

void PodRace_IncreaseLap() {
}

void PodRaceA_AlwaysUpdate(WORLDINFO_s *) {
}

void PodRace_InStartCountdown(WORLDINFO_s *) {
}

void PodLevel(AREADATA_s *) {
}

void PodSprintA_Init(WORLDINFO_s *) {
}

void PodSprintA_Panel(WORLDINFO_s *) {
}

void PodSprintA_Reset(WORLDINFO_s *) {
}

void PodSprintA_Update(WORLDINFO_s *) {
}

void MaulA_Init(WORLDINFO_s *) {
}

void MaulA_Reset(WORLDINFO_s *) {
}

void MaulA_Update(WORLDINFO_s *) {
}

void MaulA_Panel(WORLDINFO_s *) {
}

void MaulB_Init(WORLDINFO_s *) {
}

void MaulD_Init(WORLDINFO_s *) {
}

void MaulD_Update(WORLDINFO_s *) {
}

void MaulE_Init(WORLDINFO_s *) {
}

void MaulE_Update(WORLDINFO_s *) {
}

void MaulF_Init(WORLDINFO_s *) {
}

void MaulF_Reset(WORLDINFO_s *) {
}

void MaulF_Update(WORLDINFO_s *) {
}

void MaulF_Panel(WORLDINFO_s *) {
}
