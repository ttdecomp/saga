#pragma once

// Permanent arena lifecycle.
//
// The permanent buffer grows monotonically from `permbuffer_base` toward
// `superbuffer_end` for the life of the process.  These three symbols are
// the original public surface:
//
//   StartPerm() — rewind the bump pointer to the arena base.
//   LoadPerm()  — populate the arena (fonts, strings, audio, level/char
//                 tables, fades, gizmos, …) and drive the loading-screen
//                 loop when background loading is enabled.
//   EndPerm()   — original no-op tail, kept for link compatibility.

void StartPerm(void);
void LoadPerm(void);
void EndPerm(void);
