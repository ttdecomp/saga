#pragma once

// ---------------------------------------------------------------------------
// HOST-ONLY interim present path (not part of the original binary).
//
// WHY THIS EXISTS: the engine's real frame consumer is the GLES2 display-list
// executor inside the render thread (NuRenderThread / NuRndr* submit path).
// That code has not been decompiled yet, so nothing drains the committed
// scene ring produced by NuRndrEndScene(). To make the "window" test able to
// show the legal texture while that work is pending, this module snapshots
// the 2D immediate-mode streams (NuPrim2DBegin/AddXYZ/End) plus the clear
// colour, and draws them from the window context during SwapBuffers().
//
// Everything here is guarded by HOST_BUILD and is deleted once the real
// display-list consumer exists.
// ---------------------------------------------------------------------------

#ifdef HOST_BUILD

#include "decomp.h"
#include "nu2api/nu3d/numtl.h"

namespace host_present {

    void PrimStreamBegin(u32 prim_type, NUMTL *mtl);
    void PrimStreamEnd();
    // Snapshot the streams accumulated since the last commit together with the
    // current clear state taken from the committed scene parameter block.
    void SceneCommitted(void *scene_parameters);

    // Draw the latest committed snapshot with the currently bound GL context
    // (called by NuRenderDevice::SwapBuffers before the buffer swap).
    void Present();

} // namespace host_present

#endif
