#include "legoapi/character.h"

struct ApiCharacterSystem;

ApiCharacterSystem *apicharsys;
GAMECHARACTERDATA_s GCDATA_DEFAULT;

void *APICharacterLoaded(i32) {
    return NULL;
}

void APILoadCharacterModels(i16 *, i32, void **, void *, i32) {}
