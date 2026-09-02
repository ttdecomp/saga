#include "MechInputTouch_types.h"

void MechAutoJumpManager::AddAutoJumpConnection(AIPATH_s *, AIPATHCNX_s *, i32, bool, i32, bool) {
}

void MechAutoJumpManager::DeleteJumpConnection(MechAutoJumpConnection *) {
}

void MechAutoJumpManager::DeleteJumpConnectionsAndStreaks() {
}

void MechAutoJumpManager::Init() {
}

MechAutoJumpManager::MechAutoJumpManager(AISYS_s *ai_system) {
    if (ai_system != nullptr) {
        streak_time = 0.0f;
        ai_sys = ai_system;
        streaks.head = nullptr;
        streaks.tail = nullptr;
        jump_connections.head = nullptr;
        jump_connections.tail = nullptr;
    }
}

void MechAutoJumpManager::PreProcessJumpConnections() {
}

void MechAutoJumpManager::Process() {
}

void MechAutoJumpManager::ProcessJumpConnections() {
}

void MechAutoJumpManager::Render() {
}

MechAutoJumpManager::~MechAutoJumpManager() {
}
