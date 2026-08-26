#include "nu2api/nu3d/NuRenderDevice.h"

#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

// ---------------------------------------------------------------------------
// NuRenderDevice — Android GLES2 render device + host EGL shim
//
// Original TU: nu2api.saga/nu3d/android/NuRenderDevice_gles2.cpp
//  (see doc/source-structure.md). This file merges the device lifetime,
//  EGL config/context management, extension probing, and the re-entrant
//  GL critical section that serialises all GL calls on Android. Host
//  additions (HOST_BUILD) provide a desktop EGL window + readback path
//  so the host test harness can present and capture frames without
//  diverging from the shared render path.
// ---------------------------------------------------------------------------

NuRenderDevice g_renderDevice{};

// Android render work is dispatched across a small ring of shared EGL
// contexts (indices 0..3). Index 3 is the "main" / window context; the
// others are worker contexts. gt_glContextIndex tracks which slot the
// calling thread was assigned.
thread_local i32 gt_glContextIndex = 0;

// Host-only present context: a dedicated context in the same share group
// that the host present thread uses so it never collides with whatever
// the engine left current on contexts[3].
#ifdef HOST_BUILD
static EGLContext g_hostPresentCtx = EGL_NO_CONTEXT;
#endif

// Re-entrant GL critical section.
//
// The original Android code uses a counting critical section — nested
// Begin/End pairs are common (e.g. NuIOSInitOpenGLES →
// NuIOS_AllocateSystemFramebuffers). The host port models that with a
// thread-local recursion depth: only the outermost Begin locks mutex2
// and binds the thread's assigned context; the outermost End releases
// both.
static thread_local i32 s_criticalDepth = 0;
static i32 g_nextGLContextIndex = 0;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

NuRenderDeviceGen::NuRenderDeviceGen() : value(false) {
}

static void InitRecursiveMutex(pthread_mutex_t *mutex) {
    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &attrs);
    pthread_mutexattr_destroy(&attrs);
}

NuRenderDevice::NuRenderDevice() : NuRenderDeviceGen() {
    InitRecursiveMutex(&this->mutex);

    // is_not_amazon_kf — true unless the device is an Amazon Kindle Fire
    // (manufacturer "Amazon" + model prefix "KF"). Kindle Fire gets a
    // down-scaled backbuffer path in DetermineBackBufferResolution().
    this->is_not_amazon_kf = true;
    this->focus = false;
    this->context_valid = false;
    this->egl_display = EGL_NO_DISPLAY;

    for (i32 i = 0; i < 4; i++) {
        this->contexts[i] = EGL_NO_CONTEXT;
    }

    // field54_0x54 — original bool that selects whether worker contexts
    // get private 1x1 pbuffers (true) or alias the window surface (false).
    // Always true on this build.
    this->field54_0x54 = true;

    this->backing_width = 1280;
    this->width = 1280;
    this->backing_height = 720;
    this->height = 720;
}

// ---------------------------------------------------------------------------
// Optional GLES2 extensions (loaded via eglGetProcAddress)
// ---------------------------------------------------------------------------

// Untyped proc pointers — kept as in the original; only existence is
// checked elsewhere. Typed PFN typedefs would be more precise but would
// diverge from the decompiled signature.
void (*glGetProgramBinaryOES)();
void (*glProgramBinaryOES)();
void (*glDiscardFramebufferEXT)();
void (*glGenVertexArraysOES)();
void (*glBindVertexArrayOES)();
void (*glDeleteVertexArraysOES)();

void NuGLES2ExtensionsInit() {
    glGetProgramBinaryOES = reinterpret_cast<void (*)()>(eglGetProcAddress("glGetProgramBinaryOES"));
    glProgramBinaryOES = reinterpret_cast<void (*)()>(eglGetProcAddress("glProgramBinaryOES"));
    glDiscardFramebufferEXT = reinterpret_cast<void (*)()>(eglGetProcAddress("glDiscardFramebufferEXT"));
    glGenVertexArraysOES = reinterpret_cast<void (*)()>(eglGetProcAddress("glGenVertexArraysOES"));
    glBindVertexArrayOES = reinterpret_cast<void (*)()>(eglGetProcAddress("glBindVertexArrayOES"));
    glDeleteVertexArraysOES = reinterpret_cast<void (*)()>(eglGetProcAddress("glDeleteVertexArraysOES"));
}

