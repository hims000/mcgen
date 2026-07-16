#include "core/cave_system.hpp"
#include "shared/mappings/block_mapping.hpp"
#include <cmath>

namespace mcgen {

CaveSystem::CaveSystem(uint64_t seed) 
    : seed_(seed) {
    caveNoise1_ = std::make_unique<PerlinNoise>(seed + 100);
    caveNoise2_ = std::make_unique<PerlinNoise>(seed + 101);
    caveScaleNoise_ = std::make_unique<PerlinNoise>(seed + 102);
}

void CaveSystem::carve(ChunkData& chunk) {
    int chunkX = chunk.getChunkX();
    int chunkZ = chunk.getChunkZ();

    for (int y = CAVE_MIN_Y; y <= CAVE_MAX_Y; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                int worldX = chunkX * CHUNK_SIZE + x;
                int worldZ = chunkZ * CHUNK_SIZE + z;

                if (isCave(worldX, y, worldZ)) {
                    Block airBlock;
                    airBlock.id = static_cast<BlockID>(BlockType::Air);
                    chunk.set(x, y, z, airBlock);
                }
            }
        }
    }
}

bool CaveSystem::isCave(int worldX, int worldY, int worldZ) const {
    if (worldY < CAVE_MIN_Y || worldY > CAVE_MAX_Y) return false;

    double scale = caveScaleNoise_->noise(worldX * 0.01, worldY * 0.01, worldZ * 0.01);
    double freq = 0.02 + scale * 0.01;

    double n1 = caveNoise1_->noise(worldX * freq, worldY * freq, worldZ * freq);
    double n2 = caveNoise2_->noise(worldX * freq + 100, worldY * freq, worldZ * freq);

    double combined = n1 * n1 + n2 * n2;
    return combined < CAVE_THRESHOLD;
}

} // namespace mcgen
