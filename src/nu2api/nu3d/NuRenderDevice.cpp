#include "nu2api/nu3d/NuRenderDevice.h"

#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"
#include "nu2api/nu3d/nutex.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <math.h>
#include <string.h>

NuRenderDevice g_renderDevice{};

thread_local i32 gt_glContextIndex;

NuRenderDeviceGen::NuRenderDeviceGen() : value(false) {
}

NuRenderDevice::NuRenderDevice() : NuRenderDeviceGen() {
    // this->field1_0x4 = 0;

    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, 1);
    pthread_mutex_init(&this->mutex, &attrs);
    pthread_mutexattr_destroy(&attrs);

    // this->field4_0x10 = 0;
    // this->field5_0x14 = 0;
    this->is_not_amazon_kf = true;
    // this->field45_0x45 = true;
    this->focus = false;
    // this->field80_0xc0 = 0;
    // this->field81_0xc4 = 1.3333334;
    this->context_valid = false;
    this->egl_display = EGL_NO_DISPLAY;
    // this->field61_0x80 = -1;

    this->contexts[0] = EGL_NO_CONTEXT;
    this->contexts[1] = EGL_NO_CONTEXT;
    this->contexts[2] = EGL_NO_CONTEXT;
    this->contexts[3] = EGL_NO_CONTEXT;

    this->field54_0x54 = true;
    this->backing_width = 1280;
    this->width = 1280;
    this->backing_height = 720;
    this->height = 720;
}

void (*glGetProgramBinaryOES)();
void (*glProgramBinaryOES)();
void (*glDiscardFramebufferEXT)();
void (*glGenVertexArraysOES)();
void (*glBindVertexArrayOES)();
void (*glDeleteVertexArraysOES)();

void NuGLES2ExtensionsInit() {
    glGetProgramBinaryOES = eglGetProcAddress("glGetProgramBinaryOES");
    glProgramBinaryOES = eglGetProcAddress("glProgramBinaryOES");
    glDiscardFramebufferEXT = eglGetProcAddress("glDiscardFramebufferEXT");
    glGenVertexArraysOES = eglGetProcAddress("glGenVertexArraysOES");
    glBindVertexArrayOES = eglGetProcAddress("glBindVertexArrayOES");
    glDeleteVertexArraysOES = eglGetProcAddress("glDeleteVertexArraysOES");
}

i32 NuRenderDevice::DetermineNominalAspectRatio(u32 width, u32 height) {
    i32 ret;
    f32 fVar2;
    f32 fVar3;
    f32 fVar4;

    f32 fVar1 = (f32)width / (f32)height;

    fVar2 = fabsf(fVar1 - 1.3333334);
    if (1000.0 <= fabsf(fVar1 - 1.3333334)) {
        fVar2 = 1000.0;
    }
    fVar3 = fabsf(fVar1 - 1.7777778);
    fVar4 = fVar3;
    if (fVar3 >= fVar2) {
        fVar4 = fVar2;
    }
    ret = (int)(fVar3 < fVar2);
    if (fabsf(fVar1 - 1.6) < fVar4) {
        ret = 2;
    }
    return ret;
}

bool NuRenderDevice::IsExtensionSupported(const char *exts) {
    if (strchr(exts, ' ') != NULL || *exts == '\0') {
        return false;
    }

    const char *extensions = this->extensions;

    bool bVar1;
    const char *name;
    do {
        name = strstr(extensions, exts);
        if (name == NULL) {
            return false;
        }

        usize len = strlen(exts);
        bVar1 = extensions != name;
        extensions = name + len;
    } while ((bVar1 && name[-1] != ' ') || (*extensions & 0xdf) != 0);

    return true;
}

i32 _NuCheckGLErrors(const char *file) {
    return 0;
}

