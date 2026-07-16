#!/bin/bash
# mcgen Build Script
# Supports: Ubuntu/Debian, macOS, Windows (MSYS2)
# Auto-detects LevelDB and selects appropriate backend

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_NAME="mcgen"
BUILD_TYPE="Release"
BUILD_DIR="build"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Parse arguments
CLEAN=0
VERBOSE=0
INSTALL_LEVELDB=0
ARM_BUILD=0

while [[ $# -gt 0 ]]; do
    case $1 in
        debug|Debug|DEBUG)
            BUILD_TYPE="Debug"
            shift
            ;;
        release|Release|RELEASE)
            BUILD_TYPE="Release"
            shift
            ;;
        clean)
            CLEAN=1
            shift
            ;;
        verbose)
            VERBOSE=1
            shift
            ;;
        --install-leveldb)
            INSTALL_LEVELDB=1
            shift
            ;;
        --arm|--arm64)
            ARM_BUILD=1
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: ./build.sh [debug|release] [clean] [verbose] [--install-leveldb] [--arm] [-j N]"
            echo ""
            echo "Options:"
            echo "  debug|release     Build type (default: release)"
            echo "  clean             Clean build directory before building"
            echo "  verbose           Verbose CMake output"
            echo "  --install-leveldb  Build and install leveldb-mcpe from source"
            echo "  --arm             Cross-compile for ARM64"
            echo "  -j N              Number of parallel jobs (default: auto)"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}  mcgen Build System${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# Step 1: Check dependencies
echo -e "${BLUE}[1/6] Checking dependencies...${NC}"

MISSING_DEPS=()

# Check CMake
if ! command -v cmake &> /dev/null; then
    MISSING_DEPS+=("cmake")
fi

# Check C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    MISSING_DEPS+=("g++ or clang++")
fi

# Check vcpkg (optional but recommended)
VCPKG_ROOT="${VCPKG_ROOT:-}"
if [ -z "$VCPKG_ROOT" ] && [ -d "$HOME/vcpkg" ]; then
    VCPKG_ROOT="$HOME/vcpkg"
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    echo -e "${RED}✗ Missing dependencies:${NC}"
    for dep in "${MISSING_DEPS[@]}"; do
        echo "    - $dep"
    done
    echo ""
    echo "Run ./install-deps.sh to install dependencies."
    exit 1
fi

echo -e "  ${GREEN}✓${NC} CMake: $(cmake --version | head -1)"
echo -e "  ${GREEN}✓${NC} Compiler detected"

# Step 2: LevelDB Detection
echo ""
echo -e "${BLUE}[2/6] Detecting LevelDB...${NC}"

LEVELDB_STATUS="stub"
HAS_ZLIBRAW=0

# Run detection script if available
if [ -f "$SCRIPT_DIR/scripts/check-leveldb.sh" ]; then
    echo "  Running check-leveldb.sh..."
    bash "$SCRIPT_DIR/scripts/check-leveldb.sh" > /tmp/leveldb_check.log 2>&1 || true

    if grep -q "ZlibRaw support" /tmp/leveldb_check.log 2>/dev/null; then
        if grep -q "ZlibRaw support detected" /tmp/leveldb_check.log 2>/dev/null; then
            LEVELDB_STATUS="system"
            HAS_ZLIBRAW=1
            echo -e "  ${GREEN}✓${NC} leveldb-mcpe with ZlibRaw detected"
        else
            LEVELDB_STATUS="system-limited"
            echo -e "  ${YELLOW}⚠${NC}  LevelDB found but no ZlibRaw support"
        fi
    else
        echo -e "  ${YELLOW}ℹ${NC}  No system LevelDB found"
    fi
else
    # Manual detection
    if pkg-config --exists leveldb 2>/dev/null; then
        echo -e "  ${YELLOW}⚠${NC}  pkg-config found leveldb (version unknown)"
        LEVELDB_STATUS="system"
    elif ldconfig -p 2>/dev/null | grep -q libleveldb; then
        echo -e "  ${YELLOW}⚠${NC}  libleveldb found in cache"
        LEVELDB_STATUS="system"
    else
        echo -e "  ${YELLOW}ℹ${NC}  No LevelDB found, will use stub backend"
    fi
fi

# Offer to install leveldb-mcpe if not found or limited
if [ "$INSTALL_LEVELDB" -eq 1 ] || [ "$LEVELDB_STATUS" = "stub" ] || [ "$LEVELDB_STATUS" = "system-limited" ]; then
    if [ "$INSTALL_LEVELDB" -eq 1 ]; then
        echo ""
        echo -e "${BLUE}[2.5/6] Building leveldb-mcpe from source...${NC}"
        if [ -f "$SCRIPT_DIR/scripts/build-leveldb-mcpe.sh" ]; then
            bash "$SCRIPT_DIR/scripts/build-leveldb-mcpe.sh"
            # Re-check
            if [ -f "/usr/local/include/leveldb/zlib_compressor.h" ] ||                [ -f "/usr/include/leveldb/zlib_compressor.h" ]; then
                LEVELDB_STATUS="system"
                HAS_ZLIBRAW=1
                echo -e "  ${GREEN}✓${NC} leveldb-mcpe installed successfully"
            fi
        else
            echo -e "  ${RED}✗ build-leveldb-mcpe.sh not found${NC}"
        fi
    elif [ "$LEVELDB_STATUS" = "stub" ] || [ "$LEVELDB_STATUS" = "system-limited" ]; then
        echo ""
        echo -e "${YELLOW}⚠ LevelDB not found or lacks ZlibRaw support.${NC}"
        echo "  Bedrock world generation will use stub backend (no persistence)."
        echo "  To install leveldb-mcpe, run:"
        echo "    ./build.sh --install-leveldb"
        echo "  Or manually:"
        echo "    ./scripts/build-leveldb-mcpe.sh"
    fi
fi

# Step 3: Clean if requested
echo ""
if [ $CLEAN -eq 1 ]; then
    echo -e "${BLUE}[3/6] Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "  ${GREEN}✓${NC} Build directory cleaned"
else
    echo -e "${BLUE}[3/6] Build directory: $BUILD_DIR${NC}"
fi

# Step 4: Configure with CMake
echo ""
echo -e "${BLUE}[4/6] Configuring with CMake...${NC}"

CMAKE_ARGS=(
    -S "$SCRIPT_DIR"
    -B "$BUILD_DIR"
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
)

# ARM build
if [ $ARM_BUILD -eq 1 ]; then
    if [ -f "$SCRIPT_DIR/cmake/arm64-toolchain.cmake" ]; then
        CMAKE_ARGS+=(-DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/arm64-toolchain.cmake")
        echo -e "  ${CYAN}→${NC} Using ARM64 toolchain"
    else
        echo -e "  ${YELLOW}⚠ ARM toolchain not found, using native${NC}"
    fi
fi

# vcpkg integration
if [ -n "$VCPKG_ROOT" ] && [ -f "$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ]; then
    CMAKE_ARGS+=(-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake")
    echo -e "  ${CYAN}→${NC} Using vcpkg: $VCPKG_ROOT"
fi

# Verbose
if [ $VERBOSE -eq 1 ]; then
    CMAKE_ARGS+=(--debug-output)
fi

echo "  CMake arguments: ${CMAKE_ARGS[*]}"

cmake "${CMAKE_ARGS[@]}"

# Step 5: Build
echo ""
echo -e "${BLUE}[5/6] Building with $JOBS parallel jobs...${NC}"

BUILD_ARGS=(
    --build "$BUILD_DIR"
    --config "$BUILD_TYPE"
    --parallel "$JOBS"
)

if [ $VERBOSE -eq 1 ]; then
    BUILD_ARGS+=(--verbose)
fi

cmake "${BUILD_ARGS[@]}"

# Step 6: Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Build Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "  Build type: $BUILD_TYPE"
echo "  Backend:    $LEVELDB_STATUS"
echo "  ZlibRaw:    $([ $HAS_ZLIBRAW -eq 1 ] && echo 'Yes' || echo 'No')"
echo "  Binary:     $BUILD_DIR/mcgen"
echo "  Tests:      $BUILD_DIR/mcgen_tests"
echo ""

if [ "$LEVELDB_STATUS" = "stub" ]; then
    echo -e "${YELLOW}⚠ Note: Using stub LevelDB backend.${NC}"
    echo "  Bedrock worlds will be generated in memory only."
    echo "  Install leveldb-mcpe for full persistence:"
    echo "    ./scripts/build-leveldb-mcpe.sh"
fi

echo ""
echo "Run tests:    ./$BUILD_DIR/mcgen_tests"
echo "Run mcgen:    ./$BUILD_DIR/mcgen --help"
echo ""
