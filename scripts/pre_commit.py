#!/usr/bin/env python3
"""Run repository checks and refresh generated Pages artifacts before commit."""

from __future__ import annotations

import hashlib
import os
from pathlib import Path
import platform
import shlex
import subprocess
import sys

FORMAT_SUFFIXES = frozenset(
    {
        ".c",
        ".cc",
        ".cpp",
        ".cxx",
        ".h",
        ".hh",
        ".hpp",
        ".hxx",
        ".inc",
        ".inl",
        ".ipp",
        ".m",
        ".mm",
    }
)
MAX_COMMAND_LENGTH = 7000


def run(command: list[str], root: Path) -> int:
    print("+", shlex.join(command), flush=True)
    return subprocess.run(command, cwd=root, check=False).returncode


def resolve_runfile(path: Path) -> Path:
    if path.is_absolute():
        return path

    runfiles_dir = os.environ.get("RUNFILES_DIR")
    if runfiles_dir:
        runfiles_root = Path(runfiles_dir)
        for candidate in (runfiles_root / "_main" / path, runfiles_root / path):
            if candidate.exists():
                return candidate

    manifest_path = os.environ.get("RUNFILES_MANIFEST_FILE")
    if manifest_path:
        suffix = path.as_posix()
        with Path(manifest_path).open(encoding="utf-8") as manifest:
            for line in manifest:
                logical_path, separator, physical_path = line.rstrip("\n").partition(
                    " "
                )
                if separator and (
                    logical_path == suffix or logical_path.endswith("/" + suffix)
                ):
                    return Path(physical_path)

    return path


def git_paths(root: Path, arguments: list[str]) -> set[Path]:
    result = subprocess.run(
        ["git", *arguments, "-z", "--", "src"],
        cwd=root,
        check=True,
        capture_output=True,
    )
    return {
        Path(path.decode("utf-8"))
        for path in result.stdout.split(b"\0")
        if path
    }


def command_batches(files: list[Path]) -> list[list[str]]:
    batches = []
    batch = []
    length = 0
    for path in files:
        argument = path.as_posix()
        if batch and length + len(argument) + 1 > MAX_COMMAND_LENGTH:
            batches.append(batch)
            batch = []
            length = 0
        batch.append(argument)
        length += len(argument) + 1
    if batch:
        batches.append(batch)
    return batches


def format_sources(root: Path, clang_format: Path) -> int:
    files = sorted(
        path.relative_to(root)
        for path in (root / "src").rglob("*")
        if path.is_file() and path.suffix in FORMAT_SUFFIXES
    )
    before = {
        path: hashlib.sha256((root / path).read_bytes()).digest() for path in files
    }
    tracked = git_paths(root, ["ls-files"])
    had_unstaged_changes = git_paths(root, ["diff", "--name-only"])

    print(f"+ clang-format -i --style=file ({len(files)} files)", flush=True)
    for batch in command_batches(files):
        status = subprocess.run(
            [clang_format, "-i", "--style=file", *batch],
            cwd=root,
            check=False,
        ).returncode
        if status:
            return status

    changed = [
        path
        for path, digest in before.items()
        if hashlib.sha256((root / path).read_bytes()).digest() != digest
    ]
    if not changed:
        return 0

    safe_to_stage = [
        path for path in changed if path in tracked and path not in had_unstaged_changes
    ]
    needs_review = [path for path in changed if path not in safe_to_stage]
    if safe_to_stage:
        for batch in command_batches(safe_to_stage):
            status = run(["git", "add", "--", *batch], root)
            if status:
                return status
        print("Staged clang-format changes:")
        for path in safe_to_stage:
            print(f"  {path.as_posix()}")
    if needs_review:
        print(
            "clang-format changed files that already had unstaged or untracked edits; "
            "review and stage them before retrying:",
            file=sys.stderr,
        )
        for path in needs_review:
            print(f"  {path.as_posix()}", file=sys.stderr)
        return 1
    return 0


def clang_tidy_commands() -> list[list[str]]:
    system = platform.system()
    commands = []
    if system != "Darwin" or platform.machine().lower() in {"amd64", "x86_64"}:
        commands.append(
            [
                "bazel",
                "build",
                "--config=target",
                "//src:clang_tidy_target",
            ]
        )
    if system != "Darwin":
        native_command = ["bazel", "build", "--config=native"]
        if system == "Windows":
            native_command.append("--config=windows-mingw")
        native_command.extend(
            ["//src:clang_tidy_native"]
        )
        commands.append(native_command)
    commands.append(
        [
            "bazel",
            "build",
            "--config=wasm",
            "//src:clang_tidy_wasm",
        ]
    )
    return commands


def main() -> int:
    workspace = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
    if not workspace:
        print("error: pre_commit must be run with bazel run", file=sys.stderr)
        return 2
    root = Path(workspace)
    if len(sys.argv) != 2:
        print("error: Bazel did not provide clang-format", file=sys.stderr)
        return 2
    clang_format = resolve_runfile(Path(sys.argv[1]))

    status = format_sources(root, clang_format)
    if status:
        return status

    for command in (
        ["git", "--no-pager", "diff", "--cached", "--check"],
        ["bazel", "test", "//scripts/checks:checks"],
    ):
        status = run(command, root)
        if status:
            return status

    for command in clang_tidy_commands():
        status = run(command, root)
        if status:
            return status

    if not (root / "res/libTTapp.so").is_file():
        print("gh pages generation skipped: res/libTTapp.so is not present")
        return 0

    for command in (
        ["bazel", "build", "--config=target", "//src:saga_target"],
        ["bazel", "run", "//scripts/checks:check_symbols"],
        ["bazel", "run", "//scripts:generate_bazel_objdiff_report"],
    ):
        status = run(command, root)
        if status:
            return status

    generated = ["matching.json", "README.md"]
    status = run(["git", "add", "--", *generated], root)
    if status:
        return status
    print("Staged generated files:")
    for relative_path in generated:
        print(f"  {relative_path}")
    return run(["git", "--no-pager", "diff", "--cached", "--check"], root)


if __name__ == "__main__":
    sys.exit(main())
