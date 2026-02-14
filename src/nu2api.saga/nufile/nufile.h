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

typedef enum {
    NUFILETYPE_UNKNOWN = -1,
    NUFILETYPE_ANIM = 0,
    NUFILETYPE_BSANIM = 1,
    NUFILETYPE_CHARACTER = 2,
    NUFILETYPE_SCENE = 3,
    NUFILETYPE_TEXTURE = 4,
    NUFILETYPE_CUTSCENE = 5,
    NUFILETYPE_SFX = 6,
    NUFILETYPE_SOUNDSTREAM = 7,
    NUFILETYPE_VIDEO = 8,
    NUFILETYPE_CNT = 9,
} NUFILETYPE;

typedef struct filebuff_s {
    struct fileinfo_s *file_info;
    i32 time;
    char data[1024];
} FILEBUFF;

typedef struct fileinfo_s {
    i32 handle;
    i64 read_pos;
    i64 file_pos;
    i64 file_len;
    i64 buf_start;
    i32 buf_len;
    i32 use_buf;
    i32 unused;
    i32 mode;
    FILEBUFF *buf;
} FILEINFO;

struct nufile_device_s;

typedef i32 nufiledevFormatName(struct nufile_device_s *, char *, char *, i32);
typedef i32 nufiledevInterrogate(struct nufile_device_s *);

typedef struct nufile_device_s {
    i32 id;
    i32 unit_id;
    i32 attr;
    i32 status;
    char dir_separator;

    i32 max_name_size;
    i32 max_ext_size;
    i32 max_dir_entries;

    i32 free_space;
    i32 params[4];
    char name[16];
    i32 match_len;
    i32 name_len;
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
    i32 used;
} NUMEMFILE;

typedef enum nufileseek_e {
    NUFILE_SEEK_START = 0,
    NUFILE_SEEK_CURRENT = 1,
    NUFILE_SEEK_END = 2,
} NUFILESEEK;

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
    i32 info_idx;
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
    i32 file_len;
    i32 decompressed_len;
    i32 open_file_idx;
    i32 is_used;
    i32 compression_mode;
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

extern i32 read_critical_section;

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
        virtual i32 GetCapabilities() const {
        }

        virtual const char *GetFilename() const {
        }

        virtual u32 GetType() const {
        }

        virtual i64 Seek(i64 offset, SeekOrigin::T) {
        }

        virtual isize Read(void *buf, usize size) {
        }

        virtual isize Write(const void *buf, usize size) {
        }

        virtual i64 GetPos() const {
        }

        virtual i64 GetSize() const {
        }

        virtual void Close() {
        }

        virtual void Flush() {
        }
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

    i32 DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, i32 buf_size);
    i32 DEVHOST_Interrogate(NUFILE_DEVICE *device);

    void NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path);
    void NuFileReldirFix(NUFILE_DEVICE *device, char *path);

    // NuFile stuff
    NUFILE NuFileOpen(char *filepath, NUFILEMODE mode);
    void NuFileClose(NUFILE file);
    i32 NuFileStatus(NUFILE file);
    NUFILE NuFileOpenDF(char *filepath, NUFILEMODE mode, NUDATHDR *header, i32 _unused);
    i32 NuFileRead(NUFILE file, void *buf, i32 size);
    i32 NuFileWrite(NUFILE file, void *data, i32 size);
    NUFILE_DEVICE *NuFileGetDeviceFromPath(char *path);
    i64 NuFileOpenSize(NUFILE file);
    i64 NuFileSeek(NUFILE file, i64 offset, NUFILESEEK seekMode);
    i32 NuFileLoadBuffer(char *filepath, void *buf, i32 buf_size);
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
    NUFILE NuMemFileOpen(void *buf, i32 buf_size, NUFILEMODE mode);
    void NuMemFileClose(NUFILE file);
    i32 NuMemFileRead(NUFILE file, void *buf, i32 size);
    i32 NuMemFileWrite(NUFILE file, void *data, i32 size);
    i64 NuMemFileSeek(NUFILE file, i64 offset, NUFILESEEK whence);
    i64 NuMemFilePos(NUFILE file);
    void *NuMemFileAddr(NUFILE file);

    // Memory card functions
    i32 NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async);
    i32 NuMcClose(i32 fd, i32 async);
    i32 NuMcSeek(i32 fd, i32 offset, NUFILESEEK mode, i32 async);
    i32 NuMcOpenSize(i32 fd);
    i32 NuMcRead(i32 fd, void *buf, i32 size, i32 async);
    i32 NuMcWrite(i32 fd, void *data, i32 size, i32 async);

    // NuDat functions
    NUDATHDR *NuDatOpen(char *filepath, VARIPTR *buf, i32 *_unused);
    NUDATHDR *NuDatOpenEx(char *filepath, VARIPTR *buf, i32 *_unused, i16 mode);
    void NuDatFileClose(NUFILE file);
    NUDATHDR *NuDatSet(NUDATHDR *header);
    i32 NuDatFileOpenSize(NUFILE file);

    // NuDatFile functions
    NUFILE NuDatFileOpen(NUDATHDR *hdr, char *filepath, NUFILEMODE mode);
    i64 NuDatFilePos(NUFILE file);
    i32 NuDatFileRead(NUFILE file, void *buf, i32 size);
    i64 NuDatFileSeek(NUFILE file, i64 offset, NUFILESEEK whence);
    i32 NuDatFileFindTree(NUDATHDR *header, char *name);
    i32 NuDatFileLoadBuffer(NUDATHDR *dat, char *name, void *dest, i32 maxSize);

    i32 NuPPLoadBuffer(NUFILE file, void *buf, i32 buf_size);

    void NuPSFileInitDevices(i32 device_id, i32 reboot_iop, i32 eject);
    i32 NuFileInitEx(i32 device_id, i32 reboot_iop, i32 eject);
    void NuFileInit(i32 device_id);
#ifdef __cplusplus
}
#endif

i32 NuMemFileOpenSize(NUFILE file);
i32 NuMcFileOpenSize(NUFILE file);

#ifdef __cplusplus

i64 NuDatCalcPos(NUDATHDR *header, i32 index);

i32 NuDatFileFindHash(NUDATHDR *header, char *name);
i32 NuDatFileGetFreeInfo(void);
i32 NuDatFileGetFreeHandleIX(NUDATHDR *hdr, i32 info_idx);

#endif
