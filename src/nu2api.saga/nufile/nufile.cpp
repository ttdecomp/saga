#include "decomp.h"

#include "deflate/deflate.h"
#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/export.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuthread/nuthread.h"

namespace NuFile {
    namespace SeekOrigin {
        typedef int32_t T;
    };

    namespace OpenMode {
        typedef int T;
    }
}; // namespace NuFile

class NuFileAndroidAPK {
  public:
    static SAGA_NOMATCH NUFILE OpenFile(const char *filepath, NuFile::OpenMode::T mode) {
        UNIMPLEMENTED("android specific");
    }

    static SAGA_NOMATCH int32_t CloseFile(NUFILE file) {
        UNIMPLEMENTED("android specific");
    }

    static SAGA_NOMATCH int64_t SeekFile(NUFILE file, int64_t offset, NuFile::SeekOrigin::T mode) {
        UNIMPLEMENTED("android specific");
    }

    static SAGA_NOMATCH int32_t ReadFile(NUFILE file, void *buf, int size) {
        UNIMPLEMENTED("android specific");
    }

    static SAGA_NOMATCH int64_t GetFilePos(NUFILE file) {
        UNIMPLEMENTED("android specific");
    }
    static SAGA_NOMATCH int64_t GetFileSize(NUFILE file) {
        UNIMPLEMENTED("android specific");
    }
};

class NuFileDeviceAndroidAPK {};

NuFileDeviceAndroidAPK *g_apkFileDevice;

static NUDATHDR *curr_dat;
int32_t nufile_try_packed = 0;
static FILEINFO file_info[33];
int nufile_buffering_enabled;
static int nufile_lasterror = 0;
NUDATFILEINFO dat_file_infos[20];

NUFILE_DEVICE host_device = {
    0,
    0,
    0xe,
    0,
    '/',

    0x400,
    -1,
    -1,

    -1,
    {0, 0, 0, 0},
    "d:",
    2,
    2,
    "",
    "Host Xbox 360 filesystem",
    "",
    "",
    "",

    &DEV_FormatName,
    &DEVHOST_Interrogate,
};

NUFILE_DEVICE *default_device = &host_device;

static int file_time_count;
static FileBuffer file_buff[4];

static void AquireFileBuffer(FILEINFO *info) {
    int i;
    int buf_idx;
    int time;
    FileBuffer *buf;
    int bytes_read;

    buf_idx = 0;
    time = file_time_count;

    for (i = 0; i < 4; i++) {
        if (file_buff[i].time < time) {
            time = file_buff[i].time;
            buf_idx = i;
        }
    }

    buf = &file_buff[buf_idx];
    if (buf->file_info != NULL) {
        buf->file_info->buf = NULL;
    }

    buf->file_info = info;
    buf->time = file_time_count++;
    info->buf = buf;

    if (info->buf_len != 0) {
        NuPSFileLSeek(info->handle, -info->buf_len, NUFILE_SEEK_CURRENT);
        bytes_read = NuPSFileRead(info->handle, buf->data, info->buf_len);
    }
}

NUFILE NuFileOpen(char *filepath, NUFILEMODE mode) {
    return NuFileOpenDF(filepath, mode, curr_dat, 0);
}

void NuFileClose(NUFILE file) {
    FILEINFO *info;

    if (file >= 0x2000) {
        NuFileAndroidAPK::CloseFile(file);
        return;
    }

    if (file >= 0x1000) {
        NuMcClose(file - 0x1000, 0);
        return;
    }

    if (file >= 0x400) {
        NuMemFileClose(file);
        return;
    }

    file -= 1;

    while (NuPSFileClose(file) < 0) {
    }

    info = &file_info[file];
    if (info->buf != NULL) {
        info->buf->file_info = NULL;
    }

    memset(info, 0, sizeof(fileinfo_s));
}

