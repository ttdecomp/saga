#pragma once

#include <stdint.h>
#include <stdio.h>

#include "decomp.h"
#include "export.h"

enum nufilemode_e {
    NUFILE_MODE_READ = 0,
    NUFILE_MODE_WRITE = 1,
    NUFILE_MODE_APPEND = 2,
    NUFILE_MODE_READ_NOWAIT = 3,
    NUFILE_MODE_READWRITE = 4,
};
typedef enum nufilemode_e NUFILEMODE;

enum nufileseek_e {
    NUFILE_SEEK_START = 0,
    NUFILE_SEEK_CURRENT = 1,
    NUFILE_SEEK_END = 2,
};
typedef enum nufileseek_e NUFILESEEK;

typedef struct NuFileDevice NuFileDevice;
typedef struct nudathdr_s NUDATHDR;

typedef int32_t NUFILE;
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

typedef struct nudatfileinfo_s nudatfileinfo_s;
typedef struct NuMemFile NuMemFile;
typedef struct fileinfo_s fileinfo_s;
typedef struct FileBuffer FileBuffer;
typedef struct NUDATHDRtruct1 NUDATHDRtruct1;
typedef struct NUDATHDRtruct2 NUDATHDRtruct2;

C_API_START

extern NuFileDevice host_device;
extern NuFileDevice *default_device;
extern int32_t numdevices;
extern NuFileDevice devices[16];
extern int32_t file_criticalsection;
extern nudatfileinfo_s dat_file_infos[20];
extern FILE *g_fileHandles[32];
extern NuMemFile memfiles[16];
extern int32_t nufile_buffering_enabled;
extern fileinfo_s file_info[32];
extern int32_t nufile_lasterror;

uint8_t DEV_FormatName(NuFileDevice *device, char *dest, char *path, int length);

void NuFileCorrectSlashes(NuFileDevice *device, char *path);
void NuFileReldirFix(NuFileDevice *device, char *path);

// NuFile stuff
NUFILE NuFileOpen(const char *path, NUFILEMODE mode);
void NuFileClose(NUFILE file);
int32_t NuFileStatus(NUFILE file);
NUFILE NuFileOpenDF(const char *path, NUFILEMODE mode, NUDATHDR *header, int32_t _unused);
int32_t NuFileRead(NUFILE index, void *dest, int32_t length);
NuFileDevice *NuFileGetDeviceFromPath(const char *path);
uint32_t NuFileOpenSize(NUFILE file);
int32_t NuFileSeek(NUFILE file, int64_t offset, NUFILESEEK seekMode);
int32_t NuFileLoadBuffer(const char *name, void *dest, int32_t size);
int32_t NuFileExists(const char *name);
uint64_t NuFileSize(const char *name);
uint64_t NuFilePos(NUFILE file);

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
int32_t NuPSFileOpen(const char *path, NUFILEMODE mode);
int32_t NuPSFileClose(int32_t index);
int32_t NuPSFileRead(int32_t index, void *dest, int32_t len);
int32_t NuPSFileWrite(int32_t index, const void *src, int32_t len);
int64_t NuPSFileLSeek(int32_t index, int64_t offset, NUFILESEEK seekMode);

// Memory file functions
size_t NuMemFileRead(NUFILE file, char *dest, size_t size);
int32_t NuMemFilePos(NUFILE file);
void NuMemFileClose(NUFILE file);
int64_t NuMemFileSeek(NUFILE file, int64_t seek, NUFILESEEK whence);

// Memory card functions
int32_t NuMcClose(int32_t, int32_t);
int64_t NuMcSeek(int32_t, int64_t, NUFILESEEK);

// NuDat functions
NUDATHDR *NuDatOpen(char *name, void **bufferBase, int32_t zero);
NUDATHDR *NuDatOpenEx(char *name, void **bufferBase, int zero, short mode);
void NuDatFileClose(NUFILE file);
NUDATHDR *NuDatSet(NUDATHDR *header);
int32_t NuDatFileOpenSize(NUFILE file);

// NuDatFile functions
NUFILE NuDatFileOpen(NUDATHDR *header, char *name, NUFILEMODE mode);
uint64_t NuDatFilePos(NUFILE file);
int32_t NuDatFileRead(NUFILE file, void *dest, int32_t size);
int64_t NuDatFileSeek(NUFILE file, int64_t offset, NUFILESEEK whence);
int32_t NuDatFileFindTree(NUDATHDR *header, char *name);
int32_t NuDatFileLoadBuffer(NUDATHDR *dat, char *name, void *dest, int32_t maxSize);

void NuFileUpCase(void *param_1, char *path);

C_API_END

#ifdef __cplusplus

int64_t NuDatCalcPos(NUDATHDR *header, int32_t index);

int32_t NuDatFileFindHash(NUDATHDR *header, char *name);
int32_t NuDatFileGetFreeInfo(void);
int32_t NuDatFileGetFreeHandleIX(NUDATHDR *header, int32_t freeIndex);
// static int32_t OpenDatFileBase(NUDATHDR *header, int32_t someIndex);

#endif
