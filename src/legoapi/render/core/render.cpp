#include "legoapi/render/core/render.h"
#include "legoapi/world/world_shared.h"
#include "globals.h"
struct starfighter_s;
struct rtl_s;
struct rtlidata_s;

#include "legoapi/render/core/SwipeDecalRenderer.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/android/nuiosdl_gl.h"
#include "nu2api/nu3d/android/nugscn_android.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuptrblock.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numath.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

extern NuVertexFormatPS *g_nuFaceOnVertexFormat;
extern NuVertexFormatPS *g_nuDebrisVertexFormat;

void DisplayListGenerateTransforms(nudisplayscene_s *scene);

struct GAMEMESSAGE_s;
struct HINT_s;
struct MENU_s;
struct NuBloomParameters;
struct STATUSPACKET_s;
struct TouchHolder;
struct _vum_s;
struct _vuv_s;
struct nuhspecial_s;
struct ripple_node_s;
struct ripple_set_s;
struct VuVec;

extern "C" void SetQFont2D(void);
extern "C" void DrawMenu(i32 paused);
extern i32 editor_active;
extern i32 Paused;

// Camera zoom state
f32 CameraZoom = 1.0f;

// Graphics loading flags
i32 RemoveDirectionalMaps = 0;
i32 RemoveNormalMaps = 0;

NUVIDEORESHEADER g_VideoResHeader;

extern "C" {
    void RndrStateCopyGlobalState(NUGLOBALRNDRSTATE *state);

    void NuDisplaySceneRndr(void *display_scene) {
        NUDLDLISTSCENE *scene = static_cast<NUDLDLISTSCENE *>(display_scene);
        if ((scene->flags & NUDL_SCENE_FLAG_CLIPPING) != 0) {
            return;
        }

        RndrStateCopyGlobalState(scene->local_state);
        if (scene->nclip_objects != 0) {
            scene->flags |= NUDL_SCENE_FLAG_CLIPPING;
        }
        DisplayListGenerateTransforms(reinterpret_cast<nudisplayscene_s *>(scene));
    }

    void NuPortalVisibility(NUGSCN *scene) {
        (void)scene;
    }
    void NuGScnRndr3(NUGSCN *scene) {
        NuDisplaySceneRndr(scene->display_list);
    }
}

void SetCameraZoom(f32 zoom) {
    CameraZoom = zoom;
}

extern "C" void NuGScnUpdate(NUGSCN *gscn, i32 param) {
    (void)gscn;
    (void)param;
}

// --- NuGScn gfx-upload helpers: C++ / file-local (static) in original ---
// NuGScnUploadGfxDataFromFilePS has C++ linkage (mangled `_Z29NuGScnUploadGfxDataFromFilePSP9variptr_uS_i`);
// NuReadGraphicsData is a C++ static function in the original (GCC clones it,
// hence the `.isra.NNN` suffix in the ROM symbol table).

extern u32 g_lastBoundVAO;

static void NuIOSBindVAO(u32 vao_handle) {
    if (vao_handle != g_lastBoundVAO) {
        g_lastBoundVAO = vao_handle;
    }
}

static u32 UploadDataToGLBuffer(NUFILE file, u32 size, GLenum target, GLuint *gl_buf, VARIPTR *buf, VARIPTR buf_end) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x56);
    glGenBuffers(1, gl_buf);
    NuIOSBindVAO(0);
    glBindBuffer(target, *gl_buf);
    glBufferData(target, size, 0, GL_STATIC_DRAW);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x5d);

    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    u32 chunk_limit = g_loadingCharacterInHub != 0 ? 0x4000 : 0x10000;
    u32 buf_size = buf_end.char_ptr - buf->char_ptr;
    u32 max_chunk_size = NuMin(chunk_limit, buf_size);
    u32 largest_chunk = 0;

    for (u32 n = 0, chunk_size = 0; n < size; n += chunk_size) {
        chunk_size = NuMin(max_chunk_size, size - n);
        NuFileRead(file, buf->void_ptr, chunk_size);

        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x73);
        NuIOSBindVAO(0);
        glBindBuffer(target, *gl_buf);
        if (chunk_size == size) {
            glBufferData(target, chunk_size, buf->void_ptr, GL_STATIC_DRAW);
        } else {
            glBufferSubData(target, n, chunk_size, buf->void_ptr);
        }
        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x80);

        if (bgProcIsBgThread()) {
            NuIOS_YieldThread();
        }
        largest_chunk = NuMax(largest_chunk, chunk_size);
    }
    return largest_chunk;
}