NUFILE NuFileOpenDF(char *filepath, NUFILEMODE mode, NUDATHDR *header, int32_t _unused) {
    char buf[256];
    int64_t len;
    NUFILE_DEVICE *device;
    int mc_slot;
    int mc_port;
    int ps_index;
    int file_index;
    NUFILE file_handle;

    device = NuFileGetDeviceFromPath(filepath);
    if (device == NULL) {
        if (mode != NUFILE_WRITE && mode != NUFILE_APPEND) {
            file_handle = 0;

            if (header != NULL) {
                file_handle = NuDatFileOpen(header, filepath, mode);
            } else {
                if (g_apkFileDevice != NULL) {
                    file_handle = NuFileAndroidAPK::OpenFile(filepath, 0);
                }
            }

            if (file_handle > 0) {
                return file_handle;
            }
        }

        device = default_device;
    }

    if ((device->attr & 1) != 0 && mode == NUFILE_WRITE && mode == NUFILE_APPEND) {
        device = &host_device;
    }

    (*device->format_name_fn)(device, buf, filepath, 0x100);

    if (device->id == 2) {
        NuStrCat(buf, ";1");
    }

    if (device->id == 1) {
        mc_port = buf[2] - 0x30;
        mc_slot = buf[3] - 0x30;

        file_index = NuMcOpen(mc_port, mc_slot, buf + device->name_len, mode, 0);
        if (file_index >= 0) {
            file_index += 0x1000;
        } else {
            file_index = 0;
        }

        return file_index;
    } else {
        file_index = NuPSFileOpen(buf, mode);
        if (file_index < 0) {
            return 0;
        }

        ps_index = file_index;

        memset(&file_info[ps_index], 0, sizeof(FILEINFO));

        file_info[ps_index].handle = file_index;
        file_info[ps_index].mode = mode;

        if (mode != NUFILE_WRITE) {
            if (file_index <= 16) {
                do {
                    len = NuFileSeek(ps_index + 1, 0, NUFILE_SEEK_END);
                } while (len < 0);

                file_info[ps_index].file_len = len;

                if (mode == NUFILE_READ_NOWAIT) {
                    while (NuFileStatus(ps_index + 1) != 0) {
                    }
                }

                do {
                    len = NuFileSeek(ps_index + 1, 0, NUFILE_SEEK_START);
                } while (len < 0);

                if (mode == NUFILE_READ_NOWAIT) {
                    while (NuFileStatus(ps_index + 1) != 0) {
                    }
                }
            }
        } else {
            file_info[ps_index].file_len = 0;
        }

        if (mode == NUFILE_READ) {
            file_info[ps_index].use_buf = nufile_buffering_enabled;
        }
    }

    return ps_index + 1;
}

int64_t NuFileSeek(NUFILE file, int64_t offset, NUFILESEEK whence) {
    FILEINFO *info;
    int native_mode;
    int64_t pos;

    if (file >= 0x2000) {
        switch (whence) {
            default:
                native_mode = 0;
                break;
            case NUFILE_SEEK_CURRENT:
                native_mode = 1;
                break;
            case NUFILE_SEEK_END:
                native_mode = 2;
                break;
        }

        return NuFileAndroidAPK::SeekFile(file, offset, native_mode);
    }

    if (file >= 0x1000) {
        pos = (int64_t)NuMcSeek(file - 0x1000, offset, whence, 0);
        return pos;
    }

    if (file >= 0x400) {
        pos = NuMemFileSeek(file, (int)offset, whence);
        return pos;
    }

    file -= 1;
    info = &file_info[file];

    if (info->use_buf) {
        switch (whence) {
            default:
                info->read_pos = offset;
                break;
            case NUFILE_SEEK_CURRENT:
                info->read_pos += offset;
                break;
            case NUFILE_SEEK_END:
                info->read_pos = info->file_len - offset;
                break;
        }
    } else {
        pos = NuPSFileLSeek(file, offset, whence);
        return pos;
    }

    return info->read_pos;
}

int64_t NuFileSize(char *filepath) {
    int file;
    int64_t pos;
    int buffering;

    buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;

    if (curr_dat != NULL) {
        file = NuDatFileFindTree(curr_dat, filepath);
        if (file >= 0) {
            return (uint64_t)curr_dat->file_info[file].file_len;
        }
    }

    pos = -1;

    if (filepath != NULL && *filepath != '\0') {
        int file;

        file = NuFileOpen(filepath, NUFILE_READ);
        if (file != 0) {
            do {
                pos = NuFileSeek(file, 0, NUFILE_SEEK_END);
            } while (pos < 0);

            pos = NuFilePos(file);
            NuFileClose(file);
        }
    }

    nufile_buffering_enabled = buffering;

    return pos;
}

int64_t NuFilePos(NUFILE file) {
    FILEINFO *info;
    int64_t pos;

    if (file >= 0x2000) {
        return NuFileAndroidAPK::GetFilePos(file);
    }

    if (file >= 0x1000) {
        pos = NuMcSeek(file - 0x1000, 0, NUFILE_SEEK_END, 0);
        return pos;
    }

    if (file >= 0x400) {
        return NuMemFilePos(file);
    }

    file -= 1;
    info = &file_info[file];

    if (info->use_buf) {
        return info->read_pos;
    }

    do {
        pos = NuPSFileLSeek(file, 0, NUFILE_SEEK_CURRENT);
    } while (pos < 0);

    return pos;
}

