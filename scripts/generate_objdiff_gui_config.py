#!/usr/bin/env python3
"""Generate the repository-local project file used only by the objdiff GUI.

The original is available only as one linked shared library. The GUI project
therefore pairs that library with every source/object unit found in Bazel's
compile actions. ``--build-base`` is an internal adapter for objdiff's build
hook: objdiff appends the selected file path, while Bazel expects a target
label instead.
"""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys

from scripts.lib.bazel_actions import bazel_units

BAZEL_TARGET = "//src:saga_target"
BASE_PATH = "bazel-bin/src/libTTapp.so"
ORIGINAL_PATH = "res/libTTapp.so"


def workspace_root() -> Path:
    """Return the source workspace both under ``bazel run`` and direct Python."""
    bazel_workspace = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
    if bazel_workspace:
        return Path(bazel_workspace).resolve()
    return Path(__file__).resolve().parents[1]


def build_base(root: Path, bazel: str) -> int:
    command = [bazel, "build", "--config=target", BAZEL_TARGET]
    print("+", shlex.join(command), flush=True)
    return subprocess.run(command, cwd=root, check=False).returncode


def project_config(bazel: str, units: list[dict]) -> dict:
    return {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": bazel,
        "custom_args": [
            "run",
            "//scripts:generate_objdiff_gui_config",
            "--",
            "--build-base",
            "--bazel",
            bazel,
        ],
        "build_target": False,
        "build_base": True,
        "watch_patterns": [
            "src/**/*",
            "bazel/**/*",
            "third_party/**/*",
            ".bazelrc",
            ".bazelversion",
            "MODULE.bazel",
            "MODULE.bazel.lock",
        ],
        "units": [
            {
                "name": unit["name"],
                "target_path": ORIGINAL_PATH,
                "base_path": unit["object"],
                "metadata": {"source_path": unit["source"]},
            }
            for unit in units
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--bazel", default="bazel", help="Bazel or Bazelisk executable"
    )
    parser.add_argument("--output", default="objdiff.json")
    parser.add_argument("--build-base", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("objdiff_path", nargs="?", help=argparse.SUPPRESS)
    args = parser.parse_args()

    root = workspace_root()
    if args.build_base:
        return build_base(root, args.bazel)
    if args.objdiff_path is not None:
        parser.error("unexpected positional argument")

    original = root / ORIGINAL_PATH
    if not original.is_file():
        print(f"error: missing original binary {original}", file=sys.stderr)
        return 1
    if build_base(root, args.bazel):
        return 1

    base = root / BASE_PATH
    if not base.is_file():
        print(f"error: Bazel did not produce {base}", file=sys.stderr)
        return 1

    try:
        units = bazel_units(root, args.bazel, BAZEL_TARGET)
    except RuntimeError as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    output = (root / args.output).resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(
        json.dumps(project_config(args.bazel, units), indent=2) + "\n",
        encoding="utf-8",
    )
    try:
        output_display = output.relative_to(root)
    except ValueError:
        output_display = output
    print(f"Wrote {len(units)} Bazel units to {output_display} for the objdiff GUI")
    return 0


if __name__ == "__main__":
    sys.exit(main())
