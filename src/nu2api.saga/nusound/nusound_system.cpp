#include "nu2api.saga/nusound/nusound_system.hpp"

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nusound/nusound_buffer.hpp"
#include "nu2api.saga/nusound/nusound_bus.hpp"
#include "nu2api.saga/nusound/nusound_streamer.hpp"

#include "decomp.h"

#include <cstdio>
#include <cstring>
#include <new>

NuSoundBus *NuSoundSystem::sMasterBus = NULL;
i32 NuSoundSystem::sAllocdMemory[3] = {0};
i32 NuSoundSystem::sTotalMemory[3] = {0};
void *NuSoundSystem::sScratchMemory = NULL;
void *NuSoundSystem::sSampleMemory = NULL;
void *NuSoundSystem::sDecoderMemory = NULL;
NuSoundMemoryManager *NuSoundSystem::s_mmSample = NULL;
NuSoundMemoryManager *NuSoundSystem::s_mmDecoder = NULL;
typeof(NuSoundSystem::g_handler) NuSoundSystem::g_handler = {};
const char *NuSoundSystem::sFileExtensions[12] = {"wav", "adp", "ima", "caf", "xma", "ogg",
                                                  "dsp", "msf", "vag", "gcm", "wua", "cbx"};

NuMemoryManager *NuSoundSystem::sScratchMemMgr = NULL;

static struct : NuMemoryManager::IEventHandler {
    u32 unknown;
    void *scratch;
    u32 scratch_size;

    virtual bool AllocatePage(NuMemoryManager *manager, u32 size, u32 _unknown) {
        UNIMPLEMENTED("g_handler::AllocatePage");
    }
    virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, u32 _unknown) {
        UNIMPLEMENTED("g_handler::ReleasePage");
    }
} g_handler;

static NuMemoryManager *sScratchMemMgr;

extern "C" {
    void NuSoundInitDefaultRoutingTables(void) {
        LOG_WARN("NuSoundInitDefaultRoutingTables is not implemented");
    }
};

bool NuSoundSystem::Initialise(i32 size) {
    sTotalMemory[(i32)MemoryDiscipline::SCRATCH] = GetScratchMemorySize();
    sTotalMemory[(i32)MemoryDiscipline::DECODER] = GetDecoderMemorySize();

    sTotalMemory[(i32)MemoryDiscipline::SAMPLE] =
        size - sTotalMemory[(i32)MemoryDiscipline::SCRATCH] - sTotalMemory[(i32)MemoryDiscipline::DECODER];

    sScratchMemory = NU_ALLOC(sTotalMemory[(i32)MemoryDiscipline::SCRATCH], 4, 1, "", NUMEMORY_CATEGORY_NONE);
    sSampleMemory = NU_ALLOC(sTotalMemory[(i32)MemoryDiscipline::SAMPLE], 0x800, 1, "", NUMEMORY_CATEGORY_NONE);
    sDecoderMemory = NU_ALLOC(sTotalMemory[(i32)MemoryDiscipline::DECODER], 0x800, 1, "", NUMEMORY_CATEGORY_NONE);

    NuMemoryGet()->GetThreadMem()->SetBlockDebugCategory(sScratchMemory, 7);

    g_handler.scratch = sScratchMemory;
    g_handler.scratch_size = sTotalMemory[(i32)MemoryDiscipline::SCRATCH];

    sScratchMemMgr = NuMemoryGet()->CreateMemoryManager(&g_handler, "NuSoundSystem Memory");

    if (sTotalMemory[(i32)MemoryDiscipline::DECODER] != 0) {
        s_mmDecoder = NU_ALLOC_T(NuSoundMemoryManager, 1, "", 0);
        if (s_mmDecoder != NULL) {
            new (s_mmDecoder) NuSoundMemoryManager{};
        }

        s_mmDecoder->Init("decoder", sDecoderMemory, sTotalMemory[(i32)MemoryDiscipline::DECODER], 4, 0x800);
    }

    s_mmSample = NU_ALLOC_T(NuSoundMemoryManager, 1, "", 0);
    if (s_mmSample != NULL) {
        new (s_mmSample) NuSoundMemoryManager{};
    }

    s_mmSample->EnableDefragOnAlloc(true);
    s_mmSample->Init("sample", sSampleMemory, sTotalMemory[(i32)MemoryDiscipline::SAMPLE], 4, 0x800);

    this->samples = (NuSoundSample **)_AllocMemory(MemoryDiscipline::SCRATCH, this->sample_count * sizeof(void *), 4,
                                                   "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:348");
    memset(this->samples, 0, this->sample_count * sizeof(void *));

    if (InitAudioDevice()) {
        sMasterBus = CreateBus("Master", true);
        if (sMasterBus != 0) {
            NuSoundInitDefaultRoutingTables();
            return true;
        }
    }

    return false;
}

