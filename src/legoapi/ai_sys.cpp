#include "legoapi/world_shared.h"

extern "C" void *AISysLoad(void *buf, void *buf_end, i32 size, void *gscn, char *dir, char *name, char *param) {
    (void)buf;
    (void)buf_end;
    (void)size;
    (void)gscn;
    (void)dir;
    (void)name;
    (void)param;
    return NULL;
}
void *AIPathCnxControlSysCreate(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AIPathCnxHelperSysCreate(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AITriggerSetSysCreate(VARIPTR *buf, VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void GameAIScriptAddLevelSfx(WORLDINFO *world, NULISTHDR *scripts) {
    (void)world;
    (void)scripts;
}
void *CreateClimbObjectSys(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
extern "C" void *APIObjectSysInit(i32 size, void *buf, void *buf_end) {
    (void)size;
    (void)buf;
    (void)buf_end;
    return NULL;
}
