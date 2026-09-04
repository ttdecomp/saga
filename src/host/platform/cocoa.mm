#include "host/platform/cocoa.hpp"

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include <SDL3/SDL.h>

void *HostCocoaGetEGLNativeWindow(SDL_Window *window) {
    if (window == nullptr) {
        return nullptr;
    }

    const SDL_PropertiesID properties = SDL_GetWindowProperties(window);
    void *window_pointer = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    if (window_pointer == nullptr) {
        return nullptr;
    }

    NSWindow *cocoa_window = (__bridge NSWindow *)window_pointer;
    NSView *content_view = cocoa_window.contentView;
    if (content_view == nil) {
        return nullptr;
    }

    content_view.wantsLayer = YES;
    CALayer *layer = content_view.layer;
    return layer != nil ? (__bridge void *)layer : nullptr;
}
#endif