int64_t NuFileOpenSize(NUFILE file) {
    if (file >= 0x2000) {
        return NuFileAndroidAPK::GetFilePos(file);
    }

    if (file >= 0x1000) {
        return NuMcFileOpenSize(file);
    }

    if (file >= 0x800) {
        return NuDatFileOpenSize(file);
    }

    if (file >= 0x400) {
        return NuMemFileOpenSize(file);
    }

    file -= 1;

    return file_info[file].file_len;
}

int NuFileRead(NUFILE file, void *buf, int size) {
    FILEINFO *info;
    int bytes_read;
    char *char_buf;
    int available;

    if (file >= 0x2000) {
        return NuFileAndroidAPK::ReadFile(file, buf, size);
    }

    if (file >= 0x1000) {
        return NuMcRead(file - 0x1000, buf, size, 0);
    }

    if (file >= 0x400) {
        return NuMemFileRead(file, buf, size);
    }

    file -= 1;
    info = &file_info[file];

    if (info->use_buf) {
        if (info->buf == NULL) {
            AquireFileBuffer(info);
        }

        bytes_read = 0;
        char_buf = (char *)buf;

        while (size > 0) {
            if (info->read_pos >= info->file_len) {
                return bytes_read;
            }

            if (info->read_pos < info->buf_start || info->read_pos >= info->buf_start + info->buf_len) {
                if (info->read_pos != info->file_pos) {
                    NuPSFileLSeek(info->handle, info->read_pos, NUFILE_SEEK_START);
                    info->file_pos = info->read_pos;
                }

                info->buf_len = NuPSFileRead(info->handle, info->buf->data, 0x400);
                info->buf_start = info->file_pos;
                info->file_pos += info->buf_len;
            }

            available = MIN(info->buf_len + (int)(info->buf_start - info->read_pos), size);
            if (available != 0) {
                unsigned int to_read;
                void *file_buf;

                to_read = available;
                file_buf = info->buf;
                memcpy(char_buf, (void *)((int)file_buf + (info->read_pos - info->buf_start) + 8), to_read);
            }

            char_buf += available;
            bytes_read += available;
            size -= available;
            info->read_pos += available;
        }

        return bytes_read;
    } else {
        return NuPSFileRead(file, buf, size);
    }
}

int32_t NuFileLoadBuffer(char *filepath, void *buf, int size) {
    int j;
    int iVar2;
    longlong lVar1;
    int i;

    nufile_lasterror = 0;
    i = 0;

    if ((curr_dat == NULL) || (i = NuDatFileLoadBuffer(curr_dat, filepath, buf, size), nufile_lasterror != -1)) {
        if (i == 0) {
            j = NuFileExists(filepath);
            if (j == 0) {
                nufile_lasterror = -2;
            } else {
                j = NuFileOpen(filepath, NUFILE_READ);
                if (j == 0) {
                    nufile_lasterror = -3;
                } else {
                    if (nufile_try_packed == 0) {
                        i = NuFileOpenSize(j);
                        if ((size < i) || (i == 0)) {
                            nufile_lasterror = -1;
                            i = 0;
                        } else {
                            while (iVar2 = NuFileRead(j, buf, i), iVar2 < 0) {
                                do {
                                    lVar1 = NuFileSeek(j, 0, NUFILE_SEEK_START);
                                } while (lVar1 < 0);
                            }
                        }
                    } else {
                        // i = NuPPLoadBuffer(j, dest, size);
                        UNIMPLEMENTED("PP");
                    }
                    NuFileClose(j);
                }
            }
        }
    } else {
        i = 0;
    }

    return i;
}

int32_t NuFileLoadBufferVP(char *filepath, VARIPTR *buf, VARIPTR *buf_end) {
    int32_t len;

    len = NuFileLoadBuffer(filepath, buf->void_ptr, buf_end->addr - buf->addr);

    buf->addr += len;

    return len;
}

void NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path) {
    char separator;

    if (device != NULL) {
        separator = device->dir_separator;
    } else {
        separator = '\\';
    }

    for (; *path != '\0'; path++) {
        if (*path == '\\' || *path == '/') {
            *path = separator;
        }
    }
}

