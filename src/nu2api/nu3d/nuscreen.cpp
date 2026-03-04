#include "nuscreen.hpp"

#include <cstddef>
#include <cstdlib>

NuScreen *NuScreen::ms_instance = NULL;

bool NuScreen::Exists() {
    return ms_instance != NULL;
}

void NuScreen::Create() {
    if (ms_instance != NULL) {
        return;
    }

    ms_instance = (NuScreen *)malloc(sizeof(NuScreen));
}

void NuScreen::SetSceeenDimensions(f32 width, f32 height) {
    this->width = width;
    this->height = height;
}
