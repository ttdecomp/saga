#include "nu2api/nu3d/NuRenderDevice.h"

#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"

#include <EGL/egl.h>
#include <pthread.h>
#include <GLES2/gl2.h>
#include <math.h>
#include <string.h>

#ifdef HOST_BUILD
#endif

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

    DetermineNominalAspectRatio(this->width, this->height);
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
    this->enabled_extensions[17] = etc1;
    this->enabled_extensions[20] = pvrtc;
    this->enabled_extensions[21] = pvrtc;
    this->enabled_extensions[22] = pvrtc;
    this->enabled_extensions[23] = pvrtc;
    this->enabled_extensions[24] = atc;
    this->enabled_extensions[25] = atc;

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

// The original critical sections are reentrant (Android uses counting
// sections); nested Begin/End pairs like NuIOSInitOpenGLES ->
// NuIOS_AllocateSystemFramebuffers rely on that. Model the recursion with a
// thread-local depth: the outermost Begin takes the mutex and binds this
// thread's GL context, the outermost End releases both.
static thread_local i32 t_glcs_depth = 0;

void NuRenderDevice::BeginCriticalSection(const char *file, i32 line) {
    static thread_local i32 gl_context_index = -1;

    if (t_glcs_depth == 0) {
        pthread_mutex_lock(&this->mutex2);

        i32 i = gl_context_index;
        if (i == -1) {
            i = g_nextGLContextIndex;
            g_nextGLContextIndex = (g_nextGLContextIndex + 1) % 4;
            gl_context_index = i;
        }

        LOG_DEBUG("this->egl_display: %p, this->pbuffers[%d]: %p, this->contexts[%d]: %p", this->egl_display, i,
                  this->pbuffers[i], i, this->contexts[i]);
        eglMakeCurrent(this->egl_display, this->pbuffers[i], this->pbuffers[i], this->contexts[i]);
    }
    t_glcs_depth++;
}

void NuRenderDevice::EndCriticalSection(const char *file, i32 line) {
#ifdef HOST_BUILD
    if (t_glcs_depth > 0) {
        t_glcs_depth--;
    }
    if (t_glcs_depth == 0) {
        pthread_mutex_unlock(&this->mutex2);
    }
#else
    UNIMPLEMENTED();
#endif
}

#ifdef HOST_BUILD
// See note in InitialiseOpenGLContext().
static EGLContext g_hostPresentCtx = EGL_NO_CONTEXT;
#endif

