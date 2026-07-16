#include "core/structures.hpp"
#include "shared/mappings/block_mapping.hpp"
#include <cmath>

namespace mcgen {

StructureGenerator::StructureGenerator(uint64_t seed) 
    : seed_(seed), rng_(static_cast<uint32_t>(seed)) {}

void StructureGenerator::generate(ChunkData& chunk) {
    int chunkX = chunk.getChunkX();
    int chunkZ = chunk.getChunkZ();

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int> xDist(0, CHUNK_SIZE - 1);
    std::uniform_int_distribution<int> zDist(0, CHUNK_SIZE - 1);

    // Generate trees
    for (int i = 0; i < 3; ++i) {  // Max 3 attempts per chunk
        if (prob(rng_) > TREE_DENSITY) continue;

        int x = xDist(rng_);
        int z = zDist(rng_);

        // Find surface
        for (int y = CHUNK_HEIGHT - 1; y >= MIN_Y; --y) {
            auto& block = chunk.get(x, y, z);
            if (block.id == static_cast<BlockID>(BlockType::Grass) ||
                block.id == static_cast<BlockID>(BlockType::Dirt)) {
                generateTree(chunk, chunkX * CHUNK_SIZE + x, y + 1, chunkZ * CHUNK_SIZE + z);
                break;
            }
        }
    }
}

void StructureGenerator::generateTree(ChunkData& chunk, int worldX, int worldY, int worldZ) {
    std::uniform_int_distribution<int> heightDist(MIN_TREE_HEIGHT, MAX_TREE_HEIGHT);
    int height = heightDist(rng_);

    int cx = worldX - chunk.getChunkX() * CHUNK_SIZE;
    int cz = worldZ - chunk.getChunkZ() * CHUNK_SIZE;

    if (cx < 0 || cx >= CHUNK_SIZE || cz < 0 || cz >= CHUNK_SIZE) return;
    if (worldY + height >= MIN_Y + CHUNK_HEIGHT) return;

    // Trunk
    for (int y = 0; y < height; ++y) {
        int py = worldY + y;
        if (py >= MIN_Y + CHUNK_HEIGHT) break;
        auto& block = chunk.get(cx, py, cz);
        block.id = static_cast<BlockID>(BlockType::Wood);
    }

    // Leaves
    int leafStart = worldY + height - 2;
    for (int ly = leafStart; ly <= worldY + height; ++ly) {
        for (int lx = cx - 2; lx <= cx + 2; ++lx) {
            for (int lz = cz - 2; lz <= cz + 2; ++lz) {
                if (lx < 0 || lx >= CHUNK_SIZE || lz < 0 || lz >= CHUNK_SIZE) continue;
                if (ly < MIN_Y || ly >= MIN_Y + CHUNK_HEIGHT) continue;

                int dist = std::abs(lx - cx) + std::abs(ly - leafStart) + std::abs(lz - cz);
                if (dist <= 3) {
                    auto& block = chunk.get(lx, ly, lz);
                    if (block.id == 0) {  // Only replace air
                        block.id = static_cast<BlockID>(BlockType::Leaves);
                    }
                }
            }
        }
    }
}

void StructureGenerator::generateDungeon(ChunkData& chunk, int worldX, int worldY, int worldZ) {
    // Simplified dungeon: 7x5x7 room with mob spawner
    int cx = worldX - chunk.getChunkX() * CHUNK_SIZE;
    int cz = worldZ - chunk.getChunkZ() * CHUNK_SIZE;

    for (int dy = 0; dy < 5; ++dy) {
        for (int dx = -3; dx <= 3; ++dx) {
            for (int dz = -3; dz <= 3; ++dz) {
                int px = cx + dx;
                int py = worldY + dy;
                int pz = cz + dz;

                if (px < 0 || px >= CHUNK_SIZE || pz < 0 || pz >= CHUNK_SIZE) continue;
                if (py < MIN_Y || py >= MIN_Y + CHUNK_HEIGHT) continue;

                auto& block = chunk.get(px, py, pz);
                if (dx == -3 || dx == 3 || dz == -3 || dz == 3 || dy == 0 || dy == 4) {
                    block.id = static_cast<BlockID>(BlockType::Cobblestone);
                } else {
                    block.id = static_cast<BlockID>(BlockType::Air);
                }
            }
        }
    }
}

} // namespace mcgen
