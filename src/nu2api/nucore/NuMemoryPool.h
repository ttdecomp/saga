#include <pthread.h>

#include "nu2api/nucore/common.h"

class NuMemoryPool {
  public:
    class IEventHandler {
      public:
        virtual i32 AllocatePage(NuMemoryPool *pool, u32 _unknown, u32 _unknown2, const char *_unknown3) = 0;
        virtual i32 ReleasePage(NuMemoryPool *pool, void *ptr) = 0;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) = 0;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, u32 size, u32 alignment, const char *_unknown3) = 0;
        virtual void FreeLargeBlock(NuMemoryPool *pool, void *ptr) = 0;
    };

  private:
    struct FreeBlock {
        FreeBlock *next;
    };

    struct Page;

  public:
    class IVisitor {
      public:
        virtual ~IVisitor() = default;
    };

    void AddPage(void *ptr, u32 size);

    NuMemoryPool(IEventHandler *event_handler, u32 size, const char *name);
    ~NuMemoryPool();

    void GetAllocatedBytes();
    void GetDebugName() const;
    void GetFreeBytes();
    void GetLargeBlockBytes();
    void GetPagedBytes();
    void InterlockedPop(FreeBlock volatile **out_head);
    void InterlockedPush(FreeBlock volatile **head, void *block);
    void Merge(FreeBlock volatile *a, FreeBlock volatile *b);
    void Merge(Page *a, Page *b);
    void MergeSort(FreeBlock volatile *list, u32 count);
    void MergeSort(Page *list, u32 count);
    void PageAlloc(u32 size, const char *name);
    void ReleaseAllPages();
    void ReleaseUnreferencedPages();
    void ReleaseUnreferencedPages_OLD();
    void VisitPools(IVisitor *visitor);

  private:
    static NuMemoryPool *m_firstPool;
    static pthread_mutex_t m_globalCriticalSection;

    NuMemoryPool *next;
    const char *name;

    static void InterlockedAdd(volatile u32 *augend, u32 addend);
    static void InterlockedSub(volatile u32 *minuend, u32 subtrahend);
};
