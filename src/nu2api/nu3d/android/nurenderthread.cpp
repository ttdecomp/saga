#include "nu2api/nu3d/android/nurenderthread.h"

#include <GLES2/gl2.h>
#include <pthread.h>

#include "decomp.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nuthread.h"
#include "globals.h"
#include "nu2api/nu3d/nutex.h"
#include <unistd.h>
static volatile i32 renderThreadCS;
static i32 renderThreadIsLocked;
pthread_t g_renderThread;

void NuRenderThreadLock(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 134);
    NuThreadCriticalSectionBegin(renderThreadCS);
    renderThreadIsLocked++;
}

void NuRenderThreadUnlock(void) {
    renderThreadIsLocked--;
    NuThreadCriticalSectionEnd(renderThreadCS);
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 153);
}

i32 NuRenderThreadIsLocked(void) {
    return renderThreadIsLocked;
}

void NuRenderThreadCreate(void) {
    LOG_INFO("FRAME RENDER Created");
    NuIOS_InitRenderThread();
    pthread_create(&g_renderThread, NULL, renderThread_main, NULL);
    renderThreadCS = NuThreadCreateCriticalSection();
}

void *renderThread_main(void *arg) {
   
   
    NuRenderSetThisTreadAsRender();
    NuIOSInitOpenGLES();
    LOG_INFO("render thread started");
    usleep(160000);

    for (;;) {
        //NuIOS_WaitUntilAllowedToRender();
        usleep(1600);
        NuIOS_SetRenderIncomplete();
        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 259);
        NuRenderThreadLock();
        glFrontFace(GL_CW);
        renderThread_processRenderScenes();
        NuRenderThreadUnlock();
        NuRenderDeviceSwapBuffers();
        NuIOS_SetRenderComplete();
        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nurenderthread.cpp", 269);
    }
}

i32 renderThread_processRenderScenes(void) {


}


/*
i32 renderThread_processRenderScenes(void) {
    static bool initialized = false;
    static GLuint program = 0;
    static GLuint vbo = 0;
    static GLuint textureID = 0;

    if (!initialized) {
        LOG_INFO("Initializing Render Scene...");

        // 1. SHADERS
        const char* vShaderStr =
            "attribute vec4 vPosition;\n"
            "attribute vec2 vTexCoord;\n"
            "varying vec2 fTexCoord;\n"
            "void main() {\n"
            "  gl_Position = vPosition;\n"
            "  fTexCoord = vTexCoord;\n"
            "}\n";

        const char* fShaderStr =
            "precision mediump float;\n"
            "varying vec2 fTexCoord;\n"
            "uniform sampler2D uTexture;\n"
            "void main() {\n"
            "  gl_FragColor = texture2D(uTexture, fTexCoord);\n" 
            "}\n";

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderStr, NULL);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderStr, NULL);
        glCompileShader(fragmentShader);

        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        // 2. FULL SCREEN VERTEX DATA (Interleaved: X, Y, Z,   U, V)
        // Changed to -1.0 and 1.0 so it fills the whole screen
        GLfloat vertices[] = {
            // Positions          // UV Coords
            -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,  // Top Left
            -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,  // Bottom Left
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f,  // Top Right
             1.0f, -1.0f, 0.0f,   1.0f, 1.0f   // Bottom Right
        };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 3. LOAD THE ACTUAL GAME TEXTURE
        VARIPTR legalTex;
        // Ensure superbuffer_end is correctly initialized before this point!
        legalTex.addr = superbuffer_end.addr + -0x400000; 
        
        textureID = NuTexRead("stuff\\legal\\LEGAL_ENGLISH", &legalTex, &superbuffer_end);
        LOG_WARN("NuTexRead returned textureID: %d", textureID);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LOG_WARN("OPENGL ERROR DURING LOAD: 0x%x", err);
        }
        
        // DO NOT overwrite textureID here!
        initialized = true;
    }

    // --- RENDER LOOP ---

    // glViewport(0, 0, g_renderDevice->width, g_renderDevice->height); // Uncomment if safe

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    // SAFETY: Ensure the engine didn't leave culling or weird blend states on
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Bind our geometry buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLsizei stride = 5 * sizeof(GLfloat);

    // Setup Position Attribute
    GLint posAttrib = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // Setup UV Attribute
    GLint texAttrib = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

    // Bind the texture to Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Tell the shader's sampler to read from Texture Unit 0
    GLint texUniform = glGetUniformLocation(program, "uTexture");
    glUniform1i(texUniform, 0);

    // Draw the textured square!
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Clean up
    glDisableVertexAttribArray(posAttrib);
    glDisableVertexAttribArray(texAttrib);
    glBindTexture(GL_TEXTURE_2D, 0);

    return 1;
}
*/