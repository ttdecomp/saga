#include "NuInputManager.h"

#include <new>

#include "nu2api/nucore/NuInputDevice.h"
#include "nu2api/nucore/NuMemoryManager.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nucore/nupad.h"

NuInputManager::NuInputManager() {
    u32 i;
    u32 max_devices;

    max_devices = NuInputDevicePS::ClassInitPS();

    this->devices = (NuInputDevice **)NU_ALLOC(max_devices * sizeof(NuInputDevice *), 0x4,
                                               NuMemoryManager::MEM_ALLOC_SET_TO_ZERO, "", NUMEMORY_CATEGORY_NONE);
    this->max_devices = max_devices;

    this->unknown_08 = 0;

    for (i = 0; i < this->max_devices; i++) {
        this->devices[i] = (NuInputDevice *)NU_ALLOC_T(NuInputDevice, NuMemoryManager::MEM_ALLOC_SET_TO_ZERO, "",
                                                       NUMEMORY_CATEGORY_NONE);
        new (this->devices[i]) NuInputDevice(i, 0);
    }
}

NuInputManager::~NuInputManager() {
    u32 i;

    if (this->max_devices == 0) {
        return;
    }

    for (i = 0; i < this->max_devices; i++) {
        NuMemoryGet()->GetThreadMem()->BlockFree(this->devices[i], 0);
    }

    NuMemoryGet()->GetThreadMem()->BlockFree(this->devices, 0);

    NuInputDevicePS::ClassShutdownPS();
}

void NuInputManager::UpdateAll(f32 delta_time) {
    u32 i;
    NUPADTYPE type;

    NuInputDevicePS::UpdateAllPS(delta_time);

    NuPad_UpdateTouchScreenData();

    memset(this->next_idx_by_type, 0, sizeof(this->next_idx_by_type));

    for (i = 0; i < this->max_devices; i++) {
        this->devices[i]->Update(delta_time, enable_touch_controls);

        type = this->devices[i]->GetType();

        this->devices[i]->SetIndexByType(this->next_idx_by_type[type]);
        this->next_idx_by_type[type]++;
    }
}

u32 NuInputManager::GetMaxDevices() const {
    return this->max_devices;
}

NuInputDevice *NuInputManager::GetDevice(u32 port) {
    return this->devices[port];
}
