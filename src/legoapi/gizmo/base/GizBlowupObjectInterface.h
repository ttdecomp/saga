#pragma once

struct VuVec;
struct GIZMOBLOWUP_s;
struct WORLDINFO_s;

struct GizBlowupObjectInterface {
    GizBlowupObjectInterface(GIZMOBLOWUP_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void IsDead();
    void TargetedFlash();
    virtual ~GizBlowupObjectInterface();
};

GIZMOBLOWUP_s *GizmoBlowUp_FindByName(WORLDINFO_s *, char *);
void UpdateMidPos(GIZMOBLOWUP_s *);
