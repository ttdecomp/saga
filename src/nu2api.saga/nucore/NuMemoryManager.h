#pragma once

#include <pthread.h>

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus

class NuMemoryManager {
  public:
    enum Flags {
        MEM_MANAGER_DEBUG = 1 << 0x2,
        MEM_MANAGER_EXTENDED_DEBUG = 1 << 0x3,
        MEM_MANAGER_IN_ERROR_STATE = 1 << 0x7,
    };

    enum ErrorCode {
        MEM_ERROR_LEAK_DETECTED = 0x8000000,
        MEM_ERROR_BAD_POINTER = 0x8000001,
        MEM_ERROR_CORRUPTION = 0x8000003,
        MEM_ERROR_UNALLOCATED_BLOCK = 0x8000004,
        MEM_ERROR_ALLOC_FLAG_MISMATCH = 0x8000005,
        MEM_ERROR_OUT_OF_MEMORY = 0x8000006,
        MEM_ERROR_DEFERRED_CONTENT_CHANGED = 0x8000007,
    };

    enum AllocFlags {
        MEM_ALLOC_SET_TO_ZERO = 1 << 0x0,
        MEM_ALLOC_ARRAY = 1 << 0x1,
        MEM_ALLOC_UNKNOWN_4 = 1 << 0x2,
    };

    class IErrorHandler {
      public:
        virtual void HandleError(NuMemoryManager *manager, ErrorCode code, const char *msg);
        virtual int OpenDump(NuMemoryManager *manager, const char *filename, u32 &id);
        virtual void CloseDump(NuMemoryManager *manager, u32 id);
        virtual void Dump(NuMemoryManager *manager, u32 id, const char *msg);
    };

    class IEventHandler {
      public:
        virtual bool AllocatePage(NuMemoryManager *manager, u32 size, u32 _unknown) = 0;
        virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, u32 _unknown) = 0;
    };

  private:
    struct Header {
        u32 value;
    };

    struct FreeHeader {
        Header block_header;
        FreeHeader *next;
        FreeHeader *prev;
    };

    struct DebugFlags {
        u16 alloc_flags : 7;
        u16 ctx_id : 5;
        u16 unknown : 4;
    };

    struct DebugHeader {
        Header block_header;
        const char *name;
        u16 category;
        DebugFlags flags;
    };

    struct ExtendedDebugInfo {
        // Type uncertain.
        int unknown[32];
    };

    // The existence of this type isn't directly attested, but it lets us
    // prevent access to the extended debug info field without an explicit cast.
    struct ExtendedDebugHeader {
        DebugHeader header;
        ExtendedDebugInfo extended_info;

        // Type uncertain.
        int unknown;
    };

    struct Page {
        void *original_ptr;
        u32 size;
        Header *first_header;
        u32 *end;
        Page *next;
        Page *prev;
        bool is_external;
    };

    struct Context {
        const char *name;
        u32 id;

        // Type uncertain.
        int used_block_count;

        Context *next;
    };

    struct Stats {
        int unknown_00;

        u32 free_frag_bytes;
        u32 min_free_bytes;
        u32 frag_count;
        u32 max_frag_count;
        u32 used_block_count;

        // Type uncertain.
        int unknown_18;

        u32 bytes_alloc_by_category[100];
    };

    enum PopDebugMode {
        POP_DEBUG_MODE_NONE = 0,
        POP_DEBUG_MODE_NORMAL = 1,
        POP_DEBUG_MODE_FREE_STRANDED_BLOCKS = 2,
    };

  private:
    static u32 m_flags;
    static pthread_mutex_t *m_globalCriticalSection;
    static pthread_mutex_t m_globalCriticalSectionBuff;
    static u32 m_headerSize;
    static NuMemoryManager *m_memoryManagers[0x100];

    const char **category_names;
    u32 category_count;
    bool is_zombie;
    u32 idx;
    char name[0x80];
    IEventHandler *event_handler;
    IErrorHandler *error_handler;
    Page *pages;

    u32 small_bin_has_free_map[32];

    FreeHeader small_bins[256];

    u32 large_bin_has_free_map;
    u32 large_bin_dirty_map;

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

    void **stranded_blocks;
    u32 stranded_block_count;

    // Types uncertain.
    int unknown_1814;
    int unknown_1818;

    u16 override_category;
    u16 override_category_bg_thread;

  public:
    NuMemoryManager(IEventHandler *event_handler, IErrorHandler *error_handler, const char *name,
                    const char **category_names, u32 category_count);
    ~NuMemoryManager();

    static void SetFlags(u32 flags);

    void *_BlockAlloc(u32 size, u32 alignment, u32 flags, const char *name, u16 category);
    void BlockFree(void *ptr, u32 flags);
    void *_BlockReAlloc(void *ptr, u32 size, u32 alignment, u32 flags, const char *name, u16 category);

    void AddPage(void *ptr, u32 size, bool _unknown);

    void SetBlockDebugCategory(void *block, u16 category);

    void *_TryBlockAlloc(u32 size, u32 alignment, u32 flags, const char *name, u16 category);

  private:
    static u32 GetLargeBinIndex(u32 size);
    static u32 GetSmallBinIndex(u32 size);

    void ConvertToUsedBlock(FreeHeader *header, u32 alignment, u32 flags, const char *name, u16 category);
    void *ClearUsedBlock(Header *header, u32 flags);

    void ____WE_HAVE_RUN_OUT_OF_MEMORY_____(u32 size, const char *name);
    void SetZombie();

    inline void MergeBlocks(Header *left, Header *right, const char *caller);

    void ReleaseUnreferencedPages();

    u32 CalculateBlockSize(u32 size);

    void BinLink(FreeHeader *header, bool keep_sorted);
    void BinUnlink(FreeHeader *header);
    void BinLinkAfterNode(FreeHeader *after, FreeHeader *header);

    bool PopContext(PopDebugMode debug_mode);

    void Validate();
    void ValidateAddress(void *ptr, const char *caller);
    void ValidateAllocAlignment(u32 alignment);
    void ValidateAllocSize(u32 size);
    void ValidateBlockEndTags(Header *header, const char *caller);
    void ValidateBlockFlags(Header *header, u32 flags, const char *caller);
    void ValidateBlockIsAllocated(Header *header, const char *caller);
    void ValidateBlockIsPaged(void *block, const char *caller);

    void StatsAddFragment(FreeHeader *header);
    void StatsRemoveFragment(FreeHeader *header);

    u32 CalculateLargestFragmentSize();
    u32 CalculateFreeBytes();

    FreeHeader *FindLargestFragment();

    void Dump(u32 _unknown, const char *filepath);

    void StrandBlocksForContext(Context *ctx, u32 &stranded_block_count, u32 &_unknown, Header *&largest_stranded,
                                u32 &stranded_bytes_count);
    void FreeStrandedBlocks();
};

#endif
