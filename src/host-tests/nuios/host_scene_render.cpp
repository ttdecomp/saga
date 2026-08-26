#include "host-tests/nuios/host_scene_render.h"

#ifdef HOST_BUILD

#include <stdio.h>

#include <string.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>

#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nutex.h"

namespace host_present {

    static const int MAX_STREAMS = 16;

    struct PendingStream {
        u32 prim_type;
        NUMTL *mtl;
        u32 start; // byte offset into the stream buffer at Begin
    };

    struct CommittedQuad {
        GLuint tex; // GL texture name (0 = untextured)
        // HOST-ONLY: kept so Present() can lazily create the GL texture on the
        // thread that owns the context. On Android NuTexCreatePS() uploads from
        // the loading thread, which has its own share-group context there; the
        // host has none, so the upload is deferred to first use.
        NUNATIVETEX *native;
        f32 verts[12 * 6]; // up to two expanded pairs * {x,y,z,c,u,v}
        int vert_count;    // vertices actually captured for this quad
    };

    struct FrameSnapshot {
        bool valid;
        bool clear;
        f32 clear_r, clear_g, clear_b;
        CommittedQuad quads[MAX_STREAMS];
        int quad_count;
    };

    // Worker -> present handoff. Single-slot latest-frame snapshot guarded by a
    // mutex: the present side only needs the most recent committed frame.
    static pthread_mutex_t snapshot_mutex = PTHREAD_MUTEX_INITIALIZER;
    static FrameSnapshot snapshot = {};
    static FrameSnapshot building = {};

    static PendingStream pending[MAX_STREAMS];
    static int pending_count = 0;

    static GLuint program = 0;
    static GLint pos_loc = -1;
    static GLint uv_loc = -1;
    static GLint tex_loc = -1;

    static const char *VS_SRC =
        // The 2D immediate stream stores positions already transformed to NDC by
        // NuPrim2DAddXYZ (bias/scale of the active coordinate system), so pass
        // them straight through.
        "attribute vec3 a_pos;\n"
        "attribute vec2 a_uv;\n"
        "varying vec2 v_uv;\n"
        "void main() {\n"
        "  v_uv = a_uv;\n"
        "  gl_Position = vec4(a_pos, 1.0);\n"
        "}\n";

    static const char *FS_SRC = "precision mediump float;\n"
                                "uniform sampler2D u_tex;\n"
                                "varying vec2 v_uv;\n"
                                "void main() {\n"
                                // HOST-ONLY note: the ETC1-decoded legal texture carries a usable RGB but
                                // its alpha is meaningless for this screen; draw opaque.
                                "  vec3 c = texture2D(u_tex, v_uv).rgb;\n"
                                "  gl_FragColor = vec4(c, 1.0);\n"
                                "}\n";