i32 NuGScnUploadGfxDataFromFilePS(VARIPTR *buf, VARIPTR buf_end, i32 file) {
    VARIPTR max_buf = *buf;
    i32 section_size = 0;
    i32 bytes_read = 0;

    memset(&g_VideoResHeader, 0, sizeof(g_VideoResHeader));
    bytes_read += NuFileRead(file, &section_size, sizeof(section_size));
    bytes_read += NuGScnReadTexturesPS(file, buf, buf_end);

    bytes_read += NuFileRead(file, &g_VideoResHeader.nvertex_buffers, sizeof(g_VideoResHeader.nvertex_buffers));
    g_VideoResHeader.vertex_buffers = buf->u32_ptr;
    buf->u32_ptr += g_VideoResHeader.nvertex_buffers;
    for (u32 i = 0; i < g_VideoResHeader.nvertex_buffers; ++i) {
        u32 size = 0;
        bytes_read += NuFileRead(file, &size, sizeof(size));
        if (size == 0) {
            g_VideoResHeader.vertex_buffers[i] = 0;
            continue;
        }

        u32 keep_in_memory = size & 0x80000000;
        size &= 0x7fffffff;
        if (keep_in_memory != 0) {
            g_VideoResHeader.vertex_buffers[i] = buf->addr;
            buf->addr += size;
            bytes_read += NuFileRead(file, (void *)g_VideoResHeader.vertex_buffers[i], size);
        } else {
            u32 largest =
                UploadDataToGLBuffer(file, size, GL_ARRAY_BUFFER, &g_VideoResHeader.vertex_buffers[i], buf, buf_end);
            bytes_read += size;
            max_buf.addr = NuMax(max_buf.addr, buf->addr + largest);
        }
    }

    bytes_read += NuFileRead(file, &g_VideoResHeader.nindex_buffers, sizeof(g_VideoResHeader.nindex_buffers));
    g_VideoResHeader.index_buffers = buf->u32_ptr;
    buf->u32_ptr += g_VideoResHeader.nindex_buffers;
    for (u32 i = 0; i < g_VideoResHeader.nindex_buffers; ++i) {
        u32 size = 0;
        bytes_read += NuFileRead(file, &size, sizeof(size));
        if (size == 0) {
            g_VideoResHeader.index_buffers[i] = 0;
            continue;
        }
        u32 largest =
            UploadDataToGLBuffer(file, size, GL_ELEMENT_ARRAY_BUFFER, &g_VideoResHeader.index_buffers[i], buf, buf_end);
        bytes_read += size;
        max_buf.addr = NuMax(max_buf.addr, buf->addr + largest);
    }

    i32 total_size = section_size + 4;
    u8 padding;
    while (bytes_read < total_size) {
        bytes_read += NuFileRead(file, &padding, 1);
    }
    if (buf->addr < max_buf.addr) {
        memset(buf->void_ptr, 0, max_buf.addr - buf->addr);
    }
    return total_size;
}

static NUGSCN *NuReadGraphicsData(VARIPTR *buf, VARIPTR *buf_end, char *path, NUGSCN *scene) {
    if (scene == NULL) {
        char converted_path[1033];
        NuFileExtConvert(converted_path, path);
        NUFILE file = NuFileOpen(converted_path, NUFILE_READ);
        if (file == 0) {
            return NULL;
        }

        i32 file_size = (i32)NuFileOpenSize(file);
        buf->addr = ALIGN(buf->addr, 0x20);
        i32 uploaded = NuGScnUploadGfxDataFromFilePS(buf, *buf_end, (i32)file);
        scene = (NUGSCN *)ALIGN(buf->addr, 0x20);
        buf->addr = (usize)((char *)scene + file_size - uploaded);
        NuFileRead(file, scene, file_size - uploaded);
        NuFileClose(file);
    }

    void **fixed = (void **)NuPtrBlockFix((char *)scene + 0x18);
    NUGSCN *fixed_scene = (NUGSCN *)*fixed;
    if (fixed_scene != NULL) {
        NuGScnCreatePS(fixed_scene, buf, buf_end);

        i32 *texture_ids = *reinterpret_cast<i32 **>(fixed_scene);
        NUNATIVETEX **textures = reinterpret_cast<NUNATIVETEX **>(fixed_scene->field5_0x8);
        for (i32 i = 0; i < fixed_scene->field4_0x4; ++i) {
            if (textures[i]->ref_count < 0) {
                texture_ids[i] = 0xabcdabcd;
            } else {
                texture_ids[i] = NuTexCreateNative(textures[i], true);
            }
        }
        NuGScnFixupTIDs(fixed_scene);
        if (fixed_scene->display_list != NULL) {
            NuDisplaySceneAdd(reinterpret_cast<NUDLDLISTSCENE *>(fixed_scene->display_list));
        }
        NuGScnFixupPS(fixed_scene);
        *reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(fixed_scene) + 0x40) = 0;
        *reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(fixed_scene) + 0x44) = 0;
    }
    return fixed_scene;
}

