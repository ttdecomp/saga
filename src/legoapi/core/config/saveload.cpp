#include <string.h>

#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nutime.h"

extern i32 MenuLoadStarted;
extern i32 memcard_slot;
extern i32 memcard_saveneeded;
extern i32 memcard_savestarted;
extern i32 memcard_savefailed;
extern i32 memcard_loadneeded;
extern i32 memcard_loadstarted;
extern i32 memcard_loadfailed;
extern i32 memcard_loadcorrupt;
extern f32 memcard_savemessage_delay;
extern f32 memcard_saveresult_delay;
extern f32 memcard_loadmessage_delay;
extern f32 memcard_loadresult_delay;
extern i16 (*memcard_hashfn)(void);

void InitMemCard() {
}

void SaveGizmoSys(GIZMOSYS_s *, char *, char *) {
}

void SerialiseInt(EdStream &, void *, i32) {
}

void FS_GetDirList(char *, char *, char *) {
}

void SerialiseChar(EdStream &, void *, i32) {
}

i32 CanSaveAndExit(WORLDINFO_s *world) {
    extern i32 GAMEDEMO;
    extern i32 SuperStory;
    extern i32 ChallengeMode;
    extern i32 Arcade;
    extern i32 CutScenePlayer_Active(void);

    if (GAMEDEMO != 0 || SuperStory != 0 || world->area == NULL || world->area == HUB_ADATA ||
        (world->area->flags & 0x146) != 0 || Mission_Active(NULL) != NULL || ChallengeMode != 0 || Arcade != 0 ||
        CutScenePlayer_Active() != 0 || Game_AreaSave == NULL ||
        Game_AreaSave[world->level_sub_id].area_complete == 0 || AreaGlobals.values.field_0x18 <= 0) {
        return 0;
    }

    return Area_FindStatusLevel(world->area, NULL) != NULL;
}

void FS_PrevNameLen(char *) {
}

void FS_SortStrings(char *, char *, i32) {
}

void SerialiseFloat(EdStream &, void *, i32) {
}

void SerialiseNuMtx(EdStream &, void *, i32) {
}

void SerialiseNuVec(EdStream &, void *, i32) {
}

void SerialiseShort(EdStream &, void *, i32) {
}

void SerialiseVuMtx(EdStream &, void *, i32) {
}

void SerialiseVuVec(EdStream &, void *, i32) {
}

void FS_MoveCursorUp(i32) {
}

void SerialiseString(EdStream &, void *, i32) {
}

void SerialiseColour3(EdStream &, void *, i32) {
}

void FS_FileNameFilter(char *) {
}

void FS_MakeDateString(FS_FILEENTRYHDR *, char *) {
}

void FS_MakeTimeString(FS_FILEENTRYHDR *, char *) {
}

void FS_MoveCursorDown(i32) {
}

void FS_GetDirTextWidth() {
}

void FS_GetFilterString(char *, char *) {
}

void getsaveload_status() {
}

void FS_GetPadWithRepeat(nupad_s *, float, float) {
}

void SerialiseNuHSpecial(EdStream &, void *, i32) {
}

void SerialiseStringAddr(EdStream &, void *, i32) {
}

void FS_BuildFilterBlocks(char *) {
}

void saveloadGetDirectory() {
}

void FS_MakeDateTimeString(FS_FILEENTRYHDR *, char *) {
}

void FS_BuildFilterOutBlocks(char *) {
}

void FS_SetCursorToLastFileName() {
}

void LevelScriptReStoreProgress(WORLDINFO_s *, LEVELSCRIPTPROCESS_s *) {
}

void LoadState(i32, variptr_u *, variptr_u *, variptr_u *, variptr_u *, variptr_u *, variptr_u *) {
}

