#!/bin/bash
# Fix permissions for mcgen project files

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Fixing permissions..."

# Make scripts executable
find "$SCRIPT_DIR" -name "*.sh" -exec chmod +x {} \;

# Source files should be readable
find "$SCRIPT_DIR/src" -type f -exec chmod 644 {} \; 2>/dev/null || true
find "$SCRIPT_DIR/include" -type f -exec chmod 644 {} \; 2>/dev/null || true

# CMake files
find "$SCRIPT_DIR" -name "CMakeLists.txt" -exec chmod 644 {} \;
find "$SCRIPT_DIR/cmake" -type f -exec chmod 644 {} \; 2>/dev/null || true

echo "Done."
