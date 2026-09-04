#pragma once

#include "nu2api/nucore/common.h"

struct ANativeWindow;
union SDL_Event;
struct SDL_Window;

// Common host entry points have weak fallbacks in runtime.cpp. A selected
// platform directory may replace any of them with a strong definition.
const char *HostPlatformVideoDriver();
bool HostPlatformAudioOutputEnabled();
ANativeWindow *HostPlatformNativeWindow(SDL_Window *window);
void HostPlatformPrepareArguments(i32 *argc, char ***argv);
void HostPlatformHandleInputEvent(const SDL_Event &event, i32 width, i32 height);
u32 HostPlatformKeyboardButtons(const bool *keyboard);