// --- Extern "C": NuGScn functions have C linkage in original ---
extern "C" {
    NUGSCN *NuGScnRead(VARIPTR *buf, VARIPTR buf_end, char *path) {
        RemoveDirectionalMaps = 1;
        RemoveNormalMaps = 1;
        NUGSCN *scene = NuReadGraphicsData(buf, &buf_end, path, NULL);
        RemoveNormalMaps = 0;
        RemoveDirectionalMaps = 0;
        return scene;
    }
    void NuGScnReadFromMemory(NUGSCN *scene) {
        NuReadGraphicsData(NULL, NULL, NULL, scene);
    }
    void NuGHGFixup(NUGSCN *scene) {
        NuGScnReadFromMemory(scene);
    }
} // extern "C"

namespace {
    struct NuSpecialHandleLayout {
        NUGSCN *scene;
        void *special;
        void *display_special;
    };

    struct NuLegacySpecialLayout {
        u8 pad_00[0x40];
        void *instance;
        char *name;
        u32 flags;
    };

    struct NuDisplaySpecialLayout {
        u8 pad_00[0xb4];
        char *name;
        u8 pad_b8[0xd0 - 0xb8];
    };
} // namespace

i32 NuSpecialFind(NUGSCN *scene, void **dest, char *name, i32 flags) {
    (void)flags; // Present in the exported ABI; unused by the original body.

    NuSpecialHandleLayout *handle = reinterpret_cast<NuSpecialHandleLayout *>(dest);
    if (name != NULL && scene != NULL) {
        NUDLDLISTSCENE *display_scene = reinterpret_cast<NUDLDLISTSCENE *>(scene->display_list);
        if (display_scene != NULL) {
            NuDisplaySpecialLayout *special = static_cast<NuDisplaySpecialLayout *>(display_scene->specials);
            for (i32 i = 0; i < display_scene->nspecials; ++i, ++special) {
                if (NuStrICmp(name, special->name) == 0) {
                    handle->scene = scene;
                    handle->special = NULL;
                    handle->display_special = special;
                    return 1;
                }
            }
        } else {
            NuLegacySpecialLayout *special = reinterpret_cast<NuLegacySpecialLayout *>(scene->specials);
            for (i32 i = 0; i < scene->numspecial; ++i, ++special) {
                if (NuStrICmp(name, special->name) == 0) {
                    handle->scene = scene;
                    handle->special = special;
                    handle->display_special = NULL;
                    return 1;
                }
            }
        }
    }

    handle->scene = NULL;
    handle->special = NULL;
    handle->display_special = NULL;
    return 0;
}

void DrawCables() {
}

void DrawRipple(ripple_node_s *) {
}

void DrawShop3D(WORLDINFO_s *) {
}

void DrawAreaBox(nuvec_s *, nuvec_s *, i32, i32) {
}

void DrawBox_Now(_vuv_s *, _vuv_s *, i32, i32) {
}

void DrawLocator(nuvec_s *, float, i32, i32) {
}

void DrawStreaks() {
}

void Draw_LOADED() {
}

void Draw3DObject(WORLDINFO_s *, i32, nuvec_s *, u16, u16, u16, float, float, float, i32) {
}

void DrawCharIcon(i32, float, float, float, float, i32, float, float, i32, nuhspecial_s *) {
}

void DrawCodeMenu() {
}

void DrawHint_LSW(HINT_s *, i32) {
}

void DrawLine_Now(_vuv_s *, _vuv_s *, i32, i32) {
}

void DrawParallax(nuhspecial_s *) {
}

