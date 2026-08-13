#pragma once

struct VuVec;
struct TELEPORT_s;

struct TeleportObjectInterface {
    TeleportObjectInterface(TELEPORT_s &, i32);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void TargetedFlash();
    virtual ~TeleportObjectInterface();
};
