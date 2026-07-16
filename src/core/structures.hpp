#pragma once
#include "shared/chunk_data.hpp"
#include <cstdint>
#include <random>

namespace mcgen {

// Structure generator (trees, dungeons, etc.)
class StructureGenerator {
public:
    explicit StructureGenerator(uint64_t seed);

    // Generate structures in chunk
    void generate(ChunkData& chunk);

    // Specific structure types
    void generateTree(ChunkData& chunk, int worldX, int worldY, int worldZ);
    void generateDungeon(ChunkData& chunk, int worldX, int worldY, int worldZ);

private:
    uint64_t seed_;
    std::mt19937 rng_;

    // Tree parameters
    static constexpr double TREE_DENSITY = 0.1;
    static constexpr int MIN_TREE_HEIGHT = 4;
    static constexpr int MAX_TREE_HEIGHT = 7;
};

} // namespace mcgen
