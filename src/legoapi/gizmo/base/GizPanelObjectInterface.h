#pragma once

struct VuVec;
struct GIZPANEL_s;

struct GizPanelObjectInterface {
    GizPanelObjectInterface(GIZPANEL_s &);
    void GetFloorTargetPos(VuVec &, i32) const;
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void TargetedFlash();
    virtual ~GizPanelObjectInterface();
};