void NuRenderDevice::Initialize() {
    while (!this->context_valid) {
        NuThreadSleep(1);
    }

    FrameEnd();

    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, 1);
    pthread_mutex_init(&this->mutex2, &attrs);
    NuGLES2ExtensionsInit();

    BeginCriticalSection("none", -1);

    EGLint attribs[6] = {0};
    eglGetConfigAttrib(this->egl_display, this->egl_config, 0x3024, attribs);
    eglGetConfigAttrib(this->egl_display, this->egl_config, 0x3022, attribs + 1);
    eglGetConfigAttrib(this->egl_display, this->egl_config, 0x3023, attribs + 2);
    eglGetConfigAttrib(this->egl_display, this->egl_config, 0x3021, attribs + 3);
    eglGetConfigAttrib(this->egl_display, this->egl_config, 0x3025, attribs + 4);
    eglGetConfigAttrib(this->egl_display, this->egl_config, 0x3026, attribs + 5);

    this->nominal_aspect_ratio = DetermineNominalAspectRatio(this->width, this->height);
    this->aspect_ratio = (float)this->width / (float)this->height;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->max_texture_units);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->max_texture_size);

    this->extensions = (const char *)glGetString(GL_EXTENSIONS);

    u8 dxt1, atc, pvrtc, etc1;
    if (this->extensions == NULL) {
        dxt1 = 0;
        atc = 0;
        pvrtc = 0;
        etc1 = 0;
    } else {
        u8 bVar2 = IsExtensionSupported("EXT_texture_compression_dxt1");
        u8 bVar3 = IsExtensionSupported("GL_EXT_texture_compression_dxt1");
        dxt1 = bVar3 || bVar2;
        atc = IsExtensionSupported("GL_AMD_compressed_ATC_texture");
        pvrtc = IsExtensionSupported("GL_IMG_texture_compression_pvrtc");
        etc1 = IsExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
    }

    memset(this->enabled_extensions, 0, sizeof(this->enabled_extensions));

    this->enabled_extensions[7] = 1;
    this->enabled_extensions[1] = dxt1;
    this->enabled_extensions[2] = dxt1;
    this->enabled_extensions[6] = dxt1;
    this->enabled_extensions[17] = 1;// etc1;
    this->enabled_extensions[20] = pvrtc;
    this->enabled_extensions[21] = pvrtc;
    this->enabled_extensions[22] = pvrtc;
    this->enabled_extensions[23] = pvrtc;
    this->enabled_extensions[24] = atc;
    this->enabled_extensions[25] = atc;
    this->enabled_extensions[NUTEX_ETC1] = 1;

    if (this->extensions != NULL) {
        this->oes_packed_depth_stencil = IsExtensionSupported("GL_OES_packed_depth_stencil");
        this->oes_depth24 = IsExtensionSupported("GL_OES_depth24");
        this->oes_depth_texture = IsExtensionSupported("GL_OES_depth_texture");
    }

    if (g_forceETC1 == 0 || !etc1) {
        if (dxt1 != 0) {

            NuPlatform::Get()->SetCurrentPlatform(ANDROID_S3TC_PLATFORM);
        } else if (pvrtc != 0) {
            NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);
        } else if (atc != 0) {
            NuPlatform::Get()->SetCurrentPlatform(ANDROID_ATITC_PLATFORM);
        } else {
            NuPlatform::Get()->SetCurrentPlatform(ANDROID_ETC1_PLATFORM);
        }
    } else {
        NuPlatform::Get()->SetCurrentPlatform(ANDROID_ETC1_PLATFORM);
    }
    //DELETE LATER
    LOG_ERR("THIS IS FOR DEBUGGING AND MUST BE REMOVED LATER");
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_ETC1_PLATFORM);
    NuDeviceSpecs::Create();
    EndCriticalSection("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/NuRenderDevice_gles2.cpp", 0x194);

    _NuCheckGLErrors("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/NuRenderDevice_gles2.cpp");

    this->value = true;
}

void NuRenderDevice::FrameEnd() {
    return;
}

void NuRenderDevice::SetThisTreadAsRender() {
    gt_glContextIndex = 3;
}

i32 g_nextGLContextIndex = 0;

void NuRenderDevice::BeginCriticalSection(const char *file, i32 line) {
    pthread_mutex_lock(&this->mutex2);

    static thread_local i32 gl_context_index = -1;

    i32 i = this->lock_count++;

    if (i == 0) {
        i = gl_context_index;

        if (i == -1) {
            i = g_nextGLContextIndex;
            g_nextGLContextIndex = (g_nextGLContextIndex + 1) % 4;
        }

        LOG_DEBUG("this->egl_display: %p, this->pbuffers[%d]: %p, this->contexts[%d]: %p", this->egl_display, i,
                  this->pbuffers[i], i, this->contexts[i]);
        eglMakeCurrent(this->egl_display, this->pbuffers[i], this->pbuffers[i], this->contexts[i]);
    }
}

