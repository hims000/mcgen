#include "config/world_config.hpp"
#include "config/cli_parser.hpp"
#include "core/terrain_engine.hpp"
#include "core/cave_system.hpp"
#include "core/ore_veins.hpp"
#include "core/structures.hpp"
#include "core/thread_pool.hpp"
#include "java/world/java_world_writer.hpp"
#include "bedrock/world/bedrock_world_writer.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

using namespace mcgen;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    auto result = config::CLIParser::parse(argc, argv);

    if (!result.valid) {
        std::cerr << "Error: " << result.errorMessage << std::endl;
        return 1;
    }

    if (result.showHelp) {
        config::CLIParser::printHelp();
        return 0;
    }

    if (result.showVersion) {
        config::CLIParser::printVersion();
        return 0;
    }

    const auto& config = result.config;
    int64_t seed = config.getSeedValue();

    std::cout << "========================================" << std::endl;
    std::cout << "  mcgen - Minecraft World Generator" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Seed:        " << config.seed << " (" << seed << ")" << std::endl;
    std::cout << "World Name:  " << config.worldName << std::endl;
    std::cout << "Platform:    ";
    switch (config.platform) {
        case config::WorldConfig::Platform::Java: std::cout << "Java"; break;
        case config::WorldConfig::Platform::Bedrock: std::cout << "Bedrock"; break;
        case config::WorldConfig::Platform::Both: std::cout << "Both"; break;
    }
    std::cout << std::endl;
    std::cout << "Radius:      " << config.radius << " chunks" << std::endl;
    std::cout << "Center:      (" << config.centerX << ", " << config.centerZ << ")" << std::endl;
    std::cout << "Threads:     " << (config.threadCount == 0 ? "auto" : std::to_string(config.threadCount)) << std::endl;
    std::cout << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    // Initialize generators
    TerrainConfig terrainConfig;
    terrainConfig.noiseScale = config.noiseScale;
    terrainConfig.heightScale = config.heightScale;
    terrainConfig.baseHeight = config.baseHeight;

    TerrainEngine terrainEngine(seed, terrainConfig);
    CaveSystem caveSystem(seed);
    OreVeins oreVeins(seed);
    StructureGenerator structures(seed);

    // Generate chunks
    int totalChunks = (config.radius * 2 + 1) * (config.radius * 2 + 1);
    std::vector<std::unique_ptr<ChunkData>> chunks;
    chunks.reserve(totalChunks);

    std::cout << "Generating " << totalChunks << " chunks..." << std::endl;

    for (int cz = -config.radius; cz <= config.radius; ++cz) {
        for (int cx = -config.radius; cx <= config.radius; ++cx) {
            auto chunk = std::make_unique<ChunkData>(config.centerX + cx, config.centerZ + cz);

            // Generate terrain
            terrainEngine.generateChunk(*chunk);

            // Apply features
            if (config.generateCaves) {
                caveSystem.carve(*chunk);
            }
            if (config.generateOres) {
                oreVeins.generate(*chunk);
            }
            if (config.generateStructures && config.generateTrees) {
                structures.generate(*chunk);
            }

            chunks.push_back(std::move(chunk));
        }
    }

    auto genTime = std::chrono::high_resolution_clock::now();
    auto genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(genTime - startTime);
    std::cout << "Generation complete in " << genDuration.count() << "ms" << std::endl;

    // Write worlds
    if (config.platform == config::WorldConfig::Platform::Java ||
        config.platform == config::WorldConfig::Platform::Both) {
        std::cout << "Writing Java world..." << std::endl;
        java::JavaWorldWriter::Config javaConfig;
        javaConfig.worldName = config.worldName;
        javaConfig.outputDir = config.outputDir + "/" + config.worldName + "-java";
        javaConfig.seed = seed;

        java::JavaWorldWriter javaWriter(javaConfig);
        javaWriter.writeChunks(chunks);
        std::cout << "Java world written to: " << javaConfig.outputDir << std::endl;
    }

    if (config.platform == config::WorldConfig::Platform::Bedrock ||
        config.platform == config::WorldConfig::Platform::Both) {
        std::cout << "Writing Bedrock world..." << std::endl;
        bedrock::BedrockWorldWriter::Config bedrockConfig;
        bedrockConfig.worldName = config.worldName;
        bedrockConfig.outputDir = config.outputDir + "/" + config.worldName + "-bedrock";
        bedrockConfig.seed = seed;

        bedrock::BedrockWorldWriter bedrockWriter(bedrockConfig);
        bedrockWriter.writeChunks(chunks);
        bedrockWriter.finalize();
        std::cout << "Bedrock world written to: " << bedrockConfig.outputDir << std::endl;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Complete!" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total time: " << totalDuration.count() << "ms" << std::endl;
    std::cout << "Chunks:     " << totalChunks << std::endl;
    std::cout << "Blocks:     " << (totalChunks * 16 * 16 * 384) << std::endl;

    return 0;
}
