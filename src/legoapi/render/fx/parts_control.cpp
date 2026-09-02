#include "gameapi/edtools/edstubs.h"
#include "legoapi/world/world.h"

void Parts_Stop(WORLDINFO_s *world) {
    if (world->page_part != -1) {
        edpartStopPage(world->page_part);
    }
}

void Parts_Start(WORLDINFO_s *world) {
    if (world->page_part != -1) {
        edpartStartPage(world->page_part);
    }
}