void DrawQuestion(nuvec_s *, float, float) {
}

void DrawRectRGBA(float, float, float, float, u32, numtl_s *, i32, float) {
}

void DrawSubItems() {
}

void DrawTopShelf(i32) {
}

void DrawTorpedos(GameObject_s *) {
}

void Draw_LOADING() {
}

void DrawAlphaGrid(i32, i32, NuBloomParameters *) {
}

void DrawArrow_Now(_vum_s *, float, i32, i32) {
}

void DrawBonusTime(STATUSPACKET_s *, float, i32) {
}

void DrawCross_Now(_vuv_s *, float, i32, i32) {
}

void DrawGameState(float, float, i32, i32) {
}

void DrawPauseFade() {
}

void DrawRippleSet(ripple_set_s *) {
}

void DrawSaveSlots(MENU_s *, float) {
}

void DrawShopPanel() {
}

void DrawSnakeBody(GameObject_s *) {
}

void DrawAlphaImage(i32, i32, numtl_s *, i32, NuBloomParameters *) {
}

void DrawBezierLine(VuVec &, VuVec &, VuVec &, VuVec &, numtl_s *, i32) {
}

void DrawBonusScore(float, i32, i32, float, i32 *) {
}

void DrawBoxMtx_Now(_vum_s *, _vuv_s *, i32, i32) {
}

void DrawBuildUpBar(float, float, i32, i32, float, float, float, u16) {
}

void DrawCodeMenu3D() {
}

void DrawCutBorders(i32) {
}

void DrawExplosions() {
}

void DrawItemMenu2D() {
}

void DrawMessageBox(i32, float, float, float, float) {
}

void DrawRopeCurved(nuvec_s *, nuvec_s *, i32, i32, numtl_s *) {
}

void DrawRopeSingle(nuvec_s *, nuvec_s *, float, numtl_s *, float, float, float, float) {
}

void DrawStatusText(char *, u16, float, float, float, u32, i32) {
}

void DrawWallSpline(float) {
}

void Draw3DObjectMtx(WORLDINFO_s *, i32, numtx_s *) {
}

void DrawGameObjects() {
}

void DrawPaintLights() {
}

void DrawShopPrompts() {
}

void DrawStatusIcons(STATUSPACKET_s *, float, float) {
}

void DrawStillScreen(i32) {
}

void DrawTouchPrompt(char *, char *, bool, bool) {
}

void Draw_LOADFAILED() {
}

void DrawAreaCylinder(nuvec_s *, nuvec_s *, i32) {
}

void DrawCameraTarget(nuvec_s *) {
}

void DrawGameMessages() {
}

void DrawMeleeTargets(i16 *, char *, float *, i32) {
}

void DrawMiniKitCount(float, float, i32, i32) {
}

void DrawStatusBG_LSW(STATUSPACKET_s *) {
}

void DrawStatusScreen(WORLDINFO_s *) {
}

void Draw_LOADCORRUPT() {
}

void Draw3DObjectAlpha(WORLDINFO_s *, i32, nuvec_s *, u16, u16, u16, float, float, float, i32, float) {
}

void DrawBossHitPoints(GameObject_s *) {
}

void DrawCameraTarget2(nuvec_s *) {
}

void DrawPanel3DObject(float, float, float, float, float, float, u16, u16, u16, nuhspecial_s *, i32, float) {
}

void DrawStatusMiniKit(float, float, float, float, float, i32, STATUSPACKET_s *, float) {
}

void DrawSubItemMenu2D() {
}

void DrawSubItemMenu3D() {
}

void Draw_NOMEMORYCARD() {
}

void DrawFadeScreenWipe() {
}

void DrawMessageBoxRGBA(float, float, float, float, u32, u32, u32, u32, numtl_s *, i32, float) {
}

void DrawSuperStoryTime(float, float, float, i32, i32) {
}

void DrawForceBackEffect(nuhspecial_s *) {
}

void DrawForceGlowSprite(nuvec_s *, float, i32, float, GameObject_s *) {
}

void DrawGameObjectsDraw(i32) {
}

void DrawPauseScreenWipe() {
}

void Draw_AUTOSAVECANCEL() {
}

void DrawMeleeTargetsRows(i16 *, char *, float *, i32) {
}

void DrawMiniSnowTroopers(WORLDINFO_s *) {
}