// ---------------------------------------------------------------------------
// Extension string helpers
// ---------------------------------------------------------------------------

bool NuRenderDevice::IsExtensionSupported(const char *wanted) {
    // Reject malformed queries: empty string or embedded space would
    // otherwise match substrings incorrectly.
    if (wanted[0] == '\0' || strchr(wanted, ' ') != nullptr) {
        return false;
    }

    const usize wanted_len = strlen(wanted);
    const char *cursor = this->extensions;

    while (true) {
        const char *found = strstr(cursor, wanted);
        if (found == nullptr) {
            return false;
        }

        const bool at_start = (found == cursor);
        const bool preceded_by_space = at_start || found[-1] == ' ';
        const char terminator = found[wanted_len];
        // Terminator is either end-of-string ('\0'), space, or other
        // whitespace — the original uses a case-insensitive space check
        // via (c & 0xDF) != 0 which effectively treats '\0' as
        // terminator and space as boundary.
        const bool followed_by_boundary = (terminator == '\0' || terminator == ' ');

        if (preceded_by_space && followed_by_boundary) {
            return true;
        }

        // Whole-word match failed — continue searching past this occurrence.
        cursor = found + wanted_len;
    }
}

i32 _NuCheckGLErrors(const char * /*file*/) {
    // Original is a no-op in release; kept as a hook for debug builds.
    return 0;
}

// ---------------------------------------------------------------------------
// Device bring-up
// ---------------------------------------------------------------------------

void NuRenderDevice::Initialize() {
    // Wait for the EGL display / window surface to become valid. On
    // Android this is signalled from the Java activity thread via
    // OnWindowCreated() → InitialiseOpenGLContext().
    while (!this->context_valid) {
        NuThreadSleep(1);
    }

    FrameEnd();

    InitRecursiveMutex(&this->mutex2);
    NuGLES2ExtensionsInit();

    BeginCriticalSection("none", -1);

    // Probe the chosen EGL config for logging / diagnostics. The
    // attribute ids are the standard EGL_*_SIZE values:
    //  0x3024 EGL_RED_SIZE, 0x3022 EGL_BLUE_SIZE, 0x3023 EGL_GREEN_SIZE,
    //  0x3021 EGL_ALPHA_SIZE, 0x3025 EGL_DEPTH_SIZE, 0x3026 EGL_STENCIL_SIZE.
    EGLint config_attribs[6] = {};
    eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_RED_SIZE, &config_attribs[0]);
    eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_BLUE_SIZE, &config_attribs[1]);
    eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_GREEN_SIZE, &config_attribs[2]);
    eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_ALPHA_SIZE, &config_attribs[3]);
    eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_DEPTH_SIZE, &config_attribs[4]);
    eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_STENCIL_SIZE, &config_attribs[5]);
    (void)config_attribs;

    DetermineNominalAspectRatio(this->width, this->height);
    this->aspect_ratio = static_cast<f32>(this->width) / static_cast<f32>(this->height);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->max_texture_units);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->max_texture_size);

    this->extensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

    bool has_dxt1 = false;
    bool has_atc = false;
    bool has_pvrtc = false;
    bool has_etc1 = false;

    if (this->extensions != nullptr) {
        const bool dxt1_ext = IsExtensionSupported("EXT_texture_compression_dxt1");
        const bool dxt1_gl = IsExtensionSupported("GL_EXT_texture_compression_dxt1");
        has_dxt1 = dxt1_ext || dxt1_gl;
        has_atc = IsExtensionSupported("GL_AMD_compressed_ATC_texture");
        has_pvrtc = IsExtensionSupported("GL_IMG_texture_compression_pvrtc");
        has_etc1 = IsExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
    }

    memset(this->enabled_extensions, 0, sizeof(this->enabled_extensions));

    // Always available (uncompressed RGBA).
    this->enabled_extensions[NUTEX_RGBA32] = 1;

    // Compressed families — enabled iff the driver advertises support.
    this->enabled_extensions[NUTEX_DXT1] = has_dxt1;
    this->enabled_extensions[NUTEX_DX1A] = has_dxt1;
    this->enabled_extensions[NUTEX_DXT5] = has_dxt1;
    this->enabled_extensions[NUTEX_ETC1] = has_etc1;
    this->enabled_extensions[NUTEX_PVRTC2] = has_pvrtc;
    this->enabled_extensions[NUTEX_PVRTC2A] = has_pvrtc;
    this->enabled_extensions[NUTEX_PVRTC4] = has_pvrtc;
    this->enabled_extensions[NUTEX_PVRTC4A] = has_pvrtc;
    this->enabled_extensions[NUTEX_ATCA] = has_atc;
    this->enabled_extensions[NUTEX_ATC] = has_atc;

    if (this->extensions != nullptr) {
        this->oes_packed_depth_stencil = IsExtensionSupported("GL_OES_packed_depth_stencil");
        this->oes_depth24 = IsExtensionSupported("GL_OES_depth24");
        this->oes_depth_texture = IsExtensionSupported("GL_OES_depth_texture");
    }

    // Choose the runtime texture-compression platform. g_forceETC1 forces
    // ETC1 even when better formats are available; otherwise prefer
    // S3TC → PVRTC → ATC → ETC1.
    if (g_forceETC1 == 0 || !has_etc1) {
        if (has_dxt1) {
            NuPlatform::Get()->SetCurrentPlatform(ANDROID_S3TC_PLATFORM);
        } else if (has_pvrtc) {
            NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);
        } else if (has_atc) {
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
    // No-op in this build — original flushed per-frame bookkeeping.
}

