#pragma once

struct VuVec;
struct GIZOBSTACLE_s;
struct GIZOBSTACLESYS_s;

struct GizObstacleObjectInterface {
    GizObstacleObjectInterface(GIZOBSTACLE_s &);
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    void IsDead();
    void TargetedFlash();
    virtual ~GizObstacleObjectInterface();
};

GIZOBSTACLE_s *GizObstacle_FindByName(GIZOBSTACLESYS_s *, char *);
void GizObstacle_EvalAveragePosAndRadius(GIZOBSTACLE_s *obstacle, i32 unknown);