void NuFileReldirFix(NUFILE_DEVICE *device, char *path) {
    char *cursor;
    char *inner;

    cursor = inner = path;

    while (*cursor != '\0') {
        if (*cursor == device->dir_separator && cursor[1] == '.' && cursor[2] == '.' &&
            cursor[3] == device->dir_separator) {
            inner = cursor;

            while (inner > path) {
                inner--;
                if (*inner == device->dir_separator) {
                    cursor += 3;
                    break;
                }

                if (*inner == ':') {
                    inner++;
                    cursor += 4;
                    break;
                }
            }
        }

        *inner = *cursor;
        inner++;
        cursor++;
    }

    *inner = '\0';
}

int32_t NuFileStatus(NUFILE file) {
    int _unused;
    int _unused2;
    NUDATFILEINFO *info;
    NUDATOPENFILEINFO *open_info;

    _unused = 0;

    if (file >= 0x2000) {
        do {
        } while (true);
    }

    if (file >= 0x400) {
        return 0;
    }

    if (file >= 0x800) {
        file -= 0x800;
        info = dat_file_infos + file;
        open_info = &info->hdr->open_files[info->open_file_idx];

        return NuFileStatus(open_info->dat_file);
    }

    file -= 1;
    _unused2 = 0;

    return _unused;
}

static NUDATFILEINFO *unpack_file_info = NULL;
static NUDATOPENFILEINFO *unpack_file_odi = 0;
static int64_t unpack_file_pos;
static int32_t decode_buffer_left;
static char decode_buffer[0x40000];
static int32_t decode_buffer_pos;
static char read_buffer[0x40000];
static int32_t read_buffer_size;
static int32_t read_buffer_decoded_size;

static void NuDatFileDecodeNewFile(NUDATFILEINFO *file, NUDATOPENFILEINFO *open_file) {
    unpack_file_info = file;
    unpack_file_odi = open_file;
    unpack_file_pos = file->start;
    decode_buffer_left = 0;
}

static void NuDatFileDecodeNext() {
    char char_buf[12];
    int int_buf[3];
    int compressed_size;
    NUDATFILEINFO *info;
    NUDATOPENFILEINFO *open_info;

    switch (unpack_file_info->compression_mode) {
        case 2:
            NuFileRead(unpack_file_odi->dat_file, char_buf, 0xc);

            read_buffer_decoded_size = ExplodeBufferSize(char_buf);
            read_buffer_size = ExplodeCompressedSize(char_buf) - 0xc;
            NuFileRead(unpack_file_odi->dat_file, read_buffer, read_buffer_size);

            unpack_file_info->pos += read_buffer_size + 0xc;
            unpack_file_odi->pos = unpack_file_info->pos;
            break;
        case 3:
            NuFileRead(unpack_file_odi->dat_file, int_buf, 0xc);

            read_buffer_decoded_size = int_buf[2];
            read_buffer_size = int_buf[1];
            NuFileRead(unpack_file_odi->dat_file, read_buffer, read_buffer_size);

            unpack_file_info->pos += read_buffer_size + 0xc;
            unpack_file_odi->pos = unpack_file_info->pos;
            break;
    }
}

static void APIEndianSwap(void *data, int count, int size) {
}

NUDATHDR *NuDatOpen(char *filepath, VARIPTR *buf, int *_unused) {
    return NuDatOpenEx(filepath, buf, _unused, 0);
}