void NuRenderDevice::EndCriticalSection(const char *file, i32 line) {
    if (--this->lock_count == 0) {
        i32 ctx_idx = gt_glContextIndex;
        bool cVar1 = this->field54_0x54;
        u32 iVar3 = this->field_0x50;
        bool cond1 = (iVar3 - 2U <= 1);
        NuApplicationStatus status = NuCore::GetApplicationState()->GetStatus();

        if (cond1 || !cVar1 || (ctx_idx != 0) || (status == NuApplicationStatus::NUSTATUS_ERROR)) {
            eglMakeCurrent(this->egl_display, nullptr, nullptr, nullptr);
        }
    }

    pthread_mutex_unlock(&this->mutex2);
}

void NuRenderDevice::SwapBuffers() {
    if (NuCore::GetApplicationState()->GetStatus() != NuApplicationStatus::NUSTATUS_ERROR) {
        g_renderDevice.BeginCriticalSection("none", -1);
        eglSwapBuffers(this->egl_display, this->pbuffers[3]);
        g_renderDevice.EndCriticalSection(
            "i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/NuRenderDevice_gles2.cpp", 1157);
    }
    return;
}

void NuRenderDevice::OnWindowCreated(ANativeWindow *window) {
    InitialiseOpenGLContext(window);
    CheckForRenderWindowInitialisation();
}

EGLConfig NuRenderDevice::SelectEGLConfig() {
    static EGLint egl_attribs[] = {EGL_DEPTH_SIZE,      24, //
                                   EGL_LEVEL,           0,  //
                                   EGL_SURFACE_TYPE,    5,  //
                                   EGL_RENDERABLE_TYPE, 4,  //
                                   EGL_CONFORMANT,      4,  //
                                   EGL_BLUE_SIZE,       5,  //
                                   EGL_GREEN_SIZE,      6,  //
                                   EGL_RED_SIZE,        5,  //
                                   EGL_ALPHA_SIZE,      0,  //
                                   EGL_STENCIL_SIZE,    0,  EGL_NONE};

    pthread_mutex_lock(&this->mutex);

    decltype(egl_attribs) attrib_list;
    memcpy(attrib_list, egl_attribs, sizeof(egl_attribs));

    i32 num_config;
    EGLConfig configs[32];

    EGLBoolean ret = eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_config);

    if (num_config == 0 || ret == EGL_FALSE) {
        attrib_list[1] = 16;
        eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_config);
    }

    pthread_mutex_unlock(&this->mutex);

    return configs[0];
}

void NuRenderDevice::DetermineBackBufferResolution(i32 width, i32 height) {
    this->backing_width = width;
    this->backing_height = height;

    NuDeviceSpecs::Create();
    if (this->is_not_amazon_kf != false && NuDeviceSpecs::ms_instance->specs < 3 && this->backing_width > 1280) {
        this->backing_width = 1280;
        this->backing_height = (float)height / (float)width * 1280.0f;
    }
}

