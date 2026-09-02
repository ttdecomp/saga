#include "nu2api/nucore/nuapi.h"

extern "C" {

    i32 NuLanguageGet(void) {
        return nuapi.language;
    }

    void NuLanguageSet(i32 language) {
        nuapi.language = language;
    }

} // extern "C"