void NuRenderDevice::SetThisTreadAsRender() {
    // Historical typo preserved: "Tread" for "Thread". Index 3 is the
    // main render thread's slot.
    gt_glContextIndex = 3;
}

// ---------------------------------------------------------------------------
// Re-entrant GL critical section
// ---------------------------------------------------------------------------

void NuRenderDevice::BeginCriticalSection(const char * /*file*/, i32 /*line*/) {
    static thread_local i32 s_assignedContextIndex = -1;

    if (s_criticalDepth == 0) {
        pthread_mutex_lock(&this->mutex2);

        i32 slot = s_assignedContextIndex;
        if (slot == -1) {
            slot = g_nextGLContextIndex;
            g_nextGLContextIndex = (g_nextGLContextIndex + 1) % 4;
            s_assignedContextIndex = slot;
        }

        LOG_DEBUG("this->egl_display: %p, this->pbuffers[%d]: %p, this->contexts[%d]: %p", this->egl_display, slot,
                  this->pbuffers[slot], slot, this->contexts[slot]);
        eglMakeCurrent(this->egl_display, this->pbuffers[slot], this->pbuffers[slot], this->contexts[slot]);
    }
    s_criticalDepth++;
}

void NuRenderDevice::EndCriticalSection(const char * /*file*/, i32 /*line*/) {
#ifdef HOST_BUILD
    if (s_criticalDepth > 0) {
        s_criticalDepth--;
    }
    if (s_criticalDepth == 0) {
        pthread_mutex_unlock(&this->mutex2);
    }
#else
    UNIMPLEMENTED();
#endif
}

// ---------------------------------------------------------------------------
// Host presentation + readback (HOST_BUILD only)
// ---------------------------------------------------------------------------

#ifdef HOST_BUILD

namespace {

    struct PresentResources {
        GLuint program = 0;
        GLint pos_loc = -1;
        GLint uv_loc = -1;
        GLint tex_loc = -1;
        GLuint vbo = 0;
    };

