#!/bin/bash
# mcgen ARM64 Cross-Compilation Script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="build-arm"
BUILD_TYPE="Release"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}  mcgen ARM64 Cross-Build${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# Check for toolchain file
TOOLCHAIN="$SCRIPT_DIR/cmake/arm64-toolchain.cmake"
if [ ! -f "$TOOLCHAIN" ]; then
    echo -e "${YELLOW}⚠ ARM toolchain file not found: $TOOLCHAIN${NC}"
    echo "  Creating default toolchain..."
    mkdir -p "$(dirname $TOOLCHAIN)"
    cat > "$TOOLCHAIN" << 'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Cross-compilation toolchain
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Search paths
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
    echo -e "  ${GREEN}✓${NC} Default toolchain created"
fi

# Check for cross-compiler
if ! command -v aarch64-linux-gnu-g++ &> /dev/null; then
    echo -e "${YELLOW}⚠ ARM cross-compiler not found${NC}"
    echo "  Installing..."
    if command -v apt &> /dev/null; then
        sudo apt install -y g++-aarch64-linux-gnu gcc-aarch64-linux-gnu
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --needed aarch64-linux-gnu-gcc
    else
        echo -e "${RED}✗ Please install aarch64-linux-gnu-g++ manually${NC}"
        exit 1
    fi
fi

echo -e "  ${GREEN}✓${NC} Cross-compiler: $(aarch64-linux-gnu-g++ --version | head -1)"

# Clean and configure
echo ""
echo -e "${BLUE}Configuring for ARM64...${NC}"
rm -rf "$BUILD_DIR"

cmake -S "$SCRIPT_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_INSTALL_PREFIX=/usr/aarch64-linux-gnu

# Build
echo ""
echo -e "${BLUE}Building...${NC}"
cmake --build "$BUILD_DIR" --parallel $(nproc)

# Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  ARM64 Build Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "  Binary: $BUILD_DIR/mcgen"
echo "  Tests:  $BUILD_DIR/mcgen_tests"
echo ""
echo "To run on target device:"
echo "  scp $BUILD_DIR/mcgen user@arm-device:~/"
echo ""
