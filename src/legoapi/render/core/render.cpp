#include "legoapi/render/core/render.h"
#include <stdio.h>

#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/world_shared.h"
#include "globals.h"
struct starfighter_s;
struct rtl_s;
struct rtlidata_s;

#include "legoapi/render/core/SwipeDecalRenderer.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/android/nuiosdl_gl.h"
#include "nu2api/nu3d/android/nugscn_android.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nuptrblock.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numath.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

extern NuVertexFormatPS *g_nuFaceOnVertexFormat;
extern NuVertexFormatPS *g_nuDebrisVertexFormat;
extern i32 VehicleArea;

void DisplayListGenerateTransforms(nudisplayscene_s *scene);
void DrawGameObjectsDraw(i32 pass);
void EnableShadowMapRendering(i32 enable);
void ResetShadowMapRendering();
static void DrawParaphernalia(GameObject_s *object);

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
extern "C" i32 NuRndrBeginScene(i32);
extern "C" void NuRndrEndScene(void);
extern "C" void NuRndrGradRect2di(i32, i32, i32, i32, i32 *, numtl_s *);
extern "C" void NuRndrRect2di(i32, i32, i32, i32, i32, numtl_s *);
extern "C" void NuRndrGradRectUV2di(i32, i32, i32, i32, f32, f32, f32, f32, u32 *, numtl_s *);
extern "C" void NuRndrRectUV2di(i32, i32, i32, i32, f32, f32, f32, f32, u32, numtl_s *);
extern "C" void NuRndrClear(u32, u32, f32);
extern "C" NUVIEWPORT *NuVpGetCurrentViewport(void);
extern char *apiGameName;
extern char *apitxt_EMPTY;
extern char *apitxt_PRESENT;
extern char *apitxt_AUTOSAVE_WARNING;
extern char *apitxt_LOADING;
extern char *apitxt_SAVING;
extern char *apitxt_NODATAAVAILABLE;
extern i16 tCURRENTGAME;
extern i16 tEMPTY;
extern i16 tGAME;
extern i16 tNOSPACE;
extern f32 MENUTEXTSCALE;
extern f32 AUTOSAVEICONY;
extern f32 AUTOSAVEICONX;
extern f32 AUTOSAVEICONSIZE;
extern f32 MenuAlpha;
extern i32 MenuA;
extern f32 memcard_loadmessage_delay;
extern f32 memcard_loadresult_delay;
extern u8 MENUNORMALR;
extern u8 MENUNORMALG;
extern u8 MENUNORMALB;
extern u8 MENUFLASH0R;
extern u8 MENUFLASH0G;
extern u8 MENUFLASH0B;
extern u8 MENUFLASH1R;
extern u8 MENUFLASH1G;
extern u8 MENUFLASH1B;
extern f32 menu_pulse;
extern f32 menu_pulsate;
extern i32 menu_flash;
extern "C" i32 TestForController(void);
extern f32 text3d_height;
extern f32 text3d_width;
extern FadeSystem FadeSys;
extern f32 MainRenderTime;
extern numtl_s *pause_rndr_mtl;
extern i32 editor_active;
extern i32 Paused;
extern i32 noscenespecials;
extern void RotateGameMatrix(numtx_s *matrix, i32 order, u16 x, u16 y, u16 z);
extern NUGSCN *IconScene_FindById(i32 character_id);

namespace {
    struct NuSpecialHandleLayout {
        NUGSCN *scene;
        void *special;
        void *display_special;
    };

    struct NuDisplaySpecialLayout {
        NUMTX mtx;
        NUMTX draw_mtx;
        NUVEC min;
        f32 min_w;
        NUVEC max;
        f32 max_w;
        u8 pad_a0[0x10];
        NUCLIPOBJECT *clip_objects;
        char *name;
        u32 flags;
        f32 *clip_range;
        i32 instance_ix;
        NUMTX *draw_mtx_ptr;
        i16 wind_speed;
        i16 wind_scale;
        u32 pad_cc;
    };

