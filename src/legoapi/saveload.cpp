#include "legoapi_types.h"

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

void CanSaveAndExit(WORLDINFO_s *) {
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
        }

        void loadsaveCallEachFrame(void) {
        }

        void saveloadASCallEachFrame(void) {
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

} // extern "C"
