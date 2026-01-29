#pragma once

#include <pthread.h>
#include <stdint.h>

#ifdef __cplusplus

class NuMemoryManager {
  public:
    enum ErrorCode {};

    class IErrorHandler {
      public:
        virtual void HandleError(NuMemoryManager *manager, ErrorCode code, const char *msg);
        virtual int OpenDump(NuMemoryManager *manager, const char *filename, unsigned int &id);
        virtual void CloseDump(NuMemoryManager *manager, unsigned int id);
        virtual void Dump(NuMemoryManager *manager, unsigned int id, const char *msg);
    };

    class IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) = 0;
        virtual int ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) = 0;
    };

  private:
    static unsigned int m_flags;
    static pthread_mutex_t *m_globalCriticalSection;
    static pthread_mutex_t m_globalCriticalSectionBuff;
    static unsigned int m_headerSize;

    const char **categoryNames;
    unsigned int categoryCount;
    bool isZombie;
    uint32_t idx;
    char _placeholder[0x1810];

  public:
    NuMemoryManager(IEventHandler *eventHandler, IErrorHandler *errorHandler, const char *name, const char **categoryNames, unsigned int categoryCount);

    static void SetFlags(unsigned int flags);

    void *_BlockAlloc(uint32_t size, uint32_t param_2, uint32_t param_3, const char *name, uint16_t param_5);
    void BlockFree(void *ptr, unsigned int flags);

    void AddPage(void *ptr, unsigned int size, bool _unknown);

  private:
    void *_TryBlockAlloc(uint32_t size, uint32_t param_2, uint32_t param_3, const char *name, uint16_t param_5);
    void ____WE_HAVE_RUN_OUT_OF_MEMORY_____(uint32_t size, const char *name);

    unsigned int CalculateBlockSize(unsigned int size);
    static unsigned int GetLargeBinIndex(unsigned int size);
    static unsigned int GetSmallBinIndex(unsigned int size);
    void ValidateAllocAlignment(unsigned int alignment);
    void ValidateAllocSize(unsigned int size);
};

#endif
