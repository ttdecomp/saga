#!/usr/bin/env python3
"""Serve the generated static site and Bazel WASM output locally."""

from __future__ import annotations

import argparse
import os
import shutil
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlsplit

try:
    from scripts.plot_binary_match_map import generate_site
except ModuleNotFoundError:
    from plot_binary_match_map import generate_site


OBB_NAME = "main.1060.com.wb.lego.tcs.obb"


def default_wasm_directory() -> Path:
    runfiles_output = Path("src")
    if (runfiles_output / "saga.html").is_file():
        return runfiles_output
    workspace = Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", Path.cwd()))
    return workspace / "bazel-bin/src"


def default_site_directory(wasm_directory: Path) -> Path:
    workspace = Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", Path.cwd()))
    pages = workspace / "doc/pages"
    report = workspace / "matching.json"
    if report.is_file():
        generate_site(report, pages / "index.html", 256 * 512, 512)
    if (pages / "index.html").is_file():
        return pages
    return wasm_directory


def default_obb_path() -> Path | None:
    candidates = []
    workspace = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
    if workspace:
        candidates.append(Path(workspace) / "res" / OBB_NAME)
    candidates.append(Path.cwd() / "res" / OBB_NAME)

    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    return None


def argument_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    invocation_dir = Path(os.environ.get("BUILD_WORKING_DIRECTORY", Path.cwd()))
    return invocation_dir / path


class WasmRequestHandler(SimpleHTTPRequestHandler):
    extensions_map = {
        **SimpleHTTPRequestHandler.extensions_map,
        ".js": "text/javascript; charset=utf-8",
        ".wasm": "application/wasm",
    }

    def __init__(
        self,
        *args,
        obb_path: Path | None = None,
        wasm_directory: Path | None = None,
        **kwargs,
    ) -> None:
        self.obb_path = obb_path
        self.wasm_directory = wasm_directory
        super().__init__(*args, **kwargs)

    def end_headers(self) -> None:
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def do_GET(self) -> None:
        path = urlsplit(self.path).path
        if path == f"/{OBB_NAME}":
            self._serve_local_obb(head_only=False)
            return
        if path in {"/saga.js", "/saga.wasm"} and self._serve_wasm_asset(
            path[1:], head_only=False
        ):
            return
        if path == "/":
            self.path = (
                "/index.html"
                if (Path(self.directory) / "index.html").is_file()
                else "/saga.html"
            )
        super().do_GET()

    def do_HEAD(self) -> None:
        path = urlsplit(self.path).path
        if path == f"/{OBB_NAME}":
            self._serve_local_obb(head_only=True)
            return
        if path in {"/saga.js", "/saga.wasm"} and self._serve_wasm_asset(
            path[1:], head_only=True
        ):
            return
        if path == "/":
            self.path = (
                "/index.html"
                if (Path(self.directory) / "index.html").is_file()
                else "/saga.html"
            )
        super().do_HEAD()

    def _serve_wasm_asset(self, name: str, *, head_only: bool) -> bool:
        if self.wasm_directory is None:
            return False
        asset = self.wasm_directory / name
        if not asset.is_file():
            return False
        size = asset.stat().st_size
        self.send_response(200)
        self.send_header("Content-Type", self.guess_type(str(asset)))
        self.send_header("Content-Length", str(size))
        self.end_headers()
        if not head_only:
            with asset.open("rb") as source:
                shutil.copyfileobj(source, self.wfile)
        return True

    def _serve_local_obb(self, *, head_only: bool) -> None:
        if self.obb_path is None:
            self.send_error(404, f"{OBB_NAME} was not found under res/")
            return

        size = self.obb_path.stat().st_size
        self.send_response(200)
        self.send_header("Content-Type", "application/octet-stream")
        self.send_header("Content-Length", str(size))
        self.send_header("Content-Disposition", f'inline; filename="{OBB_NAME}"')
        self.end_headers()
        if not head_only:
            try:
                with self.obb_path.open("rb") as source:
                    shutil.copyfileobj(source, self.wfile)
            except (BrokenPipeError, ConnectionResetError):
                # Reloading the page legitimately abandons a large in-flight OBB.
                pass

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--directory", type=Path, help="override the WASM output directory")
    parser.add_argument("--obb", type=Path, help=f"override the path to {OBB_NAME}")
    args = parser.parse_args()

    wasm_directory = default_wasm_directory().resolve()
    directory = (
        argument_path(args.directory).resolve()
        if args.directory is not None
        else default_site_directory(wasm_directory).resolve()
    )
    if not directory.is_dir():
        parser.error(f"build output directory does not exist: {directory}")

    obb_path = argument_path(args.obb).resolve() if args.obb is not None else default_obb_path()
    if args.obb is not None and not obb_path.is_file():
        parser.error(f"OBB does not exist: {obb_path}")

    handler = partial(
        WasmRequestHandler,
        directory=str(directory),
        obb_path=obb_path,
        wasm_directory=wasm_directory,
    )
    server = ThreadingHTTPServer((args.host, args.port), handler)
    print(f"Now listening at http://{args.host}:{args.port}/")
    print(f"Serving site from {directory}")
    if obb_path is not None:
        print(f"Serving local OBB from {obb_path}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
