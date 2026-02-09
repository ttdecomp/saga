#include "nu2api.saga/nucore/NuMemoryManager.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"

#define HEADER_MGR_HI_MASK 0xf8000000
#define ALLOC_MASK 0x78000000
#define BLOCK_SIZE_MASK ~ALLOC_MASK
#define BLOCK_SIZE(header_val) ((header_val) & BLOCK_SIZE_MASK) * 4
#define END_TAG(block, size) (u32 *)((usize)block + size - 4)
#define END_TAG_HI(block, size) (u32 *)((usize)block + size - 8)

#define STRANDED_DUMP_SUFFIX "_stranded.txt"

u32 NuMemoryManager::m_flags;
pthread_mutex_t *NuMemoryManager::m_globalCriticalSection;
pthread_mutex_t NuMemoryManager::m_globalCriticalSectionBuff;
u32 NuMemoryManager::m_headerSize;

// A table of values for counting leading zeros in the binary representation of
// a 32-bit integer.
static char g_clzTable[] = {
    0x00, 0x1f, 0x09, 0x1e, 0x03, 0x08, 0x12, 0x1d, 0x02, 0x05, 0x07, 0x0e, 0x0c, 0x11, 0x16, 0x1c,
    0x01, 0x0a, 0x04, 0x13, 0x06, 0x0f, 0x0d, 0x17, 0x0b, 0x14, 0x10, 0x18, 0x15, 0x19, 0x1a, 0x1b,
};

