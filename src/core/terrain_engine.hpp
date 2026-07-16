#pragma once
#include "shared/chunk_data.hpp"
#include "noise.hpp"
#include <memory>
#include <vector>

namespace mcgen {

// Terrain generation configuration
struct TerrainConfig {
    double noiseScale = 0.01;       // Base terrain scale
    double heightScale = 64.0;      // Max terrain height variation
    double baseHeight = 64.0;       // Sea level / base terrain height
    int octaves = 4;                // Noise octaves
    double persistence = 0.5;       // Octave persistence

    // Biome parameters
    double biomeScale = 0.005;      // Biome noise scale
    double temperatureScale = 0.003;
    double humidityScale = 0.003;
};

// 5-stage terrain generation pipeline
class TerrainEngine {
public:
    explicit TerrainEngine(uint64_t seed, const TerrainConfig& config = TerrainConfig{});

    // Generate a single chunk
    void generateChunk(ChunkData& chunk);

    // Generate multiple chunks (parallel)
    void generateChunks(std::vector<std::unique_ptr<ChunkData>>& chunks);

    // Pipeline stages
    void stage1_BaseTerrain(ChunkData& chunk);      // Heightmap + basic blocks
    void stage2_BiomeDecoration(ChunkData& chunk);  // Biome-specific surface
    void stage3_Caves(ChunkData& chunk);           // Cave carving
    void stage4_Ores(ChunkData& chunk);            // Ore veins
    void stage5_Structures(ChunkData& chunk);     // Trees, dungeons, etc.

private:
    uint64_t seed_;
    TerrainConfig config_;

    // Noise generators
    std::unique_ptr<PerlinNoise> terrainNoise_;
    std::unique_ptr<PerlinNoise> biomeNoise_;
    std::unique_ptr<PerlinNoise> temperatureNoise_;
    std::unique_ptr<PerlinNoise> humidityNoise_;

    // Helper: get height at world coordinates
    double getTerrainHeight(int worldX, int worldZ) const;

    // Helper: determine biome
    enum class Biome { Plains, Forest, Desert, Mountains, Ocean };
    Biome getBiome(int worldX, int worldZ) const;
};

} // namespace mcgen
