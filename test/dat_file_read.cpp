#include "test.hpp"

#include "globals.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/numemory/numemory.h"
#include "nu2api.saga/nuplatform/nuplatform.hpp"

#include <iostream>
#include <memory>
#include <string>

int main(int argc, char **argv) {
    // NuPlatform::Create();
    // NuPlatform::ms_instance->SetCurrentPlatform(ANDROID_ETC1_PLATFORM);

    // NuMemory *memory = NuMemoryGet();
    // NuMemoryManager *manager = memory->GetThreadMem();

    // permbuffer_base.void_ptr = manager->_BlockAlloc(SUPERBUFFERSIZE, 4, 1, "", 0);
    // superbuffer_end.char_ptr = permbuffer_base.char_ptr + SUPERBUFFERSIZE;
    // original_permbuffer_base = permbuffer_base;

    const size_t buff_size = 0x200'000;
    auto ptr = std::unique_ptr<uint8_t[]>(new uint8_t[buff_size]);
    VARIPTR buf = {.void_ptr = ptr.get()};

    const char *dat_path = "res/main.1060.com.wb.lego.tcs.obb";
    nudathdr_s *dat = NuDatOpen(const_cast<char *>(dat_path), &buf, 0);
    CHECK(dat != NULL, "Failed to open dat file: {}", dat_path);

    NuDatSet(dat);

    const char *file_path = "stuff\\text\\badwords.txt";
    int32_t size = NuFileLoadBuffer(const_cast<char *>(file_path), buf.void_ptr, buff_size);
    CHECK(size > 0, "Failed to load file from dat: {}", file_path);

    // replace \n with ,
    for (int32_t i = 0; i < size; i++) {
        if (buf.char_ptr[i] == '\r') {
            buf.char_ptr[i] = ',';
        } else if (buf.char_ptr[i] == '\n') {
            buf.char_ptr[i] = ' ';
        }
    }

    std::cout.write(buf.char_ptr, size);
}