void DrawPanel3DObjectMtx(nuhspecial_s *, numtx_s *, float) {
}

void Draw_AUTOSAVEWARNING() {
}

void Draw_NODATAAVAILABLE() {
}

void DrawInDoubleScoreZone(float) {
}

void DrawObjectOnCharacter(WORLDINFO_s *, GameObject_s *, i32, nuhspecial_s *, i32, i32, numtx_s *, i32, u32, numtx_s *,
                           nuvec_s *, float, float) {
}

void DrawPlayerIconPrompts(i32, i32, float, i32, i32, i32, i32, i32, i32, float, i32, i32, i32, i32) {
}

void DrawGameObjectsProcess() {
}

void DrawMeleeTargetsNumber(i16 *, unsigned char *, i32, unsigned char, nuhspecial_s *) {
}

void DrawStatusTextFraction(i32, i32, float, float, u16, float, u32, float, float) {
}

void DrawGameMessage_Targets(GAMEMESSAGE_s *, nuvec_s *, float) {
}

void DrawTorpedoTargetSprite(void *, unsigned char, float) {
}

void DrawPanel3DObjectNoAlpha(float, float, float, float, float, float, u16, u16, u16, nuhspecial_s *, i32) {
}

void DrawPanel3DObjectMtxNoAlpha(nuhspecial_s *, numtx_s *) {
}

void Draw_OK(MENU_s *) {
}

void DrawItem(nuhspecial_s *, nuvec_s *, float, float, float, u16, u16, u16) {
}

void DrawAABox(_vuv_s *, _vuv_s *, i32) {
}

void DrawArrow(nuhspecial_s *, float) {
}

void DrawCross(nuvec_s *, float, numtl_s *, i32) {
}

void DrawMSitu(i32) {
}

void DrawPanel() {
    SetQFont2D();
    if (editor_active == 0)
        DrawMenu(Paused);
}

void DrawTimer(i32, i32, i32) {
}

void SwipeDecalRenderer::Process(float) {
}

void SwipeDecalRenderer::Render() {
}

SwipeDecalRenderer::SwipeDecalRenderer(TouchHolder &, i32, SwipeDecalRenderer::Style) {
}

static __used__ void PauseRenderOff() {
}

static __used__ bool MatrixReflection_CanOverride() {
    return false;
}

static __used__ void DrawStarFighter(starfighter_s *) {
}

static __used__ void DrawParaphernalia(GameObject_s *) {
}

static __used__ void DrawFalconSpotLights(GameObject_s *) {
}

static __used__ double ApplyAntilights(rtl_s *, rtlidata_s *, float) {
    return {};
}

static __used__ void DisplayListMaterialClipUpdate(nudisplayscene_s *) {
}

static __used__ void SelectNextFog() {
}

static __used__ void SelectPrevFog() {
}

static __used__ void PreWarmGeomsAndBakeVAOs(nudisplayscene_s *display_scene, nunativegscene_s *) {
    NUDLDLISTSCENE *scene = reinterpret_cast<NUDLDLISTSCENE *>(display_scene);
    for (i32 clip_index = 0; clip_index < scene->nclip_objects; ++clip_index) {
        u8 *clip = reinterpret_cast<u8 *>(&scene->clip_objects[clip_index]);
        u32 nitems = *reinterpret_cast<u32 *>(clip);
        u32 *materials = *reinterpret_cast<u32 **>(clip + 4);
        i32 *items = *reinterpret_cast<i32 **>(clip + 8);
        for (u32 item_index = 0; item_index < nitems; ++item_index) {
            NUDISPLAYLISTITEM *item = &scene->items[items[item_index]];
            if (item->type == 0x8f) {
                continue;
            }
            g_boundMaterial = scene->mtls[materials[item_index]];
            g_LastMtl = g_boundMaterial;
            u8 vertex_flags = reinterpret_cast<u8 *>(&g_boundMaterial->shader_desc.vtx_desc)[2];
            if ((vertex_flags & 0x10) == 0) {
                if ((vertex_flags & 0x20) == 0) {
                    NuIOS_SetVertexFormat((u32)(usize)g_boundMaterial->vertex_decl);
                } else {
                    g_boundVertexFormat = (u32)(usize)g_nuFaceOnVertexFormat;
                }
            } else {
                g_boundVertexFormat = (u32)(usize)g_nuDebrisVertexFormat;
            }
            NuIOSDLPreWarmGeomCallback(item->next);
        }
    }
}

