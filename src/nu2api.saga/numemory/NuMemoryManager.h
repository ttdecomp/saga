#pragma once

#include <pthread.h>
#include <stdint.h>

#ifdef __cplusplus

class NuMemoryManager {
  public:
    enum Flags {
        MEM_MANAGER_DEBUG = 1 << 0x2,
        MEM_MANAGER_EXTENDED_DEBUG = 1 << 0x3,
        MEM_MANAGER_IN_ERROR_STATE = 1 << 0x7,
    };

    enum ErrorCode {
        MEM_ERROR_LEAK = 0x8000000,
        MEM_ERROR_BAD_POINTER = 0x8000001,
        MEM_ERROR_CORRUPTION = 0x8000003,
        MEM_ERROR_UNALLOCATED_BLOCK = 0x8000004,
        MEM_ERROR_ALLOC_FLAG_MISMATCH = 0x8000005,
        MEM_ERROR_OUT_OF_MEMORY = 0x8000006,
        MEM_ERROR_DEFERRED_CONTENT_CHANGED = 0x8000007,
    };

    class IErrorHandler {
      public:
        virtual void HandleError(NuMemoryManager *manager, ErrorCode code, const char *msg);
        virtual int OpenDump(NuMemoryManager *manager, const char *filename, unsigned int &id);
        virtual void CloseDump(NuMemoryManager *manager, unsigned int id);
        virtual void Dump(NuMemoryManager *manager, unsigned int id, const char *msg);
    };

    class IEventHandler {
      public:
        virtual bool AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) = 0;
        virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) = 0;
    };

  private:
    struct Header {
        unsigned int value;
    };

    struct FreeHeader {
        Header block_header;
        FreeHeader *next;
        FreeHeader *prev;
    };

    struct DebugHeader {
        Header block_header;
        char *name;
        unsigned short category;
        unsigned short flags;
    };

    struct ExtendedDebugInfo {
        // Types uncertain.
        int unknown[32];
        int unknown2;
    };

    // The existence of this type isn't directly attested, but it lets us
    // prevent access to the extended debug info field without an explicit cast.
    struct ExtendedDebugHeader {
        DebugHeader header;
        ExtendedDebugInfo extended_info;
    };

    struct Page {
        void *start;
        unsigned int size;
        void *first_header;
        void *end;
        Page *next;
        Page *prev;
        bool unknown;
        unsigned int unknown2;
    };

    struct Context {
        const char *name;
        unsigned int id;

        // Type uncertain.
        int unknown;

        Context *next;
    };

    struct Stats {
        int unknown_00;

        unsigned int frag_bytes;

        unsigned int unknown_08;

        unsigned int frag_count;
        unsigned int max_frag_count;
        unsigned int used_block_count;

        // Type uncertain.
        int unknown_18;

        unsigned int bytes_alloc_by_category[100];
    };

    enum PopDebugMode {
        POP_DEBUG_MODE_UNKNOWN_2 = 2,
    };

  private:
    static unsigned int m_flags;
    static pthread_mutex_t *m_globalCriticalSection;
    static pthread_mutex_t m_globalCriticalSectionBuff;
    static unsigned int m_headerSize;
    static NuMemoryManager *m_memoryManagers[0x100];

    const char **category_names;
    unsigned int category_count;
    bool is_zombie;
    uint32_t idx;
    char name[0x80];
    IEventHandler *event_handler;
    IErrorHandler *error_handler;
    Page *pages;

    unsigned int small_bin_has_free_map[32];

    FreeHeader small_bins[256];

    unsigned int large_bin_has_free_map;
    unsigned int large_bin_dirty_map;

    FreeHeader large_bins[22];

    // Types uncertain.
    int unknown_0e2c;
    int unknown_0e30;

    Stats stats;

    pthread_mutex_t mutex;
    pthread_mutex_t error_mutex;

    Context initial_state_ctx;
    Context stranded_ctx;

    Context *cur_ctx;

    char error_msg[0x800];

    // Types uncertain.
    int unknown_180c;
    int unknown_1810;
    int unknown_1814;
    int unknown_1818;

    unsigned short override_category;
    unsigned short override_category_bg_thread;

  public:
    NuMemoryManager(IEventHandler *event_handler, IErrorHandler *error_handler, const char *name,
                    const char **category_names, unsigned int category_count);
    ~NuMemoryManager();

    static void SetFlags(unsigned int flags);

    void *_BlockAlloc(uint32_t size, uint32_t alignment, uint32_t param_3, const char *name, uint16_t param_5);
    void BlockFree(void *ptr, unsigned int flags);

    void AddPage(void *ptr, unsigned int size, bool _unknown);

  private:
    static unsigned int GetLargeBinIndex(unsigned int size);
    static unsigned int GetSmallBinIndex(unsigned int size);

    void *_TryBlockAlloc(uint32_t size, uint32_t alignment, uint32_t param_3, const char *name, uint16_t param_5);
    void ____WE_HAVE_RUN_OUT_OF_MEMORY_____(uint32_t size, const char *name);

    void ReleaseUnreferencedPages();

    unsigned int CalculateBlockSize(unsigned int size);

    void BinLink(FreeHeader *header, bool keep_sorted);
    void BinUnlink(FreeHeader *header);
    void BinLinkAfterNode(FreeHeader *after, FreeHeader *header);

    bool PopContext(PopDebugMode debug_mode);

    void ValidateAddress(void *ptr, const char *caller);
    void ValidateAllocAlignment(unsigned int alignment);
    void ValidateAllocSize(unsigned int size);

    void StatsAddFragment(FreeHeader *header);
    void StatsRemoveFragment(FreeHeader *header);
};

#endif
