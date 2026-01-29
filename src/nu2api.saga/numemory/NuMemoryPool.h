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
};