void NuRenderDevice::SwapBuffers() {
#ifdef HOST_BUILD
    // HOST: present the FBO texture (g_earlyColorTexture) to the window via
    // the dedicated present context. The render thread renders to a per-context
    // FBO wrapping the shared texture; pbuffers[0..2] are 1x1 so the window
    // would otherwise stay black. This is the only host PS present logic.
    extern GLuint g_earlyColorTexture;
    extern bool g_hostReadbackReady;
    extern u8 g_hostReadbackRGBA[1280 * 720 * 4];
    extern i32 g_backingWidth, g_backingHeight;

    EGLContext presentCtx = g_hostPresentCtx != EGL_NO_CONTEXT ? g_hostPresentCtx : this->contexts[3];
    bool have_ctx = false;
    if (this->egl_display != EGL_NO_DISPLAY && this->pbuffers[3] != EGL_NO_SURFACE && presentCtx != EGL_NO_CONTEXT) {
        have_ctx = eglMakeCurrent(this->egl_display, this->pbuffers[3], this->pbuffers[3], presentCtx);
        if (!have_ctx) {
            eglGetError();
        }
    }

    if (have_ctx && (g_earlyColorTexture != 0 || g_hostReadbackReady)) {
        static GLuint presentProg = 0;
        static GLint presentPosLoc = -1, presentUvLoc = -1, presentTexLoc = -1;
        static GLuint presentVbo = 0, presentTex = 0;
        if (presentProg == 0) {
            const char *vs = "attribute vec2 a_position; attribute vec2 a_texcoord; varying vec2 v_uv; void main(){ "
                             "gl_Position=vec4(a_position,0,1); v_uv=a_texcoord; }";
            const char *fs = "precision mediump float; varying vec2 v_uv; uniform sampler2D u_tex; void main(){ "
                             "gl_FragColor=texture2D(u_tex, v_uv); }";
            GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vsh, 1, &vs, NULL);
            glCompileShader(vsh);
            GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fsh, 1, &fs, NULL);
            glCompileShader(fsh);
            presentProg = glCreateProgram();
            glAttachShader(presentProg, vsh);
            glAttachShader(presentProg, fsh);
            glBindAttribLocation(presentProg, 0, "a_position");
            glBindAttribLocation(presentProg, 1, "a_texcoord");
            glLinkProgram(presentProg);
            glDeleteShader(vsh);
            glDeleteShader(fsh);
            presentPosLoc = glGetAttribLocation(presentProg, "a_position");
            presentUvLoc = glGetAttribLocation(presentProg, "a_texcoord");
            presentTexLoc = glGetUniformLocation(presentProg, "u_tex");
            glGenBuffers(1, &presentVbo);
            glBindBuffer(GL_ARRAY_BUFFER, presentVbo);
            float verts[] = {-1, -1, 0, 0, 1, -1, 1, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1};
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            glGenTextures(1, &presentTex);
            glBindTexture(GL_TEXTURE_2D, presentTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        GLuint srcTex = 0;
        bool useSharedTex = false;
        if (g_earlyColorTexture != 0 && glIsTexture(g_earlyColorTexture)) {
            srcTex = g_earlyColorTexture;
            useSharedTex = true;
            LOG_WARN("[present] using shared tex %u ready=%d", srcTex, g_hostReadbackReady);
        }
        if (!useSharedTex && g_hostReadbackReady) {
            glBindTexture(GL_TEXTURE_2D, presentTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_backingWidth, g_backingHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         g_hostReadbackRGBA);
            srcTex = presentTex;
            useSharedTex = true;
            LOG_WARN("[present] using readback tex %u", srcTex);
        }
        if (!useSharedTex) {
            LOG_WARN("[present] no tex (early %u isTex %d ready %d)", g_earlyColorTexture,
                     g_earlyColorTexture ? glIsTexture(g_earlyColorTexture) : 0, g_hostReadbackReady);
        }
        if (useSharedTex) {
            EGLint w = 0, h = 0;
            eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_WIDTH, &w);
            eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_HEIGHT, &h);
            if (w > 0 && h > 0)
                glViewport(0, 0, w, h);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_SCISSOR_TEST);
            glUseProgram(presentProg);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, srcTex);
            glUniform1i(presentTexLoc, 0);
            glBindBuffer(GL_ARRAY_BUFFER, presentVbo);
            glEnableVertexAttribArray(presentPosLoc);
            glVertexAttribPointer(presentPosLoc, 2, GL_FLOAT, GL_FALSE, 16, (void *)0);
            glEnableVertexAttribArray(presentUvLoc);
            glVertexAttribPointer(presentUvLoc, 2, GL_FLOAT, GL_FALSE, 16, (void *)8);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisableVertexAttribArray(presentPosLoc);
            glDisableVertexAttribArray(presentUvLoc);
            glUseProgram(0);
        }
    }

    if (this->egl_display != EGL_NO_DISPLAY && this->pbuffers[3] != EGL_NO_SURFACE) {
        if (!have_ctx) {
            eglMakeCurrent(this->egl_display, this->pbuffers[3], this->pbuffers[3], presentCtx);
        }
        eglSwapBuffers(this->egl_display, this->pbuffers[3]);
    }
#else
    UNIMPLEMENTED();
#endif
}

void NuRenderDevice::OnWindowCreated(ANativeWindow *window) {
    InitialiseOpenGLContext(window);
    CheckForRenderWindowInitialisation();
}