static inline u32 CountLeadingZeros(u32 value) {
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
                                 const char **category_names, u32 category_count) {
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

    this->stranded_blocks = NULL;
    this->stranded_block_count = 0;

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

void NuMemoryManager::SetFlags(u32 flags) {
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

void *NuMemoryManager::_BlockAlloc(u32 size, u32 alignment, u32 flags, const char *name, u16 category) {
    void *ptr = this->_TryBlockAlloc(size, alignment, flags, name, category);

    if (ptr == NULL) {
        this->____WE_HAVE_RUN_OUT_OF_MEMORY_____(size, name);
    }

    return ptr;
}

void *NuMemoryManager::_TryBlockAlloc(u32 size, u32 alignment, u32 flags, const char *name, u16 category) {
    // UNIMPLEMENTED();
    return malloc(size);
}

void NuMemoryManager::ConvertToUsedBlock(FreeHeader *header, u32 alignment, u32 flags, const char *name, u16 category) {
    u32 align_mask;
    u32 header_value;
    u32 aligned;
    u32 *end_tag;
    u32 manager_idx;

    align_mask = -(CountLeadingZeros(alignment >> 2) << 0x1b);
    header_value = header->block_header.value & BLOCK_SIZE_MASK;

    manager_idx = this->idx;
    aligned = header_value | align_mask;

    header->block_header.value = aligned;

    // Set the end tag of the block. This is used for detecting corruption and
    // clearing the block.
    end_tag = END_TAG(header, header_value * 4);
    if ((align_mask & ALLOC_MASK) == 0) {
        *end_tag = aligned & BLOCK_SIZE_MASK;
    } else if (manager_idx >= 0x1d) {
        *end_tag = aligned | HEADER_MGR_HI_MASK;
        *(u32 *)((usize)header + BLOCK_SIZE(header->block_header.value) - 8) = manager_idx;
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

void *NuMemoryManager::ClearUsedBlock(Header *header, u32 flags) {
    u32 size;
    void *ptr;
    u32 size_minus_header;
    u32 *end_tag;
    u32 tag_value;
    u32 manager_idx;
    u32 available_size;

    ptr = (void *)((usize)header + m_headerSize);
    size = BLOCK_SIZE(header->value);
    size_minus_header = size - m_headerSize;

    end_tag = END_TAG(header, size);

    tag_value = *end_tag >> 0x1b;
    if (tag_value != 0x1f) {
        manager_idx = tag_value - 1;
    } else {
        manager_idx = end_tag[-1];
    }

    available_size = manager_idx <= 0x1d ? size_minus_header - 4 : size_minus_header - 8;

    if ((flags & MEM_ALLOC_SET_TO_ZERO) != 0) {
        memset(ptr, 0, available_size);
    }

    return ptr;
}

void NuMemoryManager::____WE_HAVE_RUN_OUT_OF_MEMORY_____(u32 size, const char *name) {
    u32 largest_fragment_size;
    u32 free_bytes;
    char requested_str[14];
    char largest_fragment_size_str[14];
    char free_bytes_str[14];

    largest_fragment_size = CalculateLargestFragmentSize();
    free_bytes = CalculateFreeBytes();

    NuStrFormatSize(requested_str, sizeof(requested_str), size, true);
    NuStrFormatSize(largest_fragment_size_str, sizeof(largest_fragment_size_str), largest_fragment_size, true);
    NuStrFormatSize(free_bytes_str, sizeof(free_bytes_str), free_bytes, true);

    pthread_mutex_lock(&this->error_mutex);

    m_flags |= MEM_MANAGER_IN_ERROR_STATE;

    snprintf(this->error_msg, sizeof(this->error_msg),
             "** Out of memory **\nRequested = %s\nLargest Fragment = %s\nFree Bytes = %s\nFrom: %s\n", requested_str,
             largest_fragment_size_str, free_bytes_str, name);

    this->error_handler->HandleError(this, MEM_ERROR_OUT_OF_MEMORY, this->error_msg);

    pthread_mutex_unlock(&this->error_mutex);

    SetZombie();

    Dump(0x11, "memdump_out_of_memory.txt");
}

void NuMemoryManager::SetZombie() {
    this->is_zombie = true;
}

void NuMemoryManager::BlockFree(void *ptr, u32 flags) {
    NuMemoryManager *manager;

    if (ptr == NULL) {
        return;
    }

    manager = this;

    // Loop until the correct manager is located. Should be on the second pass
    // at latest.
    while (true) {
        Header *header;
        u32 *end_tag;
        u32 tag_value;
        u32 manager_idx;

        manager->ValidateAddress(ptr, __FUNCTION__);

        header = (Header *)((usize)ptr - m_headerSize);

        manager->ValidateBlockIsAllocated(header, __FUNCTION__);
        manager->ValidateBlockFlags(header, flags, __FUNCTION__);
        manager->ValidateBlockEndTags(header, __FUNCTION__);

        end_tag = END_TAG(header, BLOCK_SIZE(header->value));

        tag_value = *end_tag >> 0x1b;
        if (tag_value != 0x1f) {
            manager_idx = tag_value - 1;
        } else {
            manager_idx = end_tag[-1];
        }

        if (manager->idx == manager_idx) {
            u32 header_value;
            Header *right;
            u32 left_end;
            FreeHeader *final;

            manager->ValidateBlockIsPaged(ptr, __FUNCTION__);

            pthread_mutex_lock(&manager->mutex);

            manager->stats.used_block_count--;

            header_value = header->value & BLOCK_SIZE_MASK;

            header->value = header_value;
            *END_TAG(header, header_value * 4) = header_value;

            if ((m_flags & MEM_MANAGER_DEBUG) != 0) {
                manager->stats.bytes_alloc_by_category[((DebugHeader *)header)->category] -= BLOCK_SIZE(header->value);
            }

            // See if we can combine this block with the one to the right.
            // It seems like a big assumption that there's a valid block to the
            // right in memory, but that seems to be what they assume.
            right = (Header *)((usize)header + BLOCK_SIZE(header->value));
            if ((right->value & ALLOC_MASK) == 0) {
                manager->MergeBlocks(header, right, "BlockFree[R]");
            }

            final = (FreeHeader *)header;

            // See if we can combine this block with the one to the left.
            // Again, it seems like a big assumption, but the assumption is
            // made.
            left_end = *(u32 *)((usize)header - 4);
            if ((left_end & HEADER_MGR_HI_MASK) == 0) {
                final = (FreeHeader *)((usize)header - BLOCK_SIZE(left_end));
                manager->MergeBlocks(&final->block_header, header, "BlockFree[L]");
            }

            manager->BinLink(final, false);

            if (manager->stranded_blocks != NULL) {
                for (int i = 0; i < manager->stranded_block_count; i++) {
                    if (ptr == manager->stranded_blocks[i]) {
                        manager->stranded_blocks[i] = NULL;
                    }
                }
            }

            pthread_mutex_unlock(&manager->mutex);
        } else {
            manager = m_memoryManagers[manager_idx];
        }
    }
}

inline void NuMemoryManager::MergeBlocks(Header *left, Header *right, const char *caller) {
    u32 combined_values;
    u32 alloc_value;
    u32 new_value;
    u32 combined_no_hi_bit;
    u32 *end_tag;

    ValidateBlockEndTags(right, caller);

    BinUnlink((FreeHeader *)right);

    combined_values = (left->value & BLOCK_SIZE_MASK) + (right->value & BLOCK_SIZE_MASK);
    alloc_value = left->value & ALLOC_MASK;
    new_value = combined_values & 0x3fffffff | alloc_value;
    combined_no_hi_bit = combined_values & BLOCK_SIZE_MASK;

    left->value = new_value;

    u32 manager_idx = this->idx;

    end_tag = END_TAG(left, combined_no_hi_bit * 4);
    if ((combined_values & ALLOC_MASK) == 0 && alloc_value == 0) {
        *end_tag = combined_no_hi_bit;
    } else if (manager_idx < 0x1e) {
        *end_tag = ((manager_idx + 1) << 0x1b) | combined_no_hi_bit;
    } else {
        *end_tag = new_value | HEADER_MGR_HI_MASK;
        *END_TAG_HI(left, BLOCK_SIZE(left->value)) = manager_idx;
    }
}

void NuMemoryManager::AddPage(void *ptr, u32 size, bool _unknown) {
    Page *page;

    page = (Page *)ALIGN((usize)ptr, 0x4);

    memset(page, 0, sizeof(Page));

    page->end = 0;
}

void NuMemoryManager::ReleaseUnreferencedPages() {
    Page *page;

    pthread_mutex_lock(&this->mutex);

    for (page = this->pages; page != NULL; page = page->next) {
    }

    pthread_mutex_unlock(&this->mutex);
}

u32 NuMemoryManager::CalculateBlockSize(u32 size) {
    u32 aligned;
    int block_size;
    u32 mem_size;

    aligned = ALIGN(size, 0x4);
    mem_size = MAX(aligned, 8);

    block_size = mem_size + m_headerSize + 4;
    if (this->idx > 29) {
        block_size = mem_size + m_headerSize + 8;
    }

    return block_size;
}

u32 NuMemoryManager::GetLargeBinIndex(u32 size) {
    // This could be negative if the size passed in were small enough, but in
    // practice this function is only called for allocations of sufficient size.
    return 0x15 - CountLeadingZeros(size);
}

u32 NuMemoryManager::GetSmallBinIndex(u32 size) {
    return size >> 2;
}

void NuMemoryManager::BinLink(NuMemoryManager::FreeHeader *header, bool keep_sorted) {
    u32 size;
    u32 bin_idx;

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
    u32 size;
    u32 bin_idx;

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
            u32 shift = 0x1f - bin_idx;
            this->large_bin_has_free_map &= ((u32)-2 << shift) | ((u32)-2 >> (0x20 - shift));
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
    u32 stranded_block_count;
    u32 stranded_bytes_count;
    u32 leak_count;
    u32 _unknown;
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
            usize len;
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
                u32 value_len;
                int i;

                value_len = MIN(BLOCK_SIZE(largest_stranded->value) - m_headerSize - 4, 0x100);

                for (i = 0; i != value_len; i++) {
                    char byte;

                    if ((unsigned char)largest_leak[i] - 0x30 < 10 || (unsigned char)largest_leak[i] - 0x41 < 0x1a) {
                        byte = *(char *)((usize)largest_stranded + m_headerSize + i);
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

void NuMemoryManager::ValidateAddress(void *ptr, const char *caller) {
    // Addresses should always be aligned to at minimum 4 bytes.
    if (((usize)ptr & 3) != 0) {
        char address[19];

        NuStrFormatAddress(address, sizeof(address), ptr);

        pthread_mutex_lock(&this->error_mutex);

        m_flags |= MEM_MANAGER_IN_ERROR_STATE;

        snprintf(this->error_msg, sizeof(this->error_msg), "Bad pointer detected in %s\nAddress: %s\n", caller,
                 address);

        this->error_handler->HandleError(this, MEM_ERROR_BAD_POINTER, this->error_msg);

        pthread_mutex_unlock(&this->error_mutex);
    }
}

void NuMemoryManager::ValidateAllocAlignment(u32 alignment) {
}

void NuMemoryManager::ValidateAllocSize(u32 size) {
}

void NuMemoryManager::ValidateBlockEndTags(Header *header, const char *caller) {
}

void NuMemoryManager::ValidateBlockFlags(Header *header, u32 flags, const char *caller) {
}

void NuMemoryManager::ValidateBlockIsAllocated(Header *header, const char *caller) {
}

void NuMemoryManager::ValidateBlockIsPaged(void *block, const char *caller) {
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

u32 NuMemoryManager::CalculateLargestFragmentSize() {
    u32 largest_frag_unadjusted;
    u32 largest_frag_size;
    FreeHeader *largest_frag;

    if ((m_flags & MEM_MANAGER_IN_ERROR_STATE) != 0) {
        return 0;
    }

    pthread_mutex_lock(&this->mutex);

    largest_frag = FindLargestFragment();
    if (largest_frag == NULL) {
        pthread_mutex_unlock(&this->mutex);

        return 0;
    }

    largest_frag_unadjusted = BLOCK_SIZE(largest_frag->block_header.value);

    pthread_mutex_unlock(&this->mutex);

    largest_frag_unadjusted -= m_headerSize;

    largest_frag_size = this->idx < 0x1e ? largest_frag_unadjusted - 4 : largest_frag_unadjusted - 8;

    return largest_frag_size;
}

u32 NuMemoryManager::CalculateFreeBytes() {
    u32 free_bytes = 0;

    if ((m_flags & MEM_MANAGER_IN_ERROR_STATE) != 0) {
        return 0;
    }

    pthread_mutex_lock(&this->mutex);

    for (i32 i = 0x15; i >= 0; i--) {
        for (FreeHeader *header = &this->large_bins[i]; header != NULL; header = header->next) {
            free_bytes += BLOCK_SIZE(header->block_header.value);
        }
    }

    for (i32 i = 0xff; i >= 0; i--) {
        for (FreeHeader *header = &this->small_bins[i]; header != NULL; header = header->next) {
            free_bytes += BLOCK_SIZE(header->block_header.value);
        }
    }

    pthread_mutex_unlock(&this->mutex);

    return free_bytes;
}

NuMemoryManager::FreeHeader *NuMemoryManager::FindLargestFragment() {
    FreeHeader *frag;

    if ((m_flags & MEM_MANAGER_IN_ERROR_STATE) != 0) {
        return NULL;
    }

    pthread_mutex_lock(&this->mutex);

    for (i32 i = 0x15; i >= 0; i--) {
        FreeHeader *header = &this->large_bins[i];

        do {
            frag = header;
            header = header->next;
        } while (header != NULL);

        if (BLOCK_SIZE(frag->block_header.value) != 0) {
            goto done;
        }
    }

    for (i32 i = 0xff; i >= 0; i--) {
        frag = this->small_bins[i].next;

        if (frag != NULL) {
            break;
        }
    }

done:
    pthread_mutex_unlock(&this->mutex);

    return frag;
}

void NuMemoryManager::Dump(u32 _unknown, const char *filepath) {
}

void NuMemoryManager::StrandBlocksForContext(Context *ctx, u32 &stranded_block_count, u32 &_unknown,
                                             Header *&largest_stranded, u32 &stranded_bytes_count) {
}

void NuMemoryManager::FreeStrandedBlocks() {
}

void NuMemoryManager::IErrorHandler::HandleError(NuMemoryManager *manager, ErrorCode code, const char *msg) {
}

int NuMemoryManager::IErrorHandler::OpenDump(NuMemoryManager *manager, const char *filename, u32 &id) {
    return 0;
}

void NuMemoryManager::IErrorHandler::CloseDump(NuMemoryManager *manager, u32 id) {
}

void NuMemoryManager::IErrorHandler::Dump(NuMemoryManager *manager, u32 id, const char *msg) {
}