extern "C" void NuGScnFixupPS(NUGSCN *scene) {
    struct NativeScene {
        u16 nvertex_buffers;
        u16 pad_02;
        u32 *vertex_buffers;
        u16 nindex_buffers;
        u16 pad_0a;
        u32 *index_buffers;
        u8 **geometries;
        i32 ngeometries;
        u8 **vertex_streams;
        i32 nvertex_streams;
    };

    NativeScene *native_scene = reinterpret_cast<NativeScene *>(scene->field437_0x1d0);
    i32 dynamic_indices[64];
    i32 ndynamic = 0;
    for (i32 i = 0; i < native_scene->ngeometries; ++i) {
        u8 *geometry = native_scene->geometries[i];
        i32 vertex_index = *reinterpret_cast<i32 *>(geometry + 0x20);
        i32 index_index = *reinterpret_cast<i32 *>(geometry + 0x24);
        *reinterpret_cast<u32 *>(geometry + 0x20) = g_VideoResHeader.index_buffers[vertex_index];
        *reinterpret_cast<u32 *>(geometry + 0x24) = g_VideoResHeader.vertex_buffers[index_index];
        if (*reinterpret_cast<u32 *>(geometry + 0x24) == 0) {
            *reinterpret_cast<u32 *>(geometry + 0x04) = 0;
            *reinterpret_cast<u32 *>(geometry + 0x18) = 0;
        }
        if (*reinterpret_cast<u32 *>(geometry + 0x28) == 0) {
            *reinterpret_cast<u32 *>(geometry + 0x30) = 0;
        } else {
            dynamic_indices[ndynamic++] = index_index;
        }
    }
    for (i32 i = 0; i < ndynamic; ++i) {
        g_VideoResHeader.vertex_buffers[dynamic_indices[i]] = 0;
    }
    for (i32 i = 0; i < native_scene->nvertex_streams; ++i) {
        u8 *stream = native_scene->vertex_streams[i];
        i32 index = *reinterpret_cast<i32 *>(stream + 8);
        *reinterpret_cast<u32 *>(stream + 8) = g_VideoResHeader.vertex_buffers[index];
    }
    native_scene->nvertex_buffers = g_VideoResHeader.nvertex_buffers;
    native_scene->nindex_buffers = g_VideoResHeader.nindex_buffers;
    memcpy(native_scene->vertex_buffers, g_VideoResHeader.vertex_buffers,
           g_VideoResHeader.nvertex_buffers * sizeof(u32));
    memcpy(native_scene->index_buffers, g_VideoResHeader.index_buffers, g_VideoResHeader.nindex_buffers * sizeof(u32));

    for (i32 i = 0; i < scene->nummtl; ++i) {
        NuMtlUpdate(scene->mtls[i]);
    }
    NuPortalMaxDepth(scene, scene->max_portals);
    PreWarmGeomsAndBakeVAOs(scene->display_list, scene->field437_0x1d0);
}

#include "legoapi/legoapi_types.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nu3d/nurndr.h"

extern "C" {
    void NuRndrGradClear(i32 a, i32 b, i32 c, f32 d);
    void NuRndrClear(u32 flags, u32 colour, f32 alpha);
    void NuSpecialGetMtx(nuhspecial_s *hs, NUMTX *out);
}
extern i32 qrand(void);

static NUGSCN *s_backdrop_scene = nullptr;

static NuSpecialHandleLayout s_backdrop_hspecial[4];

static f32 s_backdrop_back_wait = 0.0f;

f32 backdrop_top_r = 0.0f;
f32 backdrop_top_g = 0.0f;
f32 backdrop_top_b = 0.0f;
f32 backdrop_bot_r = 0.0f;
f32 backdrop_bot_g = 0.0f;
f32 backdrop_bot_b = 0.0f;

f32 backdrop_top_tr = 0.0f;
f32 backdrop_top_tg = 0.0f;
f32 backdrop_top_tb = 0.0f;
f32 backdrop_bot_tr = 0.0f;
f32 backdrop_bot_tg = 0.0f;
f32 backdrop_bot_tb = 0.0f;

i32 backdrop_black = 0;

void (*BackDrop_AlphaFn)(float *) = nullptr;

