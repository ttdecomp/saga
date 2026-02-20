#include "nusound_weakptr.hpp"

pthread_mutex_t NuSoundWeakPtrListNode::sPtrListLock = PTHREAD_MUTEX_INITIALIZER;
