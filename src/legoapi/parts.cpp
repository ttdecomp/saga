#include "legoapi_types.h"

// Forward declarations for local (static) part/gizmo helper stubs.
struct CUSTOMPIECEANIM;
struct spacelevel_s;
struct quickboltinfo;

void PartObjectInterface::GetPos(VuVec &, i32) const {
}

void PartObjectInterface::GetRadius() const {
}

void PartObjectInterface::GetTargetName() const {
}

PartObjectInterface::PartObjectInterface(PART_s &) {
}

PartObjectInterface::~PartObjectInterface() {
}

void PART_s::ClearMechObjectInterface() {
}

void PART_s::GetMechObjectInterface() {
}

// Local (static) part (PART_s), pickup (GIZMOPICKUP_s), gizmo flow
// (GIZFLOW_s/FLOWBOX_s), power-up and space/starfighter helpers. Stubbed as
// local `t` symbols matching res/libTTapp.so.

static __used__ void PartCollide(PART_s *, i32) {
}

static __used__ void TiePart_Kill(PART_s *, i32) {
}

static __used__ void TiePart_Move(PART_s *, f32) {
}

static __used__ void TiePart_Impact(PART_s *) {
}

static __used__ void TiePart_KillExplode(PART_s *, i32) {
}

static __used__ void TieSpinZPart_Move(PART_s *, f32) {
}

static __used__ void PartImpact_Coin(PART_s *) {
}

static __used__ void PartStolen_Coin(PART_s *) {
}

static __used__ void PartExtra_BlueCoin(PART_s *) {
}

static __used__ void PartExtra_PurpleCoin(PART_s *) {
}

static __used__ void PowerUp_DrawPart(PART_s *) {
}

static __used__ void PowerUp_ImpactPart(PART_s *) {
}

static __used__ void PowerUp_UpdatePart(PART_s *) {
}

static __used__ void PowerUp_EndMsg(GAMEMESSAGE_s *) {
}

static __used__ void PowerUp_UpdateMsg(GAMEMESSAGE_s *) {
}

static __used__ void SpeederPart_Draw(PART_s *) {
}

static __used__ void SpeederPart_Kill(PART_s *, i32) {
}

static __used__ void SpeederPart_Update(PART_s *) {
}

static __used__ void SuperCarry_PartKill(PART_s *, i32) {
}

static __used__ void SuperCarry_PartImpact(PART_s *) {
}

static __used__ void SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_s *, nuvec_s *, u16, i32) {
}

static __used__ void PartDraw_VehicleHeart(PART_s *) {
}

static __used__ void PartKill_DrawCreature(PART_s *) {
}

static __used__ void PartMove_VehicleHeart(PART_s *, f32) {
}

static __used__ void PartMove_VehiclePickup(PART_s *, f32) {
}

static __used__ void PartKill_EjectedCreature(PART_s *, i32) {
}

static __used__ void UpdateAnimTimer(CHARACTERMODEL_s *, ANIMPACKET_s *, i16, f32, f32, f32, i32, char *, i32, f32) {
}

static __used__ void UpdateCustomPieceAnim(CUSTOMPIECEANIM *, u16, u16) {
}




















