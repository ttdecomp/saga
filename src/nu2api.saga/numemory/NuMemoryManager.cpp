#include <pthread.h>
#include <string.h>

#include "decomp.h"

#include "nu2api.saga/numemory/NuMemoryManager.h"

#include "nu2api.saga/nucore/common.h"

#define ALLOC_FLAGS 0x78000000
#define BLOCK_SIZE(header) ((header)->block_header.value & ~ALLOC_FLAGS) * 4

unsigned int NuMemoryManager::m_flags;
pthread_mutex_t *NuMemoryManager::m_globalCriticalSection;
pthread_mutex_t NuMemoryManager::m_globalCriticalSectionBuff;
unsigned int NuMemoryManager::m_headerSize;

// A table of values for counting leading zeros in the binary representation of
// a 32-bit integer.
static char g_clzTable[] = {
    0x00, 0x1f, 0x09, 0x1e, 0x03, 0x08, 0x12, 0x1d, 0x02, 0x05, 0x07, 0x0e, 0x0c, 0x11, 0x16, 0x1c,
    0x01, 0x0a, 0x04, 0x13, 0x06, 0x0f, 0x0d, 0x17, 0x0b, 0x14, 0x10, 0x18, 0x15, 0x19, 0x1a, 0x1b,
};

static unsigned int CountLeadingZeros(unsigned int value) {
    // An almost-branchless algorithm for calculating the leading zeros of a
    // binary value by exploiting de Bruijn sequences. The value is hashed
    // using a perfect hash function and the hash provides the index to a table
    // of leading zero counts.
    value |= value >> 0x01;
    value |= value >> 0x02;
    value |= value >> 0x04;
    value |= value >> 0x08;
    value |= value >> 0x10;

    if (value != 0) {
        return g_clzTable[((value + 1) * 0x07dcd629) >> 0x1b];
    }

    return 0x20;
}

NuMemoryManager *NuMemoryManager::m_memoryManagers[0x100];

NuMemoryManager::NuMemoryManager(IEventHandler *event_handler, IErrorHandler *error_handler, const char *name,
                                 const char **category_names, unsigned int category_count) {
    pthread_mutexattr_t attrs;

    this->event_handler = event_handler;
    this->error_handler = error_handler;

    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&this->mutex, &attrs);
    pthread_mutexattr_destroy(&attrs);

    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&this->error_mutex, &attrs);
    pthread_mutexattr_destroy(&attrs);

    this->category_names = category_names;
    this->category_count = category_count;

    // The default initializers almost get these right, but they're out of order
    // and compile fails when trying to specify designated initializers out of
    // order.
    this->initial_state_ctx.unknown = 0;
    this->initial_state_ctx.id = 0;
    this->initial_state_ctx.next = 0;
    this->initial_state_ctx.name = 0;

    this->stranded_ctx.unknown = 0;
    this->stranded_ctx.id = 0;
    this->stranded_ctx.next = 0;
    this->stranded_ctx.name = 0;

    this->unknown_1814 = 0x4000;
    this->unknown_1818 = 0;

    this->is_zombie = false;

    strcpy(this->name, name);

    this->pages = NULL;

    this->unknown_180c = 0;
    this->unknown_1810 = 0;

    memset(this->small_bins, 0, sizeof(this->small_bins));
    memset(this->large_bins, 0, sizeof(this->large_bins));

    memset(this->small_bin_has_free_map, 0, sizeof(this->small_bin_has_free_map));

    this->unknown_0e2c = 0;
    this->unknown_0e30 = 0;

    this->large_bin_has_free_map = 0;
    this->large_bin_dirty_map = 0;

    memset(&this->stats, 0, sizeof(Stats));

    this->stats.unknown_08 = this->stats.frag_bytes;

    this->initial_state_ctx.name = "Initial State";
    this->cur_ctx = &this->initial_state_ctx;

    this->stranded_ctx.name = "Stranded";

    this->override_category = 1;
    this->override_category_bg_thread = 1;

    this->stranded_ctx.id = 31;

    if (m_globalCriticalSection == NULL) {
        pthread_mutexattr_init(&attrs);
        pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_globalCriticalSectionBuff, &attrs);
        pthread_mutexattr_destroy(&attrs);

        m_globalCriticalSection = &m_globalCriticalSectionBuff;
    }

    pthread_mutex_lock(m_globalCriticalSection);

    for (int i = 0; i != 0x100; i++) {
        if (m_memoryManagers[i] == NULL) {
            this->idx = i;
            m_memoryManagers[i] = this;

            break;
        }
    }

    pthread_mutex_unlock(m_globalCriticalSection);
}

