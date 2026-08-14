#include "legoapi_types.h"

void Text_LoadFont(char *, variptr_u *, variptr_u *) {
}

void Text_MakeTime(float, i32, i32, i32, char *) {
}

void TextCrawl_Draw(float, i32, float, char *) {
}

void TextCrawl_Init(TEXTCRAWL_s *, i32, i32) {
}

void TextPulseTimer(float) {
}

void Text_InitTable(TEXTENTRY *, i32, i32) {
}

void Text_MakeScore(u32, char *) {
}

void Text_LoadStrings(variptr_u *, variptr_u *) {
}

void Text_SetLanguage(i32) {
}

void Text_IsFontLoaded() {
}

void TextDecodeCodeword(char *, char *) {
}

void TextPrintSubstring(unsigned char *, float, float, float, float, float, i32, i32) {
}

void Text_DecodeButtons(char *, char *) {
}

void Text_GetLanguagePath(i32) {
}

void Text_InitStringTable(i32, variptr_u *, variptr_u *) {
}

void Text_InitLanguageList(LANGUAGEDATA *) {
}

void Text_SetLanguage_Game(i32) {
}

void Text3DStringEncodeFont(unsigned char *, u16 *, void *) {
}

void Text_ExpandButtonString(char *, char *) {
}

void Text_InitDefaultStrings() {
}

void Text_LoadAndFixUpStrings(unsigned char *, unsigned char **, char **, i32) {
}

void Text_GetMaxOverallStrings() {
}

void Text_LocaliseDecimalPoint(char *) {
}

void Text_ExpandAllButtonStrings(char *, char *) {
}

void Text_FillInExtendedSaveInfo() {
}

void Text_InsertCommasIntoNumber(char *, char *, i32) {
}

extern "C" {

    void FixUpButtonsInFont(void) {
    }

    void LoadButtonFont(void) {
    }

    void LoadGameFont(void) {
    }

    void MatrixText(void) {
    }

    void MenuSmartTextEx(void) {
    }

    void MenuText3DEx(void) {
    }

    void Set3DGameFont(void) {
    }

    void SetGameFont(void) {
    }

    void SetQFont2D(void) {
    }

    void SmartText(void) {
    }

    void SmartTextEx(void) {
    }

    void SmartTextEx2(void) {
    }

    void SmartTextExDrop(void) {
    }

    void SmartTextGetWidescreen(void) {
    }

    void SmartTextSetFWNMode(void) {
    }

    void SmartTextSetFont(void) {
    }

    void SmartTextSetWidescreen(void) {
    }

    void SplitText(void) {
    }

    void Text3D(void) {
    }

    void Text3DEx(void) {
    }

    void Text3DEx2(void) {
    }

    void Text3DStringEncode(void) {
    }

    void TextDecode(void) {
    }

    void TextRegisterButtonMapFn(void) {
    }

    void TextRegisterPulseTimerFn(void) {
    }

    void UnloadGameFont(void) {
    }

} // extern "C"
