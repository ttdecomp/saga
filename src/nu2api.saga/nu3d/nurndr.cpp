#include "nu2api.saga/nu3d/nurndr.h"

#include "nu2api.saga/nu3d/nustream.h"

void NuRndrInitEx(int stream_buffer_size, VARIPTR *buffer) {
    NuRndrStreamInit(stream_buffer_size, buffer);
}

void NuRndrInitWorld(void) {
}