#include "gamelib_util_types.h"

#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"

TouchHacks::TintStack::TintStack() {
    ambient[0] = NuRndrLightingStateCurrent.ambient.r;
    ambient[1] = NuRndrLightingStateCurrent.ambient.g;
    ambient[2] = NuRndrLightingStateCurrent.ambient.b;
}

TouchHacks::TintStack::~TintStack() {
    NuRndrLightingStateCurrent.ambient.r = ambient[0];
    NuRndrLightingStateCurrent.ambient.g = ambient[1];
    NuRndrLightingStateCurrent.ambient.b = ambient[2];
    NuRndrSetAmbientLightPS(reinterpret_cast<const NUCOLOUR3 *>(ambient));
}

void NetworkObjectManager::NetPeerPush::FlushMessages() {
}

void NetworkObjectManager::NetPeerPush::GetMessage(i32) {
}

void NetworkObjectManager::NetPeerPush::GetReliableMessage(i32) {
}

void NetworkObjectManager::NetPeerPush::NextStage() {
}

void NetworkObjectManager::NetPeerPush::Stop() {
}

void NetworkObjectManager::NetPeerPush::Sync() {
}
