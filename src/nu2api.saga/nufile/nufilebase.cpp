#include "nu2api.saga/nufile/nufile.h"

#include "nu2api.saga/nucore/nustring.h"

NuFileBase::NuFileBase(const char *filepath, NuFile::OpenMode::T mode, u32 type) {
    this->type = type;
    this->mode = mode;
    this->unknown = 0;

    NuStrNCpy(this->filepath, filepath, sizeof(this->filepath));
}

NuFileBase::~NuFileBase() {
}

i32 NuFileBase::GetCapabilities() const {
    if (mode == NuFile::OpenMode::READ) {
        return 3;
    }

    return 6;
}

const char *NuFileBase::GetFilename() const {
    return this->filepath;
}

u32 NuFileBase::GetType() const {
    return this->type;
}

i64 NuFileBase::GetPos() const {
    return const_cast<NuFileBase *>(this)->Seek(0, NuFile::SeekOrigin::T::CURRENT);
}

i64 NuFileBase::GetSize() const {
    i64 cur_pos;
    i64 size;

    cur_pos = const_cast<NuFileBase *>(this)->Seek(0, NuFile::SeekOrigin::T::CURRENT);

    size = const_cast<NuFileBase *>(this)->Seek(0, NuFile::SeekOrigin::T::END);

    // Restore position.
    const_cast<NuFileBase *>(this)->Seek(cur_pos, NuFile::SeekOrigin::T::START);

    return size;
}

void NuFileBase::Flush() {
}