void NuRenderDevice::InitialiseOpenGLContext(ANativeWindow *window_) {
    EGLNativeWindowType window = (EGLNativeWindowType)window_;

    pthread_mutex_lock(&this->mutex);

    this->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    this->is_not_amazon_kf = NuStrICmp(g_deviceManufacturer, "Amazon") != 0 || NuStrNICmp(g_deviceModel, "KF", 2) != 0;

    LOG_DEBUG("this->context_valid: %d, this->native_window: %p, window: %p", this->context_valid, this->native_window,
              window);

    if (!this->context_valid) {
        this->native_window = window;

        EGLint major = 0;
        EGLint minor = 0;

        eglInitialize(this->egl_display, &major, &minor);
        eglBindAPI(EGL_OPENGL_ES_API);

        this->egl_config = SelectEGLConfig();

        this->pbuffers[3] = eglCreateWindowSurface(this->egl_display, this->egl_config, this->native_window, NULL);
        if (this->pbuffers[3] == EGL_NO_SURFACE) {
            LOG_ERR("eglCreateWindowSurface failed: %d", eglGetError());
            return;
        }

        this->attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
        this->attrib_list[1] = 2;
        this->attrib_list[2] = EGL_NONE;

        this->contexts[3] = eglCreateContext(this->egl_display, this->egl_config, EGL_NO_CONTEXT, this->attrib_list);

        EGLContext context3;
        if (this->field54_0x54) {
            EGLint stack_attrib_list[] = {
                EGL_WIDTH,      1,       EGL_HEIGHT, 1, EGL_TEXTURE_TARGET, EGL_NO_TEXTURE, EGL_TEXTURE_FORMAT,
                EGL_NO_TEXTURE, EGL_NONE};

            this->pbuffers[0] = eglCreatePbufferSurface(this->egl_display, this->egl_config, stack_attrib_list);
            this->contexts[0] =
                eglCreateContext(this->egl_display, this->egl_config, this->contexts[3], this->attrib_list);
            this->pbuffers[1] = eglCreatePbufferSurface(this->egl_display, this->egl_config, stack_attrib_list);
            this->contexts[1] =
                eglCreateContext(this->egl_display, this->egl_config, this->contexts[3], this->attrib_list);
            this->pbuffers[2] = eglCreatePbufferSurface(this->egl_display, this->egl_config, stack_attrib_list);
            context3 = this->contexts[3];
        } else {
            this->pbuffers[0] = this->pbuffers[3];
            this->contexts[0] =
                eglCreateContext(this->egl_display, this->egl_config, this->contexts[3], this->attrib_list);
            this->pbuffers[1] = this->pbuffers[3];
            this->contexts[1] =
                eglCreateContext(this->egl_display, this->egl_config, this->contexts[3], this->attrib_list);
            this->pbuffers[2] = this->pbuffers[3];
            context3 = this->contexts[3];
        }

        this->contexts[2] = eglCreateContext(this->egl_display, this->egl_config, context3, attrib_list);

        eglMakeCurrent(this->egl_display, this->pbuffers[0], this->pbuffers[0], this->contexts[0]);
        i32 width = 0;
        i32 height = 0;

        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_WIDTH, &width);
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_HEIGHT, &height);
        this->height = height;
        this->width = width;
        DetermineBackBufferResolution(width, height);

        EGLint visual_id;
        eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_NATIVE_VISUAL_ID, &visual_id);

        // ANativeWindow_setBuffersGeometry(this->native_window, this->backing_width, this->backing_height, visual_id);

        eglMakeCurrent(this->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        g_backingWidth = this->backing_width;
        g_backingHeight = this->backing_height;

        nurndr_pixel_width = this->width;
        nurndr_pixel_height = this->height;

        this->context_valid = true;

    } else if (this->native_window != window) {
        eglMakeCurrent(this->egl_display, 0, 0, 0);
        eglDestroySurface(this->egl_display, this->pbuffers[3]);
        eglGetError();
    }

    LOG_DEBUG("this->egl_display: %p, this->pbuffers = {%p, %p, %p, %p}, this->contexts = {%p, %p, %p, %p}",
              this->egl_display, this->pbuffers[0], this->pbuffers[1], this->pbuffers[2], this->pbuffers[3],
              this->contexts[0], this->contexts[1], this->contexts[2], this->contexts[3]);

    pthread_mutex_unlock(&this->mutex);
}

void NuRenderDevice::CheckForRenderWindowInitialisation() {
    if (g_appWindow != 0 && this->field48_0x45 == '\0' && this->focus) {
        if (NuCore::GetApplicationState()->GetStatus() != NuApplicationStatus::ZERO) {
            NuCore::GetApplicationState()->SetStatus(NuApplicationStatus::ZERO);
        }
    }
}

void NuRenderSetThisTreadAsRender() {
    g_renderDevice.SetThisTreadAsRender();
}

void BeginCriticalSectionGL(const char *file, i32 line) {
    g_renderDevice.BeginCriticalSection(file, line);
}

void EndCriticalSectionGL(const char *file, i32 line) {
    g_renderDevice.EndCriticalSection(file, line);
}

void NuRenderDeviceSwapBuffers() {
    g_renderDevice.SwapBuffers();
}
