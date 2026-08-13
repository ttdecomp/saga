#include "legoapi/world_shared.h"

#include "nu2api/nufile/nufile.h"

// Camera zoom state
f32 CameraZoom;

// Graphics loading flags
i32 RemoveDirectionalMaps = 0;
i32 RemoveNormalMaps = 0;

extern "C" {
    void NuDisplaySceneRndr(void *scene) {
        (void)scene;
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

i32 NuGScnUploadGfxDataFromFilePS(VARIPTR *buf, VARIPTR *buf_end, i32 file) {
    (void)buf;
    (void)buf_end;
    (void)file;
    return 0;
}

static NUGSCN *NuReadGraphicsData(VARIPTR *buf, VARIPTR *buf_end, char *path, NUGSCN *scene) {
    if (scene != NULL) {
        return scene;
    }

    char converted_path[1033];
    NuFileExtConvert(converted_path, path, 0x400);
    NUFILE file = NuFileOpen(converted_path, NUFILE_READ);
    if (file == 0) {
        return NULL;
    }

    i32 file_size = (i32)NuFileOpenSize(file);
    buf->addr = ALIGN(buf->addr, 0x20);
    i32 uploaded = NuGScnUploadGfxDataFromFilePS(buf, buf_end, (i32)file);
    NUGSCN *loaded = (NUGSCN *)ALIGN(buf->addr, 0x20);
    buf->addr = (usize)((char *)loaded + file_size - uploaded);
    NuFileRead(file, loaded, file_size - uploaded);
    NuFileClose(file);
    return loaded;
}

// --- Extern "C": NuGScn functions have C linkage in original ---
extern "C" {
    void *NuGScnRead(VARIPTR *buf, VARIPTR buf_end, char *path) {
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

void NuSpecialFind(NUGSCN *scene, void **dest, char *name) {
    (void)scene;
    (void)dest;
    (void)name;
}
