#include <stdint.h>

#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/numemory/numemory.h"

void NuMtlInitEx(void **bufferBase, int32_t usually512) {
    // iVar2 = AndroidOBBUtils::LookupPackagePath(path, 1);
    char *path = "res/main.1060.com.wb.lego.tcs.obb";

    nudathdr_s *dat = NuDatOpen(path, bufferBase, 0);
    NuDatSet(dat);

    LOG("dat->version=%d", dat->version);
    LOG("dat->fileCount=%d", dat->filesCount);
    LOG("dat->treeCount=%d", dat->treeCount);
    LOG("dat->leafnamesize=%d", dat->leafnamesize);
    LOG("dat->filetree=%p", dat->filetree);

    int32_t size = NuFileLoadBuffer("stuff\\\\text\\\\badwords.txt", *bufferBase, 0x100000);
    LOG("size=%d", size);
}

void NuInitHardware(void **bufferBase, void **bufferEnd, int32_t zero) {
    NuMtlInitEx(bufferBase, 512);
}

int32_t SUPERBUFFERSIZE = 0x2EB8EEB;
void *permbuffer_base = NULL;
void *original_permbuffer_base = NULL;
void *superbuffer_end = NULL;

void InitOnce(int32_t argc, char **param_2) {
    NuMemory *memory = NuMemoryGet();
    NuMemoryManager *manager = memory->GetThreadMem();

    permbuffer_base = manager->_BlockAlloc(SUPERBUFFERSIZE, 4, 1, "", 0);
    superbuffer_end = (void *)(SUPERBUFFERSIZE + (size_t)permbuffer_base);
    original_permbuffer_base = permbuffer_base;

    // InitGameBeforeConfig();
    //  Game_NuPad = Game_NuPad_Store;

    NuInitHardware(&permbuffer_base, &superbuffer_end, 0);
}

extern "C" int32_t NuMain(int32_t argc, char **argv) {
    InitOnce(argc, argv);
    return 0;
}

int main(int argc, char **argv) {
    NuMain(argc, argv);

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};