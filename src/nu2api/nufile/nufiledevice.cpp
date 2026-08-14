#include "nu2api_nufile_types.h"

void NuFileDevice::AddDevice(NuFileDevice *) {
}

void NuFileDevice::AddPathRule(NuFileDeviceType, char const *) {
}

void NuFileDevice::AllocDirectoryHandle(char const *) {
}

void NuFileDevice::ClearPathRules() {
}

void NuFileDevice::FileOpen(char const *, NuFile::OpenMode::T) const {
}

void NuFileDevice::FileSize(char const *) const {
}

void NuFileDevice::FormatName(char *, i32, char const *) const {
}

void NuFileDevice::FreeDirectoryHandle(i32) {
}

void NuFileDevice::GetDeviceByType(NuFileDeviceType) {
}

void NuFileDevice::GetDeviceFromDirectoryHandle(i32) {
}

void NuFileDevice::GetDeviceFromPath(char const *) {
}

void NuFileDevice::Interrogate() {
}

NuFileDevice::NuFileDevice() {
}

void NuFileDevice::QueryInstallProgress() {
}

void NuFileDevice::SetCurrentDir(char const *) {
}

void NuFileDevice::SetDefaultDevice(NuFileDeviceType) {
}

void NuFileDevice::SetLabel(char *) {
}

void NuFileDevice::SetMountName(char *) {
}

NuFileDevice::~NuFileDevice() {
}
