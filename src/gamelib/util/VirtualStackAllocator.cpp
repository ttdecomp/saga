#include "gamelib_util_types.h"

VirtualStackAllocator::VirtualStackAllocator() {
}

VirtualStackAllocator::VirtualStackAllocator(VirtualStackAllocator &, u32) {
}

VirtualStackAllocator::VirtualStackAllocator(i32) {
}

VirtualStackAllocator::VirtualStackAllocator(void *, u32) {
}

void VirtualStackAllocator::setExternalMemoryPool(void *, u32) {
}

VirtualStackAllocator::~VirtualStackAllocator() {
}
