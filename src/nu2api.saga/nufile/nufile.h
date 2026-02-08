#pragma once

#include "nu2api.saga/nucore/common.h"

typedef i32 NUFILE;

typedef enum nufilemode_e {
    NUFILE_READ = 0,
    NUFILE_WRITE = 1,
    NUFILE_APPEND = 2,
    NUFILE_READ_NOWAIT = 3,
    NUFILE_READWRITE = 4,
    NUFILE_MODE_CNT = 5,
} NUFILEMODE;

typedef struct filebuff_s {
    struct fileinfo_s *file_info;
    int time;
    char data[1024];
} FILEBUFF;

typedef struct fileinfo_s {
    int handle;
    i64 read_pos;
    i64 file_pos;
    i64 file_len;
    i64 buf_start;
    int buf_len;
    int use_buf;
    int unused;
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
    i32 file_offset;
    i32 file_len;
    i32 decompressed_len;
    i32 compression_mode;
} NUDATFINFO;

typedef struct nudfnodev1_s {
    i16 child_idx;
    i16 sibling_idx;
    char *name;
} NUDFNODE_V1;

typedef struct nudfnodev2_s {
    i16 child_idx;
    i16 sibling_idx;
    char *name;
    i16 unknown;
    i16 unknown2;
} NUDFNODE;

typedef struct nudatopenfileinfo_s {
    NUFILE dat_file;
    int info_idx;
    i64 pos;
} NUDATOPENFILEINFO;

typedef struct nudathdr_s {
    i32 version;
    i32 file_count;
    NUDATFINFO *file_info;
    i32 tree_node_count;
    NUDFNODE *file_tree;
    i32 leaf_names_len;
    char *leaf_names;
    u32 *hash_idxs;
    i32 hash_count;
    i32 hashes_len;
    char *hashes;
    NUDATOPENFILEINFO open_files[20];
    i16 unknown;
    i16 mode;
    i32 unknown2;
    char *name;
} NUDATHDR;

typedef struct nudatfileinfo_s {
    NUDATHDR *hdr;
    i64 start;
    i64 pos;
    int file_len;
    int decompressed_len;
    int open_file_idx;
    int is_used;
    int compression_mode;
} NUDATFILEINFO;

typedef i32 NUPSFILE;

enum NUFILE_OFFSETS {
    NUFILE_OFFSET_PS = 1,
    NUFILE_OFFSET_MEM = 0x400,
    NUFILE_OFFSET_DAT = 0x800,
    NUFILE_OFFSET_MC = 0x1000,
    NUFILE_OFFSET_NATIVE = 0x2000,
};

typedef struct fileextinfo_s {
    char extension[13];
    char type;
    char platform;
    char len;
} FILEEXTINFO;

extern int read_critical_section;

#ifdef __cplusplus
namespace NuFile {
    namespace SeekOrigin {
        enum T {
            START = 0,
            CURRENT = 1,
            END = 2,
        };
    };

    namespace OpenMode {
        enum T {
            READ = 0,
            WRITE = 1,
        };
    }

    class IFile {
      public:
        // Return types uncertain.
        // NOLINTBEGIN
        virtual i32 GetCapabilities() const {}
        virtual const char *GetFilename() const {}
        virtual u32 GetType() const {}
        virtual i64 Seek(i64 offset, SeekOrigin::T) {}
        virtual isize Read(void *buf, usize size) {}
        virtual isize Write(const void *buf, usize size) {}
        virtual i64 GetPos() const {}
        virtual i64 GetSize() const {}
        virtual void Close() {}
        virtual void Flush() {}
        // NOLINTEND
    };
}; // namespace NuFile

class NuFileBase : public NuFile::IFile {
  public:
    NuFileBase(const char *filepath, NuFile::OpenMode::T mode, u32 type);

    virtual i32 GetCapabilities() const override;
    virtual const char *GetFilename() const override;
    virtual u32 GetType() const override;

    virtual i64 GetPos() const override;
    virtual i64 GetSize() const override;

    virtual void Flush() override;

  protected:
    virtual ~NuFileBase();

  private:
    // Type uncertain.
    i32 unknown;

    u32 type;
    NuFile::OpenMode::T mode;

    char filepath[0x100];
};

