#pragma once

struct VuVec;
struct GIZBUILDIT_s;

struct GizBuildItObjectInterface {
    GizBuildItObjectInterface(GIZBUILDIT_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void TargetedFlash();
    virtual ~GizBuildItObjectInterface();
};
