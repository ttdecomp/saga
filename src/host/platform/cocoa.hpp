#pragma once

struct SDL_Window;

// Return the Cocoa window's Core Animation layer for ANGLE's EGL native
// window. The Objective-C++ implementation keeps Cocoa types out of C++
// translation units.
void *HostCocoaGetEGLNativeWindow(SDL_Window *window);
