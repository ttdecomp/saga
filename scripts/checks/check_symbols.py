#!/usr/bin/env python3
"""check_symbols.py -- diff defined text symbols of the build vs the original
binary, so we can see exactly which functions are still missing.

Extracts defined text symbols (global `T` and local/static `t`, plus weak `W`
when requested) from both binaries with `nm`, computes `original - built`,
then subtracts an ignore list (symbols we don't need to replicate:
compiler-runtime, platform glue, or ABI-special thunks/templates we
intentionally skip). Prints a summary and the missing list.

Exit codes:
    0  all original symbols provided, and extras exactly match the baseline
    1  at least one original symbol is still missing
    2  no symbols missing, but the extra-symbol set differs from the baseline

The extra-symbol baseline is enforced so the build cannot keep gaining
unreplicated symbols. It contains one exact symbol per line, so newly added and
newly removed extras both require a deliberate baseline update. Disable with
--no-baseline, or override the file with --extra-baseline.

Usage:
    bazel run //scripts/checks:check_symbols -- [BUILD_BIN] [ORIG_BIN]
Defaults:
    BUILD_BIN = target-config Bazel //src:libTTapp.so output
    ORIG_BIN  = res/libTTapp.so
    --ignore  = scripts/checks/symbols_ignore.txt (one mangled symbol per line;
               blank lines and '#' comments ignored)
"""

import argparse
import json
import os
import subprocess
import sys

from scripts.lib.ndk_tools import find_ndk_tool

ROOT = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
if ROOT is None:
    ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
NM = find_ndk_tool("nm")


