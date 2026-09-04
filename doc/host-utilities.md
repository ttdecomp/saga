# Host utilities

The host executable is a diagnostic aid for the decompilation. A successful
host run does not justify behavior that is absent from the original binary;
game-side fixes must still come from the original code, data, and ABI.

## Non-interactive visual captures

Use the window utility in hidden, muted mode when running alongside other desktop
applications:

```sh
bazel run --config=native //src:run_native -- window --offscreen --mute --capture
```

- `--offscreen` creates a hidden, non-focusable SDL window and ignores desktop
  input events. Rendering and framebuffer readback remain active.
- `--mute` selects SDL's dummy audio driver before audio initialization, so no
  sound reaches the real device.
- `--capture` writes changed frames to `.work/capture/`. While an image is
  changing, captures are rate-limited to roughly 500 ms; stable transitions
  are retained when their framebuffer hash changes.
- `--script-input` adds the host-only deterministic touch sequence used to
  reach the load/save flow. It may be combined with all three flags above.

On Linux (or an MSYS2 environment that provides GNU `timeout`), the standard
unattended menu check is therefore:

```sh
timeout 38s bazel run --config=native //src:run_native -- window --offscreen --mute --script-input --capture
```

On Windows the executable is `bazel-bin/src/saga_native.exe`. macOS users can
install GNU coreutils and invoke the equivalent `gtimeout`; native macOS is not
currently part of the supported CI matrix.

The command implementations belong under `src/host/harness/`. Actual platform
adapters belong under `src/host/platform/` and are limited to imported APIs,
filesystem/environment access, or a build-selected platform interface. Host
code must not replace portable game or engine functions.
