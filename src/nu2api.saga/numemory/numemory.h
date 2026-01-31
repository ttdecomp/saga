#pragma once

#include <stdint.h>

#include "nu2api.saga/numemory/NuMemoryManager.h"
#include "nu2api.saga/numemory/NuMemoryPool.h"
#include "nu2api.saga/numemory/numemory_android.h"

#ifdef __cplusplus

struct NuMemory {
  private:
    class MemErrorHandler : public NuMemoryManager::IErrorHandler {};

    class FixedPoolEventHandler : NuMemoryPool::IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int _unknown2,
                                 const char *_unknown3) override;
        virtual int ReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, unsigned int _unknown, unsigned int _unknown2,
                                         const char *_unknown3) override;
        virtual void FreeLargeBlock(NuMemoryPool *pool, void *ptr) override;
    };

    class DynamicPoolEventHandler : NuMemoryPool::IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int _unknown2,
                                 const char *_unknown3) override;
        virtual int ReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, unsigned int _unknown, unsigned int _unknown2,
                                         const char *_unknown3) override;
        virtual void FreeLargeBlock(NuMemoryPool *pool, void *ptr) override;
    };

    MemErrorHandler *error_handler;
    NuMemoryPS::Mem1EventHandler *mem1_event_handler;
    NuMemoryPS::Mem2EventHandler *mem2_event_handler;
    NuMemoryManager *mem1_manager;
    NuMemoryManager *mem2_manager;
    int unknown;
    FixedPoolEventHandler *fixed_pool_event_handler;
    DynamicPoolEventHandler *dynamic_pool_event_handler;
    int unknown2;

    int32_t tls_index;

  public:
    NuMemory(void **buf);
    NuMemoryManager *GetThreadMem();

  private:
    void InitalizeThreadLocalStorage();
};

NuMemory *NuMemoryGet();

#endif
