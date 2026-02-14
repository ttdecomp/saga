#pragma once

#include "nu2api.saga/nucore/NuMemoryManager.h"
#include "nu2api.saga/nucore/NuMemoryPool.h"
#include "nu2api.saga/nucore/common.h"

#ifdef ANDROID
#include "nu2api.saga/nucore/android/numemory_android.h"
#endif

extern bool g_disallowGlobalNew;

#define NU_ALLOC(size, alignment, flags, name, category)                                                               \
    NuMemoryGet()->GetThreadMem()->_BlockAlloc(size, alignment, flags, name, category);
#define NU_ALLOC_T(type, flags, name, category) (type *)NU_ALLOC(sizeof(type), alignof(type), flags, name, category)
#define NU_FREE(ptr) NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0)

#ifdef __cplusplus

class NuMemory {
  private:
    class MemErrorHandler : public NuMemoryManager::IErrorHandler {};

    class FixedPoolEventHandler : NuMemoryPool::IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int alignment,
                                 const char *name) override;
        virtual int ReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, unsigned int size, unsigned int alignment,
                                         const char *name) override;
        virtual void FreeLargeBlock(NuMemoryPool *pool, void *ptr) override;
    };

    class DynamicPoolEventHandler : NuMemoryPool::IEventHandler {
      public:
        virtual int AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int alignment,
                                 const char *name) override;
        virtual int ReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void ForceReleasePage(NuMemoryPool *pool, void *ptr) override;
        virtual void *AllocateLargeBlock(NuMemoryPool *pool, unsigned int size, unsigned int alignment,
                                         const char *name) override;
        virtual void FreeLargeBlock(NuMemoryPool *pool, void *ptr) override;
    };

    MemErrorHandler *error_handler;
    NuMemoryPS::Mem1EventHandler *mem1_event_handler;
    NuMemoryPS::Mem2EventHandler *mem2_event_handler;
    NuMemoryManager *mem1_manager;
    NuMemoryManager *mem2_manager;
    int unknown;
    FixedPoolEventHandler *fixed_pool_event_handler;
    DynamicPoolEventHandler *dynamic_pool_event_handler;
    int unknown2;

    i32 tls_index;

  public:
    NuMemory(void **buf);
    NuMemoryManager *GetThreadMem();
    NuMemoryManager *CreateMemoryManager(NuMemoryManager::IEventHandler *event_handler, const char *name);

  private:
    void InitalizeThreadLocalStorage();
};

NuMemory *NuMemoryGet();

extern "C" {
#endif
    enum {
        NUMEMORY_CATEGORY_NONE = 0,
        NUMEMORY_CATEGORY_INITIAL = 1,
        NUMEMORY_CATEGORY_HARDWAREINIT = 2,
        NUMEMORY_CATEGORY_NUFILE = 3,
        NUMEMORY_CATEGORY_NUFONT = 4,
        NUMEMORY_CATEGORY_NUSCENE = 5,
        NUMEMORY_CATEGORY_NURENDER = 6,
        NUMEMORY_CATEGORY_NUSOUND = 7,
        NUMEMORY_CATEGORY_NUDEBUG = 8,
        NUMEMORY_CATEGORY_CHARS = 9,
        NUMEMORY_CATEGORY_CHARANIMS = 10,
        NUMEMORY_CATEGORY_SPLITSCREEN = 11,
        NUMEMORY_CATEGORY_DYNOTERRAIN = 12,
        NUMEMORY_CATEGORY_DYNODATA = 13,
        NUMEMORY_CATEGORY_DYNOSYSTEM = 14,
        NUMEMORY_CATEGORY_AI = 15,
        NUMEMORY_CATEGORY_PARTICLE = 16,
        NUMEMORY_CATEGORY_VFX = 17,
        NUMEMORY_CATEGORY_TECH = 18,
        NUMEMORY_CATEGORY_SCRIPT = 19,
        NUMEMORY_CATEGORY_RESOURCE = 20,
        NUMEMORY_CATEGORY_GAMEFRAMEWORK = 21,
        NUMEMORY_CATEGORY_LEVEL = 22,
        NUMEMORY_CATEGORY_CUTSCENE = 23,
        NUMEMORY_CATEGORY_KRAWLIE = 24,
        NUMEMORY_CATEGORY_GAME = 25,
        NUMEMORY_CATEGORY_EDITOR = 26,
        NUMEMORY_CATEGORY_ANIMATION = 27,
        NUMEMORY_CATEGORY_MANAGERS = 28,
        NUMEMORY_CATEGORY_ENGINE = 29,
        NUMEMORY_CATEGORY_G_LEGOSETS = 30,
        NUMEMORY_CATEGORY_L_LEGOSETS = 31,
        NUMEMORY_CATEGORY_TECH_GIN = 32,
        NUMEMORY_CATEGORY_TECH_GIZMO = 33,
        NUMEMORY_CATEGORY_TECH_GADGET = 34,
        NUMEMORY_CATEGORY_TECH_FLOW = 35,
        NUMEMORY_CATEGORY_TECH_ANIM = 36,
        NUMEMORY_CATEGORY_TECH_LIGHTING = 37,
        NUMEMORY_CATEGORY_TECH_AI = 38,
        NUMEMORY_CATEGORY_TECH_AI_EDMESH = 39,
        NUMEMORY_CATEGORY_TECH_AI_GRAPH = 40,
        NUMEMORY_CATEGORY_TECH_CAMERA = 41,
        NUMEMORY_CATEGORY_TECH_AUDIO = 42,
        NUMEMORY_CATEGORY_TECH_PICKUPS = 43,
        NUMEMORY_CATEGORY_TECH_KRAWLIE = 44,
        NUMEMORY_CATEGORY_TECH_STREAM = 45,
        NUMEMORY_CATEGORY_TECH_GRASS = 46,
        NUMEMORY_CATEGORY_TECH_MAINLED = 47,
        NUMEMORY_CATEGORY_TECH_FIXUP = 48,
        NUMEMORY_CATEGORY_RUNTIME = 49,
        NUMEMORY_CATEGORY_TTSHARED = 50,
    };

    extern const char *g_categoryNames[];
#ifdef __cplusplus
}
#endif