def default_build_path():
    """Resolve the target-config library without trusting the bazel-bin link."""
    result = subprocess.run(
        [
            "bazel",
            "cquery",
            "--config=target",
            "//src:libTTapp.so",
            "--output=files",
            "--noshow_progress",
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    outputs = [
        os.path.join(ROOT, line) for line in result.stdout.splitlines() if line.strip()
    ]
    files = [output for output in outputs if os.path.isfile(output)]
    if result.returncode or len(files) != 1:
        detail = result.stderr.strip()
        suffix = f": {detail}" if detail else ""
        raise RuntimeError(
            "could not resolve the target build; run "
            "'bazel build --config=target //src:saga_target' first" + suffix
        )
    return os.path.realpath(files[0])


def defined_text_symbols(binary, include_weak=False, include_local=False):
    """Return the set of defined text symbols in a binary.

    Matches nm type T (global). Weak symbols (type W) are matched when
    include_weak=True; they are real definitions present in the binary, emitted
    weakly because they are inline or implicit special members. Local/file-static
    symbols (type t) are matched when include_local=True.
    """
    if NM is None:
        sys.stderr.write("nm was not found; install it or set the NM environment variable\n")
        sys.exit(2)
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


def load_symbol_list(path):
    out = set()
    with open(path) as source:
        for line_number, raw in enumerate(source, start=1):
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            if line in out:
                raise ValueError(f"{path}:{line_number}: duplicate symbol {line}")
            out.add(line)
    return out


def load_original_symbols(path):
    """Load the original ELF symbol surface embedded in the Pages report."""
    with open(path, encoding="utf-8") as source:
        report = json.load(source)
    surface = report.get("original_text_symbols")
    if not isinstance(surface, dict):
        raise ValueError(f"{path}: missing original_text_symbols")
    result = {}
    for binding in ("global", "local", "weak"):
        symbols = surface.get(binding)
        if not isinstance(symbols, list) or not all(
            isinstance(symbol, str) for symbol in symbols
        ):
            raise ValueError(f"{path}: invalid original_text_symbols.{binding}")
        result[binding] = set(symbols)
    return result


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "build",
        nargs="?",
        help="build binary (default: target-config //src:libTTapp.so)",
    )
    ap.add_argument("orig", nargs="?", default=os.path.join(ROOT, "res/libTTapp.so"))
    ap.add_argument(
        "--original-symbols",
        help="Pages JSON containing the original symbol surface instead of an ELF",
    )
    ap.add_argument(
        "--ignore", default=os.path.join(ROOT, "scripts/checks/symbols_ignore.txt")
    )
    ap.add_argument(
        "--list", action="store_true", help="print the full missing-symbol list"
    )
    ap.add_argument(
        "--list-ignored",
        action="store_true",
        help="also list the missing symbols that were ignored via the ignore list",
    )
    ap.add_argument(
        "--extra-baseline",
        default=os.path.join(ROOT, "scripts/checks/symbols_extra_baseline.txt"),
        help="file containing the exact allowed extra-symbol set",
    )
    ap.add_argument(
        "--no-baseline",
        action="store_true",
        help="do not enforce the extra-symbol baseline",
    )
    args = ap.parse_args()

    try:
        invocation_dir = os.environ.get("BUILD_WORKING_DIRECTORY", os.getcwd())

        def argument_path(value):
            return value if os.path.isabs(value) else os.path.join(invocation_dir, value)

        build_path = argument_path(args.build) if args.build else default_build_path()
        orig_path = argument_path(args.orig)
        original_symbols_path = (
            argument_path(args.original_symbols) if args.original_symbols else None
        )
        ignore_path = argument_path(args.ignore)
        baseline_path = argument_path(args.extra_baseline)
    except RuntimeError as error:
        sys.stderr.write(f"error: {error}\n")
        return 2

    build = defined_text_symbols(build_path, include_weak=True, include_local=True)
    build_strong = defined_text_symbols(build_path, include_weak=False)
    try:
        if original_symbols_path:
            original = load_original_symbols(original_symbols_path)
            orig_strong = original["global"]
            orig_local = original["local"] - orig_strong
            orig_any = orig_strong | orig_local | original["weak"]
        else:
            orig_strong = defined_text_symbols(orig_path, include_weak=False)
            orig_local = (
                defined_text_symbols(orig_path, include_weak=False, include_local=True)
                - orig_strong
            )
            orig_any = (
                orig_strong
                | orig_local
                | defined_text_symbols(orig_path, include_weak=True)
            )
        ignore = load_symbol_list(ignore_path)
        baseline = None if args.no_baseline else load_symbol_list(baseline_path)
    except (OSError, ValueError, json.JSONDecodeError) as error:
        sys.stderr.write(f"error: {error}\n")
        return 2

    candidate = sorted(
        s
        for s in ((orig_strong | orig_local) - build)
        if not is_thunk(s) and not is_local_label(s)
    )
    ignored = [s for s in candidate if s in ignore]
    remain = [s for s in candidate if s not in ignore]
    missing = [s for s in remain if not is_compiler_generated(s)]
    compiler_gen = [s for s in remain if is_compiler_generated(s)]
    missing_global = [s for s in missing if s in orig_strong]
    missing_local = [s for s in missing if s in orig_local]
    extras = sorted(build_strong - orig_any)
    provided = len(orig_strong) + len(orig_local) - len(missing)

    print(f"original text symbols : {len(orig_strong) + len(orig_local)} "
          f"({len(orig_strong)} global, {len(orig_local)} local)")
    print(f"provided by build     : {len(build)}")
    print(f"matched / in build    : {provided}")
    print(f"ignored               : {len(ignored)}")
    print(f"compiler-generated    : {len(compiler_gen)} (clones / static-init; auto)")
    print(f"extra (in build only) : {len(extras)}")
    print(f"total missing         : {len(missing)} "
          f"({len(missing_global)} global, {len(missing_local)} local)")
    if args.list_ignored:
        if ignored:
            print("\nignored missing symbols (from ignore list):")
            for s in ignored:
                print(f"  {s}")
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

    if baseline is not None:
        extra_set = set(extras)
        unexpected_extras = sorted(extra_set - baseline)
        stale_baseline = sorted(baseline - extra_set)
    else:
        unexpected_extras = []
        stale_baseline = []

    if unexpected_extras or stale_baseline:
        sys.stderr.write(
            "\nERROR: the build's extra-symbol set differs from "
            f"{baseline_path}.\n"
        )
        if unexpected_extras:
            sys.stderr.write("\nnew extra symbols (add deliberately to the baseline):\n")
            for symbol in unexpected_extras:
                sys.stderr.write(f"  {symbol}\n")
        if stale_baseline:
            sys.stderr.write("\nstale baseline symbols (remove from the baseline):\n")
            for symbol in stale_baseline:
                sys.stderr.write(f"  {symbol}\n")
        if code == 0:
            code = 2
    return code


if __name__ == "__main__":
    sys.exit(main())