extern "C" {

    void FS_SetFileSelPathFromName(void) {
    }

    void SaveSystemInitialiseEx(void) {
    }

    void SetSaveSuccessFn(void) {
    }

    void UpdateSaveSlots(void) {
        const f32 elapsed = NuTimeGetFrameTime();
        if (memcard_loadmessage_delay > 0.0f) {
            memcard_loadmessage_delay -= elapsed;
        }
        if (memcard_loadresult_delay > 0.0f) {
            memcard_loadresult_delay -= elapsed;
        }
        if (memcard_savemessage_delay > 0.0f) {
            memcard_savemessage_delay -= elapsed;
        }
        if (memcard_saveresult_delay > 0.0f) {
            memcard_saveresult_delay -= elapsed;
        }

        if (memcard_savestarted != 0 && saveload_status == 1) {
            memcard_savestarted = 0;
            MenuSaveOccurred = 1;
        }

        if (memcard_saveneeded != 0 && saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0 &&
            memcard_savedata != NULL && memcard_savedatabuffer != NULL) {
            const i32 save_hash = memcard_hashfn != NULL ? memcard_hashfn() : -1;

            if (memcard_extra_savedata != NULL && memcard_extra_savedatabuffer != NULL) {
                memmove(memcard_extra_savedatabuffer, memcard_extra_savedata, memcard_extra_savedatasize);
                const i32 checksum = ChecksumSaveData(memcard_extra_savedatabuffer, memcard_extra_savedatasize);
                static_cast<i32 *>(memcard_extra_savedatabuffer)[memcard_extra_savedatasize / sizeof(i32)] = checksum;
                saveloadASSave(SAVESLOTS, memcard_extra_savedatabuffer, memcard_extra_savedatasize + 4, -1);
            }

            memmove(memcard_savedatabuffer, memcard_savedata, memcard_savedatasize);
            const i32 checksum = ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize);
            static_cast<i32 *>(memcard_savedatabuffer)[memcard_savedatasize / sizeof(i32)] = checksum;
            saveloadASSave(memcard_slot, memcard_savedatabuffer, memcard_savedatasize + 4, save_hash);

            memcard_saveneeded = 0;
            memcard_savestarted = 1;
            memcard_savefailed = 0;
        }

        if (memcard_loadstarted != 0 && saveload_status == 1) {
            memcard_loadstarted = 0;
            memcard_loadcorrupt = 0;

            const i32 stored_checksum = static_cast<i32 *>(memcard_savedatabuffer)[memcard_savedatasize / sizeof(i32)];
            const i32 computed_checksum = ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize);
            if (computed_checksum != stored_checksum) {
                memcard_loadcorrupt = 1;
                saveload_autosave = -1;
            }

            if (memcard_extra_savedata != NULL && memcard_extra_savedatabuffer != NULL) {
                const i32 stored_extra_checksum =
                    static_cast<i32 *>(memcard_extra_savedatabuffer)[memcard_extra_savedatasize / sizeof(i32)];
                const i32 computed_extra_checksum =
                    ChecksumSaveData(memcard_extra_savedatabuffer, memcard_extra_savedatasize);
                if (computed_extra_checksum != stored_extra_checksum) {
                    memcard_loadcorrupt = 1;
                    saveload_autosave = -1;
                }
            }

            if (memcard_loadcorrupt == 0) {
                memmove(memcard_savedata, memcard_savedatabuffer, memcard_savedatasize);
                if (memcard_extra_savedata != NULL && memcard_extra_savedatabuffer != NULL) {
                    memmove(memcard_extra_savedata, memcard_extra_savedatabuffer, memcard_extra_savedatasize);
                }
                MenuLoadOccurred = 1;
            }
        }

        if (memcard_loadneeded == 0 || saveload_status != 1 || saveload_cardtype != 2 || saveload_cardformatted == 0 ||
            memcard_savedata == NULL || memcard_savedatabuffer == NULL) {
            return;
        }

        if (memcard_extra_savedata != NULL && memcard_extra_savedatabuffer != NULL) {
            saveloadASLoad(SAVESLOTS, memcard_extra_savedatabuffer, memcard_extra_savedatasize + 4);
        }
        saveloadASLoad(memcard_slot, memcard_savedatabuffer, memcard_savedatasize + 4);
        memcard_loadneeded = 0;
        memcard_loadcorrupt = 0;
        if (saveload_status == 1) {
            memcard_loadstarted = 0;
            memcard_loadfailed = 1;
        } else {
            memcard_loadstarted = 1;
            memcard_loadfailed = 0;
        }
    }

    void loadsaveCallEachFrame(void) {
        saveloadASCallEachFrame();
        UpdateSaveSlots();
    }

    void saveloadASFormat(void) {
    }

    void saveloadAutoSave(void) {
    }

    void saveloadCheckCardFormatted(void) {
    }

    void saveloadCheckCardFreeSpace(void) {
    }

    void saveloadCheckCardPresent(void) {
    }

    void saveloadCheckCardType(void) {
    }

    void saveloadCheckKeyCode(void) {
    }

    void saveloadCheckSlotsUsed(void) {
    }

    void saveloadDeleteSlot(void) {
    }

    void saveloadFormatCard(void) {
    }

    void saveloadLoadIcon(void) {
    }

    void saveloadUnFormatCard(void) {
    }

    void saveloadWriteKeyCode(void) {
    }

    void TriggerAutoSave(void) {
    }

} // extern "C"
