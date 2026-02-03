#include <stdlib.h>
#include <string.h>

#include "decomp.h"

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"

static u32 NameToHash(char *name) {
    u32 constant = 0x811c9dc5;
    u32 prime = 0x199933;

    u32 hash = constant;
    for (; *name != '\0'; name++) {
        hash ^= *name;
        hash *= prime;
    }

    return hash;
}

isize BinarySearch(u32 element, u32 *array, usize length) {
    isize start = 0;
    isize end = length - 1;

    while (start <= end) {
        isize index = (start + end) / 2;
        if (array[index] == element) {
            return index;
        } else if (element > array[index]) {
            start = index + 1;
        } else {
            end = index - 1;
        }
    }

    return -1;
}

i32 NuDatFileFindHash(nudathdr_s *header, char *name) {
    LOG_DEBUG("header=%p name=%s", header, name);

    u32 hash = NameToHash(name);

    i32 r = BinarySearch(hash, header->hash_idxs, header->file_count);

    if (r != -1) {
        return r;
    }

    if (header->hash_count != 0) {
        VARIPTR ptr;

        ptr.char_ptr = header->hashes;

        for (i32 i = 0; i < header->hash_count; i++) {
            if (NuStrCmp(ptr.char_ptr, name) == 0) {
                ptr.addr += NuStrLen(ptr.char_ptr) + 1;

                if ((ptr.addr & 1) != 0) {
                    ptr.addr++;
                }

                return (int)*ptr.short_ptr;
            }

            ptr.addr += NuStrLen(ptr.char_ptr) + 3;

            if ((ptr.addr & 1) != 0) {
                ptr.addr++;
            }
        }
    }

    return -1;
}

i32 datsys_offset = 0;

i32 NuDatFileFindTree(nudathdr_s *header, char *name) {
    char buf[256];
    char *backslash;
    i32 node_idx;

    LOG_DEBUG("header=%p name=%s", header, name);

    if (*name == '@') {
        name = name + 4;
    }

    NuStrCpy(buf, name);
    NuFileUpCase(0, buf);

    name = buf;

    if (header->file_tree == NULL) {
        return NuDatFileFindHash(header, name);
    }

    node_idx = header->file_tree->child_idx;

    backslash = strchr(name, L'\\');
    if (backslash != NULL) {
        *backslash = '\0';
    }

    do {
        if (NuStrICmp(header->file_tree[node_idx].name, name) == 0) {
            if (backslash == NULL) {
                if (header->file_tree[node_idx].child_idx < 1) {
                    return -header->file_tree[node_idx].child_idx;
                }

                return -1;
            }

            node_idx = header->file_tree[node_idx].child_idx;

            name = backslash + 1;

            backslash = strchr(name, L'\\');
            if (backslash != NULL) {
                *backslash = '\0';
            }
        } else {
            node_idx = header->file_tree[node_idx].sibling_idx;
        }
    } while (node_idx != 0);

    return -1;
}

i64 NuDatCalcPos(nudathdr_s *header, i32 index) {
    i64 pos = index;

    if (header->version < -1) {
        pos <<= 8;
    }

    pos += datsys_offset;

    return pos;
}
