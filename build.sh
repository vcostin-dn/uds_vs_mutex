#!/bin/bash
set -e

ARG="${1:-Release}"

if [ "${ARG,,}" = "clean" ]; then
    for dir in build debug asan; do
        if [ -d "$dir" ]; then
            rm -rf "$dir"
            echo "Removed $dir/"
        fi
    done
    exit 0
fi

CMAKE_TYPE="Release"
case "${ARG,,}" in
    release) BUILD_DIR="build" ; CMAKE_TYPE="Release" ;;
    debug)   BUILD_DIR="debug" ; CMAKE_TYPE="Debug"   ;;
    asan)    BUILD_DIR="asan"  ; CMAKE_TYPE="ASan"    ;;
    *)
        echo "Unknown argument: $ARG"
        echo "Usage: $0 [Release|Debug|ASan|clean]"
        exit 1
        ;;
esac

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$CMAKE_TYPE"
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

echo "Build complete. Binary: $BUILD_DIR/bin/uds_vs_mutex"
