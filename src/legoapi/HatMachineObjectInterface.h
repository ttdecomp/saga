#pragma once

struct VuVec;
struct HATMACHINE_s;

struct HatMachineObjectInterface {
    HatMachineObjectInterface(HATMACHINE_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void TargetedFlash();
    virtual ~HatMachineObjectInterface();
};
