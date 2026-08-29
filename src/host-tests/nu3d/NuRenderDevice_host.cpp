#include "nu2api/nu3d/NuRenderDevice.h"
#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>
#include <string.h>

static EGLContext g_hostPresentCtx = EGL_NO_CONTEXT;
EGLSurface g_hostPbufferReadback = EGL_NO_SURFACE;
EGLContext g_hostContextReadback = EGL_NO_CONTEXT;

namespace {
    struct PresentResources {
        GLuint program = 0;
        GLint pos_loc = -1;
        GLint uv_loc = -1;
        GLint tex_loc = -1;
        GLuint vbo = 0;
    };
    void EnsurePresentResources(PresentResources &res) {
        if (res.program != 0)
            return;
        const char *vertex_src = "attribute vec2 a_position; attribute vec2 a_texcoord; varying vec2 v_uv; void "
                                 "main(){ gl_Position=vec4(a_position,0,1); v_uv=a_texcoord; }";
        const char *fragment_src = "precision mediump float; varying vec2 v_uv; uniform sampler2D u_tex; void main(){ "
                                   "gl_FragColor=texture2D(u_tex, v_uv); }";
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_src, nullptr);
        glCompileShader(vs);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_src, nullptr);
        glCompileShader(fs);
        res.program = glCreateProgram();
        glAttachShader(res.program, vs);
        glAttachShader(res.program, fs);
        glBindAttribLocation(res.program, 0, "a_position");
        glBindAttribLocation(res.program, 1, "a_texcoord");
        glLinkProgram(res.program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        res.pos_loc = glGetAttribLocation(res.program, "a_position");
        res.uv_loc = glGetAttribLocation(res.program, "a_texcoord");
        res.tex_loc = glGetUniformLocation(res.program, "u_tex");
        glGenBuffers(1, &res.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
        // An FBO texture uses the same bottom-left origin as the default GL
        // framebuffer.  Present it without the image-file/readback flip.
        const float verts[] = {-1, -1, 0, 0, 1, -1, 1, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1};
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
        glVertexAttribPointer(res.pos_loc, 2, GL_FLOAT, GL_FALSE, 16, (void *)0);
        glEnableVertexAttribArray(res.uv_loc);
        glVertexAttribPointer(res.uv_loc, 2, GL_FLOAT, GL_FALSE, 16, (void *)8);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(res.pos_loc);
        glDisableVertexAttribArray(res.uv_loc);
        glUseProgram(0);
    }
} // namespace

void NuRenderDevice::SwapBuffers() {
    extern GLuint g_earlyColorTexture;
    EGLContext present_ctx = (g_hostPresentCtx != EGL_NO_CONTEXT) ? g_hostPresentCtx : this->contexts[3];
    bool have_context = false;
    if (this->egl_display != EGL_NO_DISPLAY && this->pbuffers[3] != EGL_NO_SURFACE && present_ctx != EGL_NO_CONTEXT) {
        have_context = eglMakeCurrent(this->egl_display, this->pbuffers[3], this->pbuffers[3], present_ctx);
        if (!have_context)
            eglGetError();
    }
    if (have_context && g_earlyColorTexture != 0 && glIsTexture(g_earlyColorTexture)) {
        static PresentResources s_present{};
        EnsurePresentResources(s_present);
        EGLint surf_w = 0, surf_h = 0;
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_WIDTH, &surf_w);
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_HEIGHT, &surf_h);
        if (surf_w > 0 && surf_h > 0)
            glViewport(0, 0, surf_w, surf_h);
        DrawFullscreenTexturedQuad(s_present, g_earlyColorTexture);
    }
    if (this->egl_display != EGL_NO_DISPLAY && this->pbuffers[3] != EGL_NO_SURFACE) {
        if (!have_context)
            eglMakeCurrent(this->egl_display, this->pbuffers[3], this->pbuffers[3], present_ctx);
        eglSwapBuffers(this->egl_display, this->pbuffers[3]);
    }
}