#ifdef HOST_BUILD
i32 NuRenderDevice::HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba) {
    extern u8 g_hostReadbackRGBA[1280 * 720 * 4];
    extern bool g_hostReadbackReady;
    extern i32 g_backingWidth, g_backingHeight;
    // Fast path: the render thread already copied the FBO into the shared CPU
    // buffer with correct 1280x720 size. No EGL/GL needed.
    if (g_hostReadbackReady && g_backingWidth > 0 && g_backingHeight > 0) {
        u32 cw = (u32)g_backingWidth < max_w ? (u32)g_backingWidth : max_w;
        u32 ch = (u32)g_backingHeight < max_h ? (u32)g_backingHeight : max_h;
        // g_hostReadbackRGBA is bottom-up (glReadPixels); HostReadback caller
        // expects bottom-up (test_window does flip). Copy and pack.
        // Simple memcpy with flip is done by caller; here we just memcpy the
        // requested region bottom-up as stored.
        // For simplicity, assume cw==max_w && ch==max_h (1280x720) which the
        // test uses; otherwise do a row-wise copy.
        if (cw == (u32)g_backingWidth && ch == (u32)g_backingHeight) {
            memcpy(rgba, g_hostReadbackRGBA, (usize)cw * ch * 4);
        } else {
            for (u32 y = 0; y < ch; y++) {
                memcpy(rgba + (usize)y * cw * 4, g_hostReadbackRGBA + (usize)y * g_backingWidth * 4, (usize)cw * 4);
            }
        }
        return (i32)(cw * 1000 + ch);
    }

    if (egl_display == EGL_NO_DISPLAY || pbuffer_readback == EGL_NO_SURFACE || context_readback == EGL_NO_CONTEXT) {
        return 0;
    }
    if (eglMakeCurrent(egl_display, pbuffer_readback, pbuffer_readback, context_readback) == EGL_FALSE) {
        eglGetError(); // clear; another MakeCurrent is in flight, skip this read
        return 0;
    }

    EGLint w = 0, h = 0;
    eglQuerySurface(egl_display, pbuffer_readback, EGL_WIDTH, &w);
    eglQuerySurface(egl_display, pbuffer_readback, EGL_HEIGHT, &h);
    if (w <= 0 || h <= 0 || max_w == 0 || max_h == 0) {
        return 0;
    }
    u32 cw = (u32)w < max_w ? (u32)w : max_w;
    u32 ch = (u32)h < max_h ? (u32)h : max_h;

    // Fallback: re-draw the shared texture into the readback pbuffer. This
    // path is used before the first frame is ready.
    if (g_earlyColorTexture != 0 && glIsTexture(g_earlyColorTexture)) {
        // Simple blit of the shared texture into this pbuffer's default FBO
        // so glReadPixels below sees the last frame. Use the same present
        // shader as SwapBuffers but targeting this pbuffer.
        static GLuint rbProg = 0;
        static GLint rbPosLoc = -1, rbUvLoc = -1, rbTexLoc = -1;
        static GLuint rbVbo = 0;
        if (rbProg == 0) {
            const char *vs = "attribute vec2 a_position; attribute vec2 a_texcoord; varying vec2 v_uv; void main(){ "
                             "gl_Position=vec4(a_position,0,1); v_uv=a_texcoord; }";
            const char *fs = "precision mediump float; varying vec2 v_uv; uniform sampler2D u_tex; void main(){ "
                             "gl_FragColor=texture2D(u_tex, v_uv); }";
            GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vsh, 1, &vs, NULL);
            glCompileShader(vsh);
            GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fsh, 1, &fs, NULL);
            glCompileShader(fsh);
            rbProg = glCreateProgram();
            glAttachShader(rbProg, vsh);
            glAttachShader(rbProg, fsh);
            glBindAttribLocation(rbProg, 0, "a_position");
            glBindAttribLocation(rbProg, 1, "a_texcoord");
            glLinkProgram(rbProg);
            glDeleteShader(vsh);
            glDeleteShader(fsh);
            rbPosLoc = glGetAttribLocation(rbProg, "a_position");
            rbUvLoc = glGetAttribLocation(rbProg, "a_texcoord");
            rbTexLoc = glGetUniformLocation(rbProg, "u_tex");
            glGenBuffers(1, &rbVbo);
            glBindBuffer(GL_ARRAY_BUFFER, rbVbo);
            float verts[] = {-1, -1, 0, 0, 1, -1, 1, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1};
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (GLsizei)cw, (GLsizei)ch);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(rbProg);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_earlyColorTexture);
        glUniform1i(rbTexLoc, 0);
        glBindBuffer(GL_ARRAY_BUFFER, rbVbo);
        glEnableVertexAttribArray(rbPosLoc);
        glVertexAttribPointer(rbPosLoc, 2, GL_FLOAT, GL_FALSE, 16, (void *)0);
        glEnableVertexAttribArray(rbUvLoc);
        glVertexAttribPointer(rbUvLoc, 2, GL_FLOAT, GL_FALSE, 16, (void *)8);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(rbPosLoc);
        glDisableVertexAttribArray(rbUvLoc);
        glUseProgram(0);
        glFinish();
    } else {
        glFinish();
    }

    glViewport(0, 0, (GLsizei)cw, (GLsizei)ch);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, (GLsizei)cw, (GLsizei)ch, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    return (i32)(cw * 1000 + ch); // pack w*1000+h
}
#endif

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

