#pragma once

struct VuVec;
struct GIZFORCE_s;

struct GizForceObjectInterface {
    GizForceObjectInterface(GIZFORCE_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void GetTgtVoidPtr();
    void TargetedFlash();
    virtual ~GizForceObjectInterface();
};
