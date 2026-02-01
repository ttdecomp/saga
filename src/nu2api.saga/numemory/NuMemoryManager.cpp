#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "decomp.h"

#include "nu2api.saga/numemory/NuMemoryManager.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"

#define HEADER_MGR_HI_IDX_MASK 0x80000000
#define HEADER_MGR_MASK 0x78000000
#define BLOCK_SIZE_MASK ~HEADER_MGR_MASK
#define BLOCK_SIZE(header) ((header) & BLOCK_SIZE_MASK) * 4

#define STRANDED_DUMP_SUFFIX "_stranded.txt"

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

static inline unsigned int CountLeadingZeros(unsigned int value) {
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
    this->initial_state_ctx.used_block_count = 0;
    this->initial_state_ctx.id = 0;
    this->initial_state_ctx.next = 0;
    this->initial_state_ctx.name = 0;

    this->stranded_ctx.used_block_count = 0;
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

    this->stats.min_free_bytes = this->stats.free_frag_bytes;

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
    PopContext(POP_DEBUG_MODE_FREE_STRANDED_BLOCKS);
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

void *NuMemoryManager::_BlockAlloc(unsigned int size, unsigned int alignment, unsigned int flags, const char *name,
                                   unsigned short category) {
    void *ptr = this->_TryBlockAlloc(size, alignment, flags, name, category);

    if (ptr == NULL) {
        this->____WE_HAVE_RUN_OUT_OF_MEMORY_____(size, name);
    }

    return ptr;
}

void *NuMemoryManager::_TryBlockAlloc(unsigned int size, unsigned int alignment, unsigned int flags, const char *name,
                                      unsigned short category) {
    // UNIMPLEMENTED();
    return malloc(size);
}

void NuMemoryManager::____WE_HAVE_RUN_OUT_OF_MEMORY_____(unsigned int size, const char *name) {
    UNIMPLEMENTED();
}

void NuMemoryManager::BlockFree(void *ptr, unsigned int flags) {
}

void NuMemoryManager::ConvertToUsedBlock(FreeHeader *header, unsigned int alignment, unsigned int flags,
                                         const char *name, unsigned short category) {
    unsigned int align_mask;
    unsigned int header_value;
    unsigned int aligned;
    unsigned int *end_tag;
    unsigned int manager_idx;

    align_mask = -(CountLeadingZeros(alignment >> 2) << 0x1b);
    header_value = header->block_header.value & BLOCK_SIZE_MASK;

    manager_idx = this->idx;
    aligned = header_value | align_mask;

    header->block_header.value = aligned;

    // Set the end tag of the block. This is used for detecting corruption and
    // clearing the block.
    end_tag = (unsigned int *)((int)header + header_value * 4 - 4);
    if ((align_mask & HEADER_MGR_MASK) == 0) {
        *end_tag = aligned & BLOCK_SIZE_MASK;
    } else if (manager_idx >= 0x1d) {
        *end_tag = aligned | 0xf8000000;
        *(unsigned int *)((int)header + BLOCK_SIZE(header->block_header.value) - 8) = manager_idx;
    } else {
        *end_tag = ((manager_idx + 1) << 0x1b) | aligned & BLOCK_SIZE_MASK;
    }

    this->stats.used_block_count++;
    this->stats.min_free_bytes = MIN(this->stats.min_free_bytes, this->stats.free_frag_bytes);

    if ((m_flags & MEM_MANAGER_DEBUG) != 0) {
        DebugHeader *debug = (DebugHeader *)header;
        debug->name = name;
        debug->category = category;
        debug->flags.alloc_flags = flags;
        debug->flags.ctx_id = *(char *)this->cur_ctx->id;
        debug->flags.unknown = debug->flags.ctx_id;

        this->stats.bytes_alloc_by_category[category] += BLOCK_SIZE(header->block_header.value);

        if ((m_flags & MEM_MANAGER_EXTENDED_DEBUG) != 0) {
            ExtendedDebugHeader *extended = (ExtendedDebugHeader *)header;

            memset(&extended->extended_info, 0, sizeof(ExtendedDebugInfo));
            extended->unknown = 0;
        }
    }
}

void *NuMemoryManager::ClearUsedBlock(Header *header, unsigned int flags) {
    unsigned int size;
    void *ptr;
    unsigned int size_minus_header;
    unsigned int *end_tag;
    unsigned int tag_value;
    unsigned int manager_idx;
    unsigned int available_size;

    ptr = (void *)((int)header + m_headerSize);
    size = BLOCK_SIZE(header->value);
    size_minus_header = size - m_headerSize;

    end_tag = (unsigned int *)((int)header + size - 4);

    tag_value = *end_tag >> 0x1b;
    if (tag_value != 0x1f) {
        manager_idx = tag_value - 1;
    } else {
        manager_idx = end_tag[-1];
    }

    available_size = manager_idx <= 0x1d ? size_minus_header - 4 : size_minus_header - 8;

    if ((flags & MEM_ALLOC_ZERO) != 0) {
        memset(ptr, 0, available_size);
    }

    return ptr;
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

    size = BLOCK_SIZE(header->block_header.value);

    if (size < 0x400) {
        bin_idx = GetSmallBinIndex(size);
        BinLinkAfterNode(&this->small_bins[bin_idx], header);
        this->small_bin_has_free_map[bin_idx >> 5] |= bin_idx & 0x1f;
    } else {
        bin_idx = GetLargeBinIndex(size);

        if (keep_sorted) {
            FreeHeader *after;
            FreeHeader *next;

            next = &this->large_bins[bin_idx];

            do {
                after = next;
                next = next->next;
            } while (next != NULL && BLOCK_SIZE(next->block_header.value) <= size);

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
    size = BLOCK_SIZE(header->block_header.value);

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
    char stranded_dump_path[128];
    char ctx_name[128];
    char largest_leak[128];
    char leak_value[257];
    unsigned int stranded_block_count;
    unsigned int stranded_bytes_count;
    unsigned int leak_count;
    unsigned int _unknown;
    Header *largest_stranded;
    Context *top_ctx;

    Validate();

    pthread_mutex_lock(&this->mutex);

    stranded_block_count = 0;

    memset(stranded_dump_path, 0, sizeof(stranded_dump_path));
    memset(ctx_name, 0, sizeof(ctx_name));
    memset(leak_value, 0, sizeof(leak_value));
    memset(largest_leak, 0, sizeof(largest_leak));

    stranded_bytes_count = 0;

    if ((m_flags & MEM_MANAGER_DEBUG) == 0) {
        leak_count = 0;

        if (this->stats.used_block_count > this->cur_ctx->used_block_count) {
            leak_count = this->stats.used_block_count - this->cur_ctx->used_block_count;
            strcpy(ctx_name, this->cur_ctx->name);
        }
    } else {
        _unknown = 0;
        largest_stranded = NULL;

        StrandBlocksForContext(this->cur_ctx, stranded_block_count, _unknown, largest_stranded, stranded_bytes_count);

        this->stats.unknown_18 = _unknown;

        if (stranded_block_count != 0 && debug_mode != POP_DEBUG_MODE_NONE) {
            size_t len;
            DebugHeader *largest_stranded_dbg;
            const char *block_name;

            strcpy(stranded_dump_path, this->name);

            len = strlen(stranded_dump_path);

            stranded_dump_path[len] = '_';
            stranded_dump_path[len + 1] = '\0';

            strcpy(stranded_dump_path + len + 1, this->cur_ctx->name);

            len = strlen(stranded_dump_path);

            strncpy(stranded_dump_path + len, STRANDED_DUMP_SUFFIX, strlen(STRANDED_DUMP_SUFFIX) + 1);

            if (len != -strlen(STRANDED_DUMP_SUFFIX)) {
                char *cursor = stranded_dump_path;

                do {
                    if (*cursor == '/' || *cursor == '\\' || *cursor == ':' || *cursor == ' ') {
                        *cursor = '_';
                    }

                    cursor++;
                } while (cursor != stranded_dump_path + len + strlen(STRANDED_DUMP_SUFFIX));
            }

            Dump(0x38, stranded_dump_path);

            strcpy(ctx_name, this->cur_ctx->name);

            largest_stranded_dbg = (DebugHeader *)largest_stranded;

            block_name = largest_stranded_dbg->name;
            if (block_name != NULL) {
                block_name = NuStrStripPath(block_name);
            } else {
                block_name = "UNKNOWN";
            }

            strcpy(largest_leak, block_name);

            if ((largest_stranded_dbg->flags.alloc_flags & MEM_ALLOC_UNKNOWN_4) != 0) {
                unsigned int value_len;
                int i;

                value_len = MIN(BLOCK_SIZE(largest_stranded->value) - m_headerSize - 4, 0x100);

                for (i = 0; i != value_len; i++) {
                    char byte;

                    if ((unsigned char)largest_leak[i] - 0x30 < 10 || (unsigned char)largest_leak[i] - 0x41 < 0x1a) {
                        byte = *(char *)((int)largest_stranded + m_headerSize + i);
                    } else {
                        byte = '_';
                    }

                    leak_value[i] = byte;
                }

                leak_value[i] = '\0';
            }

            leak_count = 0;
        }
    }

    ReleaseUnreferencedPages();

    top_ctx = this->cur_ctx;
    if (top_ctx != &this->initial_state_ctx) {
        this->cur_ctx = top_ctx->next;

        BlockFree(top_ctx, 0);
    }

    pthread_mutex_unlock(&this->mutex);

    if (debug_mode != POP_DEBUG_MODE_NONE) {
        if (stranded_block_count != 0) {
            pthread_mutex_lock(&this->error_mutex);

            if (leak_value[0] != '\0') {
                snprintf(this->error_msg, sizeof(this->error_msg),
                         "%u stranded memory blocks(s) [%u bytes] detected popping context %s\nReport dumped "
                         "[%s]\nLargest leak [%s]\nValue [%s]\n",
                         stranded_block_count, stranded_bytes_count, ctx_name, stranded_dump_path, largest_leak,
                         leak_value);
            } else {
                snprintf(this->error_msg, sizeof(this->error_msg),
                         "%u stranded memory blocks(s) [%u bytes] detected popping context %s\nReport dumped "
                         "[%s]\nLargest leak [%s]\n",
                         stranded_block_count, stranded_bytes_count, ctx_name, stranded_dump_path, largest_leak);
            }

            this->error_handler->HandleError(this, MEM_ERROR_LEAK_DETECTED, this->error_msg);

            pthread_mutex_unlock(&this->error_mutex);
        }

        if (debug_mode == POP_DEBUG_MODE_FREE_STRANDED_BLOCKS) {
            FreeStrandedBlocks();
        }
    }

    return stranded_block_count == 0;
}

void NuMemoryManager::Validate() {
}

void NuMemoryManager::ValidateAllocAlignment(unsigned int alignment) {
}

void NuMemoryManager::ValidateAllocSize(unsigned int size) {
}

void NuMemoryManager::StatsAddFragment(NuMemoryManager::FreeHeader *header) {
    this->stats.free_frag_bytes += BLOCK_SIZE(header->block_header.value);
    this->stats.frag_count++;
    this->stats.max_frag_count = MAX(this->stats.max_frag_count, this->stats.frag_count);
}

void NuMemoryManager::StatsRemoveFragment(NuMemoryManager::FreeHeader *header) {
    this->stats.free_frag_bytes -= BLOCK_SIZE(header->block_header.value);
    this->stats.frag_count--;
}

void NuMemoryManager::Dump(unsigned int _unknown, const char *filepath) {
}

void NuMemoryManager::StrandBlocksForContext(Context *ctx, unsigned int &stranded_block_count, unsigned int &_unknown,
                                             Header *&largest_stranded, unsigned int &stranded_bytes_count) {
}

void NuMemoryManager::FreeStrandedBlocks() {
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
