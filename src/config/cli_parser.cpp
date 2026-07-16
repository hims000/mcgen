#include "config/cli_parser.hpp"
#include <iostream>
#include <cstring>

namespace mcgen::config {

CLIParser::Result CLIParser::parse(int argc, char* argv[]) {
    Result result;
    result.config = WorldConfig{};

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            result.showHelp = true;
            return result;
        } else if (arg == "-v" || arg == "--version") {
            result.showVersion = true;
            return result;
        } else if (arg == "--verbose") {
            result.verbose = true;
        } else if ((arg == "-s" || arg == "--seed") && i + 1 < argc) {
            result.config.seed = argv[++i];
        } else if ((arg == "-n" || arg == "--name") && i + 1 < argc) {
            result.config.worldName = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            result.config.outputDir = argv[++i];
        } else if ((arg == "-p" || arg == "--platform") && i + 1 < argc) {
            std::string platform = argv[++i];
            if (platform == "java") {
                result.config.platform = WorldConfig::Platform::Java;
            } else if (platform == "bedrock") {
                result.config.platform = WorldConfig::Platform::Bedrock;
            } else if (platform == "both") {
                result.config.platform = WorldConfig::Platform::Both;
            } else {
                result.valid = false;
                result.errorMessage = "Unknown platform: " + platform;
                return result;
            }
        } else if ((arg == "-r" || arg == "--radius") && i + 1 < argc) {
            try {
                result.config.radius = std::stoi(argv[++i]);
            } catch (...) {
                result.valid = false;
                result.errorMessage = "Invalid radius value";
                return result;
            }
        } else if ((arg == "-c" || arg == "--center") && i + 2 < argc) {
            try {
                result.config.centerX = std::stoi(argv[++i]);
                result.config.centerZ = std::stoi(argv[++i]);
            } catch (...) {
                result.valid = false;
                result.errorMessage = "Invalid center coordinates";
                return result;
            }
        } else if (arg == "--no-caves") {
            result.config.generateCaves = false;
        } else if (arg == "--no-ores") {
            result.config.generateOres = false;
        } else if (arg == "--no-structures") {
            result.config.generateStructures = false;
        } else if (arg == "--no-trees") {
            result.config.generateTrees = false;
        } else if ((arg == "-j" || arg == "--threads") && i + 1 < argc) {
            try {
                result.config.threadCount = std::stoi(argv[++i]);
            } catch (...) {
                result.valid = false;
                result.errorMessage = "Invalid thread count";
                return result;
            }
        } else if (arg[0] == '-') {
            result.valid = false;
            result.errorMessage = "Unknown option: " + arg;
            return result;
        }
    }

    return result;
}

void CLIParser::printHelp() {
    std::cout << R"(
mcgen - Minecraft World Generator

Usage: mcgen [options]

Options:
  -s, --seed <string>       World seed (default: "default")
  -n, --name <string>       World name (default: "mcgen-world")
  -o, --output <path>       Output directory (default: "./worlds")
  -p, --platform <type>     Platform: java, bedrock, both (default: java)
  -r, --radius <int>        Chunk generation radius (default: 3)
  -c, --center <x> <z>      Center chunk coordinates (default: 0 0)
      --no-caves            Disable cave generation
      --no-ores             Disable ore generation
      --no-structures       Disable structure generation
      --no-trees            Disable tree generation
  -j, --threads <int>       Number of threads (0 = auto)
  -v, --version             Show version
  -h, --help                Show this help

Examples:
  mcgen --seed "hello world" --name MyWorld --platform both --radius 5
  mcgen --seed 12345 --platform bedrock --radius 3 -o ~/minecraft-worlds
  mcgen --seed test --no-caves --no-ores --radius 10
)" << std::endl;
}

void CLIParser::printVersion() {
    std::cout << "mcgen version " << MCGEN_VERSION << std::endl;
    std::cout << "LevelDB backend: " << MCGEN_LEVELDB_BACKEND << std::endl;
    std::cout << "ZlibRaw support: " << (MCGEN_HAS_ZLIBRAW ? "yes" : "no") << std::endl;
}

} // namespace mcgen::config
