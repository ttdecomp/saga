#include "nu2api/nucore/NuInputDevice.h"
#include "nu2api/nucore/NuInputManager.h"
#include "nu2api/nucore/NuTouchInputButton.h"
#include "nu2api/nucore/NuTouchInputElement.h"
#include "nu2api/nucore/NuTouchInputStick.h"
#include "nu2api/nucore/NuVirtualTouchDevice.h"
#include "nu2api/nucore/nupad.h"

NuInputManager *inputManager;
NuVirtualTouchDevice *inputTouchDevice;
bool used_touch_IDs[10];

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

extern "C" void NuPad_Interface_ResetAllTouches(void) {
    NuInputDevice *device = inputManager->GetDevice(0);

    memset(device->touch_data.touch_events, 0, sizeof(device->touch_data.touch_events));
    device->touch_data.touch_count = 0;
    memset(used_touch_IDs, 0, sizeof(used_touch_IDs));
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

void NuPadReadPS(i32, unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *,
                 unsigned char *, unsigned char *, unsigned char *, u32 *, unsigned char *, u32 *) {
}

void NuPadGetPort(i32) {
}

void NuPadGetPlayer(i32) {
}

void NuPadRecordLoad(char *, variptr_u *, variptr_u) {
}

void NuPadGetDeadzonePS(nupad_s *) {
}

void NuPadRecordEndFrame() {
}

void NuPadMapPortToPS2Port(i32, i32) {
}

void NuPadGetFirstActivePad() {
}

void NuPadRecordSetPlayEndButtons(i32) {
}

void NuPadRecordSetRecordEndButtons(i32) {
}

void NuInputDevice::DisableDPD() {
}

void NuInputDevice::EnableDPD() {
}

void NuInputDevice::GetAttachmentType() const {
}

void NuInputDevice::GetCaps() const {
}

void NuInputDevice::GetIndexByType() const {
}

void NuInputDevice::GetLastValidIndexByType() const {
}

void NuInputDevice::GetLastValidType() const {
}

void NuInputDevice::GetMotionValue(NUPADMOTIONVALUE) const {
}

void NuInputDevice::GetMouseData() const {
}

void NuInputDevice::GetPort() const {
}

void NuInputDevice::GetTouchData() const {
}

void NuInputDevice::GetVolume() const {
}

void NuInputDevice::HasHeadphonesConnected() const {
}

void NuInputDevice::IsButtonPressed(u32) const {
}

void NuInputDevice::IsIntercepted() const {
}

void NuInputDevice::KillRumble() {
}

void NuInputDevice::ProcessTouchData() {
}

void NuInputDevice::SetMotors(float, float) {
}

void NuInputDevice::SupportsCaps(u32) const {
}

NuInputDevice::~NuInputDevice() {
}

void NuButtonLayout::ActivateLayout() {
}

void NuButtonLayout::DeactivateLayout() {
}

void NuButtonLayout::Render() {
}

void NuButtonLayout::Update(NuInputTouchData const *) {
}

void NuButtonLayout::UpdateButtons(i32) {
}

NuButtonLayout::~NuButtonLayout() {
}

void NuInputManager::GetDevice(u32) const {
}

void NuInputManager::GetFirstDeviceByType(NUPADTYPE) const {
}

void NuInputManager::KillRumbleAll() {
}

void NuInputDevicePS::DisableDPDPS(u32) {
}

void NuInputDevicePS::EnableDPDPS(u32) {
}

void NuInputDevicePS::GetIdentifierPS(u32) {
}

void NuInputDevicePS::HandleGamePadAxis_ANDROID_SPECIFIC(float, float, float, float, float, float) {
}

void NuInputDevicePS::HandleSensor_ANDROID_SPECIFIC(i32, float, float, float) {
}

void NuInputDevicePS::HandleTouch_ANDROID_SPECIFIC(i32, i32, i32, float, float) {
}

NuTouchInputStick::NuTouchInputStick(NuTouchInputElement::TYPE, i32, u32, float, float, float, float) {
}

void NuTouchInputStick::Render() {
}

void NuTouchInputStick::Update(NuInputTouchData const *) {
}

NuTouchInputButton::NuTouchInputButton(i32, u32) {
}

NuTouchInputButton::NuTouchInputButton(i32, u32, float, float, float, float) {
}

void NuTouchInputButton::Render() {
}

void NuTouchInputButton::Update(NuInputTouchData const *) {
}

NuTouchInputElement::NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32) {
}

NuTouchInputElement::NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32, float, float, float, float) {
}

__attribute__((weak)) NuTouchInputElement::~NuTouchInputElement() {
}

__attribute__((weak)) void NuTouchInputElement::UpdateButtons(i32) {
}

__attribute__((weak)) bool NuTouchInputElement::IsPressed() const {
    return false;
}

__attribute__((weak)) float NuTouchInputElement::GetStickX() const {
    return 0.0f;
}

__attribute__((weak)) float NuTouchInputElement::GetStickY() const {
    return 0.0f;
}

__attribute__((weak)) void NuTouchInputElement::Deactivate() {
}

__attribute__((weak)) void NuTouchInputElement::Activate() {
}
