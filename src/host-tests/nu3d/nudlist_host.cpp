// Host sparse override for display-list scene execution.
// Provides per-context FBO wrapping the shared 1280x720 texture because
// the Android 1x1 pbuffer is not useful on desktop. Faithful code lives
// in src/nu2api/nu3d/nudlist.cpp (weak); this host version is sparse and
// correct only for BUILD_FOR_HOST.

#include <GLES2/gl2.h>

#include "decomp.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/numtl.h"

extern "C" NUDLIST_MANAGER global_dlist_manager;
extern GLuint g_earlyColorTexture;
extern i32 g_backingWidth, g_backingHeight;

extern "C" void NuDisplayListDrawRenderScene(i32 render_scene_id) {
    NUDLIST_MANAGER *mgr = &global_dlist_manager;

    GLuint hostPrevFbo = 0;
    bool hostFboBound = false;
    thread_local GLuint t_hostFbo = 0;
    thread_local GLuint t_hostDepth = 0;

    auto ensureHostFbo = [&]() -> bool {
        if (g_earlyColorTexture == 0)
            return false;
        if (t_hostFbo == 0) {
            glGenFramebuffers(1, &t_hostFbo);
            glBindFramebuffer(GL_FRAMEBUFFER, t_hostFbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_earlyColorTexture, 0);
            glGenRenderbuffers(1, &t_hostDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, t_hostDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, g_backingWidth, g_backingHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, t_hostDepth);
            glCheckFramebufferStatus(GL_FRAMEBUFFER);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint *>(&hostPrevFbo));
        glBindFramebuffer(GL_FRAMEBUFFER, t_hostFbo);
        glViewport(0, 0, g_backingWidth, g_backingHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return true;
    };

    auto drawLegalFallback = [&]() {
        NUNATIVETEX *legal = NuTexGetNative(1);
        if (!legal)
            return;
        if (legal->platform.gl_tex == 0 && legal->size != 0) {
            NuTexCreatePS(legal, true);
            if (legal->platform.gl_tex == 0)
                NuTexCreatePS(legal, false);
        }
        if (legal->platform.gl_tex == 0)
            return;
        struct LegalProg {
            GLuint prog = 0;
            GLint pos_loc = -1, uv_loc = -1, tex_loc = -1;
            GLuint vbo = 0;
            void ensure() {
                if (prog)
                    return;
                const char *vs = "attribute vec2 a_pos; attribute vec2 a_uv; varying vec2 v_uv; void "
                                 "main(){gl_Position=vec4(a_pos,0,1); v_uv=a_uv;}";
                const char *fs = "precision mediump float; varying vec2 v_uv; uniform sampler2D u_tex; void "
                                 "main(){gl_FragColor=texture2D(u_tex,v_uv);}";
                GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vsh, 1, &vs, nullptr);
                glCompileShader(vsh);
                GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fsh, 1, &fs, nullptr);
                glCompileShader(fsh);
                prog = glCreateProgram();
                glAttachShader(prog, vsh);
                glAttachShader(prog, fsh);
                glBindAttribLocation(prog, 0, "a_pos");
                glBindAttribLocation(prog, 1, "a_uv");
                glLinkProgram(prog);
                glDeleteShader(vsh);
                glDeleteShader(fsh);
                pos_loc = glGetAttribLocation(prog, "a_pos");
                uv_loc = glGetAttribLocation(prog, "a_uv");
                tex_loc = glGetUniformLocation(prog, "u_tex");
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                float verts[] = {-1, -1, 0, 0, 1, -1, 1, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1};
                glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            }
        };
        static LegalProg lp;
        lp.ensure();
        glUseProgram(lp.prog);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, legal->platform.gl_tex);
        glUniform1i(lp.tex_loc, 0);
        glBindBuffer(GL_ARRAY_BUFFER, lp.vbo);
        glEnableVertexAttribArray(lp.pos_loc);
        glVertexAttribPointer(lp.pos_loc, 2, GL_FLOAT, GL_FALSE, 16, (void *)0);
        glEnableVertexAttribArray(lp.uv_loc);
        glVertexAttribPointer(lp.uv_loc, 2, GL_FLOAT, GL_FALSE, 16, (void *)8);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(lp.pos_loc);
        glDisableVertexAttribArray(lp.uv_loc);
        glUseProgram(0);
    };

    hostFboBound = ensureHostFbo();
    if (hostFboBound)
        drawLegalFallback();

    NuThreadCriticalSectionBegin(mgr->loading_critical_section);
    nudisplaylistrenderscene_s *rs = mgr->safe_render_scenes[render_scene_id];
    if (rs) {
        for (i32 i = 0; i < rs->nsort_pris; ++i) {
            nusortpri_s *sp = rs->sort_pris[i];
            NuDisplayListCaptureSortPriority(sp);
            NuDisplayListDrawItems(sp->items);
        }
        NuDisplayListDrawItems(&rs->render_2d_first);
        mgr->safe_render_scenes[render_scene_id] = nullptr;
    }
    NuThreadCriticalSectionEnd(mgr->loading_critical_section);

    if (hostFboBound) {
        glBindFramebuffer(GL_FRAMEBUFFER, hostPrevFbo);
    }
}
