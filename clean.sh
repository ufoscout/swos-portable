#!/usr/bin/env bash
#
# Removes all temporary files created by the Linux build script
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

rm -rf "$SCRIPT_DIR/build"
rm -rf "$SCRIPT_DIR/build-linux"
rm -rf "$SCRIPT_DIR/build-linux-embedded"
rm -rf "$SCRIPT_DIR/bin"

echo "Clean complete."