NUDATHDR *NuDatOpenEx(char *filepath, VARIPTR *buf, int *_unused, short mode) {
    char _unused2[256];
    int path_len;
    int i;
    int j;
    NUFILE file;
    int64_t seek_offset;
    int file_len;
    int bytes_read;
    int _unused4;
    int read_buf[2];
    int total_read;
    int n;
    NUDATHDR *hdr;
    NUDFNODE_V1 *old_ver;
    VARIPTR hash;
    unsigned int hash_idx;
    int idx_to_swap;
    unsigned int tmp_idx;
    NUDATFINFO tmp;
    int dfnodev2_size;
    int dfnodev1_size;
    int buffering;
    int _unused3;

    _unused3 = 0;
    total_read = 0;

    buffering = nufile_buffering_enabled;

    file = NuFileOpenDF(filepath, (NUFILEMODE)mode, NULL, 0);
    if (file != 0) {
        bytes_read = 0;

        file_len = NuFileOpenSize(file);
        bytes_read = NuFileRead(file, read_buf, 8);
        APIEndianSwap(read_buf, 2, 4);

        seek_offset = (int64_t)read_buf[0];
        if (seek_offset < 0) {
            seek_offset *= -0x100;
        }

        NuFileSeek(file, seek_offset, NUFILE_SEEK_START);

        path_len = NuStrLen(filepath) + 0x10 & -0x10;

        hdr = (NUDATHDR *)buf->void_ptr;
        buf->addr += sizeof(NUDATHDR);
        memset(hdr, 0, sizeof(NUDATHDR));

        hdr->_unknown = 1;

        hdr->name = (char *)buf->void_ptr;
        buf->addr += path_len;
        NuStrCpy(hdr->name, filepath);

        bytes_read = NuFileRead(file, &hdr->version, 4);
        total_read += bytes_read;
        APIEndianSwap(&hdr->version, 1, 4);

        bytes_read = NuFileRead(file, &hdr->file_count, 4);
        total_read += bytes_read;
        APIEndianSwap(&hdr->file_count, 1, 4);

        hdr->file_info = (NUDATFINFO *)((buf->addr + 0x1f) & -0x20);
        buf->addr = (buf->addr + 0x1f) & -0x20;
        buf->addr += hdr->file_count * sizeof(NUDATFINFO);
        bytes_read = NuFileRead(file, hdr->file_info, hdr->file_count * sizeof(NUDATFINFO));
        total_read += bytes_read;
        for (i = 0; i < hdr->file_count; i++) {
            APIEndianSwap(&hdr->file_info[i].file_offset, 1, 4);
            APIEndianSwap(&hdr->file_info[i].file_len, 1, 4);
            APIEndianSwap(&hdr->file_info[i].decompressed_len, 1, 4);
        }

        bytes_read = NuFileRead(file, &hdr->tree_node_count, 4);
        total_read += bytes_read;
        APIEndianSwap(&hdr->tree_node_count, 1, 4);

        hdr->file_tree = (NUDFNODE *)buf->void_ptr;
        buf->addr += hdr->tree_node_count * sizeof(NUDFNODE);

        if (hdr->version < -4) {
            // Using `sizeof()` causes this not to match.
            dfnodev2_size = hdr->tree_node_count * 0xc;
            bytes_read = NuFileRead(file, hdr->file_tree, dfnodev2_size);
            total_read += bytes_read;
        } else {
            dfnodev1_size = hdr->tree_node_count * sizeof(NUDFNODE_V1);
            bytes_read = NuFileRead(file, hdr->file_tree, dfnodev1_size);
            total_read += bytes_read;
            old_ver = (NUDFNODE_V1 *)hdr->file_tree;

            for (n = hdr->tree_node_count - 1; n > 0; n--) {
                hdr->file_tree[n]._unknown = 0;
                hdr->file_tree[n]._unknown2 = 0;
                hdr->file_tree[n].name = old_ver[n].name;
                hdr->file_tree[n].sibling_idx = old_ver[n].sibling_idx;
                hdr->file_tree[n].child_idx = old_ver[n].child_idx;
            }
        }

        for (n = 0; n < hdr->tree_node_count; n++) {
            APIEndianSwap(&hdr->file_tree[n].child_idx, 1, 2);
            APIEndianSwap(&hdr->file_tree[n].sibling_idx, 1, 2);
            APIEndianSwap(&hdr->file_tree[n].name, 1, 4);

            if (hdr->version < -4) {
                APIEndianSwap(&hdr->file_tree[n]._unknown, 1, 2);
                APIEndianSwap(&hdr->file_tree[n]._unknown2, 1, 2);
            }
        }

        bytes_read = NuFileRead(file, &hdr->leaf_names_len, 4);
        total_read += bytes_read;
        APIEndianSwap(&hdr->leaf_names_len, 1, 4);

        hdr->leaf_names = buf->char_ptr;
        buf->addr += hdr->leaf_names_len;
        bytes_read = NuFileRead(file, hdr->leaf_names, hdr->leaf_names_len);
        total_read += bytes_read;

        for (n = 0; n < hdr->tree_node_count; n++) {
            _unused4 = 0;
            hdr->file_tree[n].name = (int)hdr->leaf_names + hdr->file_tree[n].name - _unused4;
        }

        hdr->file_tree[0].name = NULL;

        buf->addr -= hdr->leaf_names_len;
        buf->addr = buf->addr - hdr->tree_node_count * sizeof(NUDFNODE);
        hdr->file_tree = NULL;
        hdr->leaf_names = NULL;

        hdr->hash_idxs = NULL;
        hdr->hash_count = 0;
        hdr->hashes_len = 0;
        hdr->hashes = NULL;

        if (hdr->version < -1) {
            hdr->hash_idxs = (unsigned int *)((buf->addr + 0x1f) & -0x20);
            buf->addr = (buf->addr + 0x1f) & -0x20;
            buf->addr += hdr->file_count * sizeof(int32_t);
            bytes_read = NuFileRead(file, hdr->hash_idxs, hdr->file_count * sizeof(int32_t));
            total_read += bytes_read;

            for (n = 0; n < hdr->file_count; n++) {
                APIEndianSwap(&hdr->hash_idxs[n], 1, 4);
            }

            bytes_read = NuFileRead(file, &hdr->hash_count, 4);
            total_read += bytes_read;
            APIEndianSwap(&hdr->hash_count, 1, 4);

            bytes_read = NuFileRead(file, &hdr->hashes_len, 4);
            total_read += bytes_read;
            APIEndianSwap(&hdr->hashes_len, 1, 4);

            hdr->hashes = (char *)((buf->addr + 0x1f) & -0x20);
            buf->addr = ((buf->addr + 0x1f) & -0x20);
            buf->addr += hdr->hashes_len;
            bytes_read = NuFileRead(file, hdr->hashes, hdr->hashes_len);
            total_read += bytes_read;

            hash.char_ptr = hdr->hashes;
            for (n = 0; n < hdr->hash_count; n++) {
                hash.addr += NuStrLen(hash.char_ptr) + 1;
                if ((hash.addr & 1) != 0) {
                    hash.addr += 1;
                }

                APIEndianSwap(hash.char_ptr, 1, 2);
                hash.addr += 2;
            }
        }

        for (n = 0; n < 20; n++) {
            hdr->open_files[n].info_idx = -1;
            hdr->open_files[n].dat_file = 0;
            hdr->open_files[n].pos = 0;
        }

        hdr->open_files[0].dat_file = file;
        hdr->open_files[0].pos = 0;

        hdr->mode = mode;

        if (hdr->version > -3) {
            for (i = 0; i < hdr->file_count - 1; i++) {
                hash_idx = hdr->hash_idxs[i];
                idx_to_swap = i;

                for (j = i + 1; j < hdr->file_count; j++) {
                    if (hdr->hash_idxs[j] <= hash_idx) {
                        hash_idx = hdr->hash_idxs[j];
                        idx_to_swap = j;
                    }
                }

                if (i != idx_to_swap) {
                    tmp_idx = hdr->hash_idxs[i];
                    hdr->hash_idxs[j] = hdr->hash_idxs[idx_to_swap];
                    hdr->hash_idxs[idx_to_swap] = tmp_idx;

                    tmp = hdr->file_info[i];
                    hdr->file_info[i] = hdr->file_info[idx_to_swap];
                    hdr->file_info[idx_to_swap] = tmp;
                }
            }
        }

        return hdr;
    }

    nufile_buffering_enabled = buffering;

    return NULL;
}

