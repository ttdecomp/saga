#include "nu2api_nusound_types.h"

void NuSoundLoaderWAV::CreateHeader() {
}

void NuSoundLoaderWAV::EndianFlipWAVHeader(FileHeaderWAV *) {
}

void NuSoundLoaderWAV::FindChunk(i32, u32, NuSoundLoaderWAV::ChunkInfo &) {
}

void NuSoundLoaderWAV::FindChunks(i32, NuSoundStreamDesc *, NuSoundLoaderWAV::ChunkReadRequest *, u32) {
}

void NuSoundLoaderWAV::MakeFourCC(char *) {
}

NuSoundLoaderWAV::NuSoundLoaderWAV() {
}

void NuSoundLoaderWAV::ReadDataChunk(i32, NuSoundStreamDesc *, NuSoundLoaderWAV::ChunkInfo const &,
                                     NuSoundLoaderWAV *) {
}

void NuSoundLoaderWAV::ReadHeader(NuSoundStreamDesc *) {
}

void NuSoundLoaderWAV::ReadRIFFHeaderChunk(i32, NuSoundStreamDesc *, NuSoundLoaderWAV::ChunkInfo const &,
                                           NuSoundLoaderWAV *) {
}

void NuSoundLoaderWAV::SeekPCMSample(u64) {
}

void NuSoundLoaderWAV::SeekTime(double) {
}

NuSoundLoaderWAV::~NuSoundLoaderWAV() {
}
