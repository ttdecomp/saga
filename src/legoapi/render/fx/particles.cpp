#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nurndrstat.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern NUGLOBALRNDRSTATE render_state;

void OctreeRndr(unsigned char *, nuoctreenode_s *, i32) {
}

void AddCameraRain(WORLDINFO_s *, i32) {
}

void Particles_Stop(WORLDINFO_s *) {
}

void Particles_Start(WORLDINFO_s *) {
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
