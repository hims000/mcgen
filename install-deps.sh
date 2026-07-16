#!/bin/bash
# mcgen Dependency Installer
# Installs all required build dependencies
# Optionally builds leveldb-mcpe from source

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

INSTALL_LEVELDB=0
SKIP_SYSTEM_UPDATE=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --with-leveldb)
            INSTALL_LEVELDB=1
            shift
            ;;
        --skip-update)
            SKIP_SYSTEM_UPDATE=1
            shift
            ;;
        -h|--help)
            echo "Usage: ./install-deps.sh [options]"
            echo ""
            echo "Options:"
            echo "  --with-leveldb    Also build and install leveldb-mcpe from source"
            echo "  --skip-update     Skip apt/yum update (faster)"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}  mcgen Dependency Installer${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    if command -v apt &> /dev/null; then
        DISTRO="debian"
        PKG_MANAGER="apt"
    elif command -v yum &> /dev/null; then
        DISTRO="redhat"
        PKG_MANAGER="yum"
    elif command -v dnf &> /dev/null; then
        DISTRO="redhat"
        PKG_MANAGER="dnf"
    elif command -v pacman &> /dev/null; then
        DISTRO="arch"
        PKG_MANAGER="pacman"
    else
        echo -e "${RED}✗ Unsupported Linux distribution${NC}"
        exit 1
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    DISTRO="homebrew"
    PKG_MANAGER="brew"
else
    echo -e "${RED}✗ Unsupported OS: $OSTYPE${NC}"
    exit 1
fi

echo "Detected: $OS ($DISTRO)"
echo ""

# Update package lists
if [ $SKIP_SYSTEM_UPDATE -eq 0 ]; then
    echo -e "${BLUE}[1/5] Updating package lists...${NC}"
    case $PKG_MANAGER in
        apt)
            sudo apt update
            ;;
        yum|dnf)
            sudo $PKG_MANAGER check-update || true
            ;;
        pacman)
            sudo pacman -Sy
            ;;
        brew)
            brew update
            ;;
    esac
else
    echo -e "${BLUE}[1/5] Skipping package list update${NC}"
fi

# Install base dependencies
echo ""
echo -e "${BLUE}[2/5] Installing base build dependencies...${NC}"

case $PKG_MANAGER in
    apt)
        sudo apt install -y \
            build-essential \
            cmake \
            git \
            ninja-build \
            pkg-config \
            libgtest-dev \
            libgmock-dev \
            zlib1g-dev \
            libsnappy-dev
        ;;
    yum|dnf)
        sudo $PKG_MANAGER install -y \
            gcc-c++ \
            cmake \
            git \
            ninja-build \
            pkgconfig \
            gtest-devel \
            gmock-devel \
            zlib-devel \
            snappy-devel
        ;;
    pacman)
        sudo pacman -S --needed --noconfirm \
            base-devel \
            cmake \
            git \
            ninja \
            gtest \
            zlib \
            snappy
        ;;
    brew)
        brew install \
            cmake \
            git \
            ninja \
            googletest \
            zlib \
            snappy
        ;;
esac

echo -e "  ${GREEN}✓${NC} Base dependencies installed"

# Install vcpkg (optional but recommended)
echo ""
echo -e "${BLUE}[3/5] Checking vcpkg...${NC}"

if command -v vcpkg &> /dev/null; then
    echo -e "  ${GREEN}✓${NC} vcpkg already installed"
elif [ -d "$HOME/vcpkg" ]; then
    echo -e "  ${GREEN}✓${NC} vcpkg found at $HOME/vcpkg"
    echo "    Add to PATH: export PATH=\$HOME/vcpkg:\$PATH"
else
    echo -e "  ${YELLOW}ℹ${NC}  vcpkg not found (optional)"
    echo "    To install: git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg"
    echo "               ~/vcpkg/bootstrap-vcpkg.sh"
fi

# Check for existing LevelDB
echo ""
echo -e "${BLUE}[4/5] Checking existing LevelDB installation...${NC}"

LEVELDB_FOUND=0
HAS_ZLIBRAW=0

if [ -f "/usr/local/include/leveldb/db.h" ] || [ -f "/usr/include/leveldb/db.h" ]; then
    LEVELDB_FOUND=1
    if [ -f "/usr/local/include/leveldb/zlib_compressor.h" ] || [ -f "/usr/include/leveldb/zlib_compressor.h" ]; then
        HAS_ZLIBRAW=1
        echo -e "  ${GREEN}✓${NC} leveldb-mcpe with ZlibRaw already installed"
    else
        echo -e "  ${YELLOW}⚠${NC}  LevelDB found but may be Google原版 (no ZlibRaw)"
    fi
else
    echo -e "  ${YELLOW}ℹ${NC}  No LevelDB installation found"
fi

# Install leveldb-mcpe if requested
if [ $INSTALL_LEVELDB -eq 1 ]; then
    echo ""
    echo -e "${BLUE}[5/5] Building leveldb-mcpe from source...${NC}"

    if [ -f "$SCRIPT_DIR/scripts/build-leveldb-mcpe.sh" ]; then
        bash "$SCRIPT_DIR/scripts/build-leveldb-mcpe.sh"
    else
        echo -e "  ${RED}✗ build-leveldb-mcpe.sh not found${NC}"
        echo "  Manual steps:"
        echo "    git clone https://github.com/Amulet-Team/leveldb-mcpe.git"
        echo "    cd leveldb-mcpe && mkdir build && cd build"
        echo "    cmake .. && make && sudo make install"
    fi
else
    echo ""
    echo -e "${BLUE}[5/5] LevelDB installation skipped${NC}"
    if [ $LEVELDB_FOUND -eq 0 ]; then
        echo ""
        echo -e "${YELLOW}⚠ No LevelDB found.${NC}"
        echo "  mcgen will use stub backend (no Bedrock persistence)."
        echo "  To install leveldb-mcpe, run:"
        echo "    ./install-deps.sh --with-leveldb"
    fi
fi

# Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Dependency Installation Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Next steps:"
echo "  1. Build mcgen:      ./build.sh"
echo "  2. Run tests:        ./build/mcgen_tests"
echo "  3. Generate world:   ./build/mcgen --help"
echo ""

if [ $INSTALL_LEVELDB -eq 0 ] && [ $HAS_ZLIBRAW -eq 0 ]; then
    echo -e "${YELLOW}Tip: For full Bedrock support, install leveldb-mcpe:${NC}"
    echo "  ./install-deps.sh --with-leveldb"
    echo ""
fi
