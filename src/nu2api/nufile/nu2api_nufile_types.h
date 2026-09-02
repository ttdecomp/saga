#ifndef NU2API_NUFILE_TYPES_H
#define NU2API_NUFILE_TYPES_H
#pragma once

#include "nu2api/nucore/common.h"

struct NuFile;
struct NuFileAndroidAPK;
struct NuFileBase;
struct NuFileDevice;
struct NuFileDeviceAndroidAPK;
struct NuFileDeviceType;

struct NuFileDeviceType {};

struct NuFile {
    struct InitData {};
    struct OpenMode {
        struct T {};
    };
};
struct NuFileAndroidAPK {
    static NuFileAndroidAPK *ms_fileId[0x400];

    void Init();
    void Open(char const *, NuFile::OpenMode::T);
    static void ResetId(i32 id);
    static i32 SetFileId(NuFileAndroidAPK *file);
};
struct NuFileBase {
    void Closedown();
    void Init();
};
struct NuFileDevice {
    void AddDevice(NuFileDevice *);
    void AddPathRule(NuFileDeviceType, char const *);
    void AllocDirectoryHandle(char const *);
    void ClearPathRules();
    void FileOpen(char const *, NuFile::OpenMode::T) const;
    void FileSize(char const *) const;
    void FormatName(char *, i32, char const *) const;
    void FreeDirectoryHandle(i32);
    void GetDeviceByType(NuFileDeviceType);
    void GetDeviceFromDirectoryHandle(i32);
    void GetDeviceFromPath(char const *);
    void Interrogate();
    NuFileDevice();
    void QueryInstallProgress();
    void SetCurrentDir(char const *);
    void SetDefaultDevice(NuFileDeviceType);
    void SetLabel(char *);
    void SetMountName(char *);
    virtual ~NuFileDevice();
};
struct NuFileDeviceAndroidAPK {
    void CreateNuFile(char const *, NuFile::OpenMode::T) const;
    NuFileDeviceAndroidAPK(char const *, NuFile::InitData const &);
    virtual ~NuFileDeviceAndroidAPK();
};

#endif // NU2API_NUFILE_TYPES_H
