#pragma once

#include "legoapi/gizmo/base/gizmo.h"

extern i32 obstacle_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZOBSTACLE_s GIZOBSTACLE;

using GIZOBSTACLEUPDATEFN = void (*)(GIZOBSTACLE_s *);

extern GIZOBSTACLEUPDATEFN gizobstacleupdatefns[8];
extern NUVEC *gizobstacletriggers[16];
extern i32 ngizobstacletriggers;

void GizObstacle_Stop(GIZOBSTACLE_s *obstacle);
void GizObstacle_PlayForwards(GIZOBSTACLE_s *obstacle);
void GizObstacle_PlayBackwards(GIZOBSTACLE_s *obstacle);

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
