#pragma once

struct GameObject_s;
struct VuVec;

struct CharacterObjectInterface {
    CharacterObjectInterface(GameObject_s &);
    void GetHeight() const;
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void IsDead();
    void TargetedFlash();
    virtual ~CharacterObjectInterface();
};
