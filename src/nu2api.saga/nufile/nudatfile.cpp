#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nucore/nustring.h"

#include "deflate/deflate.h"

#include "decomp.h"

#include <stdlib.h>
#include <string.h>

static int32_t NameToHash(char *name) {
    int32_t hash = 0x811c9dc5;
    int32_t prime = 0x199933;

    for (char *ptr = name; *ptr != '\0'; ptr++) {
        hash = prime * (hash ^ (int32_t)*ptr);
    }

    return hash;
}

size_t BinarySearch(uint32_t element, uint32_t *array, size_t length) {
    size_t start = 0;
    size_t end = length - 1;

    while (end >= start) {
        size_t index = (start + end) / 2;
        if (array[index] == element) {
            return index;
        } else if (array[index] < element) {
            start = index + 1;
        } else {
            end = index - 1;
        }
    }

    return -1;
}

int32_t NuDatFileFindHash(nudathdr_s *header, char *name) {
    LOG_DEBUG("header=%p name=%s", header, name);

    int32_t hash = NameToHash(name);

    int32_t r = BinarySearch(hash, (uint32_t *)header->hashes, header->file_count);

    if (r == -1) {
        if (header->hash_count != 0) {
            char *ptr = header->hashes;
            for (int32_t i = 0; i < header->hash_count; i = i + 1) {
                r = NuStrCmp(ptr, name);
                if (r == 0) {
                    r = NuStrLen(ptr);
                    ptr = ptr + r + 1;
                    if (((uint)ptr & 1) != 0) {
                        ptr = ptr + 1;
                    }
                    return (int)*(short *)ptr;
                }

                r = NuStrLen(ptr);
                ptr = ptr + r + 3;

                if (((uint)ptr & 1) != 0) {
                    ptr = ptr + 1;
                }
            }
        }

        r = -1;
    }

    LOG_DEBUG("header=%p name=%s => %d", header, name, r);

    return r;
}

int32_t datsys_offset = 0;

int32_t NuDatFileFindTree(nudathdr_s *header, char *name) {
    LOG_DEBUG("header=%p name=%s", header, name);

    int32_t index;
    char buf[256];
    int32_t i;
    char *backslash;

    if (*name == '@') {
        name = name + 4;
    }
    NuStrCpy(buf, name);
    NuFileUpCase(0, buf);
    name = buf;

    if (header->file_tree == NULL) {
        index = NuDatFileFindHash(header, name);
    } else {
        i = (int)header->file_tree->child_idx;
        backslash = strchr(name, L'\\');
        if (backslash != (char *)0x0) {
            *backslash = '\0';
        }
        do {
            index = NuStrICmp(header->file_tree[i].name, name);
            if (index == 0) {
                if (backslash == (char *)0x0) {
                    if (header->file_tree[i].child_idx < 1) {
                        return -(int)header->file_tree[i].child_idx;
                    }
                    return -1;
                }
                i = (int)header->file_tree[i].child_idx;
                name = backslash + 1;
                backslash = strchr(name, L'\\');
                if (backslash != (char *)0x0) {
                    *backslash = '\0';
                }
            } else {
                i = (int)header->file_tree[i].child_idx;
            }
        } while (i != 0);
        index = -1;
    }

    return index;
}

int64_t NuDatCalcPos(nudathdr_s *header, int32_t index) {
    int64_t pos = index;

    if (header->version < -1) {
        pos <<= 8;
    }

    return pos + datsys_offset;
}
