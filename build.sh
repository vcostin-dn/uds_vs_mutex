#!/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="${1:-Release}"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

echo "Build complete. Binary: $BUILD_DIR/bin/uds_vs_mutex"