    static void EnsureProgram() {
        if (program != 0) {
            return;
        }
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &VS_SRC, NULL);
        glCompileShader(vs);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &FS_SRC, NULL);
        glCompileShader(fs);
        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        pos_loc = glGetAttribLocation(program, "a_pos");
        uv_loc = glGetAttribLocation(program, "a_uv");
        tex_loc = glGetUniformLocation(program, "u_tex");
    }

    void PrimStreamBegin(u32 prim_type, NUMTL *mtl) {
        if (pending_count >= MAX_STREAMS) {
            return;
        }
        PendingStream *p = &pending[pending_count++];
        p->prim_type = prim_type;
        p->mtl = mtl;
        p->start = (u32)(usize)(*g_NuPrim_StreamBufferPtr)->addr; // first vertex (after header)
    }

    void PrimStreamEnd() {
        if (pending_count == 0 || g_NuPrim_StreamBufferPtr == NULL) {
            return;
        }
        PendingStream *p = &pending[pending_count - 1];

        // The stream cursor advanced past every vertex; copy them out.
        u32 end = (u32)(usize)(*g_NuPrim_StreamBufferPtr)->addr;
        u32 bytes = end - p->start;
        if (bytes > sizeof(building.quads[0].verts)) {
            bytes = sizeof(building.quads[0].verts);
        }

        if (building.quad_count < MAX_STREAMS) {
            CommittedQuad *q = &building.quads[building.quad_count++];
            memset(q, 0, sizeof(*q));
            memcpy(q->verts, (void *)(usize)p->start, bytes);
            q->vert_count = (int)(bytes / 24);

            q->tex = 0;
            q->native = NULL;
            if (p->mtl != NULL && p->mtl->tex_id > 0) {
                NUNATIVETEX *native = NuTexGetNative(p->mtl->tex_id);
                q->native = native;
                if (native != NULL) {
                    // NUNATIVETEXPS holds the GL texture name; the worker and
                    // window contexts share an EGL share group, so the name is
                    // valid from the present side.
                    q->tex = native->platform.gl_tex;
                }
            }

            // NOTE: no host-side expansion here. The game's NuPrim2DAddXYZ already
            // expands each vertex pair into six stream vertices (0x29d235..), so
            // the copied bytes are final triangle-list vertices.
        }

        pending_count--;
    }

    void SceneCommitted(void *scene_parameters) {
        u8 *scn = (u8 *)scene_parameters;

        pthread_mutex_lock(&snapshot_mutex);
        building.valid = building.quad_count > 0;
        building.clear = false;
        u32 flags = *(u32 *)(scn + 0x8);
        if (flags & 0xb00) {
            u32 colour = *(u32 *)(scn + 0xc);
            building.clear = true;
            building.clear_b = (f32)((colour >> 0) & 0xff) / 255.0f;
            building.clear_g = (f32)((colour >> 8) & 0xff) / 255.0f;
            building.clear_r = (f32)((colour >> 16) & 0xff) / 255.0f;
        }
        snapshot = building;
        building.quad_count = 0;
        pthread_mutex_unlock(&snapshot_mutex);
    }

    // HOST-ONLY helper: the original uploads textures from whatever thread runs
    // NuTexCreatePS (the Android loader thread owns a GL context there). The host
    // loader thread has no context, so defer to here, where the window context is
    // current.
    static GLuint EnsureGLTex(CommittedQuad *q) {
        if (q->tex != 0) {
            return q->tex;
        }
        if (q->native != NULL && q->native->platform.gl_tex == 0 && q->native->image_data != NULL &&
            q->native->size != 0) {
            NuTexCreatePS(q->native, false);
        }
        q->tex = q->native ? q->native->platform.gl_tex : 0;
        return q->tex;
    }

    void Present() {
        pthread_mutex_lock(&snapshot_mutex);
        FrameSnapshot snap = snapshot;
        pthread_mutex_unlock(&snapshot_mutex);

        if (!snap.valid) {
            return;
        }

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        if (snap.clear) {
            glClearColor(snap.clear_r, snap.clear_g, snap.clear_b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        if (snap.quad_count == 0) {
            return;
        }

        // HOST-ONLY: this context is shared with the engine's render path, whose
        // GL state at this point is whatever the display list last left behind.
        // Reset the pieces that would suppress or distort our quad draw.
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_SCISSOR_TEST);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

        EnsureProgram();
        if (program == 0) {
            return;
        }

        glUseProgram(program);
        glUniform1i(tex_loc, 0);
        glActiveTexture(GL_TEXTURE0);
        glEnableVertexAttribArray((GLuint)pos_loc);
        glEnableVertexAttribArray((GLuint)uv_loc);

        for (int i = 0; i < snap.quad_count; i++) {
            CommittedQuad *q = &snap.quads[i];
            GLuint tex = EnsureGLTex(q);
            if (tex == 0) {
                continue;
            }
            q->tex = tex;
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            int count = q->vert_count > 0 ? q->vert_count : 6;
            GLuint vbo = 0;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, count * 24, q->verts, GL_STREAM_DRAW);

            glVertexAttribPointer((GLuint)pos_loc, 3, GL_FLOAT, GL_FALSE, 24, (void *)0);
            glVertexAttribPointer((GLuint)uv_loc, 2, GL_FLOAT, GL_FALSE, 24, (void *)16);
            glDrawArrays(GL_TRIANGLES, 0, count);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &vbo);
        }

        glDisableVertexAttribArray((GLuint)pos_loc);
        glDisableVertexAttribArray((GLuint)uv_loc);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        (void)viewport;
    }

} // namespace host_present

#endif // HOST_BUILD
