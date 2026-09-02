#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" {
    extern i16 id_BATMAN;
    extern i16 id_ROBIN;
}

struct SPECIAL_LAYER_s {
    i16 *character_id;
    char *name;
    u32 mask;
};

DECOMP_ASSERT(sizeof(SPECIAL_LAYER_s) == 0xc, "SPECIAL_LAYER_s size");

static SPECIAL_LAYER_s SpecialLayer[] = {
    {&id_BATMAN, "bombbackpack", 0},       {&id_BATMAN, "sonargun", 0},          {&id_BATMAN, "infrared_goggles", 0},
    {&id_BATMAN, "mask_black", 0},         {&id_BATMAN, "mask_blue", 0},         {&id_BATMAN, "mask_red", 0},
    {&id_BATMAN, "cape_black", 0},         {&id_BATMAN, "cape_blue", 0},         {&id_BATMAN, "body_grey_nextgen", 0},
    {&id_BATMAN, "body_grey_high", 0},     {&id_BATMAN, "body_grey_low", 0},     {&id_BATMAN, "body_black_nextgen", 0},
    {&id_BATMAN, "body_black_high", 0},    {&id_BATMAN, "body_black_low", 0},    {&id_BATMAN, "body_blue_nextgen", 0},
    {&id_BATMAN, "body_blue_high", 0},     {&id_BATMAN, "body_blue_low", 0},     {&id_BATMAN, "face_hands_black", 0},
    {&id_BATMAN, "face_hands_blue", 0},    {&id_BATMAN, "face_hands_red", 0},    {&id_BATMAN, "hips_black_nextgen", 0},
    {&id_BATMAN, "hips_black_high", 0},    {&id_BATMAN, "hips_blue_nextgen", 0}, {&id_BATMAN, "hips_blue_high", 0},
    {&id_BATMAN, "hips_red_nextgen", 0},   {&id_BATMAN, "hips_red_high", 0},     {&id_ROBIN, "magnetic_boots", 0},
    {&id_ROBIN, "scuba_gear", 0},          {&id_ROBIN, "hack_pack", 0},          {&id_ROBIN, "vacuum_gun", 0},
    {&id_ROBIN, "nextgen_limbs_green", 0}, {&id_ROBIN, "hires_limbs_green", 0},  {&id_ROBIN, "lowres_limbs_green", 0},
    {&id_ROBIN, "nextgen_limbs_white", 0}, {&id_ROBIN, "hires_limbs_white", 0},  {&id_ROBIN, "lowres_limbs_white", 0},
    {&id_ROBIN, "nextgen_limbs_grey", 0},  {&id_ROBIN, "hires_limbs_grey", 0},   {&id_ROBIN, "lowres_limbs_grey", 0},
    {&id_ROBIN, "nextgen_limbs_blue", 0},  {&id_ROBIN, "hires_limbs_blue", 0},   {&id_ROBIN, "lowres_limbs_blue", 0},
};

i32 LayerFromName(GAMECHARACTERDATA_s *character, char *name);

void FixUpLayers() {
    for (i32 model_index = 0; model_index < apicharsys->loaded_model_count; ++model_index) {
        CHARACTERMODEL_s *model = &apicharsys->models[model_index];
        GAMECHARACTERDATA_s *character = &GCDataList[model->model_id];

        for (i32 layer_index = 0; layer_index < character->layer_count; ++layer_index) {
            GAMECHARACTERLAYER_s *layer = &character->layers[layer_index];
            layer->hierarchy_layer_index = NuHGobjGetLayerIndex(layer->name, model->hierarchy);
        }
    }

    SPECIAL_LAYER_s *layer = SpecialLayer;
    SPECIAL_LAYER_s *layer_end = SpecialLayer + sizeof(SpecialLayer) / sizeof(SpecialLayer[0]);
    for (; layer != layer_end; ++layer) {
        layer->mask = 0;
        if (layer->character_id != NULL && *layer->character_id != -1) {
            layer->mask = 1 << LayerFromName(&GCDataList[*layer->character_id], layer->name);
        }
    }
}

void ResetGizFlow(GIZFLOW_s *, GIZFLOWPROGRESS_s *) {
}

i32 LayerFromName(GAMECHARACTERDATA_s *character, char *name) {
    for (i32 i = 0; i < character->layer_count; ++i) {
        if (NuStrICmp(name, character->layers[i].name) == 0) {
            return character->layers[i].mask_bit;
        }
    }
    return -1;
}

void ProcessGizFlow(GIZFLOW_s *, float) {
}

void FlowBoxFindByName(GIZFLOW_s *, char *) {
}

void SetGizFlowVisible(GIZFLOW_s *) {
}

void GizFlowStoreProgress(GIZFLOW_s *, GIZFLOWPROGRESS_s *) {
}

void GizmoTypeGetProgress(GIZMOSYS_s *, void *, i32, i32, char *, void **) {
}

void PerformActionFlowBox(GIZFLOW_s *, FLOWBOX_s *) {
}

void ResetGizFlowPointers(GIZFLOW_s *) {
}

void DynamicAddGizmoToFlow(GIZFLOW_s *, GIZMO_s *) {
}

void GizmoSysStoreProgress(GIZMOSYS_s *, void *, i32) {
}

void GizmoTypeStoreProgress(GIZMOSYS_s *, void *, i32, i32, char *) {
}
