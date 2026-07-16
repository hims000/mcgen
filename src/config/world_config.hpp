#pragma once
#include <string>
#include <cstdint>

namespace mcgen::config {

// World generation configuration
struct WorldConfig {
    // Basic settings
    std::string seed = "default";
    std::string worldName = "mcgen-world";
    std::string outputDir = "./worlds";

    // Platform
    enum class Platform { Java, Bedrock, Both };
    Platform platform = Platform::Java;

    // Generation settings
    int radius = 3;           // Chunk radius to generate
    int centerX = 0;          // Center chunk X
    int centerZ = 0;          // Center chunk Z

    // Terrain settings
    double noiseScale = 0.01;
    double heightScale = 64.0;
    double baseHeight = 64.0;

    // Features
    bool generateCaves = true;
    bool generateOres = true;
    bool generateStructures = true;
    bool generateTrees = true;

    // Parallel generation
    int threadCount = 0;      // 0 = auto-detect

    // Parse seed string to int64
    int64_t getSeedValue() const;
};

} // namespace mcgen::config