#ifdef HOST_BUILD
        // HOST-ONLY: dedicated present-side context in the same share group.
        // On Android the render threads own contexts[0..3]; the host pumps
        // presents from a separate thread and needs a context that the engine
        // never binds, otherwise eglMakeCurrent below collides with whatever
        // the worker left current.
        g_hostPresentCtx = eglCreateContext(this->egl_display, this->egl_config, this->contexts[3], attrib_list);

        // HOST-ONLY: separate 1x1 pbuffer + share-group context used only by
        // HostReadbackPixels. EGL forbids two contexts on one surface, so
        // reading frames back through pbuffers[3] raced with the game
        // thread's SwapBuffers and silently produced stale/black reads.
        EGLint rb_attribs[] = {EGL_WIDTH,          1280,           EGL_HEIGHT, 720, EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
                               EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE, EGL_NONE};
        this->pbuffer_readback = eglCreatePbufferSurface(this->egl_display, this->egl_config, rb_attribs);
        this->context_readback = eglCreateContext(this->egl_display, this->egl_config, this->contexts[3], attrib_list);
#endif

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
        NuCore::GetApplicationState()->SetStatus(NUAPPLICATIONSTATUS{});
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

struct numtl_s;
typedef struct NuVertexFormatPS NuVertexFormatPS;

extern "C" {
    static __used__ void NuIOS_BindVertexAttributes(u32, u32) {
    }

    static __used__ void NuIOS_BindVertexAttributesImmediate(u32, u32) {
    }

    static __used__ void NuIOS_BindVertexAttributesImmediateOverrideDataLayout(u32, u32, void *) {
    }

    static __used__ void NuIOS_BindVertexAttributesInternal(i32, i32, u32 *, u32) {
    }

    static __used__ void NuIOS_BindVertexAttributesOverrideDataLayout(u32, u32) {
    }

    static __used__ void NuIOS_GetAlphaTestParameters(f32 *) {
    }

    static __used__ u8 *NuRenderContextGetKTint(void) {
        return 0;
    }

    static __used__ struct numtl_s *NuRenderContextGetMaterialInUse(void) {
        return 0;
    }

    static __used__ void NuRenderContextSetKTint(f32 *) {
    }

    static __used__ void NuRenderContextSetWorld(NUMTX *) {
    }

    static __used__ void NuRenderContextSetWorld_transpose(NUMTX *) {
    }

    static __used__ void NuRenderContextSetZFunc_inline(i32) {
    }
}

static __used__ i32 NuIOS_GetOrCreateVAO(u32, u32, u32, NuVertexFormatPS *) {
    return 0;
}

static __used__ void Nu360SetObjectShadowFactor(f32) {
}
