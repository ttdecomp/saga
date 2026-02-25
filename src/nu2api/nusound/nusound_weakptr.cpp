#include "nusound_weakptr.hpp"

NuCriticalSection NuSoundWeakPtrListNode::sPtrListLock("NuSoundCriticalSection::mCriticalSection");
NuCriticalSection NuSoundWeakPtrListNode::sPtrAccessLock("NuSoundCriticalSection::mCriticalSection");

class NuSoundBufferCallback;
template class NuSoundWeakPtrObj<NuSoundBufferCallback>;
template class NuSoundWeakPtr<NuSoundBufferCallback>;
