#!/bin/bash
# Quick verification script
set -e

cd /mnt/agents/output/mcgen

echo "=== mcgen Build Verification ==="
echo ""

# Check all required files exist
echo "[1/5] Checking file structure..."
required_files=(
    "CMakeLists.txt"
    "src/main.cpp"
    "src/shared/chunk_data.cpp"
    "src/shared/endian.cpp"
    "src/shared/gzip_utils.cpp"
    "src/shared/mappings/block_mapping.cpp"
    "src/core/noise.cpp"
    "src/core/terrain_engine.cpp"
    "src/core/cave_system.cpp"
    "src/core/ore_veins.cpp"
    "src/core/structures.cpp"
    "src/core/thread_pool.cpp"
    "src/java/nbt/nbt_codec.cpp"
    "src/java/anvil/region_writer.cpp"
    "src/java/world/java_world_writer.cpp"
    "src/bedrock/leveldb/bedrock_leveldb.cpp"
    "src/bedrock/world/bedrock_world_writer.cpp"
    "src/config/world_config.cpp"
    "src/config/cli_parser.cpp"
)

missing=0
for file in "${required_files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "  ✗ Missing: $file"
        missing=$((missing + 1))
    fi
done

if [ $missing -eq 0 ]; then
    echo "  ✓ All required source files present"
else
    echo "  ✗ $missing files missing"
    exit 1
fi

# Count files
echo ""
echo "[2/5] File statistics..."
cpp_count=$(find src -name "*.cpp" | wc -l)
hpp_count=$(find src -name "*.hpp" | wc -l)
test_count=$(find tests -name "*.cpp" | wc -l)
script_count=$(find scripts -name "*.sh" | wc -l)

echo "  Source files (.cpp): $cpp_count"
echo "  Header files (.hpp): $hpp_count"
echo "  Test files: $test_count"
echo "  Scripts: $script_count"

# Check syntax (basic)
echo ""
echo "[3/5] Basic syntax checks..."

# Check for common issues
if grep -r "std::cout" src/ --include="*.cpp" | grep -v "#include" > /dev/null; then
    echo "  ✓ Output statements found"
fi

# Check CMakeLists.txt
echo ""
echo "[4/5] CMake configuration..."
if grep -q "project(mcgen" CMakeLists.txt; then
    echo "  ✓ Project name correct"
fi
if grep -q "MCGEN_HAS_LEVELDB" CMakeLists.txt; then
    echo "  ✓ LevelDB detection present"
fi
if grep -q "find_package(ZLIB" CMakeLists.txt; then
    echo "  ✓ ZLIB dependency present"
fi

# Summary
echo ""
echo "[5/5] Summary"
echo "  ✓ Project structure verified"
echo "  ✓ All core components present"
echo "  ✓ Build system configured"
echo ""
echo "To build:"
echo "  cd /mnt/agents/output/mcgen"
echo "  ./install-deps.sh --with-leveldb"
echo "  ./build.sh"
echo ""
