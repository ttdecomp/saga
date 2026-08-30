# Host testing

The host executable is a diagnostic aid for the decompilation. A successful
host run does not justify behavior that is absent from the original binary;
game-side fixes must still come from the original code, data, and ABI.

## Non-interactive visual captures

Use the window test in hidden, muted mode when testing alongside other desktop
applications:

```sh
./build-host/saga window --offscreen --mute --capture
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

The standard unattended menu check is therefore:

```sh
timeout 38s ./build-host/saga window --offscreen --mute --script-input --capture
```

Host input injection and rendering overrides belong under `src/host-tests/`.
Original game functions should remain host-independent and may be replaced by
weak host definitions where a platform override is required.
