#include "core/ore_veins.hpp"
#include "shared/mappings/block_mapping.hpp"
#include <cmath>

namespace mcgen {

OreVeins::OreVeins(uint64_t seed) : seed_(seed), rng_(static_cast<uint32_t>(seed)) {}

void OreVeins::generate(ChunkData& chunk) {
    int chunkX = chunk.getChunkX();
    int chunkZ = chunk.getChunkZ();

    for (const auto& ore : ORES) {
        std::uniform_int_distribution<int> countDist(0, ore.veinsPerChunk);
        int veins = countDist(rng_);

        for (int v = 0; v < veins; ++v) {
            std::uniform_int_distribution<int> xDist(0, CHUNK_SIZE - 1);
            std::uniform_int_distribution<int> zDist(0, CHUNK_SIZE - 1);
            std::uniform_int_distribution<int> yDist(ore.minY, ore.maxY);

            int startX = chunkX * CHUNK_SIZE + xDist(rng_);
            int startY = yDist(rng_);
            int startZ = chunkZ * CHUNK_SIZE + zDist(rng_);

            generateVein(chunk, ore, startX, startY, startZ);
        }
    }
}

void OreVeins::generateVein(ChunkData& chunk, const OreConfig& config, 
                            int startX, int startY, int startZ) {
    std::uniform_int_distribution<int> dirDist(-1, 1);

    int cx = startX - chunk.getChunkX() * CHUNK_SIZE;
    int cz = startZ - chunk.getChunkZ() * CHUNK_SIZE;

    if (cx < 0 || cx >= CHUNK_SIZE || cz < 0 || cz >= CHUNK_SIZE) return;
    if (startY < MIN_Y || startY >= MIN_Y + CHUNK_HEIGHT) return;

    int x = cx, y = startY, z = cz;

    for (int i = 0; i < config.veinSize; ++i) {
        if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) break;
        if (y < MIN_Y || y >= MIN_Y + CHUNK_HEIGHT) break;

        auto& block = chunk.get(x, y, z);
        if (block.id == static_cast<BlockID>(BlockType::Stone)) {
            block.id = static_cast<BlockID>(config.type);
        }

        // Random walk
        x += dirDist(rng_);
        y += dirDist(rng_);
        z += dirDist(rng_);
    }
}

} // namespace mcgen