    void EnsurePresentResources(PresentResources &res) {
        if (res.program != 0) {
            return;
        }

        const char *vertex_src = "attribute vec2 a_position; attribute vec2 a_texcoord; varying vec2 v_uv; "
                                 "void main(){ gl_Position=vec4(a_position,0,1); v_uv=a_texcoord; }";
        const char *fragment_src = "precision mediump float; varying vec2 v_uv; uniform sampler2D u_tex; "
                                   "void main(){ gl_FragColor=texture2D(u_tex, v_uv); }";

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_src, nullptr);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_src, nullptr);
        glCompileShader(fragment_shader);

        res.program = glCreateProgram();
        glAttachShader(res.program, vertex_shader);
        glAttachShader(res.program, fragment_shader);
        glBindAttribLocation(res.program, 0, "a_position");
        glBindAttribLocation(res.program, 1, "a_texcoord");
        glLinkProgram(res.program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        res.pos_loc = glGetAttribLocation(res.program, "a_position");
        res.uv_loc = glGetAttribLocation(res.program, "a_texcoord");
        res.tex_loc = glGetUniformLocation(res.program, "u_tex");

        glGenBuffers(1, &res.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
        // Full-screen quad as two triangles (positions + UVs interleaved).
        // V flipped to present FBO correctly to the window.
        const float verts[] = {
            -1, -1, 0, 1, //
            1,  -1, 1, 1, //
            -1, 1,  0, 0, //
            1,  -1, 1, 1, //
            1,  1,  1, 0, //
            -1, 1,  0, 0, //
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    }

    void DrawFullscreenTexturedQuad(const PresentResources &res, GLuint texture) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);
        glUseProgram(res.program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(res.tex_loc, 0);
        glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
        glEnableVertexAttribArray(res.pos_loc);
        glVertexAttribPointer(res.pos_loc, 2, GL_FLOAT, GL_FALSE, 16, reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(res.uv_loc);
        glVertexAttribPointer(res.uv_loc, 2, GL_FLOAT, GL_FALSE, 16, reinterpret_cast<void *>(8));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(res.pos_loc);
        glDisableVertexAttribArray(res.uv_loc);
        glUseProgram(0);
    }

} // namespace

void NuRenderDevice::SwapBuffers() {
    // Present the shared FBO texture (g_earlyColorTexture) to the host
    // window via the dedicated present context. The render thread draws
    // into a per-context FBO wrapping that texture; the window's own
    // default framebuffer is only written here, otherwise it would stay
    // black because pbuffers[0..2] are 1x1.
    extern GLuint g_earlyColorTexture;

    EGLContext present_ctx = (g_hostPresentCtx != EGL_NO_CONTEXT) ? g_hostPresentCtx : this->contexts[3];
    bool have_context = false;
    if (this->egl_display != EGL_NO_DISPLAY && this->pbuffers[3] != EGL_NO_SURFACE && present_ctx != EGL_NO_CONTEXT) {
        have_context = eglMakeCurrent(this->egl_display, this->pbuffers[3], this->pbuffers[3], present_ctx);
        if (!have_context) {
            eglGetError();
        }
    }

    if (have_context && g_earlyColorTexture != 0 && glIsTexture(g_earlyColorTexture)) {
        static PresentResources s_present{};
        EnsurePresentResources(s_present);

        EGLint surf_w = 0;
        EGLint surf_h = 0;
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_WIDTH, &surf_w);
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_HEIGHT, &surf_h);
        if (surf_w > 0 && surf_h > 0) {
            glViewport(0, 0, surf_w, surf_h);
        }
        DrawFullscreenTexturedQuad(s_present, g_earlyColorTexture);
    }

    if (this->egl_display != EGL_NO_DISPLAY && this->pbuffers[3] != EGL_NO_SURFACE) {
        if (!have_context) {
            eglMakeCurrent(this->egl_display, this->pbuffers[3], this->pbuffers[3], present_ctx);
        }
        eglSwapBuffers(this->egl_display, this->pbuffers[3]);
    }
}

void NuRenderDevice::OnWindowCreated(ANativeWindow *window) {
    InitialiseOpenGLContext(window);
    CheckForRenderWindowInitialisation();
}

i32 NuRenderDevice::HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba) {
    // Direct FBO screenshot: attach the shared color texture to a transient
    // read FBO and pull pixels via glReadPixels. This avoids the intermediate
    // CPU mirror (g_hostReadbackRGBA) and its per-frame memcpy.
    extern GLuint g_earlyColorTexture;
    extern i32 g_backingWidth, g_backingHeight;

    if (g_earlyColorTexture == 0 || g_backingWidth <= 0 || g_backingHeight <= 0 || max_w == 0 || max_h == 0) {
        return 0;
    }
    if (egl_display == EGL_NO_DISPLAY || pbuffer_readback == EGL_NO_SURFACE || context_readback == EGL_NO_CONTEXT) {
        return 0;
    }
    if (eglMakeCurrent(egl_display, pbuffer_readback, pbuffer_readback, context_readback) == EGL_FALSE) {
        eglGetError();
        return 0;
    }

    const u32 copy_w = static_cast<u32>(g_backingWidth) < max_w ? static_cast<u32>(g_backingWidth) : max_w;
    const u32 copy_h = static_cast<u32>(g_backingHeight) < max_h ? static_cast<u32>(g_backingHeight) : max_h;

    GLuint read_fbo = 0;
    glGenFramebuffers(1, &read_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, read_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_earlyColorTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &read_fbo);
        return 0;
    }

    glViewport(0, 0, static_cast<GLsizei>(copy_w), static_cast<GLsizei>(copy_h));
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    // Ensure any pending render-thread work on the shared texture is visible.
    glFinish();
    glReadPixels(0, 0, static_cast<GLsizei>(copy_w), static_cast<GLsizei>(copy_h), GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &read_fbo);
    return static_cast<i32>(copy_w * 1000 + copy_h);
}

