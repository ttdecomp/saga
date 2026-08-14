#pragma once

struct VuVec;
struct GIZOBSTACLE_s;

struct GizObstacleObjectInterface {
    GizObstacleObjectInterface(GIZOBSTACLE_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void IsDead();
    void TargetedFlash();
    virtual ~GizObstacleObjectInterface();
};
