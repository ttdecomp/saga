#!/usr/bin/env bash
set -euo pipefail

binary_path="$1"
shift

binary_directory=$(cd "$(dirname "$binary_path")" && pwd -P)
binary="$binary_directory/$(basename "$binary_path")"
working_directory="${BUILD_WORKING_DIRECTORY:?must be run with bazel run}"

cd "$working_directory"
exec "$binary" "$@"