static __used__ void BackDrop_Alpha(float *alpha) {
    if (alpha == nullptr)
        return;
    if (backdrop_black) {
        *alpha *= 0.0f;
    } else if (s_backdrop_back_wait > 0.0f) {
        *alpha *= 0.5f;
    }
}

void BackDrop_Init(char *path, variptr_u *buf, variptr_u *buf_end) {
    NUGSCN *scene = s_backdrop_scene;
    if (scene == NULL) {
        scene = NuGScnRead(buf, *buf_end, path);
        s_backdrop_scene = scene;
    }
    memset(s_backdrop_hspecial, 0, sizeof(s_backdrop_hspecial));
    if (scene == NULL) {
        return;
    }
    NuSpecialFind(scene, (void **)&s_backdrop_hspecial[0], (char *)"ball1", 1);
    NuSpecialFind(scene, (void **)&s_backdrop_hspecial[1], (char *)"ball2", 1);
    NuSpecialFind(scene, (void **)&s_backdrop_hspecial[2], (char *)"ball3", 1);
    NuSpecialFind(scene, (void **)&s_backdrop_hspecial[3], (char *)"ball4", 1);
}

void BackDrop_Dump() {
    s_backdrop_scene = nullptr;
    memset(s_backdrop_hspecial, 0, sizeof(s_backdrop_hspecial));
}

void BackDrop_Update(float dt) {
    if (s_backdrop_scene != NULL) {
        NuGScnUpdate(s_backdrop_scene, (i32)(dt * 60.0f));
    }
}

void BackDrop_ResetColours() {
    backdrop_top_r = 0.0f;
    backdrop_top_g = 0.0f;
    backdrop_top_b = 0.0f;
    backdrop_bot_r = 0.0f;
    backdrop_bot_g = 0.0f;
    backdrop_bot_b = 0.0f;
    backdrop_top_tr = 0.0f;
    backdrop_top_tg = 0.0f;
    backdrop_top_tb = 0.0f;
    backdrop_bot_tr = 0.0f;
    backdrop_bot_tg = 0.0f;
    backdrop_bot_tb = 0.0f;
    s_backdrop_back_wait = 1.0f;
    backdrop_black = 0;
}

void BackDrop_UpdateColours(i32 instant) {
    const f32 lerp = (instant != 0) ? 1.0f : 0.05f;
    auto seek = [&](f32 &cur, f32 tgt) { cur += (tgt - cur) * lerp; };
    if (backdrop_black) {
        seek(backdrop_top_r, 0.0f);
        seek(backdrop_top_g, 0.0f);
        seek(backdrop_top_b, 0.0f);
        seek(backdrop_bot_r, 0.0f);
        seek(backdrop_bot_g, 0.0f);
        seek(backdrop_bot_b, 0.0f);
        return;
    }
    seek(backdrop_top_r, backdrop_top_tr);
    seek(backdrop_top_g, backdrop_top_tg);
    seek(backdrop_top_b, backdrop_top_tb);
    seek(backdrop_bot_r, backdrop_bot_tr);
    seek(backdrop_bot_g, backdrop_bot_tg);
    seek(backdrop_bot_b, backdrop_bot_tb);
}

void BackDrop_Draw(float alpha, i32 flags) {
    if (s_backdrop_scene == NULL) {
        return;
    }
    if (flags == 0 && BackDrop_AlphaFn != NULL) {
        BackDrop_AlphaFn(&alpha);
    }
    if (alpha <= 0.0f) {
        return;
    }

    for (i32 special_index = 0; special_index < 2; ++special_index) {
        NuSpecialHandleLayout *special = &s_backdrop_hspecial[special_index];
        if (NuSpecialExistsFn(special) == 0) {
            continue;
        }
        NUMTX mtx = *NuSpecialGetDrawMtx(special);
        f32 x = mtx.m30;
        f32 y = mtx.m31;
        f32 z = mtx.m32;
        u16 angle = (u16)qrand();
        NuSpecialDrawAtAlpha(special, &mtx, alpha);
        for (i32 i = 0; i < 3; ++i) {
            mtx.m30 = NuTrigTable[angle >> 1] * 0.01f + x;
            mtx.m31 = NuTrigTable[((i32)angle + 0x4000) >> 1 & 0x7fff] * 0.01f + y;
            mtx.m32 = z;
            NuSpecialDrawAtAlpha(special, &mtx, alpha);
            angle = (u16)(angle + 0x5555);
        }
    }
}
