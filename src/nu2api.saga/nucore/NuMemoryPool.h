#include <pthread.h>

#include "nu2api.saga/nucore/common.h"

class NuMemoryPool {
  public:
    class IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryPool *pool, u32 _unknown, u32 _unknown2, const char *_unknown3) = 0;
        virtual int ReleasePage(NuMemoryPool *pool, void *ptr) = 0;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) = 0;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, u32 size, u32 alignment, const char *_unknown3) = 0;
        virtual void FreeLargeBlock(NuMemoryPool *pool, void *ptr) = 0;
    };

  private:
    struct FreeBlock {
        FreeBlock *next;
    };

  public:
    void AddPage(void *ptr, unsigned int size);

  private:
    static NuMemoryPool *m_firstPool;
    static pthread_mutex_t m_globalCriticalSection;

    NuMemoryPool *next;
    const char *name;

    static void InterlockedAdd(volatile u32 *augend, u32 addend);
    static void InterlockedSub(volatile u32 *minuend, u32 subtrahend);
};
