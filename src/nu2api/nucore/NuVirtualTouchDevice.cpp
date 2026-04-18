#include "nu2api/nucore/NuVirtualTouchDevice.h"
#include "decomp.h"
NuVirtualTouchDevice::NuVirtualTouchDevice(u32 unknown) {
    u32 i;

    this->unknown_08.unknown_c8 = 0;

    for (i = 0; i < 10; i++) {
        this->unknown_d4[i].unknown_c8 = 0;
    }

    this->unknown_04 = 0;
}

void NuVirtualTouchDevice::Render() {
    UNIMPLEMENTED();
}

void NuVirtualTouchDevice::Execute(u32 port, NUPADTYPE in_type, NUPADATTACHMENTTYPE in_attch_type, u32 in_caps,
                                   u32 in_buttons, const float *in_analog, const float *in_motion,
                                   const NuInputTouchData *in_touch_data, const NuInputMouseData *in_mouse_data,
                                   NUPADTYPE &out_pad_type, NUPADATTACHMENTTYPE &out_attch_type, u32 &out_caps,
                                   u32 &out_buttons, float *out_analog, float *out_motion,
                                   NuInputTouchData *out_touch_data, NuInputMouseData *out_mouse_data) {
}

void NuVirtualTouchDevice::CreateDefaultLayout(u32 unknown) {
}
