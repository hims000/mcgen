#include "core/terrain_engine.hpp"
#include "shared/mappings/block_mapping.hpp"
#include <cmath>

namespace mcgen {

TerrainEngine::TerrainEngine(uint64_t seed, const TerrainConfig& config)
    : seed_(seed), config_(config) {
    terrainNoise_ = std::make_unique<PerlinNoise>(seed);
    biomeNoise_ = std::make_unique<PerlinNoise>(seed + 1);
    temperatureNoise_ = std::make_unique<PerlinNoise>(seed + 2);
    humidityNoise_ = std::make_unique<PerlinNoise>(seed + 3);
}

void TerrainEngine::generateChunk(ChunkData& chunk) {
    stage1_BaseTerrain(chunk);
    stage2_BiomeDecoration(chunk);
    // Caves, ores, and structures are handled separately
}

void TerrainEngine::generateChunks(std::vector<std::unique_ptr<ChunkData>>& chunks) {
    for (auto& chunk : chunks) {
        generateChunk(*chunk);
    }
}

void TerrainEngine::stage1_BaseTerrain(ChunkData& chunk) {
    int chunkX = chunk.getChunkX();
    int chunkZ = chunk.getChunkZ();

    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;

            double height = getTerrainHeight(worldX, worldZ);
            int groundY = static_cast<int>(height);

            // Fill from bottom to ground
            for (int y = MIN_Y; y <= groundY; ++y) {
                Block block;
                if (y == groundY) {
                    block.id = static_cast<BlockID>(BlockType::Grass);
                } else if (y >= groundY - 3) {
                    block.id = static_cast<BlockID>(BlockType::Dirt);
                } else {
                    block.id = static_cast<BlockID>(BlockType::Stone);
                }
                chunk.set(x, y, z, block);
            }

            // Fill above ground with air (already default)
            // Water below sea level
            int seaLevel = static_cast<int>(config_.baseHeight);
            for (int y = groundY + 1; y <= seaLevel; ++y) {
                Block block;
                block.id = static_cast<BlockID>(BlockType::Water);
                chunk.set(x, y, z, block);
            }
        }
    }
}

void TerrainEngine::stage2_BiomeDecoration(ChunkData& chunk) {
    int chunkX = chunk.getChunkX();
    int chunkZ = chunk.getChunkZ();

    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;

            Biome biome = getBiome(worldX, worldZ);

            // Find surface
            for (int y = CHUNK_HEIGHT - 1; y >= MIN_Y; --y) {
                auto& block = chunk.get(x, y, z);
                if (block.id != 0) {  // Non-air
                    // Apply biome surface
                    switch (biome) {
                        case Biome::Desert:
                            if (block.id == static_cast<BlockID>(BlockType::Grass)) {
                                block.id = static_cast<BlockID>(BlockType::Sand);
                            }
                            break;
                        case Biome::Mountains:
                            if (y > 100 && block.id == static_cast<BlockID>(BlockType::Grass)) {
                                block.id = static_cast<BlockID>(BlockType::Stone);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
        }
    }
}

void TerrainEngine::stage3_Caves(ChunkData& chunk) {
    // Placeholder - caves handled by CaveSystem
}

void TerrainEngine::stage4_Ores(ChunkData& chunk) {
    // Placeholder - ores handled by OreVeins
}

void TerrainEngine::stage5_Structures(ChunkData& chunk) {
    // Placeholder - structures handled by StructureGenerator
}

double TerrainEngine::getTerrainHeight(int worldX, int worldZ) const {
    double nx = worldX * config_.noiseScale;
    double nz = worldZ * config_.noiseScale;

    double noise = terrainNoise_->octaveNoise2D(nx, nz, config_.octaves, config_.persistence);

    // Map noise [-1, 1] to height range
    double height = config_.baseHeight + noise * config_.heightScale;
    return std::max(static_cast<double>(MIN_Y), std::min(height, static_cast<double>(MIN_Y + CHUNK_HEIGHT - 1)));
}

TerrainEngine::Biome TerrainEngine::getBiome(int worldX, int worldZ) const {
    double temp = temperatureNoise_->noise2D(worldX * config_.temperatureScale, 
                                              worldZ * config_.temperatureScale);
    double humidity = humidityNoise_->noise2D(worldX * config_.humidityScale,
                                               worldZ * config_.humidityScale);

    if (temp < -0.3) return Biome::Mountains;
    if (temp > 0.5 && humidity < 0.0) return Biome::Desert;
    if (humidity > 0.3) return Biome::Forest;
    return Biome::Plains;
}

} // namespace mcgen
