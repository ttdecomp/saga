#include "legoapi/world/world_shared.h"
struct SoundTable;

i32 ActionFromQuiet(i32 idx) {
    static i16 ActionPairTab[14] = {-1};
    if (idx != -1) {
        i16 *pair = ActionPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 14;
        }
    }
    return -1;
}
i32 AmbientFromQuiet(i32 idx) {
    static i16 AmbientPairTab[2] = {-1};
    if (idx != -1) {
        i16 *pair = AmbientPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 2;
        }
    }
    return -1;
}

extern "C" void ResetSounds(void) {
}

void SetLevelSfxBits(WORLDINFO *world) {
    (void)world;
}
void ResetLevSfx(WORLDINFO *world) {
    // SFX bit array and counter in the filler8 region.
    // TODO: these offsets must be replaced with typed struct fields.
    //   0x4720 → filler8[0x2c]: SFX bit array (0x400 bytes, stride 0x10)
    //   0x4b14 → filler8[0x420]: SFX counter
    i16 *sfx = (i16 *)&world->filler8[0x2c];
    for (i32 i = 0; i < 0x40; i++) {
        sfx[i] = -1;
    }
    *(i32 *)&world->filler8[0x420] = 0;
}

void InitSpecialSfx(WORLDINFO *world) {
    (void)world;
}
void LoadSpecialSfxFile(WORLDINFO *world) {
    (void)world;
}

static __used__ bool ActionMusicFn() {
    return {};
}

static __used__ bool CheckMusicOther() {
    return {};
}

static __used__ void SetSoundFadeDistCallBackFn_LSW(WORLDINFO_s *) {
}

extern "C" {

    void GetLogicalSfxCount(void) {
    }

    void GetSfxCount(void) {
    }

    void GetSfxIdN(void) {
    }

    void GetSfxName(void) {
    }

    void IsSfxLooping(void) {
    }

    void PauseGameAudio(void) {
    }

    void PauseGameMusic(void) {
    }

    void PauseGameSfx(void) {
    }

    void PlayAltGameMusic(void) {
    }

    void PlayCutMusic(void) {
    }

    void PlayMusic(void) {
    }

    void PlaySfx(char *, struct nuvec_s *) {
    }

    void PlaySfxAndSetPitch(void) {
    }

    void PlaySfxAndSetVolume(void) {
    }

    void PlaySfxAndSetVolumeAndPitch(void) {
    }

    void PlaySfxById(void) {
    }

    void PlaySfxByIdAndSetPitch(void) {
    }

    void PlaySfxByIdAndSetVolume(void) {
    }

    void PlaySfxByIdAndSetVolumeAndPitch(void) {
    }

    void PlaySfxByIdEx(void) {
    }

    void PlayingCutMusic(void) {
    }

    void PrepareAllSounds(void) {
    }

    void RegisterSounds(void) {
    }

    void ResetPreSeek(void) {
    }

    void RestoreGameMusic(void) {
    }

    void ResumeGameAudio(void) {
    }

    void SOUND_SFXRequest_Table(void) {
    }

    void SetAPIObjPlaySfxByIdFn(void) {
    }

    void SetAudioFadeLevel(void) {
    }

    void SetCutVolume(void) {
    }

    void SetLinkedCutSceneMusic(void) {
    }

    void SetMusicVolume(void) {
    }

    void SetPreSeekStartPoint(void) {
    }

    void SetSfxBitTab_Off(void) {
    }

    void SetSfxBitTab_On(void) {
    }

    void SetSfxBit_Off(void) {
    }

    void SetSfxBit_On(void) {
    }

    void SfxBit(void) {
    }

    void SfxBitMaskTable(void) {
    }

    void SfxBitTab(void) {
    }

    void SfxBitsRestore(void) {
    }

    void SfxBitsSetAll(void) {
    }

    void SfxBitsStore(void) {
    }

    void StopAltGameMusic(void) {
    }

    void SwapMusic(void) {
    }

} // extern "C"

void PlayDieSfx(GameObject_s *) {
}

void PlayHurtSfx(GameObject_s *) {
}

void PlayJumpSfx(GameObject_s *, i32) {
}

void PlayLandSfx(GameObject_s *, i32, i32) {
}

void SfxBitTabEx(SoundTable const *, i32) {
}

void TickTockSfx() {
}

void AddFootSteps(GameObject_s *) {
}

void PlayGruntSfx(GameObject_s *) {
}

void PlaySabreSfx(char *, GameObject_s *, nuvec_s *, i32) {
}

void LevChatterSfx(char *, nuvec_s *) {
}

void PlayRepeatSfx(char *, i32, float, char, float, nuvec_s *) {
}

void ResetRepeatSfx() {
}

void SetSfxBit_OnEx(i32) {
}

void UpdateLevelSfx(WORLDINFO_s *, i32) {
}

void PlayFootStepSfx(GameObject_s *) {
}

void SetSfxBit_OffEx(i32) {
}

void UpdateRepeatSfx() {
}

void AddLevelSfxFromId(i32, i32 *, i32 *, i32) {
}

void SetSfxBitTab_OnEx(SoundTable *, i32) {
}

void SetSfxBitTab_OffEx(SoundTable *, i32) {
}

void SfxCheckMusicOnOff(OPTIONSSAVE_s *) {
}

void AddLevelSfxFromName(char *, i32 *, i32 *, i32) {
}

void AddLevelSfxGizmoSys(GIZMOSYS_s *, void *, i32 *, i32 *, i32) {
}

void LoadSfx(char const *, variptr_u *, variptr_u) {
}

void BlockSfx(GameObject_s *) {
}

void SfxBitEx(i32) {
}

void AddLevSfx(WORLDINFO_s *, nuvec_s *, char *, i32) {
}
