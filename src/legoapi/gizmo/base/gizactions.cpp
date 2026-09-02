#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

GIZACTIONDEFN_s *gizactiondefs;

void RegisterGizActions(GIZACTIONDEFN_s *definitions) {
    gizactiondefs = definitions;
}

extern "C" {

    extern ACTIONINFO_s *APIActionInfo;
    extern EXTRAACTIONDATA_s *APIExtraActionData;

    i32 ActionFromName(const char *name) {
        if (apicharsys == NULL || name == NULL) {
            return -1;
        }

        if (APIActionInfo != NULL) {
            for (i32 action = 0; action < apicharsys->model_id_capacity; ++action) {
                if (NuStrICmp(APIActionInfo[action].name, name) == 0) {
                    return action;
                }
            }
        }

        if (APIExtraActionData != NULL) {
            for (EXTRAACTIONDATA_s *extra = APIExtraActionData; extra->name != NULL; ++extra) {
                if (NuStrICmp(extra->name, name) == 0) {
                    return extra->action;
                }
            }
        }
        return -1;
    }

    u32 ActionInfoFlags(i32 action) {
        return APIActionInfo != NULL ? APIActionInfo[action].flags : 0;
    }

    const char *ActionInfoName(i32 action) {
        return APIActionInfo != NULL ? APIActionInfo[action].name : "?";
    }

} // extern "C"
