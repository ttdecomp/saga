#!/usr/bin/env python3
"""check_symbols.py -- diff defined text symbols of the build vs the original
binary, so we can see exactly which functions are still missing.

Extracts defined text symbols (global `T` and local/static `t`, plus weak `W`
when requested) from both binaries with `nm`, computes `original - built`,
then subtracts an ignore list (symbols we don't need to replicate:
compiler-runtime, platform glue, or ABI-special thunks/templates we
intentionally skip). Prints a summary and the missing list.

Exit codes:
    0  all original symbols provided, and extras within baseline
    1  at least one original symbol is still missing
    2  no symbols missing, but the build implements more extra symbols than
       the baseline (scripts/symbols_extra_baseline.txt by default)

The extra-symbol baseline is enforced so the build cannot keep gaining
unreplicated symbols; new ones must be documented in the ignore list or the
baseline bumped. Disable with --no-baseline, or override with --baseline-extra.

Usage:
    python3 scripts/check_symbols.py [BUILD_BIN] [ORIG_BIN] [--ignore FILE]
Defaults:
    BUILD_BIN = build/saga
    ORIG_BIN  = res/libTTapp.so
    --ignore  = scripts/symbols_ignore.txt   (one mangled symbol per line;
               blank lines and '#' comments ignored)
"""

import argparse
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
NM = os.path.join(
    ROOT,
    "ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/"
    "i686-linux-android-nm",
)


def defined_text_symbols(binary, include_weak=False, include_local=False):
    """Return the set of defined text symbols in a binary.

    Matches nm type T (global). Weak symbols (type W) are matched when
    include_weak=True; they are real definitions present in the binary, emitted
    weakly because they are inline or implicit special members. Local/file-static
    symbols (type t) are matched when include_local=True.
    """
    proc = subprocess.run(
        [NM, "--defined-only", binary], capture_output=True, text=True
    )
    if proc.returncode != 0:
        sys.stderr.write(f"nm failed on {binary}:\n{proc.stderr}\n")
        sys.exit(2)
    out = set()
    for line in proc.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3:
            t = parts[1]
            if t == "T" or (include_weak and t == "W") or (include_local and t == "t"):
                out.add(parts[2])
    return out


def is_compiler_generated(name):
    """Compiler-generated symbols that are never hand-declared in source:

    * specialized clones (.isra / .constprop / .part / .cold) produced
      automatically from static/inline functions, and
    * static-initialization trampolines (_GLOBAL__sub_I_ / _GLOBAL__sub_D_)
      emitted for global object constructors/destructors.

    These can only be reproduced by writing the code that triggers them (inline
    clones, global objects), never by declaring a function of that name, so they
    are excluded from the must-provide set and reported separately."""
    return (
        ".isra." in name
        or ".constprop." in name
        or ".part." in name
        or ".cold." in name
        or name.startswith("_GLOBAL__sub_I_")
        or name.startswith("_GLOBAL__sub_D_")
    )


def is_thunk(name):
    """Itanium ABI vtable adjustment thunks (_ZThnNN_...) are emitted
    automatically by the compiler; we don't need to replicate them."""
    return name.startswith("_ZThn")


def is_local_label(name):
    """Assembler-internal local labels (.L#####) are branch/constant labels in
    the symbol table, not functions; they are compiler-generated and cannot be
    declared in source."""
    return name.startswith(".L") and name[2:].isdigit()


def load_ignore(path):
    out = set()
    if os.path.exists(path):
        for raw in open(path):
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            out.add(line)
    # Also fold in the gonk.toml ignore list (mangled symbols we don't need
    # to replicate: console/platform glue, compiler runtime, etc.).
    gonk = os.path.join(ROOT, "gonk.toml")
    if os.path.exists(gonk):
        for raw in open(gonk):
            m = re.search(r'"([^"]+)"', raw)
            if m:
                out.add(m.group(1))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("build", nargs="?", default=os.path.join(ROOT, "build/saga"))
    ap.add_argument("orig", nargs="?", default=os.path.join(ROOT, "res/libTTapp.so"))
    ap.add_argument(
        "--ignore", default=os.path.join(ROOT, "scripts/symbols_ignore.txt")
    )
    ap.add_argument(
        "--list", action="store_true", help="print the full missing-symbol list"
    )
    ap.add_argument(
        "--baseline-extra",
        type=int,
        default=None,
        help="fail (exit 2) if the number of extra symbols exceeds this count",
    )
    ap.add_argument(
        "--no-baseline",
        action="store_true",
        help="do not enforce the extra-symbol baseline",
    )
    args = ap.parse_args()

    build = defined_text_symbols(args.build, include_weak=True, include_local=True)
    orig_strong = defined_text_symbols(args.orig, include_weak=False)
    orig_local = defined_text_symbols(args.orig, include_weak=False, include_local=True) - orig_strong
    orig_any = orig_strong | orig_local | defined_text_symbols(args.orig, include_weak=True)
    build_strong = defined_text_symbols(args.build, include_weak=False)
    ignore = load_ignore(args.ignore)

    missing = sorted(
        s
        for s in ((orig_strong | orig_local) - build - ignore)
        if not is_thunk(s) and not is_local_label(s) and not is_compiler_generated(s)
    )
    compiler_gen = sorted(
        s
        for s in ((orig_strong | orig_local) - build - ignore)
        if not is_thunk(s) and not is_local_label(s) and is_compiler_generated(s)
    )
    missing_global = [s for s in missing if s in orig_strong]
    missing_local = [s for s in missing if s in orig_local]
    extras = sorted(build_strong - orig_any)
    provided = len(orig_strong) + len(orig_local) - len(missing)

    print(f"original text symbols : {len(orig_strong) + len(orig_local)} "
          f"({len(orig_strong)} global, {len(orig_local)} local)")
    print(f"provided by build     : {len(build)}")
    print(f"matched / in build    : {provided}")
    print(f"missing (not ignored) : {len(missing)} "
          f"({len(missing_global)} global, {len(missing_local)} local)")
    print(f"compiler-generated    : {len(compiler_gen)} (clones / static-init; auto)")
    print(f"extra (in build only) : {len(extras)}")
    if args.list:
        if missing_global:
            print("\nmissing global symbols:")
            for s in missing_global:
                print(f"  {s}")
        if missing_local:
            print("\nmissing local symbols:")
            for s in missing_local:
                print(f"  {s}")
        if compiler_gen:
            print("\ncompiler-generated (not hand-declared):")
            for s in compiler_gen:
                print(f"  {s}")
        if extras:
            print("\nextra symbols (in build, not in original):")
            for s in extras:
                print(f"  {s}")

    code = 1 if missing else 0

    if args.no_baseline:
        args.baseline_extra = None
    elif args.baseline_extra is None:
        baseline_file = os.path.join(ROOT, "scripts/symbols_extra_baseline.txt")
        if os.path.exists(baseline_file):
            for raw in open(baseline_file):
                line = raw.strip()
                if line and not line.startswith("#"):
                    args.baseline_extra = int(line.split()[0])
                    break

    if args.baseline_extra is not None and len(extras) > args.baseline_extra:
        sys.stderr.write(
            "\nERROR: extra symbols (%d) exceed baseline (%d).\n"
            "New extra symbols must be documented; add them to the ignore list\n"
            "(scripts/symbols_ignore.txt / gonk.toml) or bump the baseline\n"
            "(scripts/symbols_extra_baseline.txt) before they can be merged.\n"
            % (len(extras), args.baseline_extra)
        )
        if code == 0:
            code = 2
    return code


if __name__ == "__main__":
    sys.exit(main())
