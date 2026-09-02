#include "nu2api/nusound/nusound.h"

extern __attribute__((visibility("hidden"))) u16 *g_NuSoundLoadBits asm("_ZL17g_NuSoundLoadBits");
extern __attribute__((visibility("hidden"))) u16 *g_NuSoundLoadBitsCache asm("_ZL22g_NuSoundLoadBitsCache");
extern __attribute__((visibility("hidden"))) i32 g_NuSoundNumLoadBitShorts asm("_ZL25g_NuSoundNumLoadBitShorts");

extern "C" {

    void NuSound3SetRequestTable(u16 *request_bits, i32 short_count) {
        if (g_NuSoundLoadBitsCache != NULL && g_NuSoundNumLoadBitShorts != short_count) {
            delete g_NuSoundLoadBitsCache;
        }

        g_NuSoundLoadBitsCache = new u16[short_count];
        g_NuSoundNumLoadBitShorts = short_count;
        g_NuSoundLoadBits = request_bits;
    }

    void NuSound3AddRumble(void) {
    }
    void NuSound3AddStream(void) {
    }
    void NuSound3AddStreamEx(void) {
    }
    void NuSound3AmplitudeTodB(void) {
    }
    void NuSound3BeginWaitUpdate(void) {
    }
    void NuSound3CancelCheckStereo(void) {
    }
    void NuSound3CheckStream(void) {
    }
    void NuSound3CheckWaitUpdate(void) {
    }
    void NuSound3ClearLoopHold(void) {
    }
    void NuSound3Close(void) {
    }
    void NuSound3CountVoices(void) {
    }
    void NuSound3DisplayVoiceInfo(void) {
    }
    void NuSound3DrawMem(void) {
    }
    void NuSound3FClose(void) {
    }
    void NuSound3FOpen(void) {
    }
    void NuSound3FOpenSize(void) {
    }
    void NuSound3FRead(void) {
    }
    void NuSound3FSeek(void) {
    }
    void NuSound3FindFree(void) {
    }
    void NuSound3FindOldestVoice(void) {
    }
    void NuSound3FindQuietestVoice(void) {
    }
    void NuSound3FlushBG(void) {
    }
    void NuSound3FlushFG(void) {
    }
    void NuSound3FlushLoops(void) {
    }
    void NuSound3GetListener(void) {
    }
    void NuSound3GetSize(void) {
    }
    void NuSound3GetStreamInfo(void) {
    }
    void NuSound3GetStreamPlaybackTime(void) {
    }
    void NuSound3HoldOffMusic(void) {
    }
    void NuSound3InitEx(void) {
    }
    void NuSound3InitLoopInfo(void) {
    }
    void NuSound3InitThreadSafeHackyMess(void) {
    }
    void NuSound3IsSampleLoaded(void) {
    }
    void NuSound3KillAllAudio(void) {
    }
    void NuSound3KillAllAudioWait(void) {
    }
    void NuSound3KillAllAudioWaitEx(void) {
    }
    void NuSound3Listener(void) {
    }
    void NuSound3LoadAllSpotFX(void) {
    }
    i32 NuSound3LoadingSfx(void) {
        return 0;
    }
    void NuSound3Play(void) {
    }
    void NuSound3Play3d(void) {
    }
    void NuSound3Play3dLoopSfx(void) {
    }
    void NuSound3Play3dPri(void) {
    }
    void NuSound3PlayChan(void) {
    }
    void NuSound3PlayInterleavedStereo(void) {
    }
    void NuSound3PlayPri(void) {
    }
    void NuSound3PlayStream(void) {
    }
    void NuSound3ReadStream(void) {
    }
    void NuSound3RemoveStreamID(void) {
    }
    void NuSound3ReserveStream(void) {
    }
    void NuSound3RestoreStreamPitch(void) {
    }
    void NuSound3SetDPL(void) {
    }
    void NuSound3SetDat(void) {
    }
    void NuSound3SetDataIopBufferSize(void) {
    }
    void NuSound3SetDuckVol(void) {
    }
    void NuSound3SetLoadCallback(void) {
    }
    void NuSound3SetLoopHoldTime(float t) {
        (void)t;
    }
    void NuSound3SetMonoIopBufferSize(void) {
    }
    i32 NuSound3SetReverb(i32) {
        return 0;
    }
    void NuSound3SetReverbVol(void) {
    }
    void NuSound3SetRumblePads(void) {
    }
    void NuSound3SetSFXPitch(void) {
    }
    void NuSound3SetSampleTableFromPakFile(void) {
    }
    void NuSound3SetStereoIopBufferSize(void) {
    }
    void NuSound3SetStreamPitch(void) {
    }
    void NuSound3SetStreamVolume(void) {
    }
    void NuSound3StopRumble(void) {
    }
    void NuSound3StopSFX(void) {
    }
    void NuSound3StopStream(void) {
    }
    void NuSound3StopVoice(void) {
    }
    void NuSound3StreamClose(void) {
    }
    void NuSound3StreamOpen(void) {
    }
    void NuSound3UpdateEx(void) {
    }
    void NuSound3UpdatePending(void) {
    }
    void NuSound3UpdateRumble(void) {
    }
    void NuSound3UpdateV(void) {
    }
}
