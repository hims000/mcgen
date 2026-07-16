#!/bin/bash
# Build leveldb-mcpe from source for mcgen
# Supports: Ubuntu/Debian, Fedora, Arch, macOS

set -e

LEVELDB_REPO="https://github.com/Amulet-Team/leveldb-mcpe.git"
BUILD_TYPE="Release"
INSTALL_PREFIX="${1:-/usr/local}"
BUILD_DIR="${2:-leveldb-mcpe-build}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== mcgen: Building leveldb-mcpe ===${NC}"
echo "Install prefix: $INSTALL_PREFIX"
echo "Build directory: $BUILD_DIR"
echo ""

# Detect OS and cores
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    if command -v nproc &> /dev/null; then
        CORES=$(nproc)
    else
        CORES=4
    fi
    DISTRO="unknown"
    if command -v apt &> /dev/null; then
        DISTRO="debian"
    elif command -v yum &> /dev/null; then
        DISTRO="redhat"
    elif command -v pacman &> /dev/null; then
        DISTRO="arch"
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    CORES=$(sysctl -n hw.ncpu)
    DISTRO="homebrew"
else
    OS="unknown"
    CORES=4
    DISTRO="unknown"
fi

echo "OS: $OS | Distro: $DISTRO | Cores: $CORES"

# Install dependencies
echo ""
echo -e "${BLUE}[1/4] Installing dependencies...${NC}"

install_deps() {
    case $DISTRO in
        debian)
            sudo apt update
            sudo apt install -y build-essential cmake git                 libsnappy-dev zlib1g-dev || true
            ;;
        redhat)
            sudo yum install -y gcc-c++ cmake git snappy-devel zlib-devel || true
            ;;
        arch)
            sudo pacman -S --needed --noconfirm base-devel cmake git snappy zlib || true
            ;;
        homebrew)
            brew install cmake snappy zlib || true
            ;;
        *)
            echo -e "${YELLOW}⚠ Please manually install: cmake, git, snappy, zlib${NC}"
            ;;
    esac
}

install_deps

# Clone repository
echo ""
echo -e "${BLUE}[2/4] Cloning leveldb-mcpe...${NC}"

if [ -d "$BUILD_DIR" ]; then
    echo "Directory exists, pulling latest changes..."
    cd "$BUILD_DIR"
    git pull
else
    git clone "$LEVELDB_REPO" "$BUILD_DIR"
    cd "$BUILD_DIR"
fi

# Build
echo ""
echo -e "${BLUE}[3/4] Building...${NC}"

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
      -DLEVELDB_BUILD_TESTS=OFF \
      -DLEVELDB_BUILD_BENCHMARKS=OFF \
      -DBUILD_SHARED_LIBS=ON \
      ..

make -j$CORES

# Install
echo ""
echo -e "${BLUE}[4/4] Installing...${NC}"

sudo make install

# Update ldconfig on Linux
if [ "$OS" == "linux" ]; then
    sudo ldconfig
    echo "Updated ldconfig cache"
fi

# Verify
echo ""
echo -e "${GREEN}=== Installation Complete ===${NC}"
echo ""

if [ -f "$INSTALL_PREFIX/include/leveldb/zlib_compressor.h" ]; then
    echo -e "${GREEN}✅ ZlibRaw support confirmed${NC}"
else
    echo -e "${YELLOW}⚠️  ZlibRaw header not found at expected location${NC}"
fi

echo "Library: $INSTALL_PREFIX/lib"
echo "Headers: $INSTALL_PREFIX/include/leveldb"
echo ""
echo "You can now build mcgen with full Bedrock support:"
echo "  ./build.sh"
