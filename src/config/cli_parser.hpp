#pragma once
#include "world_config.hpp"
#include <string>
#include <vector>

namespace mcgen::config {

// Command line argument parser
class CLIParser {
public:
    struct Result {
        WorldConfig config;
        bool showHelp = false;
        bool showVersion = false;
        bool verbose = false;
        bool valid = true;
        std::string errorMessage;
    };

    static Result parse(int argc, char* argv[]);
    static void printHelp();
    static void printVersion();
};

} // namespace mcgen::config