NUDATHDR *NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

static int OpenDatFileBase(NUDATHDR *hdr, int file_idx) {
    NUDATOPENFILEINFO *open_file;

    open_file = hdr->open_files + file_idx;
    if (open_file->dat_file == 0) {
        open_file->dat_file = NuFileOpenDF(hdr->name, (NUFILEMODE)hdr->mode, NULL, 0);
        open_file->pos = 0;
    }

    return open_file->dat_file;
}

NUFILE NuDatFileOpen(NUDATHDR *hdr, char *filepath, NUFILEMODE mode) {
    int node_idx;
    int info_idx;
    int file_idx;
    NUDATOPENFILEINFO *open_info;
    NUDATFILEINFO *info;
    int64_t start;
    int64_t seek_pos;

    if (mode == NUFILE_READ) {
        node_idx = NuDatFileFindTree(hdr, filepath);
        if (node_idx > -1 && hdr->file_info[node_idx].file_len != 0) {
            info_idx = NuDatFileGetFreeInfo();
            if (info_idx != -1) {
                file_idx = NuDatFileGetFreeHandleIX(hdr, info_idx);
                if (file_idx != -1) {
                    open_info = &hdr->open_files[file_idx];

                    NuThreadCriticalSectionBegin(file_criticalsection);

                    OpenDatFileBase(hdr, file_idx);

                    dat_file_infos[info_idx].hdr = hdr;
                    dat_file_infos[info_idx].start = NuDatCalcPos(hdr, hdr->file_info[node_idx].file_offset);
                    dat_file_infos[info_idx].pos = dat_file_infos[info_idx].start;

                    dat_file_infos[info_idx].file_len = hdr->file_info[node_idx].file_len;
                    dat_file_infos[info_idx].decompressed_len = hdr->file_info[node_idx].decompressed_len;
                    dat_file_infos[info_idx].compression_mode = hdr->file_info[node_idx].compression_mode;

                    dat_file_infos[info_idx].open_file_idx = file_idx;

                    open_info->pos = dat_file_infos[info_idx].pos;

                    do {
                        seek_pos = NuFileSeek(open_info->dat_file, dat_file_infos[info_idx].start, NUFILE_SEEK_START);
                    } while (seek_pos < 0);

                    if (dat_file_infos[info_idx].compression_mode == 2 ||
                        dat_file_infos[info_idx].compression_mode == 3) {
                        NuDatFileDecodeNewFile(&dat_file_infos[info_idx], open_info);
                    }

                    NuThreadCriticalSectionEnd(file_criticalsection);

                    return info_idx + 0x800;
                }

                dat_file_infos[info_idx].used = 0;
            }
        }
    }

    return 0;
}

