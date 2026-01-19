#pragma once

#include <stdint.h>

#include "decomp.h"

#include "nu2api.saga/nucore/common.h"

typedef int32_t NUFILE;

typedef enum nufilemode_e {
    NUFILE_READ = 0,
    NUFILE_WRITE = 1,
    NUFILE_APPEND = 2,
    NUFILE_READ_NOWAIT = 3,
    NUFILE_READWRITE = 4,
} NUFILEMODE;

typedef struct filebuff_s {
    struct fileinfo_s *file_info;
    int time;
    char data[1024];
} FILEBUFF;

typedef struct fileinfo_s {
    int handle;
    int64_t read_pos;
    int64_t file_pos;
    int64_t file_len;
    int64_t buf_start;
    int buf_len;
    int use_buf;
    int _unused;
    int mode;
    FILEBUFF *buf;
} FILEINFO;

struct nufile_device_s;

typedef int nufiledevFormatName(struct nufile_device_s *, char *, char *, int);
typedef int nufiledevInterrogate(struct nufile_device_s *);

typedef struct nufile_device_s {
    int id;
    int unit_id;
    int attr;
    int status;
    char dir_separator;

    int max_name_size;
    int max_ext_size;
    int max_dir_entries;

    int free_space;
    int params[4];
    char name[16];
    int match_len;
    int name_len;
    char root[32];
    char desc[64];
    char cur_dir[128];
    char sys_dir[128];
    char dll_dir[128];

    nufiledevFormatName *format_name_fn;
    nufiledevInterrogate *interrogate_fn;
} NUFILE_DEVICE;

typedef struct numemfile_s {
    char *buffer;
    char *end;
    char *ptr;
    NUFILEMODE mode;
    int used;
} NUMEMFILE;

enum nufileseek_e {
    NUFILE_SEEK_START = 0,
    NUFILE_SEEK_CURRENT = 1,
    NUFILE_SEEK_END = 2,
};
typedef enum nufileseek_e NUFILESEEK;

typedef struct nudatinfo_s {
    int file_offset;
    int file_len;
    int decompressed_len;
    int compression_mode;
} NUDATFINFO;

typedef struct nudfnodev1_s {
    short child_idx;
    short sibling_idx;
    char *name;
} NUDFNODE_V1;

typedef struct nudfnodev2_s {
    int16_t child_idx;
    int16_t sibling_idx;
    char *name;
    int16_t _unknown;
    int16_t _unknown2;
} NUDFNODE;

typedef struct nudatopenfileinfo_s {
    NUFILE dat_file;
    int info_idx;
    int64_t pos;
} NUDATOPENFILEINFO;

typedef struct nudathdr_s {
    int32_t version;
    int32_t file_count;
    NUDATFINFO *file_info;
    int32_t tree_node_count;
    NUDFNODE *file_tree;
    int32_t leaf_names_len;
    char *leaf_names;
    uint32_t *hash_idxs;
    int32_t hash_count;
    int32_t hashes_len;
    char *hashes;
    NUDATOPENFILEINFO open_files[20];
    short _unknown;
    short mode;
    int _unknown2;
    char *name;
} NUDATHDR;

typedef struct nudatfileinfo_s {
    NUDATHDR *hdr;
    int64_t start;
    int64_t pos;
    int file_len;
    int decompressed_len;
    int open_file_idx;
    int used;
    int compression_mode;
} NUDATFILEINFO;

typedef int32_t NUPSFILE;

enum NUFILE_OFFSETS {
    NUFILE_OFFSET_PS = 1,
    NUFILE_OFFSET_MEM = 0x400,
    NUFILE_OFFSET_DAT = 0x800,
    NUFILE_OFFSET_MC = 0x1000,
    NUFILE_OFFSET_NATIVE = 0x2000,
};

#define NUFILE_PS(index) (NUFILE)(index + NUFILE_OFFSET_PS)
#define NUFILE_MEM(index) (NUFILE)(index + NUFILE_OFFSET_MEM)
#define NUFILE_DAT(index) (NUFILE)(index + NUFILE_OFFSET_DAT)
#define NUFILE_MC(index) (NUFILE)(index + NUFILE_OFFSET_MC)
#define NUFILE_NATIVE(index) (NUFILE)(index + NUFILE_OFFSET_NATIVE)
#define NUFILE_IS_PS(handle) ((handle) < NUFILE_OFFSET_MEM)
#define NUFILE_IS_MEM(handle) (((handle) >= NUFILE_OFFSET_MEM) && ((handle) < NUFILE_OFFSET_DAT))
#define NUFILE_IS_DAT(handle) (((handle) >= NUFILE_OFFSET_DAT) && ((handle) < NUFILE_OFFSET_MC))
#define NUFILE_IS_MC(handle) (((handle) >= NUFILE_OFFSET_MC) && ((handle) < NUFILE_OFFSET_NATIVE))
#define NUFILE_IS_NATIVE(handle) ((handle) >= NUFILE_OFFSET_NATIVE)
#define NUFILE_INDEX_PS(handle) ((handle) - NUFILE_OFFSET_PS)
#define NUFILE_INDEX_MEM(handle) ((handle) - NUFILE_OFFSET_MEM)
#define NUFILE_INDEX_DAT(handle) ((handle) - NUFILE_OFFSET_DAT)
#define NUFILE_INDEX_MC(handle) ((handle) - NUFILE_OFFSET_MC)
#define NUFILE_INDEX_NATIVE(handle) ((handle) - NUFILE_OFFSET_NATIVE)
#define NUFILE_INVALID ((NUFILE)(-1))

