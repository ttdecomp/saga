#include "nu2api.saga/numemory/NuMemoryManager.h"

class NuMemory;

class NuMemoryPS {
  protected:
    class Mem1EventHandler : public NuMemoryManager::IEventHandler {
      public:
        Mem1EventHandler();

        virtual int AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) override;
        virtual int ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) override;

      private:
        unsigned int pageCount;
    };

    class Mem2EventHandler : public NuMemoryManager::IEventHandler {
      public:
        Mem2EventHandler();

        virtual int AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) override;
        virtual int ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) override;
    };

    friend NuMemory;
};