void NuDatFileClose(NUFILE file) {
    NUDATFILEINFO *info;
    NUDATHDR *hdr;

    file -= 0x800;
    info = dat_file_infos + file;
    hdr = info->hdr;

    if (info->open_file_idx >= 0) {
        hdr->open_files[info->open_file_idx].info_idx = -1;
    }

    info->used = 0;
}

int64_t NuDatFileSeek(NUFILE file, int64_t seek, NUFILESEEK whence) {
    int32_t index = NUFILE_INDEX_DAT(file);

    nudathdr_s *hdr = dat_file_infos[index].hdr;
    int ofile = dat_file_infos[index].open_file_idx;

    int64_t offset;

    if (whence == NUFILE_SEEK_CURRENT) {
        offset = seek + dat_file_infos[index].start;
    } else if (whence == NUFILE_SEEK_END) {
        int64_t end = (int64_t)dat_file_infos[index].file_len + dat_file_infos[index].start;

        offset = end - seek;
    } else {
        offset = seek + dat_file_infos[index].start;
    }

    int64_t pos = NuFileSeek(hdr->open_files[ofile].dat_file, offset, NUFILE_SEEK_START);

    dat_file_infos[index].pos = pos;
    hdr->open_files[ofile].pos = pos;

    return pos;
}

int64_t NuDatFilePos(NUFILE file) {
    NUDATFILEINFO *info;

    file -= 0x800;
    info = dat_file_infos + file;

    return info->pos - info->start;
}

int NuDatFileRead(NUFILE file, void *buf, int size) {
    NUDATFILEINFO *info;
    NUDATOPENFILEINFO *open_file;
    int inflated_size;
    int total_read;
    int bytes_read;
    int to_read;

    file -= 0x800;
    info = &dat_file_infos[file];
    open_file = &info->hdr->open_files[info->open_file_idx];

    if (info->compression_mode != 0) {
        total_read = 0;

        while (size != 0) {
            if (decode_buffer_left == 0) {
                NuDatFileDecodeNext();

                if (read_buffer_size == read_buffer_decoded_size) {
                    memcpy(decode_buffer, read_buffer, read_buffer_size);
                } else if (info->compression_mode == 2) {
                    ExplodeBufferNoHeader(read_buffer, decode_buffer, read_buffer_size, read_buffer_decoded_size);
                } else if (info->compression_mode == 3) {
                    inflated_size =
                        InflateBuffer(decode_buffer, read_buffer_decoded_size, read_buffer, read_buffer_size);
                }

                decode_buffer_pos = 0;
                decode_buffer_left = read_buffer_decoded_size;
            }

            bytes_read = MIN(size, decode_buffer_left);
            memcpy(buf, decode_buffer + decode_buffer_pos, bytes_read);
            buf = (void *)((char *)buf + bytes_read);

            decode_buffer_pos += bytes_read;
            decode_buffer_left -= bytes_read;
            size -= bytes_read;
            total_read += bytes_read;
        }

        return total_read;
    }

    if (info->pos != open_file->pos) {
        NuFileSeek(open_file->dat_file, info->pos, NUFILE_SEEK_START);
        open_file->pos = info->pos;
    }

    to_read = MIN(size, MAX(info->file_len + info->start - info->pos, 0));
    if (to_read != 0) {
        total_read = NuFileRead(open_file->dat_file, buf, to_read);
        if (total_read > -1) {
            info->pos += total_read;
            open_file->pos = info->pos;
        }

        return total_read;
    }

    return 0;
}

int32_t NuDatFileGetFreeInfo(void) {
    int32_t found;
    int32_t i;

    found = -1;

    NuThreadCriticalSectionBegin(file_criticalsection);

    for (i = 0; i < 20; i++) {
        if (!dat_file_infos[i].used) {
            dat_file_infos[i].used = 1;
            found = i;
            break;
        }
    }

    NuThreadCriticalSectionEnd(file_criticalsection);

    return found;
}

