#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"

void Teleport_Find(GameObject_s *, float, VuVec *) {
}

void Teleports_Reset(WORLDINFO_s *world) {
    if (world->teleports == NULL || WORLD->teleport_count <= 0) {
        return;
    }

    TELEPORT_s *teleport = world->teleports;
    for (i32 i = 0; i < WORLD->teleport_count; ++i, ++teleport) {
        teleport->active = 0;
        teleport->enabled = 1;
        teleport->field_78 = 0;
        teleport->field_74 = 0;
        teleport->field_7a = 0;
        teleport->field_76 = 0;
    }
}

void Teleport_MoveCode(GameObject_s *, i32) {
}

void Teleport_NetMoveCode(GameObject_s *) {
}

void Teleport_UpdateHints(HINT_s *) {
}

void Teleports_UpdateAfterGameObjects(WORLDINFO_s *) {
}

void Teleports_UpdateBeforeGameObjects(WORLDINFO_s *) {
}

void TELEPORT_s::ClearMechObjectInterface() {
}

void TELEPORT_s::GetMechObjectInterface() {
}
