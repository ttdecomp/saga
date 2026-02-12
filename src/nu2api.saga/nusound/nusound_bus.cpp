#include "nu2api.saga/nusound/nusound_bus.hpp"

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nusound/nusound_system.hpp"

const char *NuSoundBus::GetName() {
    return this->name;
}

NuSoundBus *NuSoundSystem::GetBus(const char *name) {
    LOG_WARN("NuSoundSystem::GetBus is not implemented");
    return NULL;
}

NuSoundBus::NuSoundBus(const char *name, bool is_master) {
    // this->field15_0x3c = (undefined *)&this->field11_0x2c;
    // this->field13_0x34 = (undefined *)&this->field11_0x2c;
    // this->field1_0x4 = (int *)0x0;
    // this->field0_0x0 = 0;
    // this->field11_0x2c = 0;
    // this->field14_0x38 = 0;
    // this->field17_0x44 = 0;
    // this->field16_0x40 = &this->field13_0x34;
    // this->field12_0x30 = &this->field13_0x34;

    this->parent_bus = NULL;

    NuStrCpy(this->name, name);
    // this->field2_0x8 = 0x3f800000;
    // this->field3_0xc = 0x3f800000;
    // this->field4_0x10 = 0x3f800000;
    // this->field5_0x14 = 0x3f800000;
    // this->field6_0x18 = 0x3f800000;
    // this->field7_0x1c = 0x3f800000;
    // this->field8_0x20 = 0x3f800000;
    // this->field9_0x24 = 0x3f800000;

    if (!is_master) {
        this->parent_bus = NuSoundSystem::sMasterBus;
    }
}