extern "C" {
#endif
    extern char g_datfileMode;
    extern i32 NuFile_SwapEndianOnWrite;

    int DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, int buf_size);
    int DEVHOST_Interrogate(NUFILE_DEVICE *device);

    void NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path);
    void NuFileReldirFix(NUFILE_DEVICE *device, char *path);

    // NuFile stuff
    NUFILE NuFileOpen(char *filepath, NUFILEMODE mode);
    void NuFileClose(NUFILE file);
    i32 NuFileStatus(NUFILE file);
    NUFILE NuFileOpenDF(char *filepath, NUFILEMODE mode, NUDATHDR *header, i32 _unused);
    int NuFileRead(NUFILE file, void *buf, int size);
    int NuFileWrite(NUFILE file, void *data, int size);
    NUFILE_DEVICE *NuFileGetDeviceFromPath(char *path);
    i64 NuFileOpenSize(NUFILE file);
    i64 NuFileSeek(NUFILE file, i64 offset, NUFILESEEK seekMode);
    i32 NuFileLoadBuffer(char *filepath, void *buf, int buf_size);
    i32 NuFileLoadBufferVP(char *filepath, VARIPTR *buf, VARIPTR *buf_end);
    i32 NuFileExists(char *name);
    i64 NuFileSize(char *filepath);
    i64 NuFilePos(NUFILE file);
    void NuFileUpCase(NUFILE_DEVICE *device, char *filepath);

    // read types
    i8 NuFileReadChar(NUFILE file);
    i32 NuFileReadDir(NUFILE file);
    f32 NuFileReadFloat(NUFILE file);
    i32 NuFileReadInt(NUFILE file);
    i16 NuFileReadShort(NUFILE file);
    u8 NuFileReadUnsignedChar(NUFILE file);
    u32 NuFileReadUnsignedInt(NUFILE file);
    u16 NuFileReadUnsignedShort(NUFILE file);
    u16 NuFileReadWChar(NUFILE file);

    i32 NuFileWriteInt(NUFILE file, i32 value);
    u32 NuFileWriteUnsignedInt(NUFILE file, u32 value);

    // Platform-specific file functions
    i32 NuGetFileHandlePS(void);
    i32 NuPSFileOpen(char *filepath, NUFILEMODE mode);
    i32 NuPSFileClose(i32 index);
    i32 NuPSFileRead(i32 index, void *dest, i32 len);
    i32 NuPSFileWrite(i32 index, const void *src, i32 len);
    i64 NuPSFileLSeek(i32 index, i64 offset, NUFILESEEK whence);

    // Memory file functions
    NUFILE NuMemFileOpen(void *buf, int buf_size, NUFILEMODE mode);
    void NuMemFileClose(NUFILE file);
    int NuMemFileRead(NUFILE file, void *buf, int size);
    int NuMemFileWrite(NUFILE file, void *data, int size);
    i64 NuMemFileSeek(NUFILE file, i64 offset, NUFILESEEK whence);
    i64 NuMemFilePos(NUFILE file);
    void *NuMemFileAddr(NUFILE file);

    // Memory card functions
    int NuMcOpen(int port, int slot, char *filepath, int mode, int async);
    i32 NuMcClose(int fd, int async);
    int NuMcSeek(int fd, int offset, NUFILESEEK mode, int async);
    int NuMcOpenSize(int fd);
    int NuMcRead(int fd, void *buf, int size, int async);
    int NuMcWrite(int fd, void *data, int size, int async);

    // NuDat functions
    NUDATHDR *NuDatOpen(char *filepath, VARIPTR *buf, int *_unused);
    NUDATHDR *NuDatOpenEx(char *filepath, VARIPTR *buf, int *_unused, i16 mode);
    void NuDatFileClose(NUFILE file);
    NUDATHDR *NuDatSet(NUDATHDR *header);
    i32 NuDatFileOpenSize(NUFILE file);

    // NuDatFile functions
    NUFILE NuDatFileOpen(NUDATHDR *hdr, char *filepath, NUFILEMODE mode);
    i64 NuDatFilePos(NUFILE file);
    int NuDatFileRead(NUFILE file, void *buf, int size);
    i64 NuDatFileSeek(NUFILE file, i64 offset, NUFILESEEK whence);
    i32 NuDatFileFindTree(NUDATHDR *header, char *name);
    i32 NuDatFileLoadBuffer(NUDATHDR *dat, char *name, void *dest, i32 maxSize);

    int NuPPLoadBuffer(NUFILE file, void *buf, int buf_size);

    void NuPSFileInitDevices(int device_id, int reboot_iop, int eject);
    int NuFileInitEx(int device_id, int reboot_iop, int eject);
    void NuFileInit(int device_id);
#ifdef __cplusplus
}
#endif

int NuMemFileOpenSize(NUFILE file);
int NuMcFileOpenSize(NUFILE file);

#ifdef __cplusplus

i64 NuDatCalcPos(NUDATHDR *header, i32 index);

i32 NuDatFileFindHash(NUDATHDR *header, char *name);
i32 NuDatFileGetFreeInfo(void);
i32 NuDatFileGetFreeHandleIX(NUDATHDR *hdr, i32 info_idx);

#endif
