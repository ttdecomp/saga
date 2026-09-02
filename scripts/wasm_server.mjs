import { createReadStream } from "node:fs";
import { stat } from "node:fs/promises";
import { createServer } from "node:http";
import { extname, resolve, sep } from "node:path";
import { Readable } from "node:stream";

const host = "127.0.0.1";
const port = 8000;
const root = resolve("build-wasm");
const contentTypes = new Map([
  [".css", "text/css; charset=utf-8"],
  [".html", "text/html; charset=utf-8"],
  [".js", "text/javascript; charset=utf-8"],
  [".json", "application/json"],
  [".wasm", "application/wasm"],
]);

function setIsolationHeaders(response) {
  response.setHeader("Cross-Origin-Embedder-Policy", "require-corp");
  response.setHeader("Cross-Origin-Opener-Policy", "same-origin");
  response.setHeader("Cross-Origin-Resource-Policy", "same-origin");
}

function fail(response, status, message) {
  response.writeHead(status, { "Content-Type": "text/plain; charset=utf-8" });
  response.end(message);
}

async function serveProxy(request, response, url) {
  if (request.headers["sec-fetch-site"] && request.headers["sec-fetch-site"] !== "same-origin") {
    fail(response, 403, "Cross-origin proxy requests are not allowed.");
    return;
  }

  const targetValue = url.searchParams.get("url");
  if (!targetValue) {
    fail(response, 400, "Missing proxy URL.");
    return;
  }

  let target;
  try {
    target = new URL(targetValue);
  } catch {
    fail(response, 400, "Invalid proxy URL.");
    return;
  }
  if (target.protocol !== "http:" && target.protocol !== "https:") {
    fail(response, 400, "Only HTTP and HTTPS URLs are supported.");
    return;
  }

  const controller = new AbortController();
  response.on("close", () => {
    if (!response.writableEnded) {
      controller.abort();
    }
  });

  const remote = await fetch(target, {
    method: request.method,
    signal: controller.signal,
  });
  response.statusCode = remote.status;
  const contentType = remote.headers.get("content-type");
  if (contentType) {
    response.setHeader("Content-Type", contentType);
  }
  if (!remote.headers.has("content-encoding")) {
    const contentLength = remote.headers.get("content-length");
    if (contentLength) {
      response.setHeader("Content-Length", contentLength);
    }
  }
  if (!remote.body) {
    response.end();
    return;
  }
  Readable.fromWeb(remote.body).pipe(response);
}

async function serveFile(request, response, url) {
  let pathname;
  try {
    pathname = decodeURIComponent(url.pathname);
  } catch {
    fail(response, 400, "Invalid path.");
    return;
  }
  if (pathname === "/") {
    pathname = "/index.html";
  }

  const path = resolve(root, `.${pathname}`);
  if (path !== root && !path.startsWith(`${root}${sep}`)) {
    fail(response, 403, "Path is outside the build directory.");
    return;
  }

  let file;
  try {
    file = await stat(path);
  } catch {
    fail(response, 404, "Not found.");
    return;
  }
  if (!file.isFile()) {
    fail(response, 404, "Not found.");
    return;
  }

  response.setHeader("Content-Length", file.size);
  response.setHeader("Content-Type", contentTypes.get(extname(path)) || "application/octet-stream");
  response.setHeader("Cache-Control", "no-store");
  if (request.method === "HEAD") {
    response.end();
    return;
  }
  createReadStream(path).pipe(response);
}

const server = createServer(async (request, response) => {
  setIsolationHeaders(response);
  if (request.method !== "GET" && request.method !== "HEAD") {
    fail(response, 405, "Method not allowed.");
    return;
  }

  try {
    const url = new URL(request.url, `http://${host}:${port}`);
    if (url.pathname === "/proxy") {
      await serveProxy(request, response, url);
    } else {
      await serveFile(request, response, url);
    }
  } catch (error) {
    if (!response.headersSent) {
      fail(response, 502, error instanceof Error ? error.message : String(error));
    } else {
      response.destroy(error);
    }
  }
});

server.listen(port, host, () => {
  console.log(`Now listening at http://${host}:${port}/`);
});
