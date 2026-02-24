#!/bin/bash
set -e

./build.sh Release
BINARY="build/bin/uds_vs_mutex"

if [ ! -f "$BINARY" ]; then
    echo "Binary not found. Run ./build.sh first."
    exit 1
fi

"$BINARY"
