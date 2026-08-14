#include "nu2api/nucore/NuInputDevice.h"

class NuInputManager {
  public:
    NuInputManager();
    ~NuInputManager();

    void UpdateAll(f32 delta_time);

    u32 GetMaxDevices() const;

    NuInputDevice *GetDevice(u32 port);
    NuInputDevice *GetFirstDeviceByType(NUPADTYPE type);

    void GetDevice(u32 port) const;
    void GetFirstDeviceByType(NUPADTYPE type) const;
    void KillRumbleAll();

  private:
    NuInputDevice **devices;
    u32 max_devices;

    // Type uncertain.
    u32 unknown_08;

    u32 next_idx_by_type[8];

    // Type uncertain.
    u32 unknown_2c;
};
