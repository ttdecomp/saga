#pragma once

// Shared C-API function declarations used by the Episode 1 level code
// (podrace / sprint / gungan / retake / maul handlers). These functions
// are defined across the codebase as C-linkage; consolidated here so the
// level translation units don't each need local `extern` declarations.

struct AISYS_s;
struct nuvec_s;

#ifdef __cplusplus
extern "C" {
#endif

    void *AIPathFindLocator(AISYS_s *, char *);
    void *AIPAthFindPathCnx(AISYS_s *, i32, char *, void *);
    void *AISysFindArea(void *, char *);

    void NuSpecialSetVisibility(void *, i32);
    struct nuvec_s *NuSpecialGetPos(void *);
    void *NuSpecialGetDrawMtx(void *);
    i32 NuSpecialClipTestExtents(void *, void *);
    float NuSpecialGetOriginRadius(void *);
    void NuSpecialDrawAt(void *, void *);

    i16 FindGameDebris(void *, char *);
    i32 PARTLookupType(char *);
    float AnimEndFrame(void *, i32);

    void Text3DEx(char *, i32, float, float, float, float, float, i32, i32, i32, i32, i32);
    void PlaySfx(char *);

    void *AddGameDebris(void *, i32, void *);
    void AddFiniteShotPART(i32, void *, i32);

#ifdef __cplusplus
}
#endif