NuMemoryManager::~NuMemoryManager() {
    PopContext(POP_DEBUG_MODE_UNKNOWN_2);
    ReleaseUnreferencedPages();

    pthread_mutex_lock(m_globalCriticalSection);

    m_memoryManagers[this->idx] = NULL;

    pthread_mutex_unlock(m_globalCriticalSection);

    pthread_mutex_destroy(&this->error_mutex);
    pthread_mutex_destroy(&this->mutex);
}

void NuMemoryManager::SetFlags(unsigned int flags) {
    m_flags = flags;

    if ((flags & MEM_MANAGER_EXTENDED_DEBUG) != 0) {
        m_flags |= MEM_MANAGER_DEBUG;
        m_headerSize = sizeof(ExtendedDebugHeader);
    } else if ((flags & MEM_MANAGER_DEBUG) != 0) {
        m_headerSize = sizeof(DebugHeader);
    } else {
        m_headerSize = sizeof(Header);
    }
}

void *NuMemoryManager::_BlockAlloc(uint32_t size, uint32_t alignment, uint32_t param_3, const char *name,
                                   uint16_t param_5) {
    void *ptr = this->_TryBlockAlloc(size, alignment, param_3, name, param_5);

    if (ptr == NULL) {
        this->____WE_HAVE_RUN_OUT_OF_MEMORY_____(size, name);
    }

    return ptr;
}

void *NuMemoryManager::_TryBlockAlloc(uint32_t size, uint32_t alignment, uint32_t param_3, const char *name,
                                      uint16_t param_5) {
    LOG_INFO("size=%x, param_2=%u, param_3=%u, name='%s', param_5=%u", size, alignment, param_3, name, param_5);

    // UNIMPLEMENTED();
    return malloc(size);
}

void NuMemoryManager::____WE_HAVE_RUN_OUT_OF_MEMORY_____(uint32_t size, const char *name) {
    UNIMPLEMENTED();
}

void NuMemoryManager::BlockFree(void *ptr, unsigned int flags) {
}

void NuMemoryManager::AddPage(void *ptr, unsigned int size, bool _unknown) {
}

void NuMemoryManager::ReleaseUnreferencedPages() {
    Page *entry;

    pthread_mutex_lock(&this->mutex);

    for (entry = this->pages; entry != NULL; entry = entry->next) {
    }

    pthread_mutex_unlock(&this->mutex);
}

unsigned int NuMemoryManager::CalculateBlockSize(unsigned int size) {
    unsigned int aligned;
    int block_size;
    unsigned int mem_size;

    aligned = ALIGN(size, 0x4);
    mem_size = MAX(aligned, 8);

    block_size = mem_size + m_headerSize + 4;
    if (this->idx > 29) {
        block_size = mem_size + m_headerSize + 8;
    }

    return block_size;
}

unsigned int NuMemoryManager::GetLargeBinIndex(unsigned int size) {
    // This could be negative if the size passed in were small enough, but in
    // practice this function is only called for allocations of sufficient size.
    return 0x15 - CountLeadingZeros(size);
}

unsigned int NuMemoryManager::GetSmallBinIndex(unsigned int size) {
    return size >> 2;
}

