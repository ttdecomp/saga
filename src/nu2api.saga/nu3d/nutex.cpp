#include "nu2api.saga/nu3d/nutex.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nuplatform/nuplatform.hpp"

#include <pthread.h>
#include <string.h>

int32_t g_datfileMode = 0;

int32_t NuTexRead(char *name, void **permbuffer) {
    LOG_DEBUG("name=%s, permbuffer=%p", name, permbuffer);

    bool bVar1;
    PLATFORMS_SUPPORTED supported;
    int id;
    void *buffer;
    NUTEX *texture;
    NUNATIVETEX *nativeTex;
    char *extension;
    size_t local_34;
    NUFILE file;
    size_t size;

    local_34 = 0;
    FILE *file2 = NULL;

    NuPlatform *platform = NuPlatform::Get();
    extension = platform->GetCurrentTextureExtension();
    platform = NuPlatform::Get();
    supported = platform->GetCurrentPlatform();

    if (supported != IOS_PLATFORM) {
        platform = NuPlatform::Get();
        supported = platform->GetCurrentPlatform();
        if (supported != ANDROID_PVRTC_PLATFORM) {
            bVar1 = false;
            goto LAB_002ad96d;
        }
    }
    bVar1 = true;

LAB_002ad96d:
    char dst[2048] = {0};

    if (bVar1) {
        NuStrFixExtPlatform(dst, name, "tex", 0x400, "ios");
    } else {
        NuStrFixExtPlatform(dst, name, extension, 0x400, "MOB");
    }

    g_datfileMode = 1;

    file = NuFileOpen(dst, NUFILE_READ);
    if (file == 0) {
        id = 0;
    } else {
        size = NuFileOpenSize(file);
        nativeTex = (nunativetex_s *)((int)*permbuffer + 3U & 0xfffffffc);
        *permbuffer = (void *)((int)*permbuffer + 3U & 0xfffffffc);
        *permbuffer = (void *)((int)*permbuffer + 0x28);
        texture = (nutex_s *)*permbuffer;
        buffer = (void *)((int)*permbuffer + 3U & 0xfffffffc);
        *permbuffer = (void *)((int)*permbuffer + 3U & 0xfffffffc);
        *permbuffer = (void *)((int)*permbuffer + size);
        nativeTex->imagedata = buffer;
        nativeTex->size = size;

        if (g_datfileMode == 0) {
            if (0 < (int)size) {
                local_34 = fread(buffer, 1, size, file2);
                fclose(file2);
            }
        } else {
            NuFileRead(file, buffer, size);
            NuFileClose(file);
        }

        // dump
#ifdef HOST_BUILD
        char _dump_filename[256];
        sprintf(_dump_filename, "zz_dump_%s.pvr", name);
        for (int i = 0; _dump_filename[i] != '\0'; i++) {
            if (_dump_filename[i] == '\\' || _dump_filename[i] == '/') {
                _dump_filename[i] = '_';
            }
        }

        FILE *f = fopen(_dump_filename, "wb");
        fwrite(nativeTex->imagedata, 1, nativeTex->size, f);
        fclose(f);
#endif

        id = NuTexCreateNative(nativeTex, 0);

        memset(texture, 0, (int)buffer + (size - (int)texture));

        *permbuffer = texture;
    }

    return id;
}

void NuTexCreatePS(nunativetex_s *nativeTex, bool isPvr) {
    if (nativeTex != NULL && nativeTex->imagedata != NULL && nativeTex->size != 0) {
        // nativeTex->glTex =
        // NuIOS_CreateGLTexFromPlatformInMemory(nativeTex->imagedata, &nativeTex->ptr, &nativeTex->field1_0x4,
        // isPvr);
        UNIMPLEMENTED();
        nativeTex->imagedata = NULL;
        nativeTex->size = 0;
    }
    return;
}

pthread_mutex_t criticalSection = PTHREAD_MUTEX_INITIALIZER;
int32_t *texture_order = NULL;
int32_t max_textures = 0;
nunativetex_s **texture_list = NULL;
int32_t gTextureLoadCount = 0;

int32_t NuTexCreateNative(nunativetex_s *tex, bool isPvr) {
    if (tex != NULL) {
        pthread_mutex_lock(&criticalSection);

        int32_t *piVar1 = texture_order;
        if (0 < max_textures) {

            nunativetex_s **texture = texture_list;

            int32_t iVar1;
            int32_t id;

            if (*texture_list == NULL) {
                id = 1;
                iVar1 = 0;
            } else {
                int32_t count = 0;
                do {
                    id = count;
                    texture = texture + 1;
                    count = id + 1;
                    if (count == max_textures)
                        goto LAB_00305e01;
                    iVar1 = count * 4;
                } while (*texture != NULL);
                id = id + 2;
            }

            *texture = tex;
            *(int *)((int)piVar1 + iVar1) = gTextureLoadCount;
            gTextureLoadCount = gTextureLoadCount + 1;
            pthread_mutex_unlock(&criticalSection);

            NuTexCreatePS(tex, isPvr);

            return id;
        }

    LAB_00305e01:
        pthread_mutex_unlock(&criticalSection);
    }

    return 0;
}