int32_t NuDatFileOpenSize(NUFILE file) {
    NUDATFILEINFO *info;

    file -= 0x800;
    info = &dat_file_infos[file];

    if (info->compression_mode != 0) {
        return info->decompressed_len;

    } else {
        return info->file_len;
    }
}

int32_t NuDatFileLoadBuffer(nudathdr_s *dat, char *name, void *dest, int32_t buf_size) {
    NUFILE file;
    char *buf;

    nufile_lasterror = 0;

    file = NuDatFileOpen(dat, name, NUFILE_READ);

    if (file != 0) {
        int32_t size;

        if (dat->mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }

        if (dat_file_infos[file - 0x800].compression_mode != 0) {
            size = dat_file_infos[file - 0x800].decompressed_len;
        } else {
            size = dat_file_infos[file - 0x800].file_len;
        }

        buf = (char *)dest;

        if (size <= buf_size && size != 0) {
            LOG_DEBUG("Loading %d bytes from dat file", size);

            while (NuDatFileRead(file, buf, size) < 0) {
                while (NuDatFileSeek(file, 0, NUFILE_SEEK_START) < 0) {
                }
            }

            if (dat->mode == 3) {
                while (NuFileStatus(file) != 0) {
                }
            }

            NuFileClose(file);

            return size;
        }

        if (size != 0) {
            nufile_lasterror = -1;
        }

        NuFileClose(file);
    }

    return 0;
}

NUMEMFILE memfiles[20];

NUFILE NuMemFileOpen(void *buf, int buf_size, NUFILEMODE mode) {
    int i;

    if (buf_size > 0 && (mode == NUFILE_READ || mode == NUFILE_WRITE)) {
        for (i = 0; i < 20; i++) {
            if (!memfiles[i].used) {
                memfiles[i].buffer = (char *)buf;
                memfiles[i].end = (char *)buf + buf_size - 1;
                memfiles[i].ptr = memfiles[i].buffer;
                memfiles[i].mode = mode;
                memfiles[i].used = 1;

                return i + 0x400;
            }
        }
    }

    return 0;
}

void NuMemFileClose(NUFILE file) {
    if (file >= 0x2000) {
        do {
        } while (true);
    }

    if (file >= 0x800) {
        NuDatFileClose(file);
    } else {
        file -= 0x400;
        memfiles[file].used = 0;
    }
}

int NuMemFileRead(NUFILE file, void *buf, int size) {
    int remaining;

    if (file >= 0x800) {
        return NuDatFileRead(file, buf, size);
    }

    file -= 0x400;

    size = memfiles[file].end - memfiles[file].ptr + 1 <= size ? memfiles[file].end - memfiles[file].ptr + 1 : size;

    if (size != 0) {
        memcpy(buf, memfiles[file].ptr, size);

        memfiles[file].ptr = memfiles[file].ptr + size;
    }

    return size;
}

int NuMemFileWrite(NUFILE file, void *data, int size) {
    file -= 0x400;

    size = memfiles[file].end - memfiles[file].ptr + 1 <= size ? memfiles[file].end - memfiles[file].ptr + 1 : size;

    if (size != 0) {
        memcpy(memfiles[file].ptr, data, size);

        memfiles[file].ptr += size;
    }

    return size;
}

int64_t NuMemFileSeek(NUFILE file, int64_t offset, NUFILESEEK whence) {
    if (file >= 0x800) {
        return NuDatFileSeek(file, offset, whence);
    }

    file -= 0x400;

    switch (whence) {
        default:
            memfiles[file].ptr = memfiles[file].buffer + offset;
            break;
        case NUFILE_SEEK_CURRENT:
            memfiles[file].ptr = memfiles[file].ptr + offset;
            break;
        case NUFILE_SEEK_END:
            memfiles[file].ptr = memfiles[file].end - offset;
            break;
    }

    return (int64_t)(memfiles[file].ptr - memfiles[file].buffer);
}

int64_t NuMemFilePos(NUFILE file) {
    if (file >= 0x800) {
        return NuDatFilePos(file);
    }

    file -= 0x400;

    return (int64_t)(memfiles[file].ptr - memfiles[file].buffer);
}

int NuMemFileOpenSize(NUFILE file) {
    file -= 0x400;

    return memfiles[file].end - memfiles[file].buffer;
}

void *NuMemFileAddr(NUFILE file) {
    file -= 0x400;

    return (void *)memfiles[file].ptr;
}
