#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nufile/nufpar.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void PortalGameObject(GameObject_s *, i32, i32, i16, nugscn_s *) {
}

void PortalDoors_Configure(WORLDINFO_s *world, char *config) {
    world->portal_doors = NULL;
    world->portal_door_count = 0;
    if (world->current_gscn == NULL) {
        return;
    }

    NUFPAR *parser = NuFParCreateMem(const_cast<char *>("portaldoors"), config, 0xffff);
    if (parser == NULL) {
        return;
    }

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
    PORTALDOOR *portal_door = static_cast<PORTALDOOR *>(world->giz_buffer.void_ptr);
    world->portal_doors = portal_door;

    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0 || NuStrICmp(parser->word_buf, const_cast<char *>("portaldoor")) != 0) {
            continue;
        }

        *portal_door = {};
        if (NuFParGetWord(parser) == 0 ||
            NuSpecialFind(world->current_gscn, reinterpret_cast<void **>(portal_door), parser->word_buf, 1) == 0) {
            continue;
        }

        portal_door->portal_id = static_cast<u8>(NuFParGetInt(parser));
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buf, const_cast<char *>("trigger_at_end")) == 0) {
                portal_door->flags |= PORTALDOOR_TRIGGER_AT_END;
            }
        }

        ++portal_door;
        ++world->portal_door_count;
    }

    NuFParDestroy(parser);
    if (world->portal_door_count == 0) {
        world->portal_doors = NULL;
        return;
    }
    world->giz_buffer.addr = ALIGN(reinterpret_cast<usize>(portal_door), 16);
}