    struct NuLegacySpecialLayout {
        u8 pad_00[0x40];
        void *instance;
        char *name;
        u32 flags;
    };
} // namespace

DECOMP_ASSERT(sizeof(NuSpecialHandleLayout) == 0xc, "special handle size");
DECOMP_ASSERT(sizeof(NuDisplaySpecialLayout) == 0xd0, "display special size");

// Camera zoom state
f32 CameraZoom = 1.0f;

// Graphics loading flags
i32 RemoveDirectionalMaps = 0;
i32 RemoveNormalMaps = 0;

NUVIDEORESHEADER g_VideoResHeader;

extern "C" {
    void RndrStateCopyGlobalState(NUGLOBALRNDRSTATE *state);
    i32 NuDisplayListRndrSpecial(nuhspecial_s *special, NUMTX *mtx, i32 skinned, void *skin_mtx, void *blend_values);

    static void DisplaySceneSetClipResult(NUDLDLISTSCENE *scene, i32 clip_index, i32 clip_result) {
        const u32 buffer = scene->render_buffer >> 7;
        u8 *clip_bits = scene->clip_used[buffer];
        const u32 shift = static_cast<u32>(clip_index & 3) * 2;
        clip_bits[clip_index >> 2] |= static_cast<u8>(clip_result << shift);

        if (clip_result == 0) {
            return;
        }

        NUCLIPOBJECT &clip = scene->clip_objects[clip_index];
        u8 *material_bits = scene->mtl_used[buffer];
        for (i32 i = 0; i < clip.nmaterials; ++i) {
            const i32 material_id = clip.material_ids[i];
            material_bits[material_id >> 3] |= static_cast<u8>(1u << (material_id & 7));
        }
    }

    // NuDisplaySceneRndr.part.101's non-NuVisi branch.  This is the original
    // camera-culling fallback used when no visibility tree supplies a result.
    // LOD child records (visibility bit 0 clear) are selected by their parent;
    // zero-range scenes such as the Map therefore evaluate only the parent
    // records and skip their child entries exactly as the original loop does.
    static void DisplaySceneEvaluateClipFallback(NUDLDLISTSCENE *scene) {
        scene->alpha_values = NULL;
        if (scene->nclip_objects <= 0) {
            return;
        }

        if (scene->fade_ranges != NULL) {
            VARIPTR *buffer = NuDisplayListGetBuffer();
            scene->alpha_values = static_cast<f32 *>(buffer->void_ptr);
            buffer->addr += static_cast<usize>(scene->nclip_objects) * sizeof(f32);
        }

        for (i32 clip_index = 0; clip_index < scene->nclip_objects; ++clip_index) {
            const u8 flags = static_cast<u8>(scene->visibility_flags[clip_index]);
            if ((flags & 1) == 0) {
                continue;
            }

            NUCLIPBOUNDS &bounds = scene->clip_bounds[clip_index];
            const f32 far_clip = scene->far_clip_ranges != NULL ? scene->far_clip_ranges[clip_index] : 0.0f;
            const i32 clip_result = NuCameraClipTestExtentsAxisAligned(&bounds.min, &bounds.max, far_clip);
            if (scene->alpha_values != NULL) {
                scene->alpha_values[clip_index] = clip_result != 0 ? 1.0f : 0.0f;
            }
            DisplaySceneSetClipResult(scene, clip_index, clip_result);
        }
    }

    void NuDisplaySceneRndr(void *display_scene) {
        NUDLDLISTSCENE *scene = static_cast<NUDLDLISTSCENE *>(display_scene);
        if ((scene->flags & NUDL_SCENE_FLAG_CLIPPING) != 0) {
            return;
        }

        RndrStateCopyGlobalState(scene->local_state);
        if (scene->nclip_objects != 0) {
            scene->flags |= NUDL_SCENE_FLAG_CLIPPING;
            DisplaySceneEvaluateClipFallback(scene);
        }
        DisplayListGenerateTransforms(reinterpret_cast<nudisplayscene_s *>(scene));

        if ((scene->pad_76[0] & 1) == 0 && noscenespecials == 0 && scene->nspecials > 0) {
            NUGSCN temporary_scene = {};
            NUGSCN *gscene = scene->gscene;
            if (gscene == NULL) {
                temporary_scene.display_list = scene;
                gscene = &temporary_scene;
            }

            NuSpecialHandleLayout handle = {gscene, NULL, NULL};
            NuDisplaySpecialLayout *special = static_cast<NuDisplaySpecialLayout *>(scene->specials);
            for (i32 i = 0; i < scene->nspecials; ++i, ++special) {
                if ((special->flags & 2) != 0) {
                    handle.display_special = special;
                    NUMTX *draw_mtx = special->draw_mtx_ptr;
                    if (draw_mtx == NULL || draw_mtx == reinterpret_cast<NUMTX *>(-1)) {
                        draw_mtx = &special->draw_mtx;
                    }
                    NuDisplayListRndrSpecial(reinterpret_cast<nuhspecial_s *>(&handle), draw_mtx, 0, NULL, NULL);
                }
            }
        }
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

        i32 *texture_ids = fixed_scene->texture_ids;
        NUNATIVETEX **textures = fixed_scene->textures;
        for (i32 i = 0; i < fixed_scene->ntextures; ++i) {
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
    nuhgobj_s *NuGHGRead(char *path, VARIPTR *buf, VARIPTR buf_end) {
        nuapi.loading_hgobj = 1;
        nuhgobj_s *object = reinterpret_cast<nuhgobj_s *>(NuReadGraphicsData(buf, &buf_end, path, NULL));
        if (object != NULL && nuapi.force_hgobj_visibility != 0 && object->display_list != NULL) {
            for (i32 i = 0; i < object->display_list->nspecials; ++i) {
                object->display_list->visibility_flags[i] |= 0x20;
            }
        }
        nuapi.loading_hgobj = 0;
        return object;
    }
} // extern "C"

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

void DrawCharIcon(i32 character_id, float x, float y, float z, float scale, i32 frame_object_id, float character_alpha,
                  float frame_alpha, i32 draw_character, nuhspecial_s *override_special) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    const u16 character_spin = static_cast<u16>(drawcharicon_hspecial_spin);
    drawcharicon_hspecial_spin = 0;

    if (ObjTabList == NULL || world == NULL) {
        return;
    }

    nuhspecial_s special;
    if (drawcharicon_find != 0) {
        NuSpecialFind(things_scene, reinterpret_cast<void **>(&special), ObjTabList[frame_object_id].name, 1);
    } else {
        special = world->lev_objs[frame_object_id].special;
    }

    if (NuSpecialExistsFn(&special) != 0) {
        DrawPanel3DObject(x, y, z + 1.0f, scale, scale, scale, 0, 0, 0, &special, 0, frame_alpha);
    }

    if (draw_character != 0) {
        if (override_special != NULL && NuSpecialExistsFn(override_special) != 0) {
            const f32 special_scale = scale * drawcharicon_hspecial_scale;
            DrawPanel3DObject(x, y, z + 1.0f + drawcharicon_hspecial_dz, special_scale, special_scale, special_scale, 0,
                              character_spin, 0, override_special, 0, character_alpha);
        } else {
            i32 icon_object_id;
            if (character_id == -1) {
                icon_object_id = LEGOOBJ_ICON_QUESTION;
            } else {
                icon_object_id = CDataList[character_id].field20_0x42;
                if (icon_object_id != -1 && GCDataList[character_id].field275_0x116 == 0 &&
                    icon_object_id != LEGOOBJ_ICON_WEIRDO) {
                    ++icon_object_id;
                }
            }

            if (icon_object_id != -1) {
                if (drawcharicon_find != 0) {
                    NUGSCN *icon_scene = character_id == -1 ? NULL : IconScene_FindById(character_id);
                    if (icon_scene != NULL) {
                        NuSpecialFind(icon_scene, reinterpret_cast<void **>(&special), ObjTabList[icon_object_id].name,
                                      1);
                    } else {
                        NuSpecialFind(things_scene, reinterpret_cast<void **>(&special),
                                      ObjTabList[icon_object_id].name, 1);
                    }
                } else {
                    special = world->lev_objs[icon_object_id].special;
                }

                if (NuSpecialExistsFn(&special) != 0) {
                    DrawPanel3DObject(x, y, z + 1.0f, scale, scale, scale, 0, 0, 0, &special, 0, character_alpha);
                }
            }
        }
    }

    drawcharicon_find = 0;
    drawcharicon_i_panel = -1;
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

void DrawGameState(float x, float y, i32 highlight, i32 slot) {
    char game_name[64];
    if (slot == -1) {
        NuStrCpy(game_name, TTab[tCURRENTGAME]);
    } else {
        sprintf(game_name, "%s %i", TTab[tGAME], slot + 1);
    }

    u8 red = MENUNORMALR;
    u8 green = MENUNORMALG;
    u8 blue = MENUNORMALB;
    if (highlight != 0 && TestForController() != 0) {
        if (menu_pulsate > 0.0f) {
            red = static_cast<u8>(static_cast<i32>(MENUFLASH0R * menu_pulsate + MENUFLASH1R * (1.0f - menu_pulsate)));
            green = static_cast<u8>(static_cast<i32>(MENUFLASH0G * menu_pulsate + MENUFLASH1G * (1.0f - menu_pulsate)));
            blue = static_cast<u8>(static_cast<i32>(MENUFLASH0B * menu_pulsate + MENUFLASH1B * (1.0f - menu_pulsate)));
        } else if (menu_flash != 0) {
            red = MENUFLASH0R;
            green = MENUFLASH0G;
            blue = MENUFLASH0B;
        } else {
            red = MENUFLASH1R;
            green = MENUFLASH1G;
            blue = MENUFLASH1B;
        }
    } else if (menu_pulse > 0.0f) {
        red = static_cast<u8>(static_cast<i32>(MENUFLASH0R * menu_pulse + MENUNORMALR * (1.0f - menu_pulse)));
        green = static_cast<u8>(static_cast<i32>(MENUFLASH0G * menu_pulse + MENUNORMALG * (1.0f - menu_pulse)));
        blue = static_cast<u8>(static_cast<i32>(MENUFLASH0B * menu_pulse + MENUNORMALB * (1.0f - menu_pulse)));
    }
    SmartTextEx(game_name, x, y, 1.0f, MENUTEXTSCALE, MENUTEXTSCALE, MENUTEXTSCALE, 4, red, green, blue, 0.45f, 1, NULL,
                0, MenuA);

    if (slot >= 0) {
        if (saveload_slotused[slot] != 0) {
            char progress[32];
            sprintf(progress, "%.1f%%", static_cast<f32>(saveload_slotcode[slot] * 100) / COMPLETIONPOINTS);
            Text_LocaliseDecimalPoint(progress);
            Text3DEx(progress, x, y, 1.0f, MENUTEXTSCALE, MENUTEXTSCALE, MENUTEXTSCALE, 1, 255, 191, 0, MenuA);
        } else {
            char *state = TTab[saveload_freespace < SAVESIZE_ADDITIONAL ? tNOSPACE : tEMPTY];
            const u8 state_red = saveload_freespace < SAVESIZE_ADDITIONAL ? 255 : 0;
            SmartTextEx(state, x, y, 1.0f, MENUTEXTSCALE, MENUTEXTSCALE, MENUTEXTSCALE, 1, state_red, 255 - state_red,
                        0, 0.45f, 1, NULL, 0, MenuA);
        }
    }
}

void DrawPauseFade() {
}

void DrawRippleSet(ripple_set_s *) {
}

void DrawSaveSlots(MENU_s *menu, float y) {
    DrawGameState(-0.5f, y, menu->selected_row == menu->first_row && menu->selected_column == 0, 0);
    menu->item_width[2] = text3d_width;
    menu->item_column[2] = 0;
    menu->item_y[2] = y;
    menu->item_x[2] = -0.5f;
    menu->item_height[2] = text3d_height * 2.0f;
    menu->item_row[2] = 0;

    DrawGameState(0.0f, y, menu->selected_row == menu->first_row && menu->selected_column == 1, 1);
    menu->item_width[3] = text3d_width;
    menu->item_column[3] = 1;
    menu->item_y[3] = y;
    menu->item_x[3] = 0.0f;
    menu->item_height[3] = text3d_height * 2.0f;
    menu->item_row[3] = 0;

    DrawGameState(0.5f, y, menu->selected_row == menu->first_row && menu->selected_column == 2, 2);
    menu->item_column[4] = 2;
    menu->item_y[4] = y;
    menu->item_row[4] = 0;
    menu->item_width[4] = text3d_width;
    menu->item_x[4] = 0.5f;
    menu->item_height[4] = text3d_height * 2.0f;
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

void Draw3DObjectMtx(WORLDINFO_s *world, i32 object_index, numtx_s *mtx) {
    if (object_index == -1) {
        return;
    }
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
        if (world == NULL) {
            return;
        }
    }
    LEVEL_OBJECT_RUNTIME &object = world->lev_objs[object_index];
    if (object.active != 0) {
        NuSpecialDrawAt(&object.special, mtx);
    }
}

void DrawGameObjects() {
    const f32 saved_far_clip = global_camera.unknown_64;
    const f32 object_far_clip = VehicleArea != 0 ? 50.0f : 20.0f;
    if (object_far_clip <= global_camera.unknown_64) {
        global_camera.unknown_64 = object_far_clip;
    }

    NuCameraSet(&global_camera);
    DrawGameObjectsDraw(0);

    global_camera.unknown_64 = saved_far_clip;
    NuCameraSet(&global_camera);
}

void DrawPaintLights() {
}

void DrawShopPrompts() {
}

void DrawStatusIcons(STATUSPACKET_s *, float, float) {
}

void DrawStillScreen(i32 clear) {
    NuRndrBeginScene(-1);
    NuVpGetCurrentViewport();
    if (clear != 0) {
        NuRndrClear(0x500, 0, 1.0f);
    }
    if (MainRenderTime >= 1.0f) {
        NuRndrRectUV2di(0, 0, 0x2800, 0xe00, 0.0f, 1.0f, 1.0f, 0.0f, 0x80808080u, pause_rndr_mtl);
    } else {
        const u32 colour = (static_cast<i32>(MainRenderTime * 128.0f) << 24) | 0x00808080u;
        u32 colours[4] = {colour, colour, colour, colour};
        NuRndrGradRectUV2di(0, 0, 0x2800, 0xe00, 0.0f, 1.0f, 1.0f, 0.0f, colours, pause_rndr_mtl);
    }
    NuRndrEndScene();
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

void DrawPanel3DObject(float x, float y, float z, float scale_x, float scale_y, float scale_z, u16 rotate_x,
                       u16 rotate_y, u16 rotate_z, nuhspecial_s *special, i32 rotate_order, float alpha) {
    if (alpha <= 0.0f || special == NULL || NuSpecialExistsFn(special) == 0) {
        return;
    }
    if (scale_x == 0.0f && scale_y == 0.0f && scale_z == 0.0f) {
        return;
    }

    NUVEC scale = {scale_x / CameraZoom, scale_y / CameraZoom, scale_z / CameraZoom};
    NUMTX matrix;
    NuMtxSetScale(&matrix, &scale);
    RotateGameMatrix(&matrix, rotate_order, rotate_x, rotate_y, rotate_z);
    matrix.m30 = x * PANEL3DMULX;
    matrix.m31 = y * PANEL3DMULY;
    matrix.m32 = z;
    NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
    NuSpecialDrawAtAlpha(special, &matrix, alpha);
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
    extern FadeSystem *pFadeInfo;

    NuRndrBeginScene(-1);
    extern numtl_s *FadeMtl2;
    extern numtl_s *SolidMtl;

    // The original routine unconditionally dereferences the shared fade
    // pointer after beginning a scene.  `pFadeInfo` is installed by
    // LoadPermData and points at FadeSys; retaining that indirection keeps
    // this call ABI-identical to the original.
    FadeSystem &fade_info = *pFadeInfo;
    const f32 fade_amount = fade_info.fade;
    const u32 direction = fade_info.direction;
    i32 gradient[4];
    i32 solid_x = 0;
    i32 solid_y = 0;
    i32 solid_width = 10240;
    i32 solid_height = 3584;

    if ((direction & 3) != 0) {
        const bool positive = (direction & 1) != 0;
        if ((positive && fade_info.rate > 0.0f) || (!positive && fade_info.rate <= 0.0f)) {
            gradient[0] = static_cast<i32>(0x80000000u);
            gradient[1] = 0;
            gradient[2] = static_cast<i32>(0x80000000u);
            gradient[3] = 0;
            solid_width = static_cast<i32>(fade_amount * 10240.0f);
            NuRndrGradRect2di(solid_width, 0, 1024, 3584, gradient, FadeMtl2);
        } else {
            gradient[0] = 0;
            gradient[1] = static_cast<i32>(0x80000000u);
            gradient[2] = 0;
            gradient[3] = static_cast<i32>(0x80000000u);
            const i32 edge = static_cast<i32>((1.0f - fade_amount) * 10240.0f);
            NuRndrGradRect2di(edge - 1024, 0, 1024, 3584, gradient, FadeMtl2);
            solid_x = edge;
            solid_width = 10240 - edge;
        }
    } else if ((direction & 0xc) != 0) {
        // The vertical sign test is the same two-way rate/direction test as
        // the horizontal one, with bit 2 selecting the opposite side.
        const bool edge_first = (direction & 4) != 0 ? fade_info.rate <= 0.0f : fade_info.rate > 0.0f;
        if (edge_first) {
            gradient[0] = static_cast<i32>(0x80000000u);
            gradient[1] = static_cast<i32>(0x80000000u);
            gradient[2] = 0;
            gradient[3] = 0;
            const i32 edge = static_cast<i32>((1.0f - fade_amount) * 3584.0f);
            NuRndrGradRect2di(0, edge - 358, 10240, 358, gradient, FadeMtl2);
            solid_y = edge;
            solid_height = 3584 - edge;
        } else {
            gradient[0] = 0;
            gradient[1] = 0;
            gradient[2] = static_cast<i32>(0x80000000u);
            gradient[3] = static_cast<i32>(0x80000000u);
            solid_height = static_cast<i32>(fade_amount * 3584.0f);
            NuRndrGradRect2di(0, solid_height, 10240, 358, gradient, FadeMtl2);
        }
    }

    if (fade_amount >= 0.0f) {
        NuRndrRect2di(solid_x, solid_y, solid_width, solid_height, 0, SolidMtl);
    }
    NuRndrEndScene();
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
    EnableShadowMapRendering(0);

    for (i32 index = 0; index < HIGHGAMEOBJECT; ++index) {
        GameObject_s *object = &Obj[index];

        if ((object->apiobj.field_0x1f4 & 0x800) != 0) {
            if (object->field_0xcc0 != NULL && object->field_0x7a5 == 0x3b) {
                object->apiobj.model_draw_result = object->field_0xcc0->apiobj.model_draw_result;
            }
            continue;
        }

        NUMTX *secondary_matrix = (object->field_0xefe & 2) != 0 ? &object->apiobj.field_0xf8 : NULL;
        NUMTX *tertiary_matrix = object->field_0x1088 != 0 ? &object->apiobj.field_0x138 : NULL;
        const i32 drawn = GameDrawCharacterModel(object->apiobj.character_model, &object->apiobj.anim_packet,
                                                 &object->apiobj.field_0xb8, secondary_matrix, tertiary_matrix,
                                                 &object->field_0x7f4, object, object->field_0x1054);

        object->apiobj.model_draw_result = static_cast<u8>(drawn);
        object->field_0xe24 =
            static_cast<u8>((object->field_0xe24 & ~8) | ((drawcharactermodel_locatorsupdated & 1) << 3));

        if (drawn != 0) {
            DrawParaphernalia(object);
        }

        // The force-glow branch rejoins here after its level-character and
        // character-variant visibility tests. Those structures are not typed
        // yet; keep the ordinary original path intact while they are recovered.
        if (object->field_0x10b8 != NULL) {
            DrawSnakeBody(object);
        }

        object->apiobj.field_0x288 = 1;
        if (object->apiobj.model_draw_result != 0) {
            object->field_0xefe |= 4;
        }

        if (Paused == 0 && object->apiobj.character_data != NULL && object->apiobj.character_data->draw_fn != NULL) {
            object->apiobj.character_data->draw_fn(object);
        }
    }

    ResetShadowMapRendering();
}

void DrawPauseScreenWipe() {
    NuRndrBeginScene(-1);

    const f32 fade = FadeSys.fade;
    i32 x = 0;
    i32 y = 0;
    i32 width = 0x2800;
    i32 height = 0xe00;
    f32 u0 = 0.0f;
    f32 v0 = 1.0f;
    f32 u1 = 1.0f;
    f32 v1 = 0.0f;
    u32 colours[4];

    if ((FadeSys.direction & 3) != 0) {
        if ((FadeSys.direction & 1) == 0) {
            width = static_cast<i32>(fade * 10240.0f);
            colours[0] = 0x80808080u;
            colours[1] = 0x00808080u;
            colours[2] = 0x80808080u;
            colours[3] = 0x00808080u;
            NuRndrGradRectUV2di(width, 0, 0x400, 0xe00, fade, 1.0f, fade + 0.1f, 0.0f, colours, pause_rndr_mtl);
            u1 = fade;
        } else {
            u0 = 1.0f - fade;
            x = static_cast<i32>(u0 * 10240.0f);
            width = 0x2800 - x;
            colours[0] = 0x00808080u;
            colours[1] = 0x80808080u;
            colours[2] = 0x00808080u;
            colours[3] = 0x80808080u;
            NuRndrGradRectUV2di(x - 0x400, 0, 0x400, 0xe00, u0 - 0.1f, 1.0f, u0, 0.0f, colours, pause_rndr_mtl);
        }
    } else if ((FadeSys.direction & 0xc) != 0) {
        if ((FadeSys.direction & 4) == 0) {
            height = static_cast<i32>(fade * 3584.0f);
            colours[0] = 0x80808080u;
            colours[1] = 0x80808080u;
            colours[2] = 0x00808080u;
            colours[3] = 0x00808080u;
            NuRndrGradRectUV2di(0, height, 0x2800, 0x166, 0.0f, 1.0f - fade, 1.0f, 1.0f - (fade + 0.1f), colours,
                                pause_rndr_mtl);
            v1 = 1.0f - fade;
        } else {
            const f32 edge = 1.0f - fade;
            y = static_cast<i32>(edge * 3584.0f);
            height = 0xe00 - y;
            colours[0] = 0x00808080u;
            colours[1] = 0x00808080u;
            colours[2] = 0x80808080u;
            colours[3] = 0x80808080u;
            NuRndrGradRectUV2di(0, y - 0x166, 0x2800, 0x166, 0.0f, 1.0f - (edge - 0.1f), 1.0f, 1.0f - edge, colours,
                                pause_rndr_mtl);
            v0 = 1.0f - edge;
        }
    }

    NuRndrRectUV2di(x, y, width, height, u0, v0, u1, v1, 0x80808080u, pause_rndr_mtl);
    NuRndrEndScene();
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
    const f32 scale = MENUTEXTSCALE * 0.8f;

    if (memcard_drawasiconfn != NULL) {
        memcard_drawasiconfn();
    }

    const char *message = apitxt_SAVING;
    if (memcard_loadmessage_delay > 0.0f || memcard_loadresult_delay > 0.0f) {
        message = apitxt_LOADING;
    }

    MenuSmartTextEx(const_cast<char *>(message), AUTOSAVEICONX - AUTOSAVEICONSIZE * 1.8f, AUTOSAVEICONY, 1.0f, scale,
                    scale, scale, 8, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.2f, 1, NULL, 0, MenuA);
}

void Draw_NODATAAVAILABLE() {
    char message[1024];
    sprintf(message, apitxt_NODATAAVAILABLE, apiGameName);
    MenuSmartTextEx(message, 0.0f, -0.4f, 1.0f, MENUTEXTSCALE, MENUTEXTSCALE, MENUTEXTSCALE, 0, MENUNORMALR,
                    MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

void DrawInDoubleScoreZone(float) {
}

void DrawObjectOnCharacter(WORLDINFO_s *, GameObject_s *, i32, nuhspecial_s *, i32, i32, numtx_s *, i32, u32, numtx_s *,
                           nuvec_s *, float, float) {
}

void DrawPlayerIconPrompts(i32, i32, float, i32, i32, i32, i32, i32, i32, float, i32, i32, i32, i32) {
}

void DrawGameObjectsProcess() {
    for (i32 index = 0; index < HIGHGAMEOBJECT; ++index) {
        GameObject_s *object = &Obj[index];

        // Mode 2 is the ordinary character path selected by InitCreature.
        // Other transform modes have distinct vehicle and attachment logic
        // and must not be approximated with this matrix.
        if ((object->apiobj.field_0x1f8 & 0x1001) != 0x1001 || object->field_0x1086 != 2 ||
            object->field_0x7a5 == 0x23 || object->field_0x7a5 == 0x24) {
            continue;
        }

        object->apiobj.field_0x1f4 |= 0x800;
        const u8 previous_draw_pending = object->apiobj.field_0x288 & 1;
        object->apiobj.field_0x288 = 0;
        object->field_0xe23 = static_cast<u8>((object->field_0xe23 & ~8) | (previous_draw_pending << 3));
        object->field_0xe24 &= ~8;

        // DropInOutScale returns exactly 1.0 for ordinary characters (states
        // other than 0x23/0x24), so their common scale is field_0xa8.
        const f32 uniform_scale = object->apiobj.field_0xa8;
        if (uniform_scale == 0.0f) {
            object->apiobj.model_draw_result = 0;
            continue;
        }

        NUVEC scale = {uniform_scale, uniform_scale, uniform_scale};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        NuMtxPreRotateY(&matrix, object->apiobj.field_0x276);

        NUVEC position = object->apiobj.position;
        NuMtxTranslate(&matrix, &position);
        object->apiobj.field_0xb8 = matrix;

        // The original sets the skip bit at the start of processing and
        // toggles it off once a non-degenerate render matrix is available.
        object->apiobj.field_0x1f4 ^= 0x800;
        object->field_0xefe &= ~2;
        object->field_0x1088 = 0;
    }
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

static __used__ void PreWarmGeomsAndBakeVAOs(nudisplayscene_s *raw_scene, nunativegscene_s *) {
    NUDLDLISTSCENE *scene = reinterpret_cast<NUDLDLISTSCENE *>(raw_scene);
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
    PreWarmGeomsAndBakeVAOs(reinterpret_cast<nudisplayscene_s *>(scene->display_list), scene->field437_0x1d0);
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
