#!/bin/bash
set -e

BUILD_TYPE="${1:-Release}"

case "${BUILD_TYPE,,}" in
    release)        BUILD_DIR="build" ;;
    debug)          BUILD_DIR="debug" ;;
    asan)           BUILD_DIR="asan" ;;
    *)
        echo "Unknown build type: $BUILD_TYPE"
        echo "Usage: $0 [Release|Debug|ASan]"
        exit 1
        ;;
esac

BINARY="$BUILD_DIR/bin/uds_vs_mutex"

if [ ! -f "$BINARY" ]; then
    ./build.sh "$BUILD_TYPE"
fi

"$BINARY"
