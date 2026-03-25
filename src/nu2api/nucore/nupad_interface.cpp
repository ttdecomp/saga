#include "nu2api/nucore/NuInputDevice.h"
#include "nu2api/nucore/NuInputManager.h"
#include "nu2api/nucore/NuVirtualTouchDevice.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nu3d/nurndr.h"

NuInputManager *inputManager;
NuVirtualTouchDevice *inputTouchDevice;
bool used_touch_IDs[10];


void NuPad_Interface_Render(void) {
  NuRndrBeginScene();
  if (inputTouchDevice != (NuVirtualTouchDevice *)0x0) {
    inputTouchDevice->Render();
  }
  NuRndrEndScene();
  return;
}




void NuPad_Interface_InputManagerInitialise(void) {
    NuInputDevice *touch_dev;
    u32 i;

    inputManager = new NuInputManager();
    inputManager->UpdateAll(0.0f);

    touch_dev = inputManager->GetFirstDeviceByType(NUPADTYPE_TOUCH);
    if (touch_dev != NULL) {
        inputTouchDevice = new NuVirtualTouchDevice(4);
        inputTouchDevice->CreateDefaultLayout(0);

        touch_dev->AddTranslator(inputTouchDevice);
    }

    for (i = 0; i < 10; i++) {
        used_touch_IDs[i] = false;
    }
}

void NuPad_Interface_InputManagerUpdate(f32 delta_time) {
    inputManager->UpdateAll(delta_time);
}

u32 NuPad_Interface_NuPadRead(i32 port, u8 *analog_left_x, u8 *analog_left_y, u8 *analog_right_x, u8 *analog_right_y,
                              u8 *analog_l1, u8 *analog_l2, u8 *analog_r1, u8 *analog_r2, u32 *digital_buttons,
                              u8 *unknown, i32 *analog_button_flags) {
    NuInputDevice *device;

    device = inputManager->GetDevice(port);

    if (device == NULL) {
        return 0;
    }

    *analog_left_x = (device->GetAnalogValue(NUPADANALOGVALUE_LEFT_X) + 1.0f) * 127.0f;
    *analog_left_y = (device->GetAnalogValue(NUPADANALOGVALUE_LEFT_Y) + 1.0f) * 127.0f;
    *analog_right_x = (device->GetAnalogValue(NUPADANALOGVALUE_RIGHT_X) + 1.0f) * 127.0f;
    *analog_right_y = (device->GetAnalogValue(NUPADANALOGVALUE_RIGHT_Y) + 1.0f) * 127.0f;

    *analog_l1 = device->GetAnalogValue(NUPADANALOGVALUE_L1) * 127.0f;
    *analog_l2 = device->GetAnalogValue(NUPADANALOGVALUE_L2) * 127.0f;
    *analog_r1 = device->GetAnalogValue(NUPADANALOGVALUE_R1) * 127.0f;
    *analog_r2 = device->GetAnalogValue(NUPADANALOGVALUE_R2) * 127.0f;

    *digital_buttons = device->GetButtons();

    return device->IsConnected();
}

i32 NuPad_Interface_GetMaxDevices(void) {
    return inputManager->GetMaxDevices();
}

void NuPad_UpdateTouchScreenData(void) {
}
