#include "nu2api.saga/nufile/nufilepak.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"

static NUFILEPAK_ERROR fpk_err;

static unsigned int CalcChecksum(void *data, int len) {
    unsigned char *bytes;
    unsigned int sum;

    sum = 0x12345678;
    bytes = (unsigned char *)data;

    while (len != 0) {
        len--;
        sum += *(bytes++);
    }

    return sum;
}

static void DecryptData(NUFILEPAKHDR *hdr, unsigned int data_len, unsigned char *key, int key_len) {
    unsigned char *bytes;
    int i;

    data_len -= sizeof(NUFILEPAKHDR);
    i = 0;
    bytes = (unsigned char *)(hdr + 1);

    while (data_len != 0) {
        data_len--;
        *bytes = *bytes - key[i++];

        if (i >= key_len) {
            i = 0;
        }

        bytes++;
    }

    hdr->flags &= ~1;
}

void *NuFilePakLoad(char *filepath, VARIPTR *buf, VARIPTR buf_end, int alignment) {
    return NuFilePakLoadKey(filepath, buf, buf_end, alignment, NULL, 0);
}

void *NuFilePakLoadKey(char *filepath, VARIPTR *buf, VARIPTR buf_end, int alignment, unsigned char *key,
                       unsigned int key_len) {
    NUFILEPAKHDR *hdr;
    int len;
    int stored_sum;

    fpk_err = NUFILEPAK_ERROR_NONE;

    if (alignment > 0x2000) {
        fpk_err = NUFILEPAK_ERROR_ALIGNMENT;
        return NULL;
    }

    alignment = alignment >= 4 ? alignment : 4;
    buf->addr = ((buf->addr + alignment - 1) & -alignment);
    hdr = (NUFILEPAKHDR *)buf->void_ptr;

    len = NuFileLoadBufferVP(filepath, buf, &buf_end);
    if (len == 0) {
        fpk_err = NUFILEPAK_ERROR_FILENOTFOUND;
        return NULL;
    }

    if (len != hdr->file_size) {
        fpk_err = NUFILEPAK_ERROR_ALIGNMENT;
        return NULL;
    }

    switch (hdr->type) {
        // Version 1
        case 0x12345679:
            stored_sum = hdr->checksum;
            hdr->checksum = 0;
            if (CalcChecksum((void *)hdr, len) != stored_sum) {
                fpk_err = NUFILEPAK_ERROR_CHECKSUM;
                return NULL;
            }
            break;
        // Version 2
        case 0x1234567a:
            stored_sum = hdr->checksum;
            hdr->checksum = 0;
            if (CalcChecksum((void *)hdr, len) != stored_sum) {
                fpk_err = NUFILEPAK_ERROR_CHECKSUM;
                return NULL;
            }

            if ((hdr->flags & 1) != 0) {
                if (key == NULL) {
                    fpk_err = NUFILEPAK_ERROR_KEYMISMATCH;
                    return NULL;
                }

                DecryptData(hdr, len, key, key_len);

                stored_sum = hdr->decrypted_checksum;
                hdr->decrypted_checksum = 0;
                if (CalcChecksum((void *)hdr, len) != stored_sum) {
                    fpk_err = NUFILEPAK_ERROR_KEYMISMATCH;
                    return NULL;
                }
            }
            break;
        // Version 0
        case 0x12345678:
            break;
        default:
            fpk_err = NUFILEPAK_ERROR_UNSUPPORTEDTYPE;
            return NULL;
            break;
    }

    return (void *)hdr;
}

static NUFILEPAKITEM *GetItems(NUFILEPAKHDR *hdr) {
    switch (hdr->type) {
        case 0x12345678:
            return (NUFILEPAKITEM *)((NUFILEPAKHDR_V0 *)hdr + 1);
        default:
            return (NUFILEPAKITEM *)((NUFILEPAKHDR_V1 *)hdr + 1);
        case 0x1234567a:
            return (NUFILEPAKITEM *)(hdr + 1);
    }
}

int NuFilePakGetItem(void *hdr_ptr, char *item_name) {
    int i;
    NUFILEPAKITEM *items;
    NUFILEPAKHDR_V0 *hdr_v0;
    NUFILEPAKHDR *hdr;

    fpk_err = NUFILEPAK_ERROR_NONE;

    hdr_v0 = (NUFILEPAKHDR_V0 *)hdr_ptr;
    hdr = (NUFILEPAKHDR *)hdr_ptr;
    items = GetItems(hdr);

    for (i = 0; hdr->item_count > i; i++) {
        if (NuStrICmp((char *)hdr_v0 + items[i].name_offset, item_name) == 0) {
            if (items[i].attr.removed) {
                return 0;
            }

            return i + 1;
        }
    }

    return 0;
}

int NuFilePakGetItemInfo(void *hdr_ptr, int item_handle, void **addr, int *size) {
    NUFILEPAKHDR *hdr;
    NUFILEPAKITEM *item;

    fpk_err = NUFILEPAK_ERROR_NONE;

    hdr = (NUFILEPAKHDR *)hdr_ptr;
    item_handle -= 1;

    if (item_handle > -1 && item_handle < hdr->item_count) {
        item = GetItems(hdr);
        item = &item[item_handle];

        if (!item->attr.removed) {
            if (addr != NULL) {
                *addr = (void *)((char *)hdr_ptr + item->data_offset);
            }

            if (size != NULL) {
                *size = item->size;
            }

            return 1;
        }
    }

    return 0;
}
