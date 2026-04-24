#!/usr/bin/env bash
#
# SWOS Port - Linux build script
#
# Dependencies (Ubuntu/Debian):
#   sudo apt install cmake g++ pkg-config \
#       libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libminizip-dev
#
# Usage:
#   ./build.sh
#
# The script will produce a binary named "swos-port-x64-Release" into the "bin" directory.
# For Launching the game:
#   ./bin/swos-port-x64-Release --swos-dir=/path/to/swos9697/
#
#  The game data directory must contain the original SWOS 96/97 files
#  (team1.dat, bench.dat, goal1.dat, etc.). 
#
# !!!WARNING!!!:
#  On Linux the entire game path must be lowercase!!! SWOS filenames must be
#   lowercase; use the following one-liner to rename them if needed:
#
#     find /path/to/swos9697 -depth -exec \
#       sh -c 'mv -- "$1" "$(dirname "$1")/$(basename "$1" | tr "[:upper:]" "[:lower:]")"' \
#       _ {} \;
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-linux-embedded"

echo "========================================"
echo "SWOS Port - Linux Build (embedded assets)"
echo "========================================"
echo

echo "Configuring..."
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DSWOS_EMBED_ASSETS=ON

echo
echo "Building..."
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

echo
echo "========================================"
echo "Build successful!"
echo "========================================"
echo "Output: bin/swos-port-x64-Release"
echo