extern "C" {

    void AddDebrisEffect(void) {
    }

    void AddFiniteShotDebrisEffect(void) {
    }

    void AddFiniteShotDebrisEffect2(void) {
    }

    void AddFiniteShotDebrisEffectUserData(void) {
    }

    void AddFiniteShotPART(void) {
    }

    void AddGameDebris(void) {
    }

    void AddGameDebrisMom(void) {
    }

    void AddGameDebrisMomentum(void) {
    }

    void AddGameDebrisMtx(void) {
    }

    void AddGameDebrisRot(void) {
    }

    void AddGameDebrisXYZ(void) {
    }

    void AddMSituExtraTerrRot(void) {
    }

    void AddPARTEffect(void) {
    }

    void AddPart(void) {
    }

    void AddPickupTerr(void) {
    }

    void AddPickupTerrRot(void) {
    }

    void AddRotatedDebrisEffect(void) {
    }

    void AddScaledFiniteShotDebrisEffect(void) {
    }

    void AddScaledFiniteShotPART(void) {
    }

    void AddScaledVariableShotDebrisEffect(void) {
    }

    void AddScaledVariableShotDebrisEffect1(void) {
    }

    void AddScaledVariableShotDebrisEffect2(void) {
    }

    void AddScaledVariableShotDebrisEffect3(void) {
    }

    void AddScaledVariableShotDebrisEffect4(void) {
    }

    void AddScaledVariableShotDebrisEffect5(void) {
    }

    void AddScaledVariableShotPARTEffect(void) {
    }

    void AddVariableShotDebrisEffect(void) {
    }

    void AddVariableShotDebrisEffectMtx(void) {
    }

    void AddVariableShotDebrisEffectMtx3(void) {
    }

    void AddVariableShotDebrisEffectMtx4(void) {
    }

    void AddVariableShotDebrisEffectTimed1(void) {
    }

    void AddVariableShotDebrisEffectTimed3(void) {
    }

    void AddVariableShotDebrisEffectTimed5(void) {
    }

    void AddVariableShotPARTEffect(void) {
    }

    void CastPart(void) {
    }

    void CheckPartCount(void) {
    }

    void DrawParts(void) {
    }

    void FindPart(void) {
    }

    void GetMaxPartTypes(void) {
    }

    void GetPartCount(void) {
    }

    void GetPartName(void) {
    }

    void InitParts(void) {
    }

    void KillAllParts(void) {
    }

    void KillPart(void) {
    }

    void KillPartsByScene(void) {
    }

    void NewPartRotation(void) {
    }

    void PARTEmitterOrientation(void) {
    }

    void PARTEmitterPos(void) {
    }

    void PARTGetTotalOffTime(void) {
    }

    void PARTGetTotalOnTime(void) {
    }

    void PARTLookupType(void) {
    }

    void PARTLookupTypePageOnly(void) {
    }

    void PARTStartOffset(void) {
    }

    void PartPlatId(void) {
    }

    void ParticleReset(void) {
    }

    void ParticlesPerFrame(void) {
    }

    void ParticlesPerSecond(void) {
    }

    void ReassignPickupInst(void) {
    }

    void RemovePARTEffect(void) {
    }

    void ResetParts(void) {
    }

    void UpdateParts(void) {
    }

} // extern "C"

void Parts_Stop(WORLDINFO_s *) {
}

void TargetPart(GameObject_s *, nuvec_s *, nuvec_s *, float, float, i32, i32) {
}

void Parts_Start(WORLDINFO_s *) {
}

void PartTimeSlip() {
}

void InitPartTable(char **) {
}

void SetPartTarget(GameObject_s *, PART_s *) {
}

void AddHeartAsPart(GameObject_s *, nuvec_s *, nuvec_s *, float, float) {
}

void FindPartDebris(PARTDEBSYS_s *, char *) {
}

void MakePartVector(nuvec_s *, nuvec_s *, float) {
}

void PartCollide_2D(PART_s *) {
}

void PartCollide_3D(PART_s *) {
}

void SetKillPartMom(nuvec_s *) {
}

void AddCoinsAsParts(i32, nuvec_s *, nuvec_s *, float, float) {
}

void UpdatePartEmits(float) {
}

void FindIncomingPart(void *, nuvec_s *, float, u32, float) {
}

void InstantKillParts(GameObject_s *, i32, float) {
}

void PartCleanupTypes() {
}

void PartImpact_Brick(PART_s *) {
}

void PartUpdate_Heart(PART_s *) {
}

void Asteroid_PartKill(PART_s *, i32) {
}

void PartDraw_Flickerer(PART_s *) {
}

void PartStop_Flickerer(PART_s *) {
}

void PartKill_ForceThrow(PART_s *, i32) {
}

void PartImpact_Basketball(PART_s *) {
}

void PartUpdate_Basketball(PART_s *) {
}

void Part_FindFromHSpecial(nuhspecial_s *) {
}

void NewPartOrderedRotation(PART_s *) {
}

void KillParts(GameObject_s *, i32, i32, i32, float, i32, u16 *) {
}
