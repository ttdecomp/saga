#pragma once

struct VuVec;
struct GIZFORCE_s;
struct GIZFORCESYS_s;

struct GizForceObjectInterface {
    GizForceObjectInterface(GIZFORCE_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void GetTgtVoidPtr();
    void TargetedFlash();
    virtual ~GizForceObjectInterface();
};

GIZFORCE_s *GizForce_FindByName(GIZFORCESYS_s *, char *);
GIZFORCE_s *GizForces_FindForce(WORLDINFO_s *, char *);
i32 GizForce_Complete(GIZFORCE_s *);
