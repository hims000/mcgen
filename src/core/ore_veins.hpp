#pragma once
#include "shared/chunk_data.hpp"
#include "shared/mappings/block_mapping.hpp"
#include <cstdint>
#include <random>

namespace mcgen {

// Random walk ore vein generator
class OreVeins {
public:
    explicit OreVeins(uint64_t seed);

    // Generate ore veins in chunk
    void generate(ChunkData& chunk);

private:
    uint64_t seed_;
    std::mt19937 rng_;

    struct OreConfig {
        BlockType type;
        int minY, maxY;
        int veinSize;
        int veinsPerChunk;
        double rarity;
    };

    static constexpr OreConfig ORES[] = {
        {BlockType::CoalOre,   0,   128, 17, 20, 1.0},
        {BlockType::IronOre,   0,   64,  9,  20, 0.8},
        {BlockType::GoldOre,   0,   32,  9,  2,  0.3},
        {BlockType::DiamondOre, -48, 16,  8,  1,  0.1},
    };

    void generateVein(ChunkData& chunk, const OreConfig& config, int startX, int startY, int startZ);
};

} // namespace mcgen
