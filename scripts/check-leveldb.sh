#!/bin/bash
# LevelDB Detection Script for mcgen
# Usage: ./scripts/check-leveldb.sh

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "=========================================="
echo "  mcgen LevelDB Detection Script"
echo "=========================================="
echo ""

# Detection results
FOUND_LEVELDB=0
HAS_ZLIBRAW=0
BACKEND="stub"
LEVELDB_PATH=""
INCLUDE_PATH=""

# 1. Check pkg-config
echo -e "${BLUE}[1/5]${NC} Checking pkg-config..."
if pkg-config --exists leveldb 2>/dev/null; then
    echo -e "  ${GREEN}✓${NC} pkg-config found leveldb"
    echo "    Version: $(pkg-config --modversion leveldb)"
    echo "    Libs: $(pkg-config --libs leveldb)"
    echo "    Cflags: $(pkg-config --cflags leveldb)"
    FOUND_LEVELDB=1
else
    echo -e "  ${YELLOW}✗${NC} pkg-config: leveldb not registered"
fi

# 2. Check library files
echo ""
echo -e "${BLUE}[2/5]${NC} Searching for library files..."
LIB_PATHS=(
    "/usr/local/lib/libleveldb*"
    "/usr/lib/libleveldb*"
    "/usr/lib/x86_64-linux-gnu/libleveldb*"
    "/usr/lib/aarch64-linux-gnu/libleveldb*"
    "/opt/local/lib/libleveldb*"
    "/usr/local/lib64/libleveldb*"
)

for pattern in "${LIB_PATHS[@]}"; do
    found=$(find $(dirname "$pattern") -name "$(basename "$pattern")" 2>/dev/null | head -5)
    if [ -n "$found" ]; then
        echo -e "  ${GREEN}✓${NC} Found:"
        echo "$found" | sed 's/^/    /'
        FOUND_LEVELDB=1
        LEVELDB_PATH=$(echo "$found" | head -1 | xargs dirname)
    fi
done

if [ $FOUND_LEVELDB -eq 0 ]; then
    echo -e "  ${YELLOW}✗${NC} No libleveldb found in standard paths"
fi

# 3. Check header files
echo ""
echo -e "${BLUE}[3/5]${NC} Searching for header files..."
HEADER_PATHS=(
    "/usr/local/include/leveldb/db.h"
    "/usr/include/leveldb/db.h"
    "/opt/local/include/leveldb/db.h"
)

for header in "${HEADER_PATHS[@]}"; do
    if [ -f "$header" ]; then
        echo -e "  ${GREEN}✓${NC} Found: $header"
        FOUND_LEVELDB=1
        INCLUDE_PATH=$(dirname "$header")

        # Check for ZlibRaw support
        if [ -f "$(dirname $header)/zlib_compressor.h" ]; then
            echo -e "  ${GREEN}✓${NC} ZlibRaw support detected (zlib_compressor.h)"
            HAS_ZLIBRAW=1
        else
            echo -e "  ${YELLOW}⚠${NC}  No ZlibRaw support (missing zlib_compressor.h)"
            echo -e "       This is likely the Google原版 LevelDB"
        fi
    fi
done

if [ -z "$INCLUDE_PATH" ]; then
    echo -e "  ${YELLOW}✗${NC} No leveldb/db.h found"
fi

# 4. Check ldconfig
echo ""
echo -e "${BLUE}[4/5]${NC} Checking dynamic linker cache..."
if ldconfig -p 2>/dev/null | grep -q libleveldb; then
    echo -e "  ${GREEN}✓${NC} libleveldb in ldconfig cache"
    ldconfig -p | grep libleveldb | sed 's/^/    /'
else
    echo -e "  ${YELLOW}✗${NC} libleveldb not in ldconfig cache"
    echo "    Run 'sudo ldconfig' after manual installation"
fi

# 5. Test compile (optional)
echo ""
echo -e "${BLUE}[5/5]${NC} Testing compilation..."

if command -v g++ &> /dev/null && [ -n "$INCLUDE_PATH" ] && [ -n "$LEVELDB_PATH" ]; then
    cat > /tmp/leveldb_test.cpp << 'EOF'
#include <leveldb/db.h>
#include <iostream>
int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb_check", &db);
    if (status.ok()) {
        std::cout << "OK" << std::endl;
        delete db;
        return 0;
    }
    return 1;
}
EOF

    if g++ -o /tmp/leveldb_test /tmp/leveldb_test.cpp -I"$INCLUDE_PATH" -L"$LEVELDB_PATH" -lleveldb -lpthread 2>/dev/null; then
        if /tmp/leveldb_test 2>/dev/null; then
            echo -e "  ${GREEN}✓${NC} Compilation and runtime test passed"
        else
            echo -e "  ${YELLOW}⚠${NC}  Compilation passed but runtime failed"
        fi
        rm -f /tmp/leveldb_test /tmp/leveldb_test.cpp
        rm -rf /tmp/testdb_check
    else
        echo -e "  ${YELLOW}⚠${NC}  Compilation test failed (may need -lleveldb in LD_LIBRARY_PATH)"
    fi
else
    echo -e "  ${YELLOW}⊘${NC}  Skipped (g++ or headers not available)"
fi

# Summary
echo ""
echo "=========================================="
echo "  Summary"
echo "=========================================="

if [ $FOUND_LEVELDB -eq 1 ]; then
    if [ $HAS_ZLIBRAW -eq 1 ]; then
        echo -e "${GREEN}✅ LevelDB Status: INSTALLED with ZlibRaw support${NC}"
        echo -e "${GREEN}   mcgen Bedrock backend: FULL (system LevelDB)${NC}"
        BACKEND="system"
    else
        echo -e "${YELLOW}⚠️  LevelDB Status: INSTALLED but NO ZlibRaw${NC}"
        echo -e "${YELLOW}   mcgen Bedrock backend: LIMITED (compatibility warning)${NC}"
        echo ""
        echo "   Recommendation: Build leveldb-mcpe from source for"
        echo "   full Bedrock Edition compatibility."
        echo "   See: docs/leveldb-install-guide.md"
        BACKEND="system (limited)"
    fi
else
    echo -e "${YELLOW}ℹ️  LevelDB Status: NOT INSTALLED${NC}"
    echo -e "${YELLOW}   mcgen Bedrock backend: STUB (memory-only, no persistence)${NC}"
    echo ""
    echo "   To enable Bedrock world generation, install leveldb-mcpe:"
    echo "   ./scripts/build-leveldb-mcpe.sh"
    BACKEND="stub"
fi

echo ""
echo "Backend: $BACKEND"
echo ""

# Return code for CI/CD
if [ "$1" == "--ci" ]; then
    if [ "$BACKEND" == "system" ]; then
        exit 0
    else
        exit 1
    fi
fi
