#pragma once

struct VuVec;
struct GIZTURRET_s;

struct GizTurretObjectInterface {
    GizTurretObjectInterface(GIZTURRET_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void TargetedFlash();
    virtual ~GizTurretObjectInterface();
};