C_API_START

int DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, int buf_size);
int DEVHOST_Interrogate(NUFILE_DEVICE *device);

void NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path);
void NuFileReldirFix(NUFILE_DEVICE *device, char *path);

// NuFile stuff
NUFILE NuFileOpen(char *filepath, NUFILEMODE mode);
void NuFileClose(NUFILE file);
int32_t NuFileStatus(NUFILE file);
NUFILE NuFileOpenDF(char *filepath, NUFILEMODE mode, NUDATHDR *header, int32_t _unused);
int NuFileRead(NUFILE file, void *buf, int size);
NUFILE_DEVICE *NuFileGetDeviceFromPath(char *path);
int64_t NuFileOpenSize(NUFILE file);
int64_t NuFileSeek(NUFILE file, int64_t offset, NUFILESEEK seekMode);
int32_t NuFileLoadBuffer(char *filepath, void *buf, int buf_size);
int32_t NuFileLoadBufferVP(char *filepath, VARIPTR *buf, VARIPTR *buf_end);
int32_t NuFileExists(char *name);
int64_t NuFileSize(char *filepath);
int64_t NuFilePos(NUFILE file);
void NuFileUpCase(void *param_1, char *path);

// read types
int8_t NuFileReadChar(NUFILE file);
int32_t NuFileReadDir(NUFILE file);
float NuFileReadFloat(NUFILE file);
int32_t NuFileReadInt(NUFILE file);
int16_t NuFileReadShort(NUFILE file);
uint8_t NuFileReadUnsignedChar(NUFILE file);
uint32_t NuFileReadUnsignedInt(NUFILE file);
uint16_t NuFileReadUnsignedShort(NUFILE file);
uint16_t NuFileReadWChar(NUFILE file);

// Platform-specific file functions
int32_t NuGetFileHandlePS(void);
int32_t NuPSFileOpen(char *filepath, NUFILEMODE mode);
int32_t NuPSFileClose(int32_t index);
int32_t NuPSFileRead(int32_t index, void *dest, int32_t len);
int32_t NuPSFileWrite(int32_t index, const void *src, int32_t len);
int64_t NuPSFileLSeek(int32_t index, int64_t offset, NUFILESEEK whence);

// Memory file functions
NUFILE NuMemFileOpen(void *buf, int buf_size, NUFILEMODE mode);
void NuMemFileClose(NUFILE file);
int NuMemFileRead(NUFILE file, void *buf, int size);
int NuMemFileWrite(NUFILE file, void *data, int size);
int64_t NuMemFileSeek(NUFILE file, int64_t offset, NUFILESEEK whence);
int64_t NuMemFilePos(NUFILE file);
void *NuMemFileAddr(NUFILE file);

// Memory card functions
int NuMcOpen(int port, int slot, char *filepath, int mode, int async);
int32_t NuMcClose(int fd, int async);
int NuMcSeek(int fd, int offset, NUFILESEEK mode, int async);
int NuMcOpenSize(int fd);
int NuMcRead(int fd, void *buf, int size, int async);

// NuDat functions
NUDATHDR *NuDatOpen(char *filepath, VARIPTR *buf, int *_unused);
NUDATHDR *NuDatOpenEx(char *filepath, VARIPTR *buf, int *_unused, short mode);
void NuDatFileClose(NUFILE file);
NUDATHDR *NuDatSet(NUDATHDR *header);
int32_t NuDatFileOpenSize(NUFILE file);

// NuDatFile functions
NUFILE NuDatFileOpen(NUDATHDR *hdr, char *filepath, NUFILEMODE mode);
int64_t NuDatFilePos(NUFILE file);
int NuDatFileRead(NUFILE file, void *buf, int size);
int64_t NuDatFileSeek(NUFILE file, int64_t offset, NUFILESEEK whence);
int32_t NuDatFileFindTree(NUDATHDR *header, char *name);
int32_t NuDatFileLoadBuffer(NUDATHDR *dat, char *name, void *dest, int32_t maxSize);

int NuPPLoadBuffer(NUFILE file, void *buf, int buf_size);

C_API_END

int NuMemFileOpenSize(NUFILE file);
int NuMcFileOpenSize(NUFILE file);

#ifdef __cplusplus

int64_t NuDatCalcPos(NUDATHDR *header, int32_t index);

int32_t NuDatFileFindHash(NUDATHDR *header, char *name);
int32_t NuDatFileGetFreeInfo(void);
int32_t NuDatFileGetFreeHandleIX(NUDATHDR *header, int32_t freeIndex);

#endif