void NuMemoryManager::BinLink(NuMemoryManager::FreeHeader *header, bool keep_sorted) {
    unsigned int size;
    unsigned int bin_idx;

    size = BLOCK_SIZE(header);

    if (size < 0x400) {
        bin_idx = GetSmallBinIndex(size);
        BinLinkAfterNode(&this->small_bins[bin_idx], header);
        this->small_bin_has_free_map[bin_idx >> 5] |= bin_idx;
    } else {
        bin_idx = GetLargeBinIndex(size);

        if (keep_sorted) {
            FreeHeader *after;
            FreeHeader *next;

            next = &this->large_bins[bin_idx];

            do {
                after = next;
                next = next->next;
            } while (next != NULL && BLOCK_SIZE(next) <= size);

            BinLinkAfterNode(after, header);
        } else {
            BinLinkAfterNode(&this->large_bins[bin_idx], header);
            this->large_bin_dirty_map |= 1 << bin_idx;
        }

        this->large_bin_has_free_map |= 1 << (0x1f - bin_idx);
    }

    StatsAddFragment(header);
}

void NuMemoryManager::BinUnlink(NuMemoryManager::FreeHeader *header) {
    FreeHeader *next;
    unsigned int size;
    unsigned int bin_idx;

    // Remove the header from the linked list.
    next = header->next;
    if (next != NULL) {
        next->prev = header->prev;
    }

    if (header->prev != NULL) {
        header->prev->next = next;
    }

    header->next = NULL;
    header->prev = NULL;

    // Update maps of bins with free blocks.
    size = BLOCK_SIZE(header);

    if (size < 0x400) {
        bin_idx = GetSmallBinIndex(size);

        if (this->small_bins[bin_idx].next == NULL) {
            this->small_bin_has_free_map[bin_idx >> 5] &= ~(bin_idx & 0x1f);
        }
    } else {
        bin_idx = GetLargeBinIndex(size);

        if (this->large_bins[bin_idx].next == NULL) {
            // Rotate left.
            unsigned int shift = 0x1f - bin_idx;
            this->large_bin_has_free_map &= ((unsigned int)-2 << shift) | ((unsigned int)-2 >> (0x20 - shift));
        }
    }

    StatsRemoveFragment(header);
}

void NuMemoryManager::BinLinkAfterNode(NuMemoryManager::FreeHeader *after, NuMemoryManager::FreeHeader *header) {
    FreeHeader *next;

    next = after->next;
    header->prev = after;
    header->next = next;

    if (after->next != NULL) {
        after->next->prev = header;
    }

    after->next = header;
}

bool NuMemoryManager::PopContext(NuMemoryManager::PopDebugMode debug_mode) {
}

void NuMemoryManager::ValidateAllocAlignment(unsigned int alignment) {
}

void NuMemoryManager::ValidateAllocSize(unsigned int size) {
}

void NuMemoryManager::StatsAddFragment(NuMemoryManager::FreeHeader *header) {
    this->stats.frag_bytes += BLOCK_SIZE(header);
    this->stats.frag_count++;
    this->stats.max_frag_count = MAX(this->stats.max_frag_count, this->stats.frag_count);
}

void NuMemoryManager::StatsRemoveFragment(NuMemoryManager::FreeHeader *header) {
    this->stats.frag_bytes -= BLOCK_SIZE(header);
    this->stats.frag_count--;
}

void NuMemoryManager::IErrorHandler::HandleError(NuMemoryManager *manager, ErrorCode code, const char *msg) {
}

int NuMemoryManager::IErrorHandler::OpenDump(NuMemoryManager *manager, const char *filename, unsigned int &id) {
    return 0;
}

void NuMemoryManager::IErrorHandler::CloseDump(NuMemoryManager *manager, unsigned int id) {
}

void NuMemoryManager::IErrorHandler::Dump(NuMemoryManager *manager, unsigned int id, const char *msg) {
}
