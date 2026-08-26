#include "legoapi/legoapi_types.h"
#include "globals.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nuprim.h"
#include <string.h>
static i32 text_font_loaded = 0;
extern char **TTab;
extern void *QFont2D;
extern "C" {
    void NuQFntSet(void *font);
    void NuQFntSetSpaceWidth(void *font, float width);
    void NuQFntSetJustifiedTolerances(float squash, float stretch);
    void NuQFntPrintJustifiedW(void *font, u16 *text, float x, float y, float sx, float sy, float a1, float a2,
                               float a3, float a4, u32 color, int flags);
}
void Text_LoadFont(char *path, variptr_u *buf, variptr_u *buf_end) {
    (void)path;
    (void)buf;
    (void)buf_end;
    text_font_loaded = 1;
    if (QFont2D == nullptr) {
        static int d = 1;
        QFont2D = &d;
    }
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
bool Text_IsFontLoaded() {
    return text_font_loaded != 0;
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
    void TextDecode(void) {
    }
    void TextRegisterButtonMapFn(void) {
    }
    void TextRegisterPulseTimerFn(void) {
    }
    void UnloadGameFont(void) {
    }
}
void LookupHash(u32, u32 *, HashRedirect *, u32) {
}
void _make_words(abi_long *, abi_long, abi_long) {
}
void MultilineDump(char const *) {
}
void GetMatchLength(unsigned char *, unsigned char *, abi_ulong) {
}
void MakeLayerList_Name(CHARACTERMODEL_s *, i16 *, u32) {
}
void UnicodeToIndexFast(vucharidx_s *, i32, u16) {
}
void SplitTextFindNextWS(unsigned char *, i32) {
}
void MatrixTextStringEncode(void *, unsigned char *, u16 *) {
}
void GetLineW(u16 *, i32) {
}
static void Host_Text3DStringEncode(char *src, u16 *dst) {
    if (!src || !dst)
        return;
    int i = 0;
    while (src[i] && i < 511) {
        dst[i] = (u16)(unsigned char)src[i];
        i++;
    }
    dst[i] = 0;
}
extern "C" void Text3DStringEncode(char *src, u16 *dst) {
    Host_Text3DStringEncode(src, dst);
}
void SetQFont2D(void) {
    NuQFntSetCoordinateSystem(static_cast<NUQFNT_CSMODE>(3));
    void *f = QFont2D;
    if (f) {
        NuQFntSet(f);
        NuQFntSetSpaceWidth(f, 12.0f);
    }
}
static const char *kIntroFallback = "A long time ago in a galaxy far, far away....";
void IntroText_SetTextID(i32 id) {
    if (id <= 0)
        return;
    if (TTab == nullptr)
        return;
    char **tbl = *reinterpret_cast<char ***>(TTab);
    if (!tbl)
        return;
    if (id >= 0x70d)
        return;
    if (!tbl[id])
        return;
    IntroText_TextID = id;
}
void IntroText_Draw(float alpha) {
    if (IntroText_TextID == -1)
        return;
    SetQFont2D();
    {
        void *f = QFont2D;
        if (f)
            NuQFntSetSpaceWidth(f, 8.0f);
    }
    u16 enc[512];
    memset(enc, 0, sizeof(enc));
    const char *src = nullptr;
    if (TTab) {
        char **tbl = *reinterpret_cast<char ***>(TTab);
        if (tbl) {
            i32 id = IntroText_TextID;
            if (id >= 0 && id < 0x70d)
                src = tbl[id];
        }
    }
    if (!src || !src[0])
        src = kIntroFallback;
    Text3DStringEncode(const_cast<char *>(src), enc);
    NuQFntSetJustifiedTolerances(1.0f, 1.0f);
    float scaled = alpha * 128.0f;
    int ab = (int)scaled;
    if (ab < 0)
        ab = 0;
    if (ab > 255)
        ab = 255;
    u32 colour = ((u32)ab << 24) | 0x007f5f00u;
    float scale = INTROTEXT_SCALE;
    float y = INTROTEXT_Y;
    void *font = QFont2D;
#ifdef HOST_BUILD
    extern float backdrop_top_r, backdrop_top_g, backdrop_top_b;
    backdrop_top_r = 0.0f * alpha;
    backdrop_top_g = 0.37f * alpha;
    backdrop_top_b = 0.50f * alpha;
    if (alpha > 0.01f) {
        extern void NuRndrClear(u32, u32, float);
        NuRndrClear(0xf00, 0x007f5f00, alpha);
    }
#endif
    NuQFntPrintJustifiedW(font, enc, -0.85f, y, 1.0f, scale, scale, 0.425f, 1.0f, 0.0f, colour, 0);
}