#else // !HOST_BUILD

void NuRenderDevice::SwapBuffers() {
    UNIMPLEMENTED();
}

void NuRenderDevice::OnWindowCreated(ANativeWindow *window) {
    InitialiseOpenGLContext(window);
    CheckForRenderWindowInitialisation();
}

#endif // HOST_BUILD

// ---------------------------------------------------------------------------
// EGL config selection + backbuffer sizing
// ---------------------------------------------------------------------------

EGLConfig NuRenderDevice::SelectEGLConfig() {
    // Preferred EGL config: 565 colour, 24-bit depth, GLES2 conformant,
    // pbuffer + window capable.
    static const EGLint kPreferredAttribs[] = {
        EGL_DEPTH_SIZE,
        24, //
        EGL_LEVEL,
        0, //
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        EGL_CONFORMANT,
        EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE,
        5, //
        EGL_GREEN_SIZE,
        6, //
        EGL_RED_SIZE,
        5, //
        EGL_ALPHA_SIZE,
        0, //
        EGL_STENCIL_SIZE,
        0, //
        EGL_NONE,
    };

    pthread_mutex_lock(&this->mutex);

    EGLint attrib_list[sizeof(kPreferredAttribs) / sizeof(kPreferredAttribs[0])];
    memcpy(attrib_list, kPreferredAttribs, sizeof(kPreferredAttribs));

    EGLConfig configs[32];
    i32 num_configs = 0;
    EGLBoolean ok = eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_configs);

    if (num_configs == 0 || ok == EGL_FALSE) {
        // Retry with 16-bit depth if 24-bit is unavailable (older devices).
        attrib_list[1] = 16; // EGL_DEPTH_SIZE value slot
        eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_configs);
    }

    pthread_mutex_unlock(&this->mutex);

    return configs[0];
}

void NuRenderDevice::DetermineBackBufferResolution(i32 width, i32 height) {
    this->backing_width = static_cast<u32>(width);
    this->backing_height = static_cast<u32>(height);

    NuDeviceSpecs::Create();
    // On low-spec non-Kindle devices, clamp width to 1280 to keep fill
    // rate in check and scale height to preserve aspect.
    if (this->is_not_amazon_kf && NuDeviceSpecs::ms_instance->specs < 3 && this->backing_width > 1280) {
        this->backing_width = 1280;
        this->backing_height = static_cast<u32>(static_cast<f32>(height) / static_cast<f32>(width) * 1280.0f);
    }
}

