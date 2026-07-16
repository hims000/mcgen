#pragma once
#include "shared/chunk_data.hpp"
#include "java/nbt/nbt_codec.hpp"
#include <string>
#include <vector>
#include <memory>

namespace mcgen::java {

// Java Edition world writer
class JavaWorldWriter {
public:
    struct Config {
        std::string worldName = "mcgen-world";
        std::string outputDir;
        int64_t seed = 0;
        std::string generator = "default";
    };

    explicit JavaWorldWriter(const Config& config);
    ~JavaWorldWriter();

    // Initialize world (creates level.dat)
    void initialize();

    // Write chunk
    void writeChunk(const ChunkData& chunk);

    // Write multiple chunks
    void writeChunks(const std::vector<std::unique_ptr<ChunkData>>& chunks);

    // Finalize world
    void finalize();

private:
    Config config_;
    bool initialized_ = false;

    // Create level.dat
    void writeLevelDat();

    // Create region directory structure
    void createDirectories();
};

} // namespace mcgen::java
