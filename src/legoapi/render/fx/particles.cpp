#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "gameapi/edtools/edstubs.h"
#include "legoapi/world/world.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern NUGLOBALRNDRSTATE render_state;
extern "C" {
    extern i32 DEBPAGE_AREA;
    extern i32 DEBPAGE_CHARACTER;
    extern i32 DEBPAGE_GENERAL;
    void DebFreeAllCreatedEffects(void);
    void DebrisSetRenderGroup(i32 group);
}

void OctreeRndr(unsigned char *, nuoctreenode_s *, i32) {
}

void AddCameraRain(WORLDINFO_s *, i32) {
}

void Particles_Stop(WORLDINFO_s *world) {
    if (world->page_anim != -1) {
        edanimStopPage(world->page_anim);
    }
    if (world->page_pp != -1) {
        edppStopPage((i8)world->page_pp);
    }
    if (DEBPAGE_CHARACTER != -1) {
        edppStopPage((i8)DEBPAGE_CHARACTER);
    }
    if (DEBPAGE_GENERAL != -1) {
        edppStopPage((i8)DEBPAGE_GENERAL);
    }
    if (DEBPAGE_AREA != -1) {
        edppStopPage((i8)DEBPAGE_AREA);
    }
    DebFreeAllCreatedEffects();
}

void Particles_Start(WORLDINFO_s *world) {
    if (DEBPAGE_AREA != -1) {
        edppStartPage((i8)DEBPAGE_AREA);
    }
    if (world->page_pp != -1) {
        edppStartPage((i8)world->page_pp);
    }
    DebrisSetRenderGroup(1);
    if (world->page_anim >= 0) {
        edanimStartPage(world->page_anim);
    }
}

void Particles_DumpAreaPage() {
}

void Particles_LoadAreaPage(char *) {
}

void AddParticleGroupToDisplayList(nunativedebrisdata_s *group) {
    NUDISPLAYLIST *list = group->material->display_list;
    if (list == NULL) {
        return;
    }

    NUDLDLISTSCENE *display_scene = list->dlist;
    display_scene->flags |= NUDL_SCENE_FLAG_CLIP_MATERIALS;
    const i32 material_byte = list->mtl_id >= 0 ? list->mtl_id / 8 : (list->mtl_id + 7) / 8;
    u8 *material_bits = display_scene->mtl_used[display_scene->render_buffer >> 7];
    material_bits[material_byte] |= static_cast<u8>(1 << (list->mtl_id & 7));

    DisplayListUpdateRenderState(list, &render_state);
    NuDisplayListLinkItems(list, 1);
    NUDISPLAYLISTITEM *item = list->items;
    item->type = 0xa7;
    item->id = 3;
    item->next = group;
    list->items++;
}

void PodDust(WORLDINFO_s *, GameObject_s *) {
}

void NoRender() {
}
