#!/bin/bash
# mcgen Ubuntu Quickstart
# One-command setup: install deps, build, and test

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}  mcgen Ubuntu Quickstart${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo "This script will:"
echo "  1. Install all dependencies"
echo "  2. Build leveldb-mcpe from source"
echo "  3. Build mcgen"
echo "  4. Run tests"
echo ""
read -p "Continue? [Y/n] " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]] && [ -n "$REPLY" ]; then
    echo "Aborted."
    exit 0
fi

# Step 1: Dependencies
echo ""
echo -e "${BLUE}[1/4] Installing dependencies...${NC}"
bash "$SCRIPT_DIR/install-deps.sh" --with-leveldb --skip-update

# Step 2: Build
echo ""
echo -e "${BLUE}[2/4] Building mcgen...${NC}"
bash "$SCRIPT_DIR/build.sh" release

# Step 3: Tests
echo ""
echo -e "${BLUE}[3/4] Running tests...${NC}"
if [ -f "$SCRIPT_DIR/build/mcgen_tests" ]; then
    "$SCRIPT_DIR/build/mcgen_tests"
else
    echo -e "  ${YELLOW}⚠ Test binary not found${NC}"
fi

# Step 4: Demo
echo ""
echo -e "${BLUE}[4/4] Generating demo world...${NC}"
if [ -f "$SCRIPT_DIR/build/mcgen" ]; then
    "$SCRIPT_DIR/build/mcgen" --seed "quickstart" --name "DemoWorld" --platform java --radius 1 --output /tmp/mcgen-demo
    echo -e "  ${GREEN}✓${NC} Demo world generated at /tmp/mcgen-demo"
else
    echo -e "  ${YELLOW}⚠ mcgen binary not found${NC}"
fi

# Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Quickstart Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "mcgen is ready to use:"
echo "  ./build/mcgen --help"
echo ""
echo "Example commands:"
echo "  ./build/mcgen --seed 'hello world' --name MyWorld --platform both --radius 5"
echo "  ./build/mcgen --seed 12345 --platform bedrock --radius 3 --output ~/minecraft-worlds"
echo ""
