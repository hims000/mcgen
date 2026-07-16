#pragma once
#include "shared/chunk_data.hpp"
#include "noise.hpp"
#include <memory>

namespace mcgen {

// Dual-noise-field cave carving system
class CaveSystem {
public:
    explicit CaveSystem(uint64_t seed);

    // Carve caves into chunk
    void carve(ChunkData& chunk);

    // Check if position is cave (for structure avoidance)
    bool isCave(int worldX, int worldY, int worldZ) const;

private:
    uint64_t seed_;
    std::unique_ptr<PerlinNoise> caveNoise1_;
    std::unique_ptr<PerlinNoise> caveNoise2_;
    std::unique_ptr<PerlinNoise> caveScaleNoise_;

    // Cave parameters
    static constexpr double CAVE_THRESHOLD = 0.15;
    static constexpr int CAVE_MIN_Y = -48;
    static constexpr int CAVE_MAX_Y = 48;
};

} // namespace mcgen
