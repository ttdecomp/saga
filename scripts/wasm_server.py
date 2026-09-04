#!/usr/bin/env python3
"""Serve the Bazel WASM output with cross-origin isolation headers."""

from __future__ import annotations

import argparse
import shutil
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.error import HTTPError, URLError
from urllib.parse import parse_qs, urlsplit
from urllib.request import Request, urlopen


def default_output_directory() -> Path:
    runfiles_output = Path("src")
    if (runfiles_output / "saga.html").is_file():
        return runfiles_output
    return Path("bazel-bin/src")


class WasmRequestHandler(SimpleHTTPRequestHandler):
    extensions_map = {
        **SimpleHTTPRequestHandler.extensions_map,
        ".js": "text/javascript; charset=utf-8",
        ".wasm": "application/wasm",
    }

    def end_headers(self) -> None:
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def do_GET(self) -> None:
        if urlsplit(self.path).path == "/proxy":
            self._serve_proxy(head_only=False)
            return
        if urlsplit(self.path).path == "/":
            self.path = "/saga.html"
        super().do_GET()

    def do_HEAD(self) -> None:
        if urlsplit(self.path).path == "/proxy":
            self._serve_proxy(head_only=True)
            return
        if urlsplit(self.path).path == "/":
            self.path = "/saga.html"
        super().do_HEAD()

    def _serve_proxy(self, *, head_only: bool) -> None:
        fetch_site = self.headers.get("Sec-Fetch-Site")
        if fetch_site and fetch_site != "same-origin":
            self.send_error(403, "Cross-origin proxy requests are not allowed")
            return

        query = parse_qs(urlsplit(self.path).query)
        target = query.get("url", [""])[0]
        parsed_target = urlsplit(target)
        if parsed_target.scheme not in {"http", "https"} or not parsed_target.netloc:
            self.send_error(400, "A valid HTTP or HTTPS proxy URL is required")
            return

        try:
            request = Request(target, method="HEAD" if head_only else "GET")
            with urlopen(request, timeout=30) as remote:
                self.send_response(remote.status)
                for header in ("Content-Type", "Content-Length"):
                    value = remote.headers.get(header)
                    if value:
                        self.send_header(header, value)
                self.end_headers()
                if not head_only:
                    shutil.copyfileobj(remote, self.wfile)
        except HTTPError as error:
            self.send_error(error.code, error.reason)
        except URLError as error:
            self.send_error(502, str(error.reason))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--directory", type=Path, default=default_output_directory())
    args = parser.parse_args()

    directory = args.directory.resolve()
    if not directory.is_dir():
        parser.error(f"build output directory does not exist: {directory}")

    handler = partial(WasmRequestHandler, directory=str(directory))
    server = ThreadingHTTPServer((args.host, args.port), handler)
    print(f"Now listening at http://{args.host}:{args.port}/")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
