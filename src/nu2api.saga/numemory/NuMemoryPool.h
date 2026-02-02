#include <pthread.h>

class NuMemoryPool {
  public:
    class IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int _unknown2,
                                 const char *_unknown3) = 0;
        virtual int ReleasePage(NuMemoryPool *pool, void *ptr) = 0;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) = 0;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, unsigned int size, unsigned int alignment,
                                         const char *_unknown3) = 0;
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

    static void InterlockedAdd(volatile unsigned int *augend, unsigned int addend);
    static void InterlockedSub(volatile unsigned int *minuend, unsigned int subtrahend);
};
