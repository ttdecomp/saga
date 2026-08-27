#include "nu2api/nusound/nusound_memorymanager.hpp"

#include <stdlib.h>

#include "decomp.h"

NuSoundMemoryBuffer *NuSoundMemoryManager::Alloc(u32 size) {
    LOG_WARN("NuSoundMemoryManager::Alloc is not implemented");
    return (NuSoundMemoryBuffer *)malloc(size);
}