void NuRenderDevice::InitialiseOpenGLContext(ANativeWindow *window_) {
    EGLNativeWindowType window = reinterpret_cast<EGLNativeWindowType>(window_);

    pthread_mutex_lock(&this->mutex);

    this->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    // Re-evaluate Kindle Fire detection from the live device properties.
    this->is_not_amazon_kf =
        (NuStrICmp(g_deviceManufacturer, "Amazon") != 0 || NuStrNICmp(g_deviceModel, "KF", 2) != 0);

    LOG_DEBUG("this->context_valid: %d, this->native_window: %p, window: %p", this->context_valid, this->native_window,
              window);

    if (!this->context_valid) {
        this->native_window = window;

        EGLint major = 0;
        EGLint minor = 0;
        eglInitialize(this->egl_display, &major, &minor);
        eglBindAPI(EGL_OPENGL_ES_API);

        this->egl_config = SelectEGLConfig();

        this->pbuffers[3] = eglCreateWindowSurface(this->egl_display, this->egl_config, this->native_window, nullptr);
        if (this->pbuffers[3] == EGL_NO_SURFACE) {
            LOG_ERR("eglCreateWindowSurface failed: %d", eglGetError());
            pthread_mutex_unlock(&this->mutex);
            return;
        }

        this->attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
        this->attrib_list[1] = 2;
        this->attrib_list[2] = EGL_NONE;

        this->contexts[3] = eglCreateContext(this->egl_display, this->egl_config, EGL_NO_CONTEXT, this->attrib_list);

        // Worker contexts share resources with the main context. When
        // field54_0x54 is set they each get a private 1x1 pbuffer so GL
        // calls don't need the window surface.
        EGLContext main_ctx = this->contexts[3];
        if (this->field54_0x54) {
            const EGLint pbuffer_attribs[] = {
                EGL_WIDTH,          1, //
                EGL_HEIGHT,         1, //
                EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
                EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
                EGL_NONE,
            };
            this->pbuffers[0] = eglCreatePbufferSurface(this->egl_display, this->egl_config, pbuffer_attribs);
            this->contexts[0] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[1] = eglCreatePbufferSurface(this->egl_display, this->egl_config, pbuffer_attribs);
            this->contexts[1] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[2] = eglCreatePbufferSurface(this->egl_display, this->egl_config, pbuffer_attribs);
        } else {
            // Alias the window surface — legacy path, not used on host.
            this->pbuffers[0] = this->pbuffers[3];
            this->contexts[0] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[1] = this->pbuffers[3];
            this->contexts[1] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[2] = this->pbuffers[3];
        }

        this->contexts[2] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);

#ifdef HOST_BUILD
        // Host extras: a present-only context and a dedicated readback
        // pbuffer/context so HostReadbackPixels never contends with
        // SwapBuffers on pbuffers[3]. Both share with the main context.
        g_hostPresentCtx = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);

        const EGLint readback_attribs[] = {
            EGL_WIDTH,          1280, //
            EGL_HEIGHT,         720,  //
            EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
            EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
            EGL_NONE,
        };
        this->pbuffer_readback = eglCreatePbufferSurface(this->egl_display, this->egl_config, readback_attribs);
        this->context_readback = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
#endif

        // Make worker 0 current briefly to query the actual window size
        // and set up the nominal backbuffer dimensions.
        eglMakeCurrent(this->egl_display, this->pbuffers[0], this->pbuffers[0], this->contexts[0]);
        i32 drawable_w = 0;
        i32 drawable_h = 0;
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_WIDTH, &drawable_w);
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_HEIGHT, &drawable_h);
        this->width = static_cast<u32>(drawable_w);
        this->height = static_cast<u32>(drawable_h);
        DetermineBackBufferResolution(drawable_w, drawable_h);

        EGLint visual_id = 0;
        eglGetConfigAttrib(this->egl_display, this->egl_config, EGL_NATIVE_VISUAL_ID, &visual_id);
        (void)visual_id;
        // Original called ANativeWindow_setBuffersGeometry here; not needed
        // on host where the SDL-provided window already has the desired
        // visual.

        eglMakeCurrent(this->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        g_backingWidth = static_cast<i32>(this->backing_width);
        g_backingHeight = static_cast<i32>(this->backing_height);
        nurndr_pixel_width = static_cast<i32>(this->width);
        nurndr_pixel_height = static_cast<i32>(this->height);

        this->context_valid = true;

    } else if (this->native_window != window) {
        // Window was recreated (e.g. orientation change) — tear down the
        // old window surface. A new one will be created on the next valid
        // call.
        eglMakeCurrent(this->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(this->egl_display, this->pbuffers[3]);
        eglGetError();
    }

    LOG_DEBUG("this->egl_display: %p, this->pbuffers = {%p, %p, %p, %p}, this->contexts = {%p, %p, %p, %p}",
              this->egl_display, this->pbuffers[0], this->pbuffers[1], this->pbuffers[2], this->pbuffers[3],
              this->contexts[0], this->contexts[1], this->contexts[2], this->contexts[3]);

    pthread_mutex_unlock(&this->mutex);
}

void NuRenderDevice::CheckForRenderWindowInitialisation() {
    // Once the window is live and focused, transition the application
    // state machine so the engine starts submitting frames.
    if (g_appWindow != 0 && this->field48_0x45 == '\0' && this->focus) {
        NuCore::GetApplicationState()->SetStatus(NUAPPLICATIONSTATUS{});
    }
}

// ---------------------------------------------------------------------------
// C linkage helpers — the engine calls these without needing the C++ type
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Stubs for iOS/legacy vertex paths that are not used on this platform
// ---------------------------------------------------------------------------

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
        return nullptr;
    }

    static __used__ struct numtl_s *NuRenderContextGetMaterialInUse(void) {
        return nullptr;
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