u32 NuSoundSystem::FreeMemory(MemoryDiscipline disc, usize address, u32 size) {
    UNIMPLEMENTED("NuSoundSystem::FreeMemory");
}

u32 NuSoundSystem::GetFreeMemory(MemoryDiscipline disc) {
    return sTotalMemory[(i32)disc] - sAllocdMemory[(i32)disc];
}

void *NuSoundSystem::_AllocMemory(MemoryDiscipline disc, u32 size, u32 align, const char *name) {
    u32 uVar1 = GetFreeMemory(disc);

    void *pvVar2 = NULL;

    if (size <= uVar1) {
        switch (disc) {
            case MemoryDiscipline::SAMPLE:
                pvVar2 = s_mmSample->Alloc(size);
                break;
            case MemoryDiscipline::DECODER:
                pvVar2 = s_mmDecoder->Alloc(size);
                break;
            case MemoryDiscipline::SCRATCH:
                pvVar2 = sScratchMemMgr->_TryBlockAlloc(size, align, 1, name, 0);
                break;
            default:
                return NULL;
        }

        if (pvVar2 != NULL) {
            sAllocdMemory[(i32)disc] = sAllocdMemory[(i32)disc] + size;
        }
    }

    return pvVar2;
}

u32 NuSoundSystem::GetStreamBufferSize() {
    return 0x20000;
}
u32 NuSoundSystem::GetScratchMemorySize() {
    return 0x4b000;
}
u32 NuSoundSystem::GetDecoderMemorySize() {
    return 0x100000;
}

NuSoundBus *NuSoundSystem::CreateBus(const char *name, bool is_master) {
    int *piVar1;
    int iVar2;

    NuSoundBus *bus = GetBus(name);

    if (bus == NULL) {
        bus = (NuSoundBus *)_AllocMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, sizeof(NuSoundBus), 4,
                                         "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:1180");

        if (bus != NULL) {
            new (bus) NuSoundBus(name, is_master);

            // TODO
            // piVar1 = *(int **)&this->field_0xb0;
            // iVar2 = *piVar1;
            //*piVar1 = (int)bus;
            // bus->field0_0x0 = iVar2;
            //*(NuSoundBus **)(iVar2 + 4) = bus;
            // bus->field1_0x4 = piVar1;
            //*(int *)&this->field_0xb4 = *(int *)&this->field_0xb4 + 1;
        }
    }

    return bus;
}

NuSoundSample *NuSoundSystem::AddSample(const char *name, FileType file_type, NuSoundSource::FeedType feed_type) {
    char buf[0x100];
    sprintf(buf, "%s.%s", name, GetFileExtension(file_type));
    NuFileNormalise(buf, 0x100, buf);

    NuSoundSample *sample = GetSample(buf);
    if (sample != NULL) {
        return sample;
    }

    if (feed_type == NuSoundSource::FeedType::ZERO) {
        sample = (NuSoundSample *)_AllocMemory(MemoryDiscipline::SCRATCH, 0x80, 4,
                                               "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:646");
        if (sample == NULL) {
            return NULL;
        }

        new (sample) NuSoundSample(buf, NuSoundSource::FeedType::ZERO);
    } else if (feed_type == NuSoundSource::FeedType::STREAMING) {

        sample = (NuSoundSample *)_AllocMemory(MemoryDiscipline::SCRATCH, 0x9c, 4,
                                               "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:654");
        if (sample == NULL) {
            return NULL;
        }

        new (sample) NuSoundStreamingSample(buf);
    } else {
        return NULL;
    }

    return sample;
}

const char *NuSoundSystem::GetFileExtension(FileType type) {
    return sFileExtensions[(i32)type];
}

NuSoundSample *NuSoundSystem::GetSample(const char *path) {
    i32 hash = GenerateHash(path);

    if (this->samples != NULL) {
        for (NuSoundSample *sample = this->samples[hash]; sample != NULL; sample = sample->next) {
            if (NuStrICmp(sample->GetName(), path) == 0) {
                return sample;
            }
        }
    }

    return NULL;
}

i32 NuSoundSystem::GenerateHash(const char *str) {
    char buf[0x100];
    NuStrUpr(buf, str);

    byte hash = 0x5;

    for (char *c = buf; *c != '\0'; c++) {
        hash = (hash * 0x21) + *c;
    }

    return hash;
}

NuSoundSystem::FileType NuSoundSystem::DetermineFileType(const char *path) {
    i32 len = NuStrLen(path);
    if (len >= 5) {
        char ext[4];
        ext[0] = path[len - 3];
        ext[1] = path[len - 2];
        ext[2] = path[len - 1];
        ext[3] = '\0';

        for (i32 i = 0; i < static_cast<i32>(FileType::_COUNT); i++) {
            if (NuStrICmp(ext, sFileExtensions[i]) == 0) {
                return static_cast<FileType>(i);
            }
        }
    }

    return FileType::INVALID;
}
