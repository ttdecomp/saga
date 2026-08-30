#include "nu2api/nusound/nusound_system.hpp"

#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nusound/nusound_bus.hpp"
#include "nu2api/nusound/nusound_decoder.hpp"
#include "nu2api/nusound/nusound_decoder_ogg.hpp"
#include "nu2api/nusound/nusound_streamer.hpp"
#include "nu2api/nusound/nusound_voice.hpp"

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
NuSoundSystem *NuSoundSystem::s_staticInstance = NULL;
NuSoundRoutingTable *NuSoundSystem::sDefaultRoutingTable = NULL;
i32 NuSoundSystem::sNumAvailableOutputDevices = 0;
i32 NuSoundSystem::sOutputConfig = 0;

NuMemoryManager *NuSoundSystem::sScratchMemMgr = NULL;

static struct : NuMemoryManager::IEventHandler {
    u32 unknown;
    void *scratch;
    u32 scratch_size;

    virtual bool AllocatePage(NuMemoryManager *manager, u32 size, u32 _unknown) {
        UNIMPLEMENTED("g_handler::AllocatePage");
        return {};
    }
    virtual bool ReleasePage(NuMemoryManager *manager, void *ptr, u32 _unknown) {
        UNIMPLEMENTED("g_handler::ReleasePage");
        return {};
    }
} g_handler;

static NuMemoryManager *sScratchMemMgr;

void NuSoundInitDefaultRoutingTables(void) {
    LOG_WARN("NuSoundInitDefaultRoutingTables is not implemented");
}

