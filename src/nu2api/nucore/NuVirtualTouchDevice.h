#include "nu2api/nucore/NuInputDevice.h"

struct NuButtonLayout {
    // Field composition uncertain.
    u8 unknown_00[0xc8];

    u32 unknown_c8;
};

class NuVirtualTouchDevice : public NuInputDeviceTranslator {
  public:
    NuVirtualTouchDevice(u32 unknown);
    void Render();
    virtual void Execute(u32 port, NUPADTYPE in_type, NUPADATTACHMENTTYPE in_attch_type, u32 in_caps, u32 in_buttons,
                         const float *in_analog, const float *in_motion, const NuInputTouchData *in_touch_data,
                         const NuInputMouseData *in_mouse_data, NUPADTYPE &out_pad_type,
                         NUPADATTACHMENTTYPE &out_attch_type, u32 &out_caps, u32 &out_buttons, float *out_analog,
                         float *out_motion, NuInputTouchData *out_touch_data,
                         NuInputMouseData *out_mouse_data) override;

    void CreateDefaultLayout(u32 unknown);

  private:
    // Type uncertain.
    u32 unknown_04;

    NuButtonLayout unknown_08;
    NuButtonLayout unknown_d4[10];
};
