#include "nu2api.saga/nucore/NuMemoryManager.h"

class NuMemory;

class NuMemoryPS {
  protected:
    class Mem1EventHandler : public NuMemoryManager::IEventHandler {
      public:
        Mem1EventHandler();

        virtual bool AllocatePage(NuMemoryManager *manager, u32 size, u32 _unknown) override;
        virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, u32 _unknown) override;

      private:
        u32 page_count;
    };

    class Mem2EventHandler : public NuMemoryManager::IEventHandler {
      public:
        Mem2EventHandler();

        virtual bool AllocatePage(NuMemoryManager *manager, u32 size, u32 _unknown) override;
        virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, u32 _unknown) override;
    };

    friend NuMemory;
};
