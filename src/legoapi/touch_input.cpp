#include "legoapi/touchhacks.h"

namespace TouchHacks {
bool TouchControlsActive;
}

i32 InCollectList_Index(i32 id, i16 *, i32) {
    return id;
}
