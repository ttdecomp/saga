
#include "nu2api/nufile/nufile.h"

#include <string.h>

extern "C" {

    void NuFileAlign(void) {
    }
    void NuFileAlignRead(void) {
    }
    void NuFileAppendPath(void) {
    }
    void NuFileBeginBlkRead(void) {
    }
    void NuFileBeginBlkWrite(void) {
    }
    void NuFileCheckBadGameDiscStatus(void) {
    }
    void NuFileCloseDir(void) {
    }
    void NuFileCopy(void) {
    }
    void NuFileCopyEx(void) {
    }
    void NuFileCreateDir(void) {
    }
    void NuFileCreatePath(void) {
    }
    void NuFileEOF(void) {
    }
    void NuFileEndBlkRead(void) {
    }
    void NuFileEndBlkWrite(void) {
    }
    void NuFileEnumerateDevices(void) {
    }
    void NuFileExistQuiet(void) {
    }
    void NuFileExtGetExt(void) {
    }
    void NuFileExtGetType(void) {
    }
    void NuFileExtRemove(void) {
    }
    void NuFileExtractExt(void) {
    }
    void NuFileExtractFile(void) {
    }
    void NuFileExtractFilename(void) {
    }
    void NuFileExtractPath(void) {
    }
    void NuFileFindDevice(void) {
    }
    void NuFileFormat(void) {
    }
    void NuFileFormatName(void) {
    }
    void NuFileGetAppDirectory(void) {
    }
    void NuFileGetBlkSize(void) {
    }
    NUFILE_DEVICE *NuFileGetCurrentDevice(void) {
        return default_device;
    }
    void NuFileGetCurrentDirectory(void) {
    }
    void NuFileGetCurrentDllPath(void) {
    }
    void NuFileGetCurrentPath(void) {
    }
    void NuFileGetCurrentSysPath(void) {
    }
    void NuFileGetDevices(void) {
    }
    i32 NuFileGetEndianSwap(void) {
        return NuFile_SwapEndianOnWrite;
    }
    void NuFileGetInfo(void) {
    }
    i32 NuFileGetMediaMode(void) {
        return 0;
    }
    void NuFileInitAddress(void) {
    }
    void NuFileIsNewer(void) {
    }
    void NuFileLoad(void) {
    }
    void NuFileOpenDir(void) {
    }
    void NuFilePakGetLastError(void) {
    }
    void NuFilePakGetSize(void) {
    }
    void NuFilePakRelocate(void) {
    }
    void NuFilePatchAddress(void) {
    }
    void NuFileRefreshDevices(void) {
    }
    void NuFileRename(void) {
    }
    void NuFileSetAddress(void) {
    }
    void NuFileSetAppDirectory(void) {
    }
    void NuFileSetBadGameDisc(void) {
    }
    void NuFileSetCurrentDevice(NUFILE_DEVICE *device) {
        default_device = device;
    }
    void NuFileSetCurrentDirectory(void) {
    }
    void NuFileSetCurrentDllDirectory(void) {
    }
    void NuFileSetCurrentSysDirectory(void) {
    }
    void NuFileSwapEndianOnWrite(void) {
    }
    void NuFileTidyAddress(void) {
    }
    void NuFileWriteAddress(void) {
    }
    void NuFileWriteChar(void) {
    }
    void NuFileWriteFloat(void) {
    }
    void NuFileWriteShort(void) {
    }
    void NuFileWriteString(NUFILE file, const char *text) {
        NuFileWrite(file, const_cast<char *>(text), strlen(text));
    }
    void NuFileWriteStringV(void) {
    }
    void NuFileWriteUnsignedChar(void) {
    }
    void NuFileWriteUnsignedShort(void) {
    }
}
