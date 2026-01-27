#pragma once

#include "decomp.h"
#include "globals.h"

#include "nu2api.saga/nufile/nufile.h"

class NuSoundLoader;

namespace NuSoundSystem {
    struct ChannelConfig {
        uint32_t channels;
    };
    enum class FileType : uint32_t { WAV = 0, OGG = 5 };

    NuSoundLoader *CreateFileLoader(FileType type);

}; // namespace NuSoundSystem

typedef void(NuSoundOutOfMemCallback)();

class NuSoundBuffer {};

class NuSoundStreamDesc {};

class NuSoundLoader {
  private:
    NUFILE file;
    int32_t field2_0x8;
    int32_t field3_0xc;
    NuSoundStreamDesc *desc;
    NuSoundOutOfMemCallback *oom;
    char *path;

  public:
    NuSoundLoader();

    int32_t CloseStream();
    uint64_t Deinterleave(byte *data, int length, char **dest, int count, NuSoundSystem::ChannelConfig config);
    int32_t LoadFromFile(char *name, NuSoundStreamDesc *desc, NuSoundBuffer *buffer, NuSoundOutOfMemCallback *oom);
    int32_t Load(NuSoundStreamDesc *desc, NuSoundBuffer *buffer);

    // virtual methods
    virtual ~NuSoundLoader();

    virtual NuSoundStreamDesc *CreateHeader() = 0;

    virtual int32_t OpenForStreaming(const char *path);
    virtual NuSoundBuffer *FillStreamBuffer(NuSoundBuffer *buffer, bool param2);

    virtual bool SeekRawData(uint64_t position);
    virtual bool SeekPCMSample(uint64_t sampleIndex) = 0;
    virtual bool SeekTime(double seconds) = 0;

    virtual bool OpenFileForStreaming(const char *path, bool unused);
    virtual void Close();

    virtual void *ReadHeader(NuSoundStreamDesc *desc) = 0;
    virtual uint64_t ReadData(void *buffer, uint64_t size);
};

enum LoadState {
    LOADSTATE_LOADED = 1,
};
enum ErrorState {
    ERRORSTATE_NONE = 0,
};

class NuSoundSample {
  private:
    LoadState loadState;
    ErrorState lastError;
    NuSoundSystem::FileType type;
    int32_t refCount;

  public:
    LoadState GetLoadState();
    ErrorState GetLastErrorState();

    void Reference();
    void Release();

    ErrorState Load(void *param_1, int param_2, NuSoundOutOfMemCallback *oomCallback);
};

struct nusound_filename_info_s {
    char *name;
};

#ifdef __cplusplus

nusound_filename_info_s *ConfigureMusic(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd);

#endif

C_API_START

int32_t NuSound3InitV(VARIPTR *bufferStart, VARIPTR bufferEnd, int32_t zero1, int32_t zero2);

C_API_END