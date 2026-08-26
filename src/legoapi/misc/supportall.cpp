#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void CatchUpCode(GameObject_s *, float, float, i32) {
}

void RndrTexQuad(float, float, float, float, i32, numtl_s *, i32) {
}

void SuperWeirdo(GameObject_s *) {
}

void bgProcClose() {
}

void BurnoutApply(i32) {
}

void bgprocFreeze() {
}

void AddPartDebris(PARTDEBSYS_s *, i32, nuvec_s *) {
}

void AddSlamDebris(GameObject_s *) {
}

void CloakMovement(GameObject_s *) {
}

void RndrTexQuad3D(VuMtx const &, i32, numtl_s *) {
}

void CheckResetBits() {
}

void DebrisTimeSlip(i32) {
}

void bgProcAbortAll() {
}

void bgprocIsFrozen() {
}

void bgprocUnFreeze() {
}

void AddSurfaceDebris(GameObject_s *) {
}

void BuildDebrisVerts(PartHeader *, uv1debdata *, numtl_s *, float, numtx_s *, i32, float, float, float, float) {
}

void bgprocIsFreezing() {
}

void DebFreeWithoutKey(debkeydatatype_s *) {
}

void DebrisKillPlayers() {
}

void RndrUnfilledCircle(float, float, float, float, float, i32, float, float, numtl_s *) {
}

void DebrisProcessSpheres(uv1deb *, float, debinftype *, debkeydatatype_s *, i32) {
}

void DisplayListPrintItem(nudisplaylistitem_s *, i32, i32, i32 *, i32) {
}

// Debug-capture output helpers consumed by NuDisplayListCaptureSortPriority.
// Transcribed from the original C-linkage symbols:
//   NuHtmlBegin    0x2d5ca0   NuHtmlFlush   0x2d5c30
//   NuHtmlWrite    0x2d5cd0   NuHtmlHeading1 0x2d5d40
static char nudl_html_buf[0xc00]; // original bss buffer @0xb9d750-rel
static char *nudl_html_cursor;    // original @0xb9d720-rel
static char *nudl_html_end;       // original @0xb9d730-rel
static void *nudl_html_file;      // original file-handle pointer

void NuHtmlFlush(i32 force) {
    if (nudl_html_cursor > nudl_html_buf || force) {
        // HOST-ONLY: the original hands the buffer to NuFileWriteString on a
        // debug dump file; that API is not decompiled yet, so emit to stdout.
        fwrite(nudl_html_buf, 1, (usize)(nudl_html_cursor - nudl_html_buf), stdout);
        nudl_html_cursor = nudl_html_buf;
        nudl_html_end = nudl_html_buf + sizeof(nudl_html_buf);
    }
}

extern "C" void NuHtmlBegin(void *file) {
    nudl_html_file = file;
    nudl_html_cursor = nudl_html_buf;
    nudl_html_end = nudl_html_buf + sizeof(nudl_html_buf);
}

extern "C" void NuHtmlWrite(const char *text) {
    if (text == NULL || text[0] == '\0') {
        text = ""; // original substitutes an empty-string constant
    }
    // The original vsprintf's with an empty vararg list, i.e. a plain copy.
    usize len = strlen(text);
    if ((usize)(nudl_html_end - nudl_html_cursor) > len) {
        memcpy(nudl_html_cursor, text, len + 1);
        nudl_html_cursor += len;
    }
    NuHtmlFlush(0);
}

extern "C" void NuHtmlHeading1(const char *fmt, ...) {
    if (fmt == NULL || fmt[0] == '\0') {
        fmt = "";
    }
    NuHtmlWrite("<h1>");
    va_list ap;
    va_start(ap, fmt);
    char tmp[0xc00];
    vsnprintf(tmp, sizeof(tmp), fmt, ap);
    va_end(ap);
    NuHtmlWrite(tmp);
    NuHtmlWrite("</h1>");
}

void AddChunkToRenderStack(particlechunkrendertype_s *, particlechunkrendertype_s **) {
}

void DebrisProcessTriggers() {
}

void ValidateShaderProgram(u32) {
}

void AddChunkControlToStack(debris_chunk_control_s *, debris_chunk_control_s **) {
}

void AddDebrisEffectToStack(debkeydatatype_s *) {
}

void DebFreeChunksInstantly(i32 *) {
}

void DebrisProcessAllocation() {
}

void DebrisProcessGeneration() {
}

void DisplayListRenderBuffer() {
}

void RndrStateBuildKonstState(nuglobalrndrstate_s *) {
}

void DebrisGetControlStackLock() {
}

void DebrisProcessControlChunks(i32) {
}

void DisplayListCreateDynMtlList(variptr_u *, variptr_u) {
}

void DisplayListCreateGeomItemPS(variptr_u *, void *, numtl_s *) {
}

void DebrisDrawCalculateClipBoxes(debinftype *, debkeydatatype_s *) {
}

void CalculateDistanceToNearestEnd(nuvec_s *, SOCKPOSITION_s *, i32, SOCKSYS *) {
}

void DebrisCleanUpDmaDebTypeTables() {
}

void DebrisReleaseControlStackLock() {
}

void DisplayListGenerateTransforms(nudisplayscene_s *) {
}

void RndrStateBuildReflectionState(nuglobalrndrstate_s *) {
}

void xxxNuDisplayListUpdateSpecial(nuhspecial_s *) {
}

void CalculateDistanceToNearestSide(nuvec_s *, SOCKPOSITION_s *, i32, SOCKSYS *) {
}

void DebrisFreeOldestDmaDebTypeTable() {
}

void DisplayListCreateInstSurfGeomPS(variptr_u *, numtx_s *) {
}

void CalculateDistanceToSpecificSideOrEnd(i32, nuvec_s *, SOCKPOSITION_s *, i32, SOCKSYS *) {
}

void DebrisSingleCollisionCheckScaleYFlag(i32, nuvec_s *, float, float, unsigned char) {
}

void DebrisSingleTorusCollisionCheckScaleYFlag(i32, nuvec_s *, float, float, unsigned char) {
}

void unref(unsigned char *, unsigned char *) {
}

void TBRESET() {
}

void RootFnEx(numtx_s *, void *, nuvec_s *, nuvec_s *, nuvec_s *, float, i32) {
}

void TBOPENFN(char *, i32) {
}

void RndrArrow(float, float, float, i32, i32) {
}

void TBCLOSEFN(char *, i32) {
}