NuSoundSystem::NuSoundSystem() {

    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, 1);
    pthread_mutex_init(&this->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    // NuSoundClock::NuSoundClock(&this->clock);
    // clock_callbacks = &(this->clock).callbacks2;
    // puVar1 = &(this->clock).field3_0xc;
    // this->clock_callbacks = clock_callbacks;
    // this->field7_0x44 = puVar1;
    // this->clock_callbacks2 = clock_callbacks;
    // this->field5_0x3c = puVar1;
    // this->field3_0x34 = 0;
    // this->field6_0x40 = 0;
    // this->field9_0x4c = 0;
    // NuSoundVoiceFactoryList::NuSoundVoiceFactoryList(&this->factory_list);
    // this->field17_0x74 = (undefined1 *)&this->field6_0x40;
    // this->field18_0x78 = &this->clock_callbacks;
    // this->field14_0x68 = (undefined1 *)&this->clock_callbacks;
    // this->field15_0x6c = (undefined1 *)&this->field6_0x40;
    // this->list_start = (NuEListNode<> *)&this->field22_0x88;
    // this->list_end = (NuEListNode<> *)&this->field20_0x80;
    // this->field21_0x84 = &this->field22_0x88;
    // this->field22_0x88 = &this->field20_0x80;
    // this->tail_bus = (NuSoundBus *)&this->field29_0xa4;
    // this->field31_0xac = (NuSoundBus *)&this->field27_0x9c;
    // this->field28_0xa0 = (i32)&this->field29_0xa4;
    // this->field29_0xa4 = (NuSoundBus *)&this->field27_0x9c;
    // this->field39_0xcc = (i32)&this->field36_0xc0;
    // this->field38_0xc8 = (i32)&this->field34_0xb8;
    // this->field35_0xbc = (i32)&this->field36_0xc0;
    // this->field36_0xc0 = (i32)&this->field34_0xb8;
    // this->field13_0x64 = 0;
    // this->field45_0xe4 = (undefined1 *)&this->field41_0xd4;
    // this->field16_0x70 = 0;
    // this->voice_count = 0;
    // this->field20_0x80 = 0;
    // this->field23_0x8c = 0;
    // this->field26_0x98 = 0;
    // this->field27_0x9c = 0;
    // this->field30_0xa8 = 0;
    // this->field33_0xb4 = 0;
    // this->field34_0xb8 = 0;
    // this->field37_0xc4 = 0;
    // this->field40_0xd0 = 0;
    // this->field41_0xd4 = 0;
    // this->field44_0xe0 = 0;
    // this->field46_0xe8 = (undefined1 *)&this->field43_0xdc;
    // this->field42_0xd8 = (undefined1 *)&this->field43_0xdc;
    // this->field43_0xdc = (undefined1 *)&this->field41_0xd4;
    // this->field47_0xec = 0;
    // this->field48_0xf0 = 0;
    // this->field49_0xf4 = 0;
    this->samples = NULL;
    // this->field50_0xf8 = 0;
    this->sample_count = 0x100;
    this->voice_list_start = NULL;
    this->voice_list_end = NULL;
    this->voice_count = 0;
    // libTTapp.so ctor (0x319552): the update gate field63_0x108 starts at 1.
    this->initialised = true;
    this->engine_object = NULL;
    this->audio_engine = NULL;
    this->output_mix = NULL;
    // this->field63_0x108 = 1;
    s_staticInstance = this;
}

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

    LOG_DEBUG("this->sample_count=%d", this->sample_count);

    this->samples =
        (NuSoundSample **)_AllocMemory(MemoryDiscipline::SCRATCH, this->sample_count * sizeof(NuSoundSample *), 4,
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

// libTTapp.so 0x319640: SAMPLE/DECODER hand the block header back to the
// pool manager; SCRATCH goes through NuMemoryManager::BlockFree and accounts
// the queried block size.
u32 NuSoundSystem::FreeMemory(MemoryDiscipline disc, usize address, u32 size) {
    u32 freed = size;

    switch (disc) {
        case MemoryDiscipline::SAMPLE:
            if (s_mmSample != NULL) {
                s_mmSample->Free((NuSoundMemoryBuffer *)address);
            }
            break;
        case MemoryDiscipline::DECODER:
            if (s_mmDecoder != NULL) {
                s_mmDecoder->Free((NuSoundMemoryBuffer *)address);
            }
            break;
        case MemoryDiscipline::SCRATCH:
            // The original accounts NuMemoryManager::GetBlockSize(ptr); the
            // host _TryBlockAlloc has no block header, so the accounting uses
            // the size the alloc was charged.
            sScratchMemMgr->BlockFree((void *)address, 0);
            break;
        default:
            return 0;
    }

    sAllocdMemory[(i32)disc] = sAllocdMemory[(i32)disc] - freed;
    return freed;
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
    i32 *piVar1;
    i32 iVar2;

    NuSoundBus *bus = GetBus(name);

    if (bus == NULL) {
        bus = (NuSoundBus *)_AllocMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, sizeof(NuSoundBus), 4,
                                         "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:1180");

        if (bus != NULL) {
            new (bus) NuSoundBus(name, is_master);

            // TODO
            // piVar1 = *(i32 **)&this->field_0xb0;
            // iVar2 = *piVar1;
            //*piVar1 = (i32)bus;
            // bus->field0_0x0 = iVar2;
            //*(NuSoundBus **)(iVar2 + 4) = bus;
            // bus->field1_0x4 = piVar1;
            //*(i32 *)&this->field_0xb4 = *(i32 *)&this->field_0xb4 + 1;
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
    LOG_DEBUG("GetSample: path=%s, hash=%d", path, hash);

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

void NuSoundSystem::ReleaseFileLoader(NuSoundLoader *loader) {
    UNIMPLEMENTED();
}

NuSoundSystem::~NuSoundSystem() {
}

i32 NuSoundStreamDesc::DecodeStreamOnOpen() const {
    return 0;
}

i32 NuSoundStreamDesc::GetLoopStart() const {
    return 0;
}

i32 NuSoundStreamDesc::GetLoopEnd() const {
    return 0;
}

void NuSoundSystem::AddListener(NuSoundListener *) {
}

void NuSoundSystem::AddRoutingTable(NuSoundRoutingTable *) {
}

void NuSoundSystem::AmplitudeTodB(float) {
}

void NuSoundSystem::CalculateCrossfadeHeight(NuSoundSystem::CurveData const &, float) const {
}

void NuSoundSystem::CreateCrossfadeCurve(u32) {
}

// libTTapp.so 0x31a810: builds the "<name>_decoder" name from the source's
// name, then constructs the format-specific decoder. Only OGG streams
// (encoded format 3) get a decoder; anything else returns NULL and plays
// through the plain sample path.
NuSoundDecoder *NuSoundSystem::CreateDecoder(NuSoundSource *source) {
    const char *name = source->GetName();

    char decoded_name[256];
    u32 name_len = (u32)strlen(name);
    if (name_len >= sizeof(decoded_name) - 9) {
        name_len = sizeof(decoded_name) - 9;
    }
    memcpy(decoded_name, name, name_len);
    memcpy(decoded_name + name_len, "_decoder", 9);

    NuSoundStreamDesc *desc = source->GetStreamDesc();
    if (desc != NULL && desc->GetEncodedDataFormat() == NuSoundStreamDesc::DataFormat::THREE) {
        u32 decoder_size = 0x13c;
#ifdef HOST_BUILD
        decoder_size = sizeof(NuSoundDecoderOGG);
#endif
        NuSoundDecoderOGG *decoder = (NuSoundDecoderOGG *)this->_AllocMemory(
            NuSoundSystem::MemoryDiscipline::SCRATCH, decoder_size, 4,
            "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_system.cpp:436");

        if (decoder != NULL) {
            new (decoder) NuSoundDecoderOGG(decoded_name, source);
        }

        return decoder;
    }

    return NULL;
}

void NuSoundSystem::CreateEffect(NuSoundEffect::EffectType) {
}

void NuSoundSystem::DefragmentSampleMemory() {
}

void NuSoundSystem::DetermineFileType(NUFILETYPE) {
}

void NuSoundSystem::Disable() {
}

void NuSoundSystem::FileTypeSupported(NuSoundSystem::FileType) {
}

void NuSoundSystem::Get() {
}

void NuSoundSystem::GetAllocdMemory(NuSoundSystem::MemoryDiscipline) {
}

void NuSoundSystem::GetBufferAlignment() {
}

i32 NuSoundSystem::GetClosestSupportedConfig(i32 config) {
    // libTTapp.so 0x31bcb0: config > 7 -> 8, config >= 6 -> 6, else 2.
    if (config > 7) {
        return 8;
    }
    return (config >= 6) ? 6 : 2;
}

void NuSoundSystem::GetCrossfadeCurve(u32) const {
}

void NuSoundSystem::GetDefaultFileType(NuSoundSource::FeedType) {
}

NuSoundRoutingTable *NuSoundSystem::GetDefaultRoutingTable() {
    return sDefaultRoutingTable;
}

void NuSoundSystem::GetGfxMemorySize() {
}

void NuSoundSystem::GetLanguageString(bool) {
}

void NuSoundSystem::GetLargestMemoryFragment(NuSoundSystem::MemoryDiscipline) {
}

void NuSoundSystem::GetListeners() {
}

i32 NuSoundSystem::GetNumAvailableOutputDevices() {
    return sNumAvailableOutputDevices;
}

void NuSoundSystem::GetOldestVoice(NuSoundSample *, float &) {
}

void NuSoundSystem::GetOutputChannelConfig() {
}

void NuSoundSystem::GetPeakAllocdMemory(NuSoundSystem::MemoryDiscipline) {
}

void NuSoundSystem::GetPlatformString() {
}

void NuSoundSystem::GetQuietestVoice(NuSoundSample *, float &) {
}

void NuSoundSystem::GetRoutingTable(char const *) {
}

void NuSoundSystem::GetTotalMemory(NuSoundSystem::MemoryDiscipline) {
}

void NuSoundSystem::LoadSample(NuSoundSample *, void *, i32, NuSoundOutOfMemCallback *) {
}

void NuSoundSystem::PauseAllVoices() {
}

void NuSoundSystem::PauseVoices(i32) {
}

void NuSoundSystem::ReAllocMemory(NuSoundSystem::MemoryDiscipline, u32, u32) {
}

void NuSoundSystem::ReleaseBus(NuSoundBus *) {
}

void NuSoundSystem::ReleaseCrossfadeCurve(u32) {
}

void NuSoundSystem::ReleaseDecoder(NuSoundDecoder *decoder) {
    (void)decoder;
    UNIMPLEMENTED("NuSoundSystem::ReleaseDecoder");
}

void NuSoundSystem::ReleaseEffect(NuSoundEffect *) {
}

void NuSoundSystem::ReleaseSample(NuSoundSample *) {
}

void NuSoundSystem::RemoveListener(NuSoundListener *) {
}

void NuSoundSystem::ResumeAllVoices() {
}

void NuSoundSystem::ResumeVoices(i32) {
}

void NuSoundSystem::SetDefaultRoutingTable(NuSoundRoutingTable *) {
}

void NuSoundSystem::SetGfxMemorySize(u32) {
}

void NuSoundSystem::SetMainThreadID(NuThread *) {
}

void NuSoundSystem::Shutdown() {
}

bool NuSoundSystem::SourceRequiresDecoder(NuSoundSource *source) {
    NuSoundStreamDesc *desc = source->GetStreamDesc();
    if (desc == NULL) {
        return false;
    }

    if (desc->GetEncodedDataFormat() != desc->GetDecodedDataFormat() && desc->DecodeStreamOnOpen() == 0) {
        // A handful of short effect sounds get their own special case (they
        // are pre-decoded elsewhere).
        const char *name = source->GetName();
        if (strstr(name, "coin") != NULL || strstr(name, "counter") != NULL || strstr(name, "fs_") != NULL ||
            strstr(name, "saber") != NULL) {
            return false;
        }
        return true;
    }
    return false;
}

void NuSoundSystem::StopAllVoices() {
}

void NuSoundSystem::StopVoices(NuSoundSource const &) {
}

void NuSoundSystem::StopVoices(i32) {
}

void NuSoundSystem::UnloadAllSamples() {
}

void NuSoundSystem::UnloadSample(NuSoundSample *) {
}

void NuSoundSystem::Update(f32 frametime) {
    if (this->initialised == false) {
        return;
    }

    // Platform hook (on Android this only polls the application state).
    this->UpdateAudioDevice();

    pthread_mutex_lock(&this->mutex);

    // Pass 1: drive every platform voice's device state.
    for (NuSoundVoice *voice = this->voice_list_end; voice != NULL; voice = voice->field_0x24) {
        voice->UpdateHardwareVoice(frametime);
    }

    // Pass 2: update the engine-side mix of every playing voice; stopped
    // auto-delete voices are released.
    NuSoundVoice *voice = this->voice_list_start;
    while (voice != NULL) {
        NuSoundVoice *next = voice->field_0x28;

        NuSoundVoice::PlayState state = voice->GetState();
        if (state == NuSoundVoice::PLAYSTATE_PLAYING) {
            pthread_mutex_lock(&NuSoundWeakPtrListNode::sPtrAccessLock.mutex);
            voice->Update(frametime);
            pthread_mutex_unlock(&NuSoundWeakPtrListNode::sPtrAccessLock.mutex);
        } else if (state == NuSoundVoice::PLAYSTATE_STOPPED && voice->GetAutoDelete()) {
            this->ReleaseVoice(voice);
        }

        voice = next;
    }

    pthread_mutex_unlock(&this->mutex);
}

void NuSoundSystem::dBToAmplitude(float) {
}

NuSoundVoice *NuSoundSystem::CreateVoice(NuSoundSource *source, bool loop) {
    NuSoundVoice *voice;

    if (this->SourceRequiresDecoder(source)) {
        // Encoded sources (OGG) play through a decoder that owns a decode
        // thread; the decoder becomes the voice's source.
        NuSoundDecoder *decoder = this->CreateDecoder(source);
        decoder->OpenStream(loop);
        if (decoder->IsStreamOpen() == false) {
            this->ReleaseDecoder(decoder);
            return NULL;
        }
        NuSoundStreamDesc *desc = decoder->GetStreamDesc();
        NuSoundVoiceFactory *factory = this->factory_list.GetFactory(desc->GetDecodedDataFormat());
        voice = factory->CreateVoice(decoder, loop);
        if (voice == NULL) {
            decoder->CloseStream();
            this->ReleaseDecoder(decoder);
            return NULL;
        }
    } else {
        if (source->IsStreamOpen() == false) {
            return NULL;
        }
        NuSoundStreamDesc *desc = source->GetStreamDesc();
        NuSoundVoiceFactory *factory = this->factory_list.GetFactory(desc->GetDecodedDataFormat());
        voice = factory->CreateVoice(source, loop);
        if (voice == NULL) {
            return NULL;
        }
    }

    // Append the voice to the system's voice list.
    pthread_mutex_lock(&this->mutex);
    voice->field_0x24 = this->voice_list_end;
    voice->field_0x28 = NULL;
    if (this->voice_list_end != NULL) {
        this->voice_list_end->field_0x28 = voice;
    } else {
        this->voice_list_start = voice;
    }
    this->voice_list_end = voice;
    this->voice_count++;
    pthread_mutex_unlock(&this->mutex);

    return voice;
}

void NuSoundSystem::ReleaseVoice(NuSoundVoice *voice) {
    pthread_mutex_lock(&this->mutex);

    // Detach effects (releasing the ones the system owns).
    for (NuEListNode<NuSoundEffect> *node = voice->effects_start; node != NULL;) {
        NuSoundEffect *effect = node->data;
        NuEListNode<NuSoundEffect> *next = node->next;
        voice->RemoveEffect(effect);
        // effect->field_0x24 marks system-owned effects; release them.
        this->ReleaseEffect(effect);
        node = next;
    }

    // Streaming sources opened through a decoder close their stream here.
    NuSoundDecoder *decoder = NULL;
    if (this->SourceRequiresDecoder(voice->sound_source)) {
        decoder = (NuSoundDecoder *)voice->sound_source;
    }

    // Unlink from the voice list.
    if (voice->field_0x24 != NULL) {
        voice->field_0x24->field_0x28 = voice->field_0x28;
    } else {
        this->voice_list_start = voice->field_0x28;
    }
    if (voice->field_0x28 != NULL) {
        voice->field_0x28->field_0x24 = voice->field_0x24;
    } else {
        this->voice_list_end = voice->field_0x24;
    }
    voice->field_0x24 = NULL;
    voice->field_0x28 = NULL;
    if (this->voice_count > 0) {
        this->voice_count--;
    }

    // libTTapp.so 0x31b394: run the voice's complete destructor (vtable slot
    // 0, no free), then hand the block back through FreeMemory(SCRATCH).
    voice->~NuSoundVoice();
    NuSoundSystem::FreeMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, (usize)voice, 0);

    if (decoder != NULL) {
        decoder->CloseStream();
        this->ReleaseDecoder(decoder);
    }

    pthread_mutex_unlock(&this->mutex);
}

void NuSound3ExitThreads() {
}

void NuSound_GetAllocdSampleMemory() {
}