i32 HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba) {
    extern GLuint g_earlyColorTexture;
    extern i32 g_backingWidth, g_backingHeight;
    if (g_earlyColorTexture == 0 || g_backingWidth <= 0 || g_backingHeight <= 0 || max_w == 0 || max_h == 0) {
        LOG_WARN("HostReadback: early tex %u backing %d x %d", g_earlyColorTexture, g_backingWidth, g_backingHeight);
        return 0;
    }
    // Try dedicated readback pbuffer/context; fall back to any valid context/pbuffer if unavailable.
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface read_surf = g_hostPbufferReadback;
    EGLContext read_ctx = g_hostContextReadback;
    if (display == EGL_NO_DISPLAY)
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY || read_surf == EGL_NO_SURFACE || read_ctx == EGL_NO_CONTEXT) {
        LOG_WARN("HostReadback: no readback pbuffer (display %p surf %p ctx %p)", display, read_surf, read_ctx);
        return 0;
    }
    if (eglMakeCurrent(display, read_surf, read_surf, read_ctx) == EGL_FALSE) {
        LOG_WARN("HostReadback: eglMakeCurrent failed %d", eglGetError());
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
    glViewport(0, 0, copy_w, copy_h);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glFinish();
    glReadPixels(0, 0, copy_w, copy_h, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &read_fbo);
    return static_cast<i32>(copy_w * 1000 + copy_h);
}

void NuRenderDevice::InitialiseOpenGLContext(ANativeWindow *window_) {
    EGLNativeWindowType window = reinterpret_cast<EGLNativeWindowType>(window_);
    pthread_mutex_lock(&this->mutex);
    this->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    this->is_not_amazon_kf =
        (NuStrICmp(g_deviceManufacturer, "Amazon") != 0 || NuStrNICmp(g_deviceModel, "KF", 2) != 0);
    if (!this->context_valid) {
        this->native_window = window;
        EGLint major, minor;
        eglInitialize(this->egl_display, &major, &minor);
        eglBindAPI(EGL_OPENGL_ES_API);
        this->egl_config = SelectEGLConfig();
        this->pbuffers[3] = eglCreateWindowSurface(this->egl_display, this->egl_config, this->native_window, nullptr);
        if (this->pbuffers[3] == EGL_NO_SURFACE) {
            pthread_mutex_unlock(&this->mutex);
            return;
        }
        this->attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
        this->attrib_list[1] = 2;
        this->attrib_list[2] = EGL_NONE;
        this->contexts[3] = eglCreateContext(this->egl_display, this->egl_config, EGL_NO_CONTEXT, this->attrib_list);
        EGLContext main_ctx = this->contexts[3];
        if (this->field54_0x54) {
            const EGLint a[] = {
                EGL_WIDTH,      1,       EGL_HEIGHT, 1, EGL_TEXTURE_TARGET, EGL_NO_TEXTURE, EGL_TEXTURE_FORMAT,
                EGL_NO_TEXTURE, EGL_NONE};
            this->pbuffers[0] = eglCreatePbufferSurface(this->egl_display, this->egl_config, a);
            this->contexts[0] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[1] = eglCreatePbufferSurface(this->egl_display, this->egl_config, a);
            this->contexts[1] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[2] = eglCreatePbufferSurface(this->egl_display, this->egl_config, a);
        } else {
            this->pbuffers[0] = this->pbuffers[3];
            this->contexts[0] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[1] = this->pbuffers[3];
            this->contexts[1] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
            this->pbuffers[2] = this->pbuffers[3];
        }
        this->contexts[2] = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
        g_hostPresentCtx = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
        const EGLint ra[] = {EGL_WIDTH,          1280,           EGL_HEIGHT, 720, EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
                             EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE, EGL_NONE};
        g_hostPbufferReadback = eglCreatePbufferSurface(this->egl_display, this->egl_config, ra);
        g_hostContextReadback = eglCreateContext(this->egl_display, this->egl_config, main_ctx, this->attrib_list);
        eglMakeCurrent(this->egl_display, this->pbuffers[0], this->pbuffers[0], this->contexts[0]);
        i32 dw = 0, dh = 0;
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_WIDTH, &dw);
        eglQuerySurface(this->egl_display, this->pbuffers[3], EGL_HEIGHT, &dh);
        this->width = dw;
        this->height = dh;
        DetermineBackBufferResolution(dw, dh);
        eglMakeCurrent(this->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        g_backingWidth = this->backing_width;
        g_backingHeight = this->backing_height;
        nurndr_pixel_width = this->width;
        nurndr_pixel_height = this->height;
        this->context_valid = true;
    } else if (this->native_window != window) {
        eglMakeCurrent(this->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(this->egl_display, this->pbuffers[3]);
        eglGetError();
    }
    pthread_mutex_unlock(&this->mutex);
}

extern thread_local i32 s_criticalDepth;
void NuRenderDevice::EndCriticalSection(const char * /*file*/, i32 /*line*/) {
    if (s_criticalDepth > 0)
        s_criticalDepth--;
    if (s_criticalDepth == 0)
        pthread_mutex_unlock(&this->mutex2);
}
