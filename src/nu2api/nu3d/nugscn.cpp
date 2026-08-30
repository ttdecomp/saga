#include "nu2api/nu3d/nugscn.h"

#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nufile/nufile.h"

#include <string.h>

__attribute__((weak)) void NuGScnCreatePS(nugscn_s *scene, variptr_u *, variptr_u *) {
    NUNATIVETEX **textures = scene->textures;
    if (g_VideoResHeader.texture_hashes == 0) {
        for (i32 i = 0; i < scene->ntextures; ++i) {
            NUNATIVETEX *texture = textures[i];
            texture->image_data = nullptr;
            texture->size = 0;
            texture->platform.gl_tex = g_VideoResHeader.textures[i];
        }
    } else {
        for (u32 i = 0; i < g_VideoResHeader.ntextures; ++i) {
            const u32 hash = g_VideoResHeader.textures[i];
            if (hash != 0) {
                g_VideoResHeader.textures[i] = NuIOS_CreateGLTexFromHash(hash);
            }
            NUNATIVETEX *texture = textures[i];
            texture->image_data = nullptr;
            texture->size = hash;
            texture->platform.gl_tex = g_VideoResHeader.textures[i];
        }
    }
}

i32 NuGScnFixupTID(nugscn_s *scene, i32 tid) {
    if (tid == -1) {
        return 0;
    }
    if ((tid & 0x4000) == 0) {
        return (*reinterpret_cast<i32 **>(scene))[tid];
    }
    // Texture-reference scenes are handled by NuTexResolveReference in the
    // original.  That subsystem is not yet transcribed.
    return 0;
}

void NuGScnDestroyPS(nugscn_s *) {
}

extern "C" void NuTexAnimRemoveList(void *texture_anims);

extern "C" void NuGScnRemove(nugscn_s *scene) {
    if (scene->texture_anims != nullptr) {
        NuTexAnimRemoveList(scene->texture_anims);
    }

    NuGScnRestoreTIDs(scene);
    for (i32 i = 0; i < scene->ntextures; ++i) {
        if (scene->textures[i]->ref_count >= 0) {
            NuTexDestroy(scene->texture_ids[i]);
        }
    }

    if (scene->additional_scenes != nullptr) {
        for (NUGSCN **additional = scene->additional_scenes; *additional != nullptr; ++additional) {
            NuDisplaySceneDestroy((*additional)->display_list);
        }
    }
    NuDisplaySceneDestroy(scene->display_list);
    NuGScnDestroyPS(scene);
}

void NuGScnFixupTIDs(nugscn_s *scene) {
    static const usize shader_tid_offsets[] = {
        0x04, 0x08, 0x0c, 0x10, 0x34, 0x38, 0x48, 0x4c, 0x50, 0x54, 0x58, 0x5c, 0x198, 0x1e4, 0x1e8,
    };

    for (i32 i = 0; i < scene->nummtl; ++i) {
        NUMTL *material = scene->mtls[i];
        material->tex_id = (i16)NuGScnFixupTID(scene, material->tex_id);
        for (usize offset : shader_tid_offsets) {
            i32 *tid = reinterpret_cast<i32 *>(reinterpret_cast<u8 *>(&material->shader_desc) + offset);
            *tid = NuGScnFixupTID(scene, *tid);
        }
        reinterpret_cast<u8 *>(material)[0x46] &= 0xbf;
        NuMtlUpdate(material);
    }
}

void NuGScnRestoreTID(nugscn_s *, i32) {
}

void NuGScnRestoreTIDs(nugscn_s *) {
}

void NuGScnMtlLayerMask(nugscn_s *, unsigned char) {
}

void NuGScnLoadShadersPS(char *, variptr_u *, variptr_u) {
}

__attribute__((weak)) i32 NuGScnReadTexturesPS(i32 file, variptr_u *buf, variptr_u buf_end) {
    (void)buf_end;
    i32 bytes_read = 0;
    bytes_read += NuFileRead(file, &g_VideoResHeader.ntextures, sizeof(g_VideoResHeader.ntextures));

    g_VideoResHeader.texture_hashes = g_VideoResHeader.ntextures & 0x8000;
    g_VideoResHeader.ntextures &= 0x7fff;
    g_VideoResHeader.textures = buf->u32_ptr;
    buf->u32_ptr += g_VideoResHeader.ntextures;
    memset(g_VideoResHeader.textures, 0, (usize)g_VideoResHeader.ntextures * sizeof(u32));

    if (g_VideoResHeader.texture_hashes != 0) {
        bytes_read += NuFileRead(file, g_VideoResHeader.textures, (i32)g_VideoResHeader.ntextures * (i32)sizeof(u32));
        return bytes_read;
    }

    for (u32 i = 0; i < g_VideoResHeader.ntextures; ++i) {
        i32 texture_header[6];
        bytes_read += NuFileRead(file, texture_header, sizeof(texture_header));
        u32 size = (u32)texture_header[5];
        if (size == 0) {
            g_VideoResHeader.textures[i] = 0;
            if (texture_header[0] < 0) {
                for (i32 j = 1; j < 6; ++j) {
                    g_VideoResHeader.textures[i + j] = 0;
                }
                i += 5;
            }
            continue;
        }
        if (texture_header[0] == 0) {
            g_VideoResHeader.textures[i] = 0;
            continue;
        }

        NUNATIVETEX texture = {};
        texture.image_data = buf->void_ptr;
        texture.size = size;
        buf->addr += size;
        bytes_read += NuFileRead(file, texture.image_data, size);

        NudxFw_D3DBeginCriticalSection();
        bool is_pvrtc = texture_header[0] < 0;
        NuTexCreatePS(&texture, is_pvrtc);
        g_VideoResHeader.textures[i] = texture.platform.gl_tex;
        NudxFw_D3DEndCriticalSection();
        buf->addr -= size;

        if (is_pvrtc) {
            for (i32 j = 1; j < 6; ++j) {
                g_VideoResHeader.textures[i + j] = 0;
            }
            i += 5;
        }
    }
    return bytes_read;
}

void NuGScnBufferAllocAligned(i32, i32) {
}
