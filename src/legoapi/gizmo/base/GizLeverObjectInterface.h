#pragma once

struct VuVec;
struct LEVER_s;

struct GizLeverObjectInterface {
    GizLeverObjectInterface(LEVER_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void TargetedFlash();
    virtual ~GizLeverObjectInterface();
